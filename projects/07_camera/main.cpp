#include "smoke_test.h"
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QVector3D>
#include <QWheelEvent>
#include <QtMath>
#include <memory>

class Window : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
  public:
    Window() { setFocusPolicy(Qt::StrongFocus); }

    ~Window() override { cleanup(); }

  private:
    std::unique_ptr<QOpenGLShaderProgram> shader_;
    GLuint vao_id_ = 0;
    GLuint vbo_id_ = 0;
    GLuint ebo_id_ = 0;
    bool initialized_ = false;
    QMetaObject::Connection cleanupConnection_;
    float yaw_ = 35.0f;
    float pitch_ = 22.0f;
    float distance_ = 5.0f;
    QPoint lastMousePosition_;

    void initializeGL() override {
        if (!initializeOpenGLFunctions()) {
            qFatal("This lesson requires a desktop OpenGL 3.3 core context.");
        }
        initialized_ = true;
        cleanupConnection_ =
            connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, [this] { cleanup(); });

        // Keep one vertex per row so the attribute layout is easy to read.
        // clang-format off
        // Eight corners; position (x, y, z) followed by color (r, g, b).
        const float vertices[] = {
            -0.5f, -0.5f, -0.5f, 1.0f, 0.2f, 0.2f,
             0.5f, -0.5f, -0.5f, 0.2f, 1.0f, 0.2f,
             0.5f,  0.5f, -0.5f, 0.2f, 0.4f, 1.0f,
            -0.5f,  0.5f, -0.5f, 1.0f, 0.9f, 0.2f,
            -0.5f, -0.5f,  0.5f, 1.0f, 0.3f, 0.8f,
             0.5f, -0.5f,  0.5f, 0.2f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f, 0.7f, 0.4f, 1.0f,
            -0.5f,  0.5f,  0.5f, 0.9f, 0.9f, 0.9f,
        };
        // Six faces, two triangles per face, three indices per triangle.
        const unsigned int indices[] = {
            0, 3, 2, 0, 2, 1, // back
            4, 5, 6, 4, 6, 7, // front
            0, 4, 7, 0, 7, 3, // left
            1, 2, 6, 1, 6, 5, // right
            0, 1, 5, 0, 5, 4, // bottom
            3, 7, 6, 3, 6, 2, // top
        };
        // clang-format on

        // VAO: remember the vertex layout and the element-buffer binding.
        glGenVertexArrays(1, &vao_id_);
        glBindVertexArray(vao_id_);

        // VBO: upload the vertex data to GPU memory.
        glGenBuffers(1, &vbo_id_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // EBO: upload indices; this binding belongs to the current VAO.
        glGenBuffers(1, &ebo_id_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              reinterpret_cast<const void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Do not unbind the EBO while our VAO is bound: it would forget the EBO.

        shader_ = std::make_unique<QOpenGLShaderProgram>();
        if (!shader_->addShaderFromSourceCode(QOpenGLShader::Vertex, R"GLSL(
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 vertexColor;
void main() {
    vertexColor = color;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
)GLSL") || !shader_->addShaderFromSourceCode(QOpenGLShader::Fragment, R"GLSL(
#version 330 core
in vec3 vertexColor;
out vec4 fragmentColor;
void main() {
    fragmentColor = vec4(vertexColor, 1.0);
}
)GLSL") || !shader_->link()) {
            qFatal("Shader compilation/linking failed: %s", qPrintable(shader_->log()));
        }

        glEnable(GL_DEPTH_TEST);
    }

    void paintGL() override {

        glClearColor(0.07f, 0.09f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (!shader_->bind()) {
            qFatal("Could not bind the shader: %s", qPrintable(shader_->log()));
        }

        const float yawRadians = qDegreesToRadians(yaw_);
        const float pitchRadians = qDegreesToRadians(pitch_);
        const QVector3D eye(distance_ * qCos(pitchRadians) * qSin(yawRadians),
                            distance_ * qSin(pitchRadians),
                            distance_ * qCos(pitchRadians) * qCos(yawRadians));
        QMatrix4x4 view;
        view.lookAt(eye, QVector3D(0, 0, 0), QVector3D(0, 1, 0));
        QMatrix4x4 projection;
        projection.perspective(45.0f, float(width()) / float(qMax(1, height())), 0.1f, 100.0f);
        shader_->setUniformValue("view", view);
        shader_->setUniformValue("projection", projection);

        glBindVertexArray(vao_id_);
        for (float x : {-1.4f, 0.0f, 1.4f}) {
            QMatrix4x4 model;
            model.translate(x, 0.0f, 0.0f);
            shader_->setUniformValue("model", model);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }
        glBindVertexArray(0);

        shader_->release();
    }

    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
        case Qt::Key_Escape:
            close();
            break;

        case Qt::Key_Left:
            yaw_ -= 5.0f;
            break;
        case Qt::Key_Right:
            yaw_ += 5.0f;
            break;
        case Qt::Key_Up:
            pitch_ = qMin(80.0f, pitch_ + 5.0f);
            break;
        case Qt::Key_Down:
            pitch_ = qMax(-80.0f, pitch_ - 5.0f);
            break;
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            distance_ = qMax(2.0f, distance_ - 0.3f);
            break;
        case Qt::Key_Minus:
            distance_ = qMin(12.0f, distance_ + 0.3f);
            break;
        case Qt::Key_R:
            yaw_ = 35.0f;
            pitch_ = 22.0f;
            distance_ = 5.0f;
            break;

        default:
            QOpenGLWidget::keyPressEvent(event);
            return;
        }
        update();
        event->accept();
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            lastMousePosition_ = event->position().toPoint();
            event->accept();
        } else {
            QOpenGLWidget::mousePressEvent(event);
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (event->buttons() & Qt::LeftButton) {
            const QPoint current = event->position().toPoint();
            const QPoint delta = current - lastMousePosition_;
            lastMousePosition_ = current;
            yaw_ += float(delta.x()) * 0.5f;
            pitch_ = qBound(-80.0f, pitch_ - float(delta.y()) * 0.5f, 80.0f);
            update();
            event->accept();
        } else {
            QOpenGLWidget::mouseMoveEvent(event);
        }
    }

    void wheelEvent(QWheelEvent *event) override {
        distance_ = qBound(2.0f, distance_ - float(event->angleDelta().y()) * 0.005f, 12.0f);
        update();
        event->accept();
    }

    void cleanup() {
        if (!initialized_) {
            return;
        }
        // OpenGL objects must be deleted while their context is current.
        makeCurrent();

        shader_.reset();
        glDeleteBuffers(1, &ebo_id_);
        glDeleteBuffers(1, &vbo_id_);
        glDeleteVertexArrays(1, &vao_id_);
        ebo_id_ = vbo_id_ = vao_id_ = 0;
        initialized_ = false;
        doneCurrent();
        disconnect(cleanupConnection_);
    }
};

int main(int argc, char **argv) {
    // Set the format before QApplication, including on macOS.
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);
    Window window;
    window.setWindowTitle("07 - An interactive orbit camera");
    window.resize(800, 600);
    window.show();
    runSmokeTestIfRequested(window,
                            {Qt::Key_Right, Qt::Key_Up, Qt::Key_Plus, Qt::Key_Minus, Qt::Key_R});
    return app.exec();
}

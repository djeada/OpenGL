#include "smoke_test.h"
#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QTimer>
#include <QVector3D>
#include <memory>

class Window : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
  public:
    Window() {
        setFocusPolicy(Qt::StrongFocus);

        elapsed_.start();
        connect(&timer_, &QTimer::timeout, this, [this] { update(); });
        timer_.start(16);
    }

    ~Window() override { cleanup(); }

  private:
    std::unique_ptr<QOpenGLShaderProgram> shader_;
    GLuint vao_id_ = 0;
    GLuint vbo_id_ = 0;
    GLuint ebo_id_ = 0;
    bool initialized_ = false;
    QMetaObject::Connection cleanupConnection_;
    QElapsedTimer elapsed_;
    QTimer timer_;
    bool depthTest_ = true;

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

        if (depthTest_) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
        QMatrix4x4 projection;
        projection.perspective(45.0f, float(width()) / float(qMax(1, height())), 0.1f, 100.0f);
        QMatrix4x4 view;
        view.lookAt(QVector3D(2.8f, 2.0f, 3.2f), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
        shader_->setUniformValue("projection", projection);
        shader_->setUniformValue("view", view);

        glBindVertexArray(vao_id_);
        const QVector3D positions[] = {QVector3D(-0.45f, 0.0f, 0.0f),
                                       QVector3D(0.45f, 0.0f, -0.75f)};
        for (const auto &position : positions) {
            QMatrix4x4 model;
            model.translate(position);
            model.rotate(float(elapsed_.elapsed()) * 0.025f, 1.0f, 0.7f, 0.2f);
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

        case Qt::Key_D:
            depthTest_ = !depthTest_;
            break;

        default:
            QOpenGLWidget::keyPressEvent(event);
            return;
        }
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
    window.setWindowTitle("06 - Perspective and depth testing");
    window.resize(800, 600);
    window.show();
    runSmokeTestIfRequested(window, {Qt::Key_D, Qt::Key_D});
    return app.exec();
}

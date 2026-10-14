#include "smoke_test.h"
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QVector3D>
#include <QVector4D>
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
    bool reverseOrder_ = false;
    float opacity_ = 0.5f;

    void initializeGL() override {
        if (!initializeOpenGLFunctions()) {
            qFatal("This lesson requires a desktop OpenGL 3.3 core context.");
        }
        initialized_ = true;
        cleanupConnection_ =
            connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, [this] { cleanup(); });

        // Keep one vertex per row so the attribute layout is easy to read.
        // clang-format off
        const float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
        };
        const unsigned int indices[] = {0, 1, 3, 1, 2, 3};
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Do not unbind the EBO while our VAO is bound: it would forget the EBO.

        shader_ = std::make_unique<QOpenGLShaderProgram>();
        if (!shader_->addShaderFromSourceCode(QOpenGLShader::Vertex, R"GLSL(
#version 330 core
layout(location = 0) in vec3 position;
uniform mat4 transform;
void main() {
    gl_Position = transform * vec4(position, 1.0);
}
)GLSL") || !shader_->addShaderFromSourceCode(QOpenGLShader::Fragment, R"GLSL(
#version 330 core
uniform vec4 color;
uniform bool checker;
out vec4 fragmentColor;
void main() {
    if (checker) {
        float cell = mod(floor(gl_FragCoord.x / 32.0) + floor(gl_FragCoord.y / 32.0), 2.0);
        fragmentColor = vec4(mix(vec3(0.12), vec3(0.28), cell), 1.0);
    } else {
        fragmentColor = color;
    }
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

        const float aspect = float(width()) / float(qMax(1, height()));
        QMatrix4x4 projection;
        projection.ortho(-aspect, aspect, -1.0f, 1.0f, 0.1f, 10.0f);
        QMatrix4x4 view;
        view.translate(0.0f, 0.0f, -3.0f);
        glBindVertexArray(vao_id_);

        // Draw opaque geometry first, with depth writes enabled.
        QMatrix4x4 background;
        background.translate(0.0f, 0.0f, -0.8f);
        background.scale(2.0f * aspect, 2.0f, 1.0f);
        shader_->setUniformValue("transform", projection * view * background);
        shader_->setUniformValue("checker", true);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        shader_->setUniformValue("checker", false);

        // Straight alpha: result = source * alpha + destination * (1 - alpha).
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE); // Keep depth testing, but transparent panels do not write depth.
        const QVector3D positions[] = {
            QVector3D(-0.35f, -0.15f, -0.4f), // farthest from the fixed camera
            QVector3D(0.0f, 0.0f, 0.0f),
            QVector3D(0.35f, 0.15f, 0.4f), // nearest
        };
        const QVector3D colors[] = {QVector3D(1.0f, 0.15f, 0.15f), QVector3D(0.15f, 1.0f, 0.15f),
                                    QVector3D(0.15f, 0.3f, 1.0f)};
        for (int draw = 0; draw < 3; ++draw) {
            // This camera never moves, so the back-to-front ordering is known.
            const int i = reverseOrder_ ? 2 - draw : draw;
            QMatrix4x4 model;
            model.translate(positions[i]);
            model.scale(1.1f);
            shader_->setUniformValue("transform", projection * view * model);
            shader_->setUniformValue("color", QVector4D(colors[i], opacity_));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }
        glDepthMask(GL_TRUE); // Restore before the next clear and opaque pass.
        glDisable(GL_BLEND);
        glBindVertexArray(0);

        shader_->release();
    }

    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
        case Qt::Key_Escape:
            close();
            break;

        case Qt::Key_O:
            reverseOrder_ = !reverseOrder_;
            break;
        case Qt::Key_Up:
            opacity_ = qMin(0.9f, opacity_ + 0.1f);
            break;
        case Qt::Key_Down:
            opacity_ = qMax(0.1f, opacity_ - 0.1f);
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
    window.setWindowTitle("09 - Alpha blending and draw order");
    window.resize(800, 600);
    window.show();
    runSmokeTestIfRequested(window, {Qt::Key_O, Qt::Key_Up, Qt::Key_Down, Qt::Key_O});
    return app.exec();
}

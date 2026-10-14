#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QTimer>
#include <memory>

#include "smoke_test.h"

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
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f,
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

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Do not unbind the EBO while our VAO is bound: it would forget the EBO.

        shader_ = std::make_unique<QOpenGLShaderProgram>();
        if (!shader_->addShaderFromSourceCode(QOpenGLShader::Vertex, R"GLSL(
#version 330 core
layout(location = 0) in vec2 position;
uniform float time;
void main() {
    vec2 offset = vec2(0.22 * sin(time), 0.0);
    gl_Position = vec4(position + offset, 0.0, 1.0);
}
)GLSL") || !shader_->addShaderFromSourceCode(QOpenGLShader::Fragment, R"GLSL(
#version 330 core
uniform float time;
out vec4 fragmentColor;
void main() {
    vec3 color = 0.5 + 0.5 * sin(time + vec3(0.0, 2.0, 4.0));
    fragmentColor = vec4(color, 1.0);
}
)GLSL") || !shader_->link()) {
            qFatal("Shader compilation/linking failed: %s", qPrintable(shader_->log()));
        }
    }

    void paintGL() override {

        glClearColor(0.07f, 0.09f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if (!shader_->bind()) {
            qFatal("Could not bind the shader: %s", qPrintable(shader_->log()));
        }
        shader_->setUniformValue("time", float(elapsed_.elapsed()) / 1000.0f);
        glBindVertexArray(vao_id_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        shader_->release();
    }

    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
        case Qt::Key_Escape:
            close();
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
    window.setWindowTitle("03 - Uniforms and animation");
    window.resize(800, 600);
    window.show();
    runSmokeTestIfRequested(window);
    return app.exec();
}

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
#include <QSize>
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
    std::unique_ptr<QOpenGLShaderProgram> postShader_;
    GLuint screen_vao_ = 0;
    GLuint screen_vbo_ = 0;
    GLuint screen_ebo_ = 0;
    GLuint framebuffer_id_ = 0;
    GLuint color_texture_ = 0;
    GLuint depth_renderbuffer_ = 0;
    QSize framebufferSize_;
    int effect_ = 1;

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

        postShader_ = std::make_unique<QOpenGLShaderProgram>();
        if (!postShader_->addShaderFromSourceCode(QOpenGLShader::Vertex, R"GLSL(
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;
out vec2 textureCoordinate;
void main() {
    textureCoordinate = uv;
    gl_Position = vec4(position, 0.0, 1.0);
}
)GLSL") || !postShader_->addShaderFromSourceCode(QOpenGLShader::Fragment, R"GLSL(
#version 330 core
in vec2 textureCoordinate;
uniform sampler2D sceneTexture;
uniform int effect;
out vec4 fragmentColor;
void main() {
    vec3 color = texture(sceneTexture, textureCoordinate).rgb;
    if (effect == 2) {
        color = vec3(dot(color, vec3(0.2126, 0.7152, 0.0722)));
    } else if (effect == 3) {
        color = vec3(1.0) - color;
    }
    fragmentColor = vec4(color, 1.0);
}
)GLSL") || !postShader_->link()) {
            qFatal("Shader compilation/linking failed: %s", qPrintable(postShader_->log()));
        }

        const float screenVertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  -1.0f, 1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f, -1.0f, 1.0f,  0.0f, 1.0f,
        };
        const unsigned int screenIndices[] = {0, 1, 3, 1, 2, 3};
        glGenVertexArrays(1, &screen_vao_);
        glBindVertexArray(screen_vao_);
        glGenBuffers(1, &screen_vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, screen_vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
        glGenBuffers(1, &screen_ebo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenIndices), screenIndices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                              reinterpret_cast<const void *>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenFramebuffers(1, &framebuffer_id_);
        glGenTextures(1, &color_texture_);
        glGenRenderbuffers(1, &depth_renderbuffer_);
        // Storage is allocated on resize, once Qt has created its widget framebuffer.
    }

    void paintGL() override {

        ensureFramebuffer(); // Also catches a change of device-pixel ratio.
        const int pixelWidth = framebufferSize_.width();
        const int pixelHeight = framebufferSize_.height();

        // Pass 1: draw the 3D scene into our framebuffer's color texture.
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
        glViewport(0, 0, pixelWidth, pixelHeight);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.07f, 0.09f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (!shader_->bind()) {
            qFatal("Could not bind the scene shader: %s", qPrintable(shader_->log()));
        }
        QMatrix4x4 projection;
        projection.perspective(45.0f, float(pixelWidth) / float(pixelHeight), 0.1f, 100.0f);
        QMatrix4x4 view;
        view.lookAt(QVector3D(2.8f, 2.0f, 3.2f), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
        QMatrix4x4 model;
        model.rotate(float(elapsed_.elapsed()) * 0.025f, 1.0f, 0.7f, 0.2f);
        shader_->setUniformValue("projection", projection);
        shader_->setUniformValue("view", view);
        shader_->setUniformValue("model", model);
        glBindVertexArray(vao_id_);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        shader_->release();

        // Pass 2: sample that texture while drawing a screen-filling quad.
        // QOpenGLWidget renders into its own FBO. Restore its ID, never assume ID zero.
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
        glViewport(0, 0, pixelWidth, pixelHeight);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        if (!postShader_->bind()) {
            qFatal("Could not bind the post-processing shader: %s", qPrintable(postShader_->log()));
        }
        postShader_->setUniformValue("sceneTexture", 0);
        postShader_->setUniformValue("effect", effect_);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, color_texture_);
        glBindVertexArray(screen_vao_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        postShader_->release();
    }

    void resizeGL(int, int) override { ensureFramebuffer(); }

    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
        case Qt::Key_Escape:
            close();
            break;

        case Qt::Key_1:
            effect_ = 1;
            break;
        case Qt::Key_2:
            effect_ = 2;
            break;
        case Qt::Key_3:
            effect_ = 3;
            break;

        default:
            QOpenGLWidget::keyPressEvent(event);
            return;
        }
        update();
        event->accept();
    }

    void ensureFramebuffer() {
        // QWidget dimensions are logical pixels; texture storage uses physical pixels.
        const QSize size(qMax(1, qRound(width() * devicePixelRatioF())),
                         qMax(1, qRound(height() * devicePixelRatioF())));
        if (size == framebufferSize_) {
            return;
        }
        framebufferSize_ = size;
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
        glBindTexture(GL_TEXTURE_2D, color_texture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.width(), size.height(), 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_,
                               0);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_renderbuffer_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size.width(), size.height());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                  depth_renderbuffer_);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            qFatal("The offscreen framebuffer is incomplete.");
        }
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    }

    void cleanup() {
        if (!initialized_) {
            return;
        }
        // OpenGL objects must be deleted while their context is current.
        makeCurrent();

        postShader_.reset();
        glDeleteRenderbuffers(1, &depth_renderbuffer_);
        glDeleteTextures(1, &color_texture_);
        glDeleteFramebuffers(1, &framebuffer_id_);
        glDeleteBuffers(1, &screen_ebo_);
        glDeleteBuffers(1, &screen_vbo_);
        glDeleteVertexArrays(1, &screen_vao_);
        depth_renderbuffer_ = color_texture_ = framebuffer_id_ = 0;
        screen_ebo_ = screen_vbo_ = screen_vao_ = 0;
        framebufferSize_ = QSize();

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
    window.setWindowTitle("10 - Framebuffers and post-processing");
    window.resize(800, 600);
    window.show();
    runSmokeTestIfRequested(window, {Qt::Key_2, Qt::Key_3, Qt::Key_1});
    return app.exec();
}

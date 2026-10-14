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
    int lightingMode_ = 3;

    void initializeGL() override {
        if (!initializeOpenGLFunctions()) {
            qFatal("This lesson requires a desktop OpenGL 3.3 core context.");
        }
        initialized_ = true;
        cleanupConnection_ =
            connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, [this] { cleanup(); });

        // Keep one vertex per row so the attribute layout is easy to read.
        // clang-format off
        // Position and face normal: 24 vertices for six hard-edged faces.
        const float vertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        };
        const unsigned int indices[] = {
            0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23
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
layout(location = 1) in vec3 normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
out vec3 worldPosition;
out vec3 worldNormal;
void main() {
    vec4 world = model * vec4(position, 1.0);
    worldPosition = world.xyz;
    worldNormal = normalMatrix * normal;
    gl_Position = projection * view * world;
}
)GLSL") || !shader_->addShaderFromSourceCode(QOpenGLShader::Fragment, R"GLSL(
#version 330 core
in vec3 worldPosition;
in vec3 worldNormal;
uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 objectColor;
uniform int lightingMode;
uniform bool unlit;
out vec4 fragmentColor;
void main() {
    if (unlit) {
        fragmentColor = vec4(objectColor, 1.0);
        return;
    }
    vec3 n = normalize(worldNormal);
    vec3 lightDirection = normalize(lightPosition - worldPosition);
    float diffuse = max(dot(n, lightDirection), 0.0);
    vec3 viewDirection = normalize(viewPosition - worldPosition);
    vec3 reflected = reflect(-lightDirection, n);
    float specular = diffuse > 0.0 ? pow(max(dot(viewDirection, reflected), 0.0), 32.0) : 0.0;
    vec3 color = 0.25 * objectColor;
    if (lightingMode >= 2) color += diffuse * objectColor;
    if (lightingMode >= 3) color += 0.5 * specular * vec3(1.0);
    fragmentColor = vec4(color, 1.0);
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

        const QVector3D eye(3.0f, 2.0f, 4.0f);
        const QVector3D light(1.7f, 1.4f, 2.0f);
        QMatrix4x4 view;
        view.lookAt(eye, QVector3D(0, 0, 0), QVector3D(0, 1, 0));
        QMatrix4x4 projection;
        projection.perspective(45.0f, float(width()) / float(qMax(1, height())), 0.1f, 100.0f);
        shader_->setUniformValue("view", view);
        shader_->setUniformValue("projection", projection);
        shader_->setUniformValue("viewPosition", eye);
        shader_->setUniformValue("lightPosition", light);
        shader_->setUniformValue("lightingMode", lightingMode_);

        glBindVertexArray(vao_id_);
        QMatrix4x4 model;
        model.rotate(float(elapsed_.elapsed()) * 0.02f, 0.0f, 1.0f, 0.0f);
        model.scale(1.0f, 0.7f, 1.2f);
        shader_->setUniformValue("model", model);
        // Inverse transpose keeps normals perpendicular under nonuniform scaling.
        shader_->setUniformValue("normalMatrix", model.normalMatrix());
        shader_->setUniformValue("objectColor", QVector3D(0.2f, 0.6f, 0.9f));
        shader_->setUniformValue("unlit", false);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        // A small white cube marks the point light's position.
        QMatrix4x4 lamp;
        lamp.translate(light);
        lamp.scale(0.15f);
        shader_->setUniformValue("model", lamp);
        shader_->setUniformValue("normalMatrix", lamp.normalMatrix());
        shader_->setUniformValue("objectColor", QVector3D(1.0f, 1.0f, 1.0f));
        shader_->setUniformValue("unlit", true);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        shader_->release();
    }

    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
        case Qt::Key_Escape:
            close();
            break;

        case Qt::Key_1:
            lightingMode_ = 1;
            break;
        case Qt::Key_2:
            lightingMode_ = 2;
            break;
        case Qt::Key_3:
            lightingMode_ = 3;
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
    window.setWindowTitle("08 - Ambient, diffuse and specular lighting");
    window.resize(800, 600);
    window.show();
    runSmokeTestIfRequested(window, {Qt::Key_1, Qt::Key_2, Qt::Key_3});
    return app.exec();
}

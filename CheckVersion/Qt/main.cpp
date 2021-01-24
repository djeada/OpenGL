#include <QApplication>
#include <QDebug>
#include <GL/glu.h>
#include <QtOpenGL>
#include <QSurfaceFormat>
#include <QOpenGLWindow>

class MainWindow : public QOpenGLWindow
{
public:

    MainWindow() : QOpenGLWindow()
{
    setSurfaceType(QWindow::OpenGLSurface);

    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3, 2);
    setFormat(format);

    auto openGLContext = new QOpenGLContext();
    openGLContext->setFormat(format);
    openGLContext->create();
    openGLContext->makeCurrent(this);
}

void paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClearColor(0, 0, 0, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    window.setTitle("Check version");
    window.resize(640, 480);
    window.show();
    auto* p = glGetString(GL_VERSION);
    qDebug() << "version: " << p;
    return a.exec();
}

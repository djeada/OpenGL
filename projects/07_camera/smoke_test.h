// Local to this project: copying the folder also copies its rendering test.
#pragma once

#include <QApplication>
#include <QDebug>
#include <QImage>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector>
#include <initializer_list>

namespace smoke {
inline bool checkFrame(QOpenGLWidget &window) {
    if (!window.isValid() || !window.context() || window.context()->isOpenGLES()) {
        qCritical() << "A desktop OpenGL context could not be created.";
        return false;
    }
    const auto format = window.context()->format();
    if (format.majorVersion() < 3 || (format.majorVersion() == 3 && format.minorVersion() < 3)) {
        qCritical() << "OpenGL 3.3 or newer is required.";
        return false;
    }
    const QImage frame = window.grabFramebuffer().convertToFormat(QImage::Format_RGB32);
    if (frame.isNull() || frame.width() != qRound(window.width() * window.devicePixelRatioF()) ||
        frame.height() != qRound(window.height() * window.devicePixelRatioF())) {
        qCritical() << "The framebuffer has the wrong size.";
        return false;
    }
    const QColor background = frame.pixelColor(0, 0);
    int changed = 0;
    for (int y = 0; y < frame.height(); ++y) {
        const auto *row = reinterpret_cast<const QRgb *>(frame.constScanLine(y));
        for (int x = 0; x < frame.width(); ++x) {
            if (qAbs(qRed(row[x]) - background.red()) > 12 ||
                qAbs(qGreen(row[x]) - background.green()) > 12 ||
                qAbs(qBlue(row[x]) - background.blue()) > 12) {
                ++changed;
            }
        }
    }
    if (changed < frame.width() * frame.height() / 100) {
        qCritical() << "No visible geometry was rendered.";
        return false;
    }
    window.makeCurrent();
    const GLenum error = window.context()->functions()->glGetError();
    window.doneCurrent();
    if (error != GL_NO_ERROR) {
        qCritical() << "OpenGL error:" << error;
        return false;
    }
    return true;
}

inline void checkKeys(QOpenGLWidget &window, QVector<int> keys, int index) {
    if (!checkFrame(window)) {
        QApplication::exit(1);
        return;
    }
    if (index == keys.size()) {
        const QStringList args = QApplication::arguments();
        const int capture = args.indexOf("--screenshot");
        if (capture >= 0 &&
            (capture + 1 == args.size() || !window.grabFramebuffer().save(args[capture + 1]))) {
            qCritical() << "Could not save the requested screenshot.";
            QApplication::exit(1);
            return;
        }
        qInfo() << "Rendering smoke test passed (initial frame, resize, controls).";
        QApplication::exit(0);
        return;
    }
    QKeyEvent event(QEvent::KeyPress, keys[index], Qt::NoModifier);
    QApplication::sendEvent(&window, &event);
    window.update();
    QTimer::singleShot(150, &window,
                       [&window, keys, index] { checkKeys(window, keys, index + 1); });
}
} // namespace smoke

inline void runSmokeTestIfRequested(QOpenGLWidget &window,
                                    std::initializer_list<int> controls = {}) {
    if (!QApplication::arguments().contains("--smoke-test")) {
        return;
    }
    const QVector<int> keys(controls.begin(), controls.end());
    QTimer::singleShot(300, &window, [&window, keys] {
        if (!smoke::checkFrame(window)) {
            QApplication::exit(1);
            return;
        }
        window.resize(640, 480);
        QTimer::singleShot(250, &window, [&window, keys] { smoke::checkKeys(window, keys, 0); });
    });
}

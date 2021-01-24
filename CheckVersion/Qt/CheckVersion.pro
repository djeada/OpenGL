QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CheckVersion
TEMPLATE = app


DEFINES += QT_DEPRECATED_WARNINGS

# Link to OpenGL library
LIBS +=  -lGLU -lGL -lGLEW

SOURCES += \
        main.cpp \

HEADERS +=

FORMS +=


#pragma once
#include "ui_OpenGLWidget.h"

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMouseEvent>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget* parent = Q_NULLPTR);

private:
    Ui::OpenGLWidget ui;

protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void paintGL();

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);


private:
    QOpenGLShaderProgram* mShaderProgram;
    QOpenGLBuffer mBuffer;
    QOpenGLTexture* textureY = nullptr, * textureU = nullptr, * textureV = nullptr;
    GLuint textureUniformY, textureUniformU, textureUniformV;
    GLuint idY, idU, idV;
    uchar* yuvPtr = nullptr;
};


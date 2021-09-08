#pragma once
#include "ui_OpenGLWidget.h"

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QAudioFormat>
#include <QAudioOutput>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget* parent = Q_NULLPTR);
    ~OpenGLWidget();

public slots:
    void upYuvDate(uchar* data, int32_t size);
public slots:
    void upPcmDate(uchar* data, int32_t size);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void paintGL();

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);


private:
    Ui::OpenGLWidget ui;

    QOpenGLShaderProgram* mGLShaderProgram;
    QOpenGLBuffer mGLBuffer;
    QOpenGLTexture* mGLTextureY = nullptr, * mGLTextureU = nullptr, * mGLTextureV = nullptr;
    GLuint textureUniformY, textureUniformU, textureUniformV;
    GLuint idY, idU, idV;
    uchar* yuvPtr = nullptr;
    int32_t videoW, videoH;


    QAudioFormat fmt;
    QAudioOutput* out;
    QIODevice* io;
};


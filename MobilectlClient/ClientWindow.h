#pragma once
#include "ui_ClientWindow.h"

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

#define GET_GLSTR(x) #x

static const char* vsrc = GET_GLSTR(
    attribute vec4 vertexIn;
    attribute vec2 textureIn;
    varying vec2 textureOut;
    void main(void)
    {
        gl_Position = vertexIn;
        textureOut = textureIn;
    }
);
static const char* fsrc = GET_GLSTR(
    varying vec2 textureOut;
    uniform sampler2D tex_y;
    uniform sampler2D tex_u;
    uniform sampler2D tex_v;
    void main(void)
    {
        vec3 yuv;
        vec3 rgb;
        yuv.x = texture2D(tex_y, textureOut).r;
        yuv.y = texture2D(tex_u, textureOut).r - 0.5;
        yuv.z = texture2D(tex_v, textureOut).r - 0.5;
        rgb = mat3(1, 1, 1, 0, -0.39465, 2.03211, 1.13983, -0.58060, 0) * yuv;
        gl_FragColor = vec4(rgb, 1);
    }
);

class ClientWindow : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    ClientWindow(QWidget* parent = Q_NULLPTR);
    ~ClientWindow();

public slots:
    void upYuvDate(uchar* data, int32_t size);
public slots:
    void upPcmDate(uchar* data, int32_t size);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void paintGL();
    virtual void closeEvent(QCloseEvent* event);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);


private:
    Ui::ClientWindow ui;

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


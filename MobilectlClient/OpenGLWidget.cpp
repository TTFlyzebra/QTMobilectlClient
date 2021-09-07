#include "OpenGLWidget.h"

#define GL_VERSION  "#version 330 core\n"
#define GET_GLSTR(x) GL_VERSION#x

const char* vsrc = GET_GLSTR(
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }
);

const char* fsrc = GET_GLSTR(
    out vec4 FragColor;
    in vec2 TexCoord;
    uniform sampler2D texY;
    uniform sampler2D texU;
    uniform sampler2D texV;
    void main()
    {
        vec3 yuv;
        vec3 rgb;
        yuv.x = texture(texY, TexCoord).r;
        yuv.y = texture(texU, TexCoord).r - 0.5;
        yuv.z = texture(texV, TexCoord).r - 0.5;
        rgb = mat3(1.0, 1.0, 1.0,
            0.0, -0.3455, 1.779,
            1.4075, -0.7169, 0.0) * yuv;
        FragColor = vec4(rgb, 1.0);
    }
);


OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    ui.setupUi(this);
}

void OpenGLWidget::initializeGL()
{
    qDebug() << __func__;
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.3f, 0.7f, 1.0f);
}


void OpenGLWidget::resizeGL(int width, int height)
{
    qDebug() <<__func__<< "width=" << width << ",height=" << height;
}


void OpenGLWidget::paintGL()
{
    qDebug() << __func__;
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    qDebug() << event << "----" << event->pos();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    qDebug() << event <<"----" << event->pos();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    qDebug() << event << "----" << event->pos();
}


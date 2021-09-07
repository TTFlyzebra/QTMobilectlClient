#include "OpenGLWidget.h"

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    ui.setupUi(this);
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.3f, 0.7f, 1.0f);
}


void OpenGLWidget::resizeGL(int width, int height)
{
    qDebug() << "resizeGL width=" << width << ",height=" << height;
}


void OpenGLWidget::paintGL()
{
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


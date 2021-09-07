#include "MobilectlClient.h"
#include "OpenGLWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLWidget w;
    w.setWindowTitle(QString::fromLocal8Bit("OpenGL²¥·Å"));
    w.show();
    return a.exec();
}

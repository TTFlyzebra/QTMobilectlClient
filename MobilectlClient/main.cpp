#include <QtWidgets/QApplication>

#include "MobilectlClient.h"
#include "OpenGLWidget.h"
#include "LoginDialog.h"
#include "FlyPlayer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenGLWidget gl;
    //FlyPlayer flyPlay("D:\\temp\\tttt.mp4");
    FlyPlayer flyPlay("rtsp://192.168.137.11/live");
    //LoginDialog dlg;
    //dlg.setWindowTitle(QString::fromLocal8Bit("µÇÂ½"));
    //dlg.show();
    //if (dlg.exec()== QDialog::Accepted) {
    //    gl.show();
    //    a.exec();
    //}
    gl.show();
    flyPlay.start();
    return a.exec();
}

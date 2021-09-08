#include <QApplication>

#include "MobilectlClient.h"
#include "OpenGLWidget.h"
#include "LoginDialog.h"
#include "FlyPlayer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);     
    OpenGLWidget gl;
    FlyPlayer flyPlay("D:\\Video\\test.mp4");
    QObject::connect(&flyPlay,SIGNAL(yuv_signal(uchar*, int32_t)),&gl,SLOT(updateYuv(uchar*, int32_t)),Qt::BlockingQueuedConnection);
    //FlyPlayer flyPlay("rtsp://192.168.137.11/live");
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

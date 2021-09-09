#include <QApplication>
#include <QDebug>

#include "MobilectlClient.h"
#include "ClientWindow.h"
#include "LoginDialog.h"
#include "VideoDecoder.h"

int main(int argc, char *argv[])
{
    qDebug("main start!");
    QApplication a(argc, argv);     
    ClientWindow client;
    //VideoDecoder vplay("D:\\Video\\test.mp4");
    VideoDecoder vplayer;
    QObject::connect(&vplayer, SIGNAL(yuv_signal(uchar*, int32_t)), &client, SLOT(upYuvDate(uchar*, int32_t)), Qt::BlockingQueuedConnection);
    QObject::connect(&vplayer, SIGNAL(pcm_signal(uchar*, int32_t)), &client, SLOT(upPcmDate(uchar*, int32_t)), Qt::BlockingQueuedConnection);
    //LoginDialog dlg;
    //dlg.setWindowTitle(QString::fromLocal8Bit("µÇÂ½"));
    //dlg.show();
    //if (dlg.exec()== QDialog::Accepted) {
    //    gl.show();
    //    a.exec();
    //}
    client.show();
    vplayer.play("rtsp://192.168.137.11/live");
    int32_t result = a.exec();
    QObject::disconnect(&vplayer, 0, 0, 0);
    vplayer.stop(); 
    vplayer.quit();
    qDebug("wait vplayer exit!");   
    vplayer.wait();
    qDebug("main exit!");
    return result;
}

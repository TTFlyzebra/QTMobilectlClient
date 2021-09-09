#include <QApplication>
#include <QDebug>

#include "MobilectlClient.h"
#include "ClientWindow.h"
#include "LoginDialog.h"
#include "VideoDecoder.h"
#include "Controller.h"

int main(int argc, char *argv[])
{
    qDebug("main start!");
    QApplication a(argc, argv);  
    //VideoDecoder vplay("D:\\Video\\test.mp4");
    Controller ctrller;
    ClientWindow client(&ctrller);
    VideoDecoder vplayer(&client);
    //LoginDialog dlg;
    //dlg.setWindowTitle(QString::fromLocal8Bit("µÇÂ½"));
    //dlg.show();
    //if (dlg.exec()== QDialog::Accepted) {
    //    client.show();
    //    vplayer.play("rtsp://192.168.137.11/live");
    //    int32_t result = a.exec();
    //    QObject::disconnect(&vplayer, 0, 0, 0);
    //    vplayer.stop();
    //    vplayer.quit();
    //    qDebug("wait vplayer exit!");
    //    vplayer.wait();
    //    qDebug("main exit!");
    //}
    //return 0;

    ctrller.connect("192.168.137.11");
    vplayer.play("192.168.137.11");

    client.show();

    //QObject::connect(&vplayer, SIGNAL(yuv_signal(uchar*, int32_t, int32_t, int32_t)), &client, SLOT(upYuvDate(uchar*, int32_t, int32_t, int32_t)), Qt::BlockingQueuedConnection);
    //QObject::connect(&vplayer, SIGNAL(pcm_signal(uchar*, int32_t)), &client, SLOT(upPcmDate(uchar*, int32_t)), Qt::BlockingQueuedConnection);

    int32_t result = a.exec();

    vplayer.stop(); 
    vplayer.quit();
    vplayer.wait();

    ctrller.disconnect();
    ctrller.quit();
    ctrller.wait();

    qDebug("main exit!");
    return result;
}

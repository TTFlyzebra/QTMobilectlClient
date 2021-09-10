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

    /*
    //VideoDecoder vplay("D:\\Video\\test.mp4");
    Controller mCtrl;

    ClientWindow mWindow;
    mWindow.setController(&mCtrl);

    VideoDecoder mPlay;
    mPlay.setClientWindow(&mWindow);

    //LoginDialog dlg;
    //dlg.setWindowTitle(QString::fromLocal8Bit("µÇÂ½"));
    //dlg.show();
    //if (dlg.exec()== QDialog::Accepted) {
    //    client.show();
    //    mPlay.play("rtsp://192.168.137.11/live");
    //    int32_t result = a.exec();
    //    QObject::disconnect(&mPlay, 0, 0, 0);
    //    mPlay.stop();
    //    mPlay.quit();
    //    qDebug("wait mPlay exit!");
    //    mPlay.wait();
    //    qDebug("main exit!");
    //}
    //return 0;

    mCtrl.connect("192.168.137.11");
    mPlay.play("192.168.137.11");

    mWindow.show();

    //QObject::connect(&mPlay, SIGNAL(yuv_signal(uchar*, int32_t, int32_t, int32_t)), &client, SLOT(upYuvDate(uchar*, int32_t, int32_t, int32_t)), Qt::BlockingQueuedConnection);
    //QObject::connect(&mPlay, SIGNAL(pcm_signal(uchar*, int32_t)), &client, SLOT(upPcmDate(uchar*, int32_t)), Qt::BlockingQueuedConnection);

    int32_t result = a.exec();

    mPlay.stop(); 
    mPlay.wait();

    mCtrl.disconnect();
    mCtrl.wait();
    */

    MobilectlClient m;
    m.show();
    int32_t result = a.exec();
    qDebug("main exit!");
    return result;
}

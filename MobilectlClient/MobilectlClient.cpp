#include "MobilectlClient.h"
#include <Qdebug>

#define client_ip_01  "172.30.16.234"
#define client_ip_02  "172.30.16.241"

MobilectlClient::MobilectlClient(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << __func__;
    this->setStyleSheet("background-color:blue;");
    ui.setupUi(this);
    mCtrl[0] = new Controller();
    ui.ZebraGLWidget_01->setController(mCtrl[0]);

    mPlay[0] = new VideoDecoder();
    mPlay[0]->setClientWindow(ui.ZebraGLWidget_01);

    mCtrl[0]->connect(client_ip_01);
    mPlay[0]->play(client_ip_01);

    mCtrl[1] = new Controller();
    ui.ZebraGLWidget_02->setController(mCtrl[1]);

    mPlay[1] = new VideoDecoder();
    mPlay[1]->setClientWindow(ui.ZebraGLWidget_02);

    mCtrl[1]->connect(client_ip_02);
    mPlay[1]->play(client_ip_02);
}

MobilectlClient::~MobilectlClient(){
    qDebug() << __func__;
    for (int i = 0; i < 2; i++) {
        mPlay[i]->stop();
        mPlay[i]->wait();

        mCtrl[i]->disconnect();
        mCtrl[i]->wait();

        delete mPlay[i];
        delete mCtrl[i];
    }
}
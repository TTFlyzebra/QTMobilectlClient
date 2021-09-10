#include "MobilectlClient.h"
#include <Qdebug>

MobilectlClient::MobilectlClient(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << __func__;
    ui.setupUi(this);
    mCtrl = new Controller();
    ui.ZebraGLWidget->setController(mCtrl);
    mPlay = new VideoDecoder();
    mPlay->setClientWindow(ui.ZebraGLWidget);

    mCtrl->connect("192.168.137.11");
    mPlay->play("192.168.137.11");
}

MobilectlClient::~MobilectlClient()
{
    qDebug() << __func__;

    mPlay->stop();
    mPlay->wait();

    mCtrl->disconnect();
    mCtrl->wait();

    delete mCtrl;
    delete mPlay;
}
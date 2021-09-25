#include "MobilectlClient.h"
#include <Qdebug>

#define client_ip_01  "172.30.16.234"
#define client_ip_02  "172.30.16.235"

#define client_ip_03  "172.30.16.236"
#define client_ip_04  "172.30.16.239"

#define client_ip_05  "172.30.16.240"
#define client_ip_06  "172.30.16.241"

#define client_ip_07  "172.30.16.243"
#define client_ip_08  "172.30.16.244"

#define client_ip_09  "172.30.16.245"
#define client_ip_10  "172.30.16.246"

MobilectlClient::MobilectlClient(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << __func__;
    //this->setStyleSheet("background-color:blue;");
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

    QObject::connect(ui.action1, SIGNAL(triggered()), this, SLOT(action1()));
    QObject::connect(ui.action2, SIGNAL(triggered()), this, SLOT(action2()));
    QObject::connect(ui.action3, SIGNAL(triggered()), this, SLOT(action3()));
    QObject::connect(ui.action4, SIGNAL(triggered()), this, SLOT(action4()));
    QObject::connect(ui.action5, SIGNAL(triggered()), this, SLOT(action5()));
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

void MobilectlClient::action1()
{
    for (int i = 0; i < 2; i++) {
        mPlay[i]->stop();
        mPlay[i]->wait();

        mCtrl[i]->disconnect();
        mCtrl[i]->wait();
    }

    mCtrl[0]->connect(client_ip_01);
    mPlay[0]->play(client_ip_01);
    mCtrl[1]->connect(client_ip_02);
    mPlay[1]->play(client_ip_02);
}


void MobilectlClient::action2()
{
    for (int i = 0; i < 2; i++) {
        mPlay[i]->stop();
        mPlay[i]->wait();

        mCtrl[i]->disconnect();
        mCtrl[i]->wait();
    }

    mCtrl[0]->connect(client_ip_03);
    mPlay[0]->play(client_ip_03);
    mCtrl[1]->connect(client_ip_04);
    mPlay[1]->play(client_ip_04);
}

void MobilectlClient::action3()
{
    for (int i = 0; i < 2; i++) {
        mPlay[i]->stop();
        mPlay[i]->wait();

        mCtrl[i]->disconnect();
        mCtrl[i]->wait();
    }

    mCtrl[0]->connect(client_ip_05);
    mPlay[0]->play(client_ip_05);
    mCtrl[1]->connect(client_ip_06);
    mPlay[1]->play(client_ip_06);
}

void MobilectlClient::action4()
{
    for (int i = 0; i < 2; i++) {
        mPlay[i]->stop();
        mPlay[i]->wait();

        mCtrl[i]->disconnect();
        mCtrl[i]->wait();
    }

    mCtrl[0]->connect(client_ip_07);
    mPlay[0]->play(client_ip_07);
    mCtrl[1]->connect(client_ip_08);
    mPlay[1]->play(client_ip_08);;
}

void MobilectlClient::action5()
{
    for (int i = 0; i < 2; i++) {
        mPlay[i]->stop();
        mPlay[i]->wait();

        mCtrl[i]->disconnect();
        mCtrl[i]->wait();
    }

    mCtrl[0]->connect(client_ip_09);
    mPlay[0]->play(client_ip_09);
    mCtrl[1]->connect(client_ip_10);
    mPlay[1]->play(client_ip_10);
}
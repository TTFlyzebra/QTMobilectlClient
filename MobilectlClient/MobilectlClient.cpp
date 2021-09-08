#include "MobilectlClient.h"
#include <Qdebug>

MobilectlClient::MobilectlClient(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

MobilectlClient::~MobilectlClient()
{
}

void MobilectlClient::on_login_accept_clicked()
{
    qDebug() << "on_login_accept_clicked:"<< ui.text_client_ip->text();
}

void MobilectlClient::on_login_cancel_clicked()
{
    qDebug() << "on_login_cancel_clicked:" << ui.text_client_ip->text();
}

#include "LoginDialog.h"
#include <Qdebug>

LoginDialog::LoginDialog(QDialog* parent)
    : QDialog(parent)
{
    qDebug() << __func__;
    ui.setupUi(this);
}
LoginDialog::~LoginDialog()
{
    qDebug() << __func__;
}

void LoginDialog::on_okButton_clicked()
{
    qDebug() << "on_okButton_clicked:";
    accept();
}

void LoginDialog::on_cancelButton_clicked()
{
    qDebug() << "on_cancelButton_clicked:";
}

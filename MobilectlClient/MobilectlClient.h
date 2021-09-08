#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MobilectlClient.h"

class MobilectlClient : public QMainWindow
{
    Q_OBJECT

public:
    MobilectlClient(QWidget *parent = Q_NULLPTR);
    ~MobilectlClient();


public slots:
    void on_login_accept_clicked();
    void on_login_cancel_clicked();

private:
    Ui::MobilectlClientClass ui;
};

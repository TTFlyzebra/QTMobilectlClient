#pragma once

#include <QDialog>
#include "ui_LoginDialog.h"

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(QDialog* parent = Q_NULLPTR);
    ~LoginDialog();


public slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::LoginDialog ui;
};



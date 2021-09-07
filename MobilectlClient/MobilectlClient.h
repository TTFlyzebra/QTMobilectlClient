#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MobilectlClient.h"

class MobilectlClient : public QMainWindow
{
    Q_OBJECT

public:
    MobilectlClient(QWidget *parent = Q_NULLPTR);

private:
    Ui::MobilectlClientClass ui;
};

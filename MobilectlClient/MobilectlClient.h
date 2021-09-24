#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MobilectlClient.h"
#include "VideoDecoder.h"

class MobilectlClient : public QMainWindow
{
    Q_OBJECT

public:
    MobilectlClient(QWidget *parent = Q_NULLPTR);
    ~MobilectlClient();

private:
    Ui::MobilectlClientClass ui;

    Controller *mCtrl[2];
    VideoDecoder *mPlay[2];
};

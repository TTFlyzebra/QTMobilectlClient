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

public slots:
    void action1();
public slots:
    void action2();
public slots:
    void action3();
public slots:
    void action4();
public slots:
    void action5();

private:
    Ui::MobilectlClientClass ui;

    Controller *mCtrl[2];
    VideoDecoder *mPlay[2];
};

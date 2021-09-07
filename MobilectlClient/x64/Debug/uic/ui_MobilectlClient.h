/********************************************************************************
** Form generated from reading UI file 'MobilectlClient.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOBILECTLCLIENT_H
#define UI_MOBILECTLCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MobilectlClientClass
{
public:
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MobilectlClientClass)
    {
        if (MobilectlClientClass->objectName().isEmpty())
            MobilectlClientClass->setObjectName(QString::fromUtf8("MobilectlClientClass"));
        MobilectlClientClass->resize(640, 360);
        centralWidget = new QWidget(MobilectlClientClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setEnabled(true);
        MobilectlClientClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MobilectlClientClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 640, 23));
        MobilectlClientClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MobilectlClientClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MobilectlClientClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MobilectlClientClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MobilectlClientClass->setStatusBar(statusBar);

        retranslateUi(MobilectlClientClass);

        QMetaObject::connectSlotsByName(MobilectlClientClass);
    } // setupUi

    void retranslateUi(QMainWindow *MobilectlClientClass)
    {
        MobilectlClientClass->setWindowTitle(QCoreApplication::translate("MobilectlClientClass", "MobilectlClient", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MobilectlClientClass: public Ui_MobilectlClientClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOBILECTLCLIENT_H

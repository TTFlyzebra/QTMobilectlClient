/********************************************************************************
** Form generated from reading UI file 'OpenGLWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENGLWIDGET_H
#define UI_OPENGLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OpenGLWidget
{
public:

    void setupUi(QWidget *OpenGLWidget)
    {
        if (OpenGLWidget->objectName().isEmpty())
            OpenGLWidget->setObjectName(QString::fromUtf8("OpenGLWidget"));
        OpenGLWidget->resize(450, 800);

        retranslateUi(OpenGLWidget);

        QMetaObject::connectSlotsByName(OpenGLWidget);
    } // setupUi

    void retranslateUi(QWidget *OpenGLWidget)
    {
        OpenGLWidget->setWindowTitle(QCoreApplication::translate("OpenGLWidget", "MobilectlClient", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OpenGLWidget: public Ui_OpenGLWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENGLWIDGET_H

/********************************************************************************
** Form generated from reading UI file 'colourdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COLOURDIALOG_H
#define UI_COLOURDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_colourdialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_4;
    QLabel *label_2;
    QLabel *label_3;
    QSpinBox *spinBox;
    QSpinBox *spinBox_2;
    QSpinBox *spinBox_3;
    QLabel *label_5;

    void setupUi(QDialog *colourdialog)
    {
        if (colourdialog->objectName().isEmpty())
            colourdialog->setObjectName("colourdialog");
        colourdialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(colourdialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);
        label_4 = new QLabel(colourdialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(130, 150, 71, 20));
        label_2 = new QLabel(colourdialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(138, 90, 61, 20));
        label_3 = new QLabel(colourdialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(130, 120, 71, 20));
        spinBox = new QSpinBox(colourdialog);
        spinBox->setObjectName("spinBox");
        spinBox->setGeometry(QRect(210, 90, 42, 25));
        spinBox->setMaximum(255);
        spinBox_2 = new QSpinBox(colourdialog);
        spinBox_2->setObjectName("spinBox_2");
        spinBox_2->setGeometry(QRect(210, 120, 42, 25));
        spinBox_2->setMaximum(255);
        spinBox_3 = new QSpinBox(colourdialog);
        spinBox_3->setObjectName("spinBox_3");
        spinBox_3->setGeometry(QRect(210, 150, 42, 25));
        spinBox_3->setMaximum(255);
        label_5 = new QLabel(colourdialog);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(110, 180, 211, 20));

        retranslateUi(colourdialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, colourdialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, colourdialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(colourdialog);
    } // setupUi

    void retranslateUi(QDialog *colourdialog)
    {
        colourdialog->setWindowTitle(QCoreApplication::translate("colourdialog", "Dialog", nullptr));
        label_4->setText(QCoreApplication::translate("colourdialog", "Blue value:", nullptr));
        label_2->setText(QCoreApplication::translate("colourdialog", "Red value:", nullptr));
        label_3->setText(QCoreApplication::translate("colourdialog", "Green value:", nullptr));
        label_5->setText(QCoreApplication::translate("colourdialog", "(Values must be between 0 and 255)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class colourdialog: public Ui_colourdialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLOURDIALOG_H

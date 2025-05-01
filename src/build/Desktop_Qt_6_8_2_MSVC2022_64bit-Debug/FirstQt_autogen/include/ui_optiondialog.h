/********************************************************************************
** Form generated from reading UI file 'optiondialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPTIONDIALOG_H
#define UI_OPTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_OptionDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEdit;
    QLineEdit *lineEdit_2;
    QCheckBox *checkBox;
    QLineEdit *lineEdit_3;
    QLineEdit *lineEdit_4;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;

    void setupUi(QDialog *OptionDialog)
    {
        if (OptionDialog->objectName().isEmpty())
            OptionDialog->setObjectName("OptionDialog");
        OptionDialog->resize(388, 285);
        buttonBox = new QDialogButtonBox(OptionDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);
        lineEdit = new QLineEdit(OptionDialog);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(130, 50, 113, 24));
        lineEdit_2 = new QLineEdit(OptionDialog);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setGeometry(QRect(130, 80, 41, 24));
        checkBox = new QCheckBox(OptionDialog);
        checkBox->setObjectName("checkBox");
        checkBox->setGeometry(QRect(130, 170, 77, 22));
        lineEdit_3 = new QLineEdit(OptionDialog);
        lineEdit_3->setObjectName("lineEdit_3");
        lineEdit_3->setGeometry(QRect(130, 110, 41, 24));
        lineEdit_4 = new QLineEdit(OptionDialog);
        lineEdit_4->setObjectName("lineEdit_4");
        lineEdit_4->setGeometry(QRect(130, 140, 41, 24));
        label = new QLabel(OptionDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(80, 50, 49, 16));
        label_2 = new QLabel(OptionDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(58, 80, 61, 20));
        label_3 = new QLabel(OptionDialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(50, 110, 71, 20));
        label_4 = new QLabel(OptionDialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(50, 140, 71, 20));

        retranslateUi(OptionDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, OptionDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, OptionDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(OptionDialog);
    } // setupUi

    void retranslateUi(QDialog *OptionDialog)
    {
        OptionDialog->setWindowTitle(QCoreApplication::translate("OptionDialog", "Dialog", nullptr));
        checkBox->setText(QCoreApplication::translate("OptionDialog", "Visible?", nullptr));
        label->setText(QCoreApplication::translate("OptionDialog", "Name:", nullptr));
        label_2->setText(QCoreApplication::translate("OptionDialog", "Red value:", nullptr));
        label_3->setText(QCoreApplication::translate("OptionDialog", "Green value:", nullptr));
        label_4->setText(QCoreApplication::translate("OptionDialog", "Blue value:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OptionDialog: public Ui_OptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTIONDIALOG_H

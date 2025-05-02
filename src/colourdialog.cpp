#include "colourdialog.h"
#include "ui_colourdialog.h"

colourdialog::colourdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::colourdialog)
{
    ui->setupUi(this);

}

colourdialog::~colourdialog()
{
    delete ui;
}


QColor colourdialog::selectedColour() const
{
    int red = ui->spinBox->value();
    int green = ui->spinBox_2->value();
    int blue = ui->spinBox_3->value();
    return QColor(red, green, blue);
}

/**
 * @class colourdialog
 * @brief A modal dialog for selecting an RGB color to apply to CAD model parts.
 *
 * This dialog allows users to input red, green, and blue values via spin boxes.
 * The selected color is returned as a QColor object.
 */



#include "colourdialog.h"
#include "ui_colourdialog.h"

/**
 * @brief Constructs a new ColourDialog.
 * @param parent Optional parent widget.
 */


colourdialog::colourdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::colourdialog)
{
    ui->setupUi(this);

}


/**
 * @brief Destroys the ColourDialog and frees associated UI resources.
 */

colourdialog::~colourdialog()
{
    delete ui;
}

/**
 * @brief Gets the currently selected color from the dialog.
 * @return QColor object representing the selected RGB value.
 */

QColor colourdialog::selectedColour() const
{
    int red = ui->spinBox->value();
    int green = ui->spinBox_2->value();
    int blue = ui->spinBox_3->value();
    return QColor(red, green, blue);
}

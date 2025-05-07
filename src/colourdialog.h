/**
 * @class colourdialog
 * @brief A modal dialog for selecting an RGB color to apply to CAD model parts.
 *
 * This dialog allows users to input red, green, and blue values via spin boxes.
 * The selected color is returned as a QColor object.
 */

#ifndef COLOURDIALOG_H
#define COLOURDIALOG_H

#include <QDialog>

namespace Ui {
class colourdialog;
}
/**
 * @class colourdialog
 * @brief A modal dialog for selecting an RGB color to apply to CAD model parts.
 *
 * This dialog allows users to input red, green, and blue values via spin boxes.
 * It is used when changing the color of a model part in the GUI.
 *
 * Example usage:
 * @code
 * colourdialog dialog(this);
 * if (dialog.exec() == QDialog::Accepted) {
 *     QColor chosenColor = dialog.selectedColour();
 *     // Apply chosenColor to model part
 * }
 * @endcode
 */
class colourdialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new colourdialog.
     * @param parent Optional parent widget.
     */
    explicit colourdialog(QWidget *parent = nullptr);
    /**
     * @brief Destroys the colourdialog and frees associated UI resources.
     */
    ~colourdialog();

    /**
     * @brief Gets the currently selected color from the dialog.
     * @return QColor object representing the selected RGB value.
     */

    QColor selectedColour() const;

private:
    Ui::colourdialog *ui; /**< Pointer to the UI elements defined in the .ui file */
};

#endif // COLOURDIALOG_H

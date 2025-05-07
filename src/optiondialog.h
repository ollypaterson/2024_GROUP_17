#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>

namespace Ui {
class OptionDialog;
}

/**
 * @class OptionDialog
 * @brief Dialog for displaying and interacting with options.
 *
 * This dialog provides a user interface for configuring various options.
 */
class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for the OptionDialog class.
     * @param parent The parent widget.
     */
    explicit OptionDialog(QWidget *parent = nullptr);

    /**
     * @brief Destructor for the OptionDialog class.
     */
    ~OptionDialog();

private:
    Ui::OptionDialog *ui;  ///< Pointer to the user interface object.
};

#endif // OPTIONDIALOG_H

/**
 * @file optiondialog.cpp
 * @brief Implementation of the OptionDialog class.
 *
 * This file contains the implementation of the OptionDialog class, which is a dialog
 * for displaying and interacting with options.
 */

#include "optiondialog.h"
#include "ui_optiondialog.h"

/**
 * @brief OptionDialog::OptionDialog
 * @param parent The parent widget.
 *
 * Constructor for the OptionDialog class.  It sets up the user interface.
 */
OptionDialog::OptionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OptionDialog)
{
    ui->setupUi(this);
}

/**
 * @brief OptionDialog::~OptionDialog
 *
 * Destructor for the OptionDialog class.  It cleans up the user interface object.
 */
OptionDialog::~OptionDialog()
{
    delete ui;
}


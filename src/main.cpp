/**
 * @file main.cpp
 * @brief Entry point of the CAD Viewer Application.
 *
 * This file contains the main function that initializes and runs the Qt-based GUI application.
 * It creates an instance of the MainWindow class and starts the Qt event loop.
 */


#include "mainwindow.h"

#include <QApplication>

/**
 * @brief The main function – entry point of the application.
 *
 * Initializes the Qt application and displays the main window.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return Exit code returned by QApplication::exec()
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    // Start the Qt event loop
    return a.exec();
}

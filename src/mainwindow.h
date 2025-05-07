#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ModelPart.h"      ///< Custom header for the ModelPart class
#include "ModelPartList.h"  ///< Custom header for the ModelPartList class
#include "VRRenderThread.h"   ///< Custom header for the VRRenderThread class

#include <QMainWindow>      ///< Qt class for the main window

// Forward declarations to avoid including OpenGL-heavy VTK headers in the header file
class vtkLight;
class vtkRenderer;
class vtkGenericOpenGLRenderWindow;
class QVTKOpenGLNativeWidget;
template <typename T> class vtkSmartPointer;


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
  * @brief The MainWindow class provides the main window for the CAD Viewer application.
  *
  * It handles the user interface, interactions, and visualization of 3D models.
  */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
      * @brief Constructor for the MainWindow class.
      * @param parent The parent widget (default nullptr).
      */
    MainWindow(QWidget *parent = nullptr);

    /**
      * @brief Destructor for the MainWindow class.
      */
    ~MainWindow();

public slots:
    /**
      * @brief Handles a button click event (currently resets the model view).
      */
    void handleButton();

    /**
      * @brief Handles a click event in the tree view.
      */
    void handleTreeClicked();

    /**
      * @brief Handles a click event on the second button.
      */
    void handleSecondButton();

signals:
    /**
      * @brief Signal emitted to update the status bar with a message.
      * @param message The message to display.
      * @param timeout The timeout for the message display (in milliseconds).
      */
    void statusUpdateMessage(const QString& message, int timeout);

private slots:
    /**
      * @brief Opens an STL file, loads the model, and adds it to the scene.
      */
    void on_actionOpen_FIle_triggered();

    /**
      * @brief Opens the item options dialog.
      */
    void on_actionItemOptions_triggered();

    /**
      * @brief Saves the current scene to an STL file.
      */
    void on_actionSave_File_triggered();

    /**
      * @brief Displays a help message.
      */
    void on_actionHelp_triggered();

    /**
      * @brief Handles the print action (currently not implemented).
      */
    void on_actionPrint_triggered();

    /**
      * @brief Handles context menu requests for the tree view.
      * @param pos The position of the mouse click.
      */
    void on_treeViewContextMenu(const QPoint &pos);

    /**
      * @brief Handles the color change request.
      */
    void on_colourButton_triggered();

    /**
      * @brief Resets the view of the 3D scene.
      */
    void on_resetView_triggered();

    /**
      * @brief Handles changes to the light intensity slider.
      * @param value The new value of the slider.
      */
    void on_lightSlider_valueChanged(int value);

    /**
      * @brief Updates the rendering of the scene.
      */
    void updateRender();

    /**
      * @brief Recursively updates the scene from the model tree.
      * @param index The index of the current item in the tree.
      */
    void updateRenderFromTree(const QModelIndex& index);
    //void onStartVRButtonClicked();

private:
    Ui::MainWindow *ui;                                     ///< Pointer to the user interface object.
    ModelPartList* partList;                                 ///< List of model parts in the scene.
    vtkSmartPointer<vtkLight> sceneLight;                   ///< Smart pointer to the scene's light.
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;  ///< Smart pointer to the render window.
    vtkSmartPointer<vtkRenderer> renderer;                   ///< Smart pointer to the renderer.
    //VRRenderThread* vrThread;
};

#endif // MAINWINDOW_H


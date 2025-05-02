#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ModelPart.h"
#include "ModelPartList.h"
#include <vtkSmartPointer.h>
#include <vtkLight.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <QVTKOpenGLNativeWidget.h>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
} // namespace Ui
QT_END_NAMESPACE

// Include VRRenderThread.h manually (added)
#include "VRRenderThread.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void handleButton();
    void handleTreeClicked();
    void handleSecondButton();

signals:
    void statusUpdateMessage(const QString& message, int timeout);

private slots:
    void on_actionOpen_FIle_triggered();
    void on_actionItemOptions_triggered();
    void on_actionSave_File_triggered();
    void on_actionHelp_triggered();
    void on_actionPrint_triggered();
    void on_treeViewContextMenu(const QPoint &pos);
    void on_colourButton_triggered();
    void on_resetView_triggered();
    void on_lightSlider_valueChanged(int value);
    void updateRender();
    void updateRenderFromTree(const QModelIndex& index);
    void onStartVRButtonClicked(); // 🔄 Added for VR start

private:
    Ui::MainWindow *ui;
    ModelPartList* partList;
    vtkSmartPointer<vtkLight> sceneLight;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> renderer;
    VRRenderThread* vrThread; // 🔄 Added for VR thread
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ModelPart.h"
#include "ModelPartList.h"
#include <vtkSmartPointer.h>
#include <vtkLight.h>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {  
class MainWindow;
}
QT_END_NAMESPACE

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

private:
    Ui::MainWindow *ui;
    ModelPartList* partList;
    vtkSmartPointer<vtkLight> sceneLight;
};
#endif // MAINWINDOW_H

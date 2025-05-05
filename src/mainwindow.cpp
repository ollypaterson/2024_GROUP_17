#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "VRRenderThread.h"




// VTK includes moved from .h to .cpp
#include <vtkCamera.h>
#include <vtkSmartPointer.h>
#include <vtkLight.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRendererCollection.h>
#include <vtkLightCollection.h>



#include <QVTKOpenGLNativeWidget.h>

// Qt includes
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QStandardItemModel>
#include <QColorDialog>

#include "optiondialog.h"


MainWindow::MainWindow(QWidget* parent)

    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::on_treeViewContextMenu);
    connect(ui->pushButton, &QPushButton::released, this, &MainWindow::on_resetView_triggered);
    connect(this, &MainWindow::statusUpdateMessage, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    connect(ui->pushButton_2, &QPushButton::released, this, &MainWindow::on_colourButton_triggered);
    connect(ui->lightSlider, &QSlider::valueChanged, this, &MainWindow::on_lightSlider_valueChanged);
   // connect(ui->startVRButton, &QPushButton::cliscked, this, &MainWindow::onStartVRButtonClicked);


    //initalizing vtk
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    ui->vtkWidget->setRenderWindow(renderWindow);

    renderer = vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkLight> lightTop = vtkSmartPointer<vtkLight>::New();
    lightTop->SetLightTypeToSceneLight();
    lightTop->SetPosition(0, 0, 500);  // above
    lightTop->SetFocalPoint(0, 0, 0);
    lightTop->SetDiffuseColor(1.0, 1.0, 1.0);
    lightTop->SetAmbientColor(0.4, 0.4, 0.4);
    lightTop->SetSpecularColor(1.0, 1.0, 1.0);
    lightTop->SetIntensity(0.8);
    renderer->AddLight(lightTop);

    vtkSmartPointer<vtkLight> lightBottom = vtkSmartPointer<vtkLight>::New();
    lightBottom->SetLightTypeToSceneLight();
    lightBottom->SetPosition(0, 0, -500);
    lightBottom->SetFocalPoint(0, 0, 0);
    lightBottom->SetDiffuseColor(1.0, 1.0, 1.0);
    lightBottom->SetIntensity(0.4);
    renderer->AddLight(lightBottom);


    vtkSmartPointer<vtkLight> lightLeft = vtkSmartPointer<vtkLight>::New();
    lightLeft->SetLightTypeToSceneLight();
    lightLeft->SetPosition(-500, 0, 0);
    lightLeft->SetFocalPoint(0, 0, 0);
    lightLeft->SetDiffuseColor(1.0, 1.0, 1.0);
    lightLeft->SetIntensity(0.5);
    renderer->AddLight(lightLeft);


    vtkSmartPointer<vtkLight> lightRight = vtkSmartPointer<vtkLight>::New();
    lightRight->SetLightTypeToSceneLight();
    lightRight->SetPosition(500, 0, 0);
    lightRight->SetFocalPoint(0, 0, 0);
    lightRight->SetDiffuseColor(1.0, 1.0, 1.0);
    lightRight->SetIntensity(0.5);
    renderer->AddLight(lightRight);

    renderer->SetAmbient(0.2, 0.2, 0.2);

    renderWindow->AddRenderer(renderer);


    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(8);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(cylinder->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1.0, 0.0, 0.35);
    renderer->AddActor(actor);
    renderer->ResetCamera();





    ui->treeView->addAction(ui->actionItemOptions);

    /*Create / allocate the model list */
    this->partList = new ModelPartList("PartsList");

    /*Link it to the treeview in the GUI*/
    ui->treeView->setModel(this->partList);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    /* Manually create a model tree - there a much better and more flexible ways of doing this, e.g. with
    nested function. this is just a quick example and a start point*/
    ModelPart* rootItem = this->partList->getRootItem();


    /*Add 3 top level items*/
    for (int i = 0; i < 3; i++) {

        QString name = QString("TopLevel %1").arg(i);
        QString visible("true");
        QString shrink = "false";
        QString clip = "false";


        //Create child item
        ModelPart* childItem = new ModelPart({ name, visible, shrink, clip });

        //Append to tree top level
        rootItem->appendChild(childItem);

        /*
        //Add 5 sub items
        for (int j = 0; j < 5; j++) {

            QString name = QString("Item %1,%2").arg(i).arg(j);
            QString visible("true");
            QString shrink = "false";
            QString clip = "false";


            ModelPart* childChildItem = new ModelPart({ name, visible, shrink, clip });

            //Append to parent
            childItem->appendChild(childChildItem);
        }
*/



    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleButton() {
    emit statusUpdateMessage(QString("Model view reset"), 0);
}

void MainWindow::handleSecondButton() {
    OptionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        emit statusUpdateMessage(QString("Dialog accepted "), 0);
    }
    else {
        emit statusUpdateMessage(QString("Dialog rejected "), 0);

    }
}


void MainWindow::handleTreeClicked() {
    QModelIndex index = ui->treeView->currentIndex();
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    QString text = selectedPart->data(0).toString();
    emit statusUpdateMessage(QString("The selected item is: ") + text, 0);

}

void MainWindow::on_actionOpen_FIle_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open STL File", "", "STL Files (*.stl)");
    if (fileName.isEmpty()) return;

    QFileInfo fileInfo(fileName);
    QString partName = fileInfo.fileName();

    QModelIndex selectedIndex = ui->treeView->currentIndex();
    if (!selectedIndex.isValid()) {
        QMessageBox::warning(this, "No Selection", "Please select a parent item in the tree.");
        return;
    }

    QList<QVariant> data = { partName, "true", "false", "false" };
    QModelIndex newItemIndex = partList->appendChild(selectedIndex, data);

    ModelPart* part = static_cast<ModelPart*>(newItemIndex.internalPointer());
    part->loadSTL(fileName);

    if (part->getActor()) {
        renderer->AddActor(part->getActor());

        renderer->ResetCamera();
        renderer->ResetCameraClippingRange();
        renderer->GetRenderWindow()->Render();
        ui->vtkWidget->renderWindow()->Render();
    } else {
        return;
    }

    renderer->SetBackground(0.8, 0.8, 0.8);
    updateRender();
}




void MainWindow::on_actionSave_File_triggered()
{
    emit statusUpdateMessage( QString("Save file action triggered"), 0);
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", "STL Files (*.stl)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "This would be the saved STL data.\n";
            file.close();
            statusBar()->showMessage("File saved: " + fileName);
        } else {
            QMessageBox::warning(this, "Save Error", "Could not save file.");
        }
    }
}


void MainWindow::on_actionHelp_triggered()
{
    emit statusUpdateMessage( QString("Help action triggered"), 0);
    QMessageBox::about(this, "About", "Interactive formula student car program\n \nGroup 17's EEEE2076 Project");
}

void MainWindow::on_actionPrint_triggered()
{
    emit statusUpdateMessage( QString("Print action triggered"), 0);
    QMessageBox::information(this, "Print", "No print function implemented yet");
}


void MainWindow::on_actionItemOptions_triggered() {
    OptionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        emit statusUpdateMessage(QString("Dialog accepted "), 0);
    }
    else {
        emit statusUpdateMessage(QString("Dialog rejected "), 0);

    }
}


void MainWindow::on_treeViewContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    QMenu contextMenu(this);

    if (index.isValid()) {
        ModelPart* item = static_cast<ModelPart*>(index.internalPointer());
        if (!item) return;

        QString partName = item->data(0).toString();  // Column 0 = name

        QAction* changeColour = contextMenu.addAction("Change colour");

        QAction* clipFilter = contextMenu.addAction("Toggle clip filter");
        clipFilter->setCheckable(true);
        QString clipState = index.siblingAtColumn(3).data().toString();
        bool isClipped = (clipState == "true");
        clipFilter->setChecked(isClipped);

        QAction* shrinkFilter = contextMenu.addAction("Toggle shrink filter");
        shrinkFilter->setCheckable(true);
        QString shrinkState = index.siblingAtColumn(2).data().toString();
        bool isShrinked = (shrinkState == "true");
        shrinkFilter->setChecked(isShrinked);

        QAction* toggleVisibility = contextMenu.addAction("Toggle visibility");
        toggleVisibility->setCheckable(true);
        QString visibilityState = index.siblingAtColumn(1).data().toString();
        bool isVisible = (visibilityState == "true");
        toggleVisibility->setChecked(isVisible);


        QAction* renamePart = contextMenu.addAction("Rename part");

        QAction* selectedAction = contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
        if (!selectedAction) return;

        if (selectedAction == changeColour) {
            QColor chosenColour = QColorDialog::getColor(Qt::yellow, this, "Select Colour");
            if (!chosenColour.isValid()) return;

            item->setColour(chosenColour.red(), chosenColour.green(), chosenColour.blue());

            ui->vtkWidget->renderWindow()->Render();
            statusBar()->showMessage("Changed colour of: " + partName + " to " + chosenColour.name());



        }  else if (selectedAction == clipFilter) {
        bool enabled = clipFilter->isChecked();
        item->set(3, enabled ? "true" : "false");
        item->applyClip(enabled);
        ui->treeView->update(index);
        ui->vtkWidget->renderWindow()->Render();
        statusBar()->showMessage(QString("Clip filter %1 on: %2").arg(enabled ? "enabled" : "disabled", partName));




    } else if (selectedAction == shrinkFilter) {
        bool enabled = shrinkFilter->isChecked();
        item->set(2, enabled ? "true" : "false");
        item->applyShrink(enabled);
        ui->treeView->update(index);
        ui->vtkWidget->renderWindow()->Render();
        statusBar()->showMessage(QString("Shrink filter %1 on: %2").arg(enabled ? "enabled" : "disabled", partName));






        } else if (selectedAction == toggleVisibility) {
            bool newVisible = !item->visible();
            item->propagateVisibility(newVisible);

            ui->treeView->update(index);
            ui->vtkWidget->renderWindow()->Render();

            statusBar()->showMessage(QString("Toggled visibility: now %1").arg(newVisible ? "visible" : "hidden"));





        } else if (selectedAction == renamePart) {
            bool ok;
            QString newName = QInputDialog::getText(this, "Rename Part", "New name:", QLineEdit::Normal, partName, &ok);
            if (ok && !newName.isEmpty()) {
                item->setData(0, newName);
                ui->treeView->update(index);
                statusBar()->showMessage("Renamed part to: " + newName);
            }
        }

    } else {
        contextMenu.addAction("Expand All", ui->treeView, SLOT(expandAll()));
        contextMenu.addAction("Collapse All", ui->treeView, SLOT(collapseAll()));
        contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
    }
}




void MainWindow::on_colourButton_triggered() {
    QColor chosenColour = QColorDialog::getColor(Qt::yellow, this, "Select Colour");

    if (!chosenColour.isValid()) {
        statusBar()->showMessage("Colour selection cancelled");
        return;
    }

    ModelPart* root = partList->getRootItem();
    root->propagateColour(chosenColour.red(), chosenColour.green(), chosenColour.blue());

    ui->vtkWidget->renderWindow()->Render();
    statusBar()->showMessage("Changed colour of entire model to " + chosenColour.name());
}



void MainWindow::on_resetView_triggered() {
    statusBar()->showMessage("View reset");

    vtkCamera* camera = renderer->GetActiveCamera();

    camera->SetPosition(70.8764, 0, 597.675);
    camera->SetFocalPoint(70.8764, 0, 34.8563);
    camera->SetViewUp(0, 1, 0);

    renderer->ResetCameraClippingRange();
    ui->vtkWidget->renderWindow()->Render();
}




void MainWindow::on_lightSlider_valueChanged(int value) {
    double intensity = static_cast<double>(value) / 100.0;

    vtkLightCollection* lights = renderer->GetLights();
    lights->InitTraversal();

    vtkLight* light;
    while ((light = lights->GetNextItem())) {
        light->SetIntensity(intensity);
    }

    renderer->Render();
    ui->vtkWidget->renderWindow()->Render();

    statusBar()->showMessage(QString("Light intensity set to %1%").arg(value));
}



void MainWindow::updateRender() {
    renderer->RemoveAllViewProps();
    updateRenderFromTree(QModelIndex());  // start from invisible root
    renderer->ResetCamera();
    ui->vtkWidget->renderWindow()->Render();
}



void MainWindow::updateRenderFromTree(const QModelIndex& index) {
    ModelPart* part;

    if (!index.isValid()) {
        part = partList->getRootItem();  // top of the model
    } else {
        part = static_cast<ModelPart*>(index.internalPointer());
    }

    if (part && part->getActor()) {
        qDebug() << "Rendering part:" << part->data(0).toString();
        renderer->AddActor(part->getActor());
    }

    int rows = partList->rowCount(index);
    for (int i = 0; i < rows; ++i) {
        updateRenderFromTree(partList->index(i, 0, index));
    }
}





/*
// Start VR rendering thread
void MainWindow::onStartVRButtonClicked()
{
    if (!vrThread) {
        vrThread = new VRRenderThread(this);
        vrThread->start();
        emit statusUpdateMessage("VR rendering started.", 0);
    } else {
        QMessageBox::information(this, "Info", "VR is already running.");
    }
}

*/

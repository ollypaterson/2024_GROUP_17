#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QStandardItemModel>
#include "optiondialog.h"
#include "colourdialog.h"
#include <QVTKOpenGLNativeWidget.h>
#include <vtkCamera.h>

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


    //initalizing vtk
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    ui->vtkWidget->setRenderWindow(renderWindow);

    renderer = vtkSmartPointer<vtkRenderer>::New();
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


    /*
    sceneLight = vtkSmartPointer<vtkLight>::New();
    sceneLight->SetLightTypeToSceneLight();
    sceneLight->SetPosition(5, 5, 15);
    sceneLight->SetPositional(true);
    sceneLight->SetConeAngle(10);
    sceneLight->SetFocalPoint(0, 0, 0);
    sceneLight->SetDiffuseColor(1.0, 1.0, 1.0);
    sceneLight->SetAmbientColor(1.0, 1.0, 1.0);
    sceneLight->SetSpecularColor(1.0, 1.0, 1.0);
    sceneLight->SetIntensity(0.5);  // Default

    ui->vtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddLight(sceneLight);
    */


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

    // Create a new part and load STL
    QModelIndex root = QModelIndex();
    QList<QVariant> data = { partName, "true", "false", "false" };
    QModelIndex newItemIndex = partList->appendChild(root, data);

    ModelPart* part = static_cast<ModelPart*>(newItemIndex.internalPointer());
    part->loadSTL(fileName);




    if (part->getActor()) {
        renderer->AddActor(part->getActor());
        double bounds[6];
        part->getActor()->GetBounds(bounds);
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
        QString visibilityState = index.siblingAtColumn(1).data().toString();  // Column 1 = Visible?
        bool isVisible = (visibilityState == "true");
        toggleVisibility->setChecked(isVisible);


        QAction* renamePart = contextMenu.addAction("Rename part");

        QAction* selectedAction = contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
        if (!selectedAction) return;

        if (selectedAction == changeColour) {
            statusBar()->showMessage("Change colour on: " + partName);

        } else if (selectedAction == clipFilter) {
            bool enabled = clipFilter->isChecked();

            QModelIndex clipIndex = index.siblingAtColumn(3);
            ui->treeView->model()->setData(clipIndex, enabled ? "true" : "false", Qt::EditRole);
            ui->treeView->update(clipIndex);

            statusBar()->showMessage("Clip filter " + QString(enabled ? "enabled" : "disabled") + " on: " + partName);

        } else if (selectedAction == shrinkFilter) {
            bool enabled = shrinkFilter->isChecked();

            QModelIndex shrinkIndex = index.siblingAtColumn(2);
            ui->treeView->model()->setData(shrinkIndex, enabled ? "true" : "false", Qt::EditRole);
            ui->treeView->update(shrinkIndex);
            statusBar()->showMessage("Shrink filter " + QString(enabled ? "enabled" : "disabled") + " on: " + partName);


        } else if (selectedAction == toggleVisibility) {
            QString visibility = item->data(1).toString();
            bool isVisible = (visibility != "false");
            item->setData(1, isVisible ? "false" : "true");

            ui->treeView->update(index);
            statusBar()->showMessage(QString("Toggled visibility: now %1").arg(isVisible ? "hidden" : "visible"));

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

    colourdialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        QColor colour = dialog.selectedColour();
        statusBar()-> showMessage("Selected colour: " + colour.name());

    } else {
        statusBar()-> showMessage("Colour selection cancelled");
    }
}



void MainWindow::on_resetView_triggered() {
    statusBar()->showMessage("Reset model view");

    //emit resetViewRequest();
}




void MainWindow::on_lightSlider_valueChanged(int value) {
    double intensity = static_cast<double>(value) / 100.0;  // scale 0.0 to 1.0
    sceneLight->SetIntensity(intensity);


//    renderer->Render();
  //  ui->vtkWidget->GetRenderWindow()->Render();


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

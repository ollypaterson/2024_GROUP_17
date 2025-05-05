#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "VRRenderThread.h"
#include "optiondialog.h"
#include "colourdialog.h"
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkLight.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , vrThread(nullptr) // Initialize VR thread pointer
    , vrRenderWindow(nullptr)
    , vrRenderer(nullptr)
{
    ui->setupUi(this);

    // Set up tree view context menu
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::on_treeViewContextMenu);

    // Connect buttons and signals
    connect(ui->pushButton, &QPushButton::released, this, &MainWindow::on_resetView_triggered);
    connect(this, &MainWindow::statusUpdateMessage, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    connect(ui->secondButton, &QPushButton::released, this, &MainWindow::handleSecondButton);


    // Create a simple VTK scene (for the main window)
    renderer = vtkSmartPointer<vtkRenderer>::New();
    ui->vtkWidget->setRenderWindow(renderer->GetRenderWindow());
    ui->vtkWidget->show();

    // Create a light for the scene
    sceneLight = vtkSmartPointer<vtkLight>::New();
    sceneLight->SetPosition(10, 10, 10);
    sceneLight->SetFocalPoint(0, 0, 0);
    renderer->SetLight(sceneLight);
    renderer->SetBackground(0.1, 0.4, 0.8); // Blueish background

    // Set up the tree view
    partList = new ModelPartList();
    ui->treeView->setModel(partList);
    ui->treeView->header()->resizeSection(0, 200);
    ui->treeView->header()->resizeSection(1, 100);

    // Add action to toolbar and file menu
    QAction* openFileAction = new QAction("Open File", this);
    openFileAction->setShortcut(QKeySequence::Open);
    connect(openFileAction, &QAction::triggered, this, &MainWindow::on_actionOpen_FIle_triggered);
    ui->toolBar->addAction(openFileAction);
    ui->menuFile->addAction(openFileAction);

    QAction* saveFileAction = new QAction("Save File", this);
    saveFileAction->setShortcut(QKeySequence::Save);
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::on_actionSave_File_triggered);
    ui->toolBar->addAction(saveFileAction);
    ui->menuFile->addAction(saveFileAction);

    QAction* helpAction = new QAction("Help", this);
    connect(helpAction, &QAction::triggered, this, &MainWindow::on_actionHelp_triggered);
    ui->toolBar->addAction(helpAction);
    ui->menuHelp->addAction(helpAction);

    QAction* printAction = new QAction("Print", this);
    printAction->setShortcut(QKeySequence::Print);
    connect(printAction, &QAction::triggered, this, &MainWindow::on_actionPrint_triggered);
    ui->toolBar->addAction(printAction);
    ui->menuFile->addAction(printAction);

    // Add the VR start button.
    QAction* vrAction = new QAction("Start VR", this);
    connect(vrAction, &QAction::triggered, this, &MainWindow::onStartVRButtonClicked); // Connect
    ui->toolBar->addAction(vrAction);
    ui->menuFile->addAction(vrAction);

    //updateRender();  // Initial render.  Moved to after initial STL load.
}

MainWindow::~MainWindow()
{
    delete ui;
    delete partList;
    if (vrThread) {
        vrThread->stopVR();
        vrThread->wait();
        delete vrThread;
    }
    //  Cleanup vrRenderWindow and vrRenderer.  Smart pointers should handle this,
    //  but it's good practice to be explicit if needed.
    if (vrRenderWindow)
    {
       //vrRenderWindow->Finalize();  //May cause crash.
       vrRenderWindow = nullptr;
    }
    vrRenderer = nullptr;
}

void MainWindow::handleButton()
{
    qDebug() << "Button clicked!";
    statusUpdateMessage("Button Clicked", 2000); // 2-second timeout
}

void MainWindow::handleTreeClicked()
{
    qDebug() << "Tree item clicked";
}

void MainWindow::handleSecondButton()
{
    qDebug() << "Second button clicked";
}

void MainWindow::on_actionOpen_FIle_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open STL File", "", "STL Files (*.stl)");
    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);
        QString shortName = fileInfo.fileName();
        // Get the currently selected item in the tree
        QModelIndexList selected = ui->treeView->selectionModel()->selectedRows();
        if (selected.isEmpty()) {
            QMessageBox::warning(this, "No Selection", "Please select an item in the tree to load the STL file.");
            return;
        }
        QModelIndex index = selected.first();  // get the first selected index
        ModelPart* part = static_cast<ModelPart*>(index.internalPointer());
        if (part) {
            part->loadSTL(fileName);
            part->setData(0, shortName); // update the name in the tree
            updateRender(); // Update the main window render
            statusUpdateMessage(QString("Loaded file: %1").arg(shortName), 5000);
        }
        else {
             QMessageBox::warning(this, "No Selection", "Please select a valid item in the tree to load the STL file.");
             return;
        }
    }
}

void MainWindow::on_actionSave_File_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save STL File", "", "STL Files (*.stl)");
    if (!fileName.isEmpty()) {
        // For simplicity, we'll assume you want to save the currently displayed model
        // You would need to get the data from VTK and write it to the file
        // This is a placeholder - replace with actual save logic
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write("This is a placeholder for the STL data.\n"); // Replace this
            file.close();
            statusUpdateMessage(QString("Saved file: %1").arg(fileName), 5000);
        }
        else {
            QMessageBox::critical(this, "Error", "Could not save file.");
        }
    }
}

void MainWindow::on_actionHelp_triggered()
{
    QMessageBox::information(this, "Help", "This is a placeholder for help information.  Look at the tool tips.");
}

void MainWindow::on_actionPrint_triggered()
{
    QMessageBox::information(this, "Print", "This is a placeholder for print functionality.");
}

void MainWindow::on_treeViewContextMenu(const QPoint& pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    if (index.isValid()) {
        QMenu* menu = new QMenu(this);
        QAction* itemOptionsAction = new QAction("Item Options", this);
        connect(itemOptionsAction, &QAction::triggered, this, &MainWindow::on_actionItemOptions_triggered);
        menu->addAction(itemOptionsAction);
        menu->popup(ui->treeView->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::on_actionItemOptions_triggered()
{
     QModelIndexList selected = ui->treeView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select an item in the tree to edit its properties.");
        return;
    }
     QModelIndex index = selected.first();
     ModelPart *part = static_cast<ModelPart*>(index.internalPointer());
     if(part)
     {
        OptionDialog dialog(this, part);
        dialog.exec();
        updateRender();
     }
     else{
        QMessageBox::warning(this, "No Selection", "Please select a valid item in the tree to edit its properties.");
        return;
     }
}



void MainWindow::on_colourButton_triggered()
{
    QModelIndexList selected = ui->treeView->selectionModel()->selectedRows();
      if (selected.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select an item whose color you want to change.");
        return;
    }
    QModelIndex index = selected.first();
    ModelPart* part = static_cast<ModelPart*>(index.internalPointer());
     if(part)
     {
        ColourDialog dialog(this, part);
        dialog.exec();
        updateRender();
     }
     else{
         QMessageBox::warning(this, "No Selection", "Please select a valid item in the tree to change its colour.");
        return;
     }
}

void MainWindow::on_resetView_triggered()
{
    renderer->ResetCamera();
    ui->vtkWidget->renderWindow()->Render();
}



// Start VR rendering thread
void MainWindow::onStartVRButtonClicked() {
    if (!vrThread) { // Only start if it's not already running
        // 1. Create VR render window and renderer
        vrRenderWindow = vtkSmartPointer<vtkOpenVRRenderWindow>::New();
        vrRenderer = vtkSmartPointer<vtkRenderer>::New();
        vrRenderWindow->AddRenderer(vrRenderer);
        vrRenderWindow->Initialize();
        vrRenderWindow->MakeCurrent(); //make context current


        // 2. Set up VR camera and other VR settings...
        vtkSmartPointer<vtkCamera> vrCamera = vtkSmartPointer<vtkCamera>::New();
        vrRenderer->SetActiveCamera(vrCamera);
        vrCamera->SetPosition(0, 0, 0);
        vrCamera->SetFocalPoint(0, 0, -1);
        vrRenderer->SetBackground(0.0, 0.0, 0.0); // Black background for VR


        // 3. Create VR actors (or copy from main scene)
        QList<vtkSmartPointer<vtkActor>> vrActors;
         for (int i = 0; i < partList->rowCount(); ++i) {
            QModelIndex childIndex = partList->index(i, 0, QModelIndex());
            ModelPart* part = static_cast<ModelPart*>(childIndex.internalPointer());
            if (part)
            {
                vtkSmartPointer<vtkActor> vrActor = part->getVRActor(); // Get/Create VR actor
                vrActors.append(vrActor);  // Store for later
                vrRenderer->AddActor(vrActor);
            }
        }
        vrRenderWindow->SetSize(1024, 768); //set size.



        // 4. Create and start VR thread, pass the render window/renderer
        vrThread = new VRRenderThread(this);
        vrThread->setRenderWindowAndRenderer(vrRenderWindow, vrRenderer);
        vrThread->setVRActors(vrActors); // Pass the list of VR actors
        vrThread->start();
        qDebug() << "VR thread started";
    }
    else
    {
        qDebug() << "VR thread already running";
    }
}



void MainWindow::updateRender()
{
    renderer->RemoveAllViewProps();
    updateRenderFromTree(QModelIndex());  // start from invisible root
    renderer->ResetCamera();
    ui->vtkWidget->renderWindow()->Render();
}



void MainWindow::updateRenderFromTree(const QModelIndex& index)
{
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

     // If VR thread is active, add VR actor too
    if (vrThread && part) {
        vtkSmartPointer<vtkActor> vrActor = part->getVRActor();
        vrThread->addActor(vrActor);
    }


    int rows = partList->rowCount(index);
    for (int i = 0; i < rows; ++i) {
        updateRenderFromTree(partList->index(i, 0, index));
    }
}

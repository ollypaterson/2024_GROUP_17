#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QStandardItemModel>
#include "optiondialog.h"
#include "colourdialog.h"
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkLight.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up tree view context menu
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::on_treeViewContextMenu);

    // Connect buttons and signals
    connect(ui->pushButton, &QPushButton::released, this, &MainWindow::on_resetView_triggered);
    connect(this, &MainWindow::statusUpdateMessage, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    connect(ui->pushButton_2, &QPushButton::released, this, &MainWindow::on_colourButton_triggered);
    connect(ui->lightSlider, &QSlider::valueChanged, this, &MainWindow::on_lightSlider_valueChanged);
    connect(ui->startVRButton, &QPushButton::clicked, this, &MainWindow::onStartVRButtonClicked); // 🔧 New connection for VR

    // Initialize VTK render window and renderer
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);
    ui->vtkWidget->setRenderWindow(renderWindow);

    // Add lighting
    sceneLight = vtkSmartPointer<vtkLight>::New();
    sceneLight->SetLightTypeToSceneLight();
    sceneLight->SetPosition(5, 5, 15);
    sceneLight->SetPositional(true);
    sceneLight->SetConeAngle(10);
    sceneLight->SetFocalPoint(0, 0, 0);
    sceneLight->SetDiffuseColor(1, 1, 1);
    sceneLight->SetAmbientColor(1, 1, 1);
    sceneLight->SetSpecularColor(1, 1, 1);
    sceneLight->SetIntensity(0.5);
    renderer->AddLight(sceneLight);
}

MainWindow::~MainWindow()
{
    if (vrThread) {
        vrThread->stopVR();  // Stop the VR thread gracefully
        vrThread->wait();    // Wait for the thread to finish
        delete vrThread;
    }

    delete ui;
}

void MainWindow::on_treeViewContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    QMenu contextMenu(this);

    if (index.isValid()) {
        ModelPart* item = static_cast<ModelPart*>(index.internalPointer());
        if (!item) return;

        QString partName = item->data(0).toString();

        QAction* changeColour = contextMenu.addAction("Change colour");
        QAction* clipFilter = contextMenu.addAction("Toggle clip filter");
        clipFilter->setCheckable(true);
        QString clipState = index.siblingAtColumn(3).data().toString();
        bool isClipped = (clipState == "true");
        clipFilter->setChecked(isClipped);

        contextMenu.addSeparator();

        contextMenu.addAction("Expand All", ui->treeView, SLOT(expandAll()));
        contextMenu.addAction("Collapse All", ui->treeView, SLOT(collapseAll()));

        contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::on_colourButton_triggered()
{
    colourdialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QColor colour = dialog.selectedColour();
        statusBar()->showMessage("Selected colour: " + colour.name());
    } else {
        statusBar()->showMessage("Colour selection cancelled");
    }
}

void MainWindow::on_resetView_triggered()
{
    statusBar()->showMessage("Reset model view");
}

void MainWindow::on_lightSlider_valueChanged(int value)
{
    double intensity = static_cast<double>(value) / 100.0; // scale 0.0 to 1.0
    sceneLight->SetIntensity(intensity);
    ui->vtkWidget->GetRenderWindow()->Render();
    statusBar()->showMessage(QString("Light intensity set to %1%").arg(value));
}

void MainWindow::updateRender()
{
    renderer->RemoveAllViewProps();
    updateRenderFromTree(QModelIndex()); // start from invisible root
    renderer->ResetCamera();
    ui->vtkWidget->GetRenderWindow()->Render();
}

void MainWindow::updateRenderFromTree(const QModelIndex& index)
{
    if (!index.isValid()) {
        for (int i = 0; i < model->rowCount(); ++i) {
            updateRenderFromTree(model->index(i, 0));
        }
        return;
    }

    ModelPart* part = static_cast<ModelPart*>(index.internalPointer());
    if (!part) return;

    // Add actor to main renderer
    renderer->AddActor(part->getActor());

    // If VR thread is active, add VR actor too
    if (vrThread) {
        vrThread->addActor(part->getVRActor());
    }

    int rows = model->rowCount(index);
    for (int i = 0; i < rows; ++i) {
        updateRenderFromTree(model->index(i, 0, index));
    }
}

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

void MainWindow::handleTreeClicked()
{
    QModelIndex index = ui->treeView->currentIndex();
    if (index.isValid()) {
        ModelPart* part = static_cast<ModelPart*>(index.internalPointer());
        if (part) {
            QString text = part->data(0).toString();
            emit statusUpdateMessage(QString("The selected item is: ") + text, 0);
        }
    }
}

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include "optiondialog.h"
#include "VRRenderThread.h"  // Include VR thread class
#include "modelpart.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , vrThread(nullptr)  // Initialize to null
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::released, this, &MainWindow::handleButton);
    connect(this, &MainWindow::statusUpdateMessage, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    connect(ui->pushButton_2, &QPushButton::released, this, &MainWindow::handleSecondButton);
    connect(ui->startVRButton, &QPushButton::clicked, this, &MainWindow::onStartVRButtonClicked);
    ui->treeView->addAction(ui->actionItemOptions);

    this->partList = new ModelPartList("PartsList");
    ui->treeView->setModel(this->partList);

    ModelPart* rootItem = this->partList->getRootItem();

    for (int i = 0; i < 3; i++) {
        QString name = QString("TopLevel %1").arg(i);
        QString visible("true");

        ModelPart* childItem = new ModelPart({ name, visible });
        rootItem->appendChild(childItem);

        for (int j = 0; j < 5; j++) {
            QString name = QString("Item %1,%2").arg(i).arg(j);
            QString visible("true");

            ModelPart* childChildItem = new ModelPart({ name, visible });
            childItem->appendChild(childChildItem);
        }
    }

    // Add basic lighting for VR
    vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
    light->SetLightTypeToSceneLight();
    light->SetPosition(5, 5, 15);
    light->SetPositional(true);
    light->SetConeAngle(10);
    light->SetFocalPoint(0, 0, 0);
    light->SetDiffuseColor(1, 1, 1);
    light->SetAmbientColor(1, 1, 1);
    light->SetSpecularColor(1, 1, 1);
    light->SetIntensity(0.5);
    // Add this light to the renderer if available (example):
    // renderer->AddLight(light);
}

MainWindow::~MainWindow()
{
    if (vrThread) {
        vrThread->stopVR();
        vrThread->wait();
        delete vrThread;
    }
    delete ui;
}

void MainWindow::handleButton() {
    emit statusUpdateMessage(QString("Add button was clicked"), 0);
}

void MainWindow::handleSecondButton() {
    OptionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        emit statusUpdateMessage(QString("Dialog accepted "), 0);
    } else {
        emit statusUpdateMessage(QString("Dialog rejected "), 0);
    }
}

void MainWindow::handleTreeClicked() {
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) return;

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());
    QString text = selectedPart->data(0).toString();
    emit statusUpdateMessage(QString("The selected item is: ") + text, 0);

    selectedPart->linkProperties(selectedPart->getActor());

    if (vrThread) {
        vtkSmartPointer<vtkActor> vrActor = selectedPart->getVRActor();
        vrThread->addActor(vrActor);
    }
}

void MainWindow::on_actionOpen_FIle_triggered() {
    emit statusUpdateMessage(QString("Open File action triggered"), 0);
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        "C:\\",
        tr("STL Files(*.stl);;Text Files(*.txt)")
    );

    if (!fileName.isEmpty()) {
        emit statusUpdateMessage(QString("Selected file: ") + fileName, 0);

        // You would insert logic here to load the file into a ModelPart
        // and add it to the model tree, and also to VR if active.
    }
}

void MainWindow::on_actionItemOptions_triggered() {
    OptionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        emit statusUpdateMessage(QString("Dialog accepted "), 0);
    } else {
        emit statusUpdateMessage(QString("Dialog rejected "), 0);
    }
}

void MainWindow::onStartVRButtonClicked() {
    if (!vrThread) {
        vrThread = new VRRenderThread(this);
        vrThread->start();
        emit statusUpdateMessage("VR rendering started.");
    } else {
        QMessageBox::information(this, "Info", "VR is already running.");
    }
}

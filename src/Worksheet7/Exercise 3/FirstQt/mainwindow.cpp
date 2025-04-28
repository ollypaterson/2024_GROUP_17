#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include "optiondialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::released, this, &MainWindow::handleButton);
    connect(this, &MainWindow::statusUpdateMessage, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);
    connect(ui->pushButton_2, &QPushButton::released, this, &MainWindow::handleSecondButton);
    ui->treeView->addAction(ui->actionItemOptions);


    /*Create / allocate the model list */
    this->partList = new ModelPartList("PartsList");

    /*Link it to the treeview in the GUI*/
    ui->treeView->setModel(this->partList);

    /* Manually create a model tree - there a much better and more flexible ways of doing this, e.g. with
    nested function. this is just a quick example and a start point*/
    ModelPart* rootItem = this->partList->getRootItem();


    /*Add 3 top level items*/
    for (int i = 0; i < 3; i++) {

        QString name = QString("TopLevel %1").arg(i);
        QString visible("true");

        //Create child item
        ModelPart* childItem = new ModelPart({ name, visible });

        //Append to tree top level
        rootItem->appendChild(childItem);

        //Add 5 sub items
        for (int j = 0; j < 5; j++) {

            QString name = QString("Item %1,%2").arg(i).arg(j);
            QString visible("true");

            ModelPart* childChildItem = new ModelPart({ name, visible });

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
    emit statusUpdateMessage(QString("Add button was clicked"), 0);
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
    emit statusUpdateMessage( QString("Open  File action triggered"), 0);
    QString fileName = QFileDialog::getOpenFileName(
        this ,
        tr("Open File"),
        "C:\\",
        tr("STL Files(*.stl);;Text Files(*.txt)") );

    if (!fileName.isEmpty()){
        emit statusUpdateMessage(QString("Selected file: ")+ fileName, 0);
    }

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


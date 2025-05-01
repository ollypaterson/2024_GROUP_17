#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QStandardItemModel>
#include "optiondialog.h"

MainWindow::MainWindow(QWidget* parent)

    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::on_treeViewContextMenu);
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
    emit statusUpdateMessage( QString("Open file action triggered"), 0);
    QString fileName = QFileDialog::getOpenFileName(
        this ,
        tr("Open File"),
        "C:\\",
        tr("STL Files(*.stl);;Text Files(*.txt)") );

    if (!fileName.isEmpty()){
        emit statusUpdateMessage(QString("Selected file: ")+ fileName, 0);
    }

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
        QAction* clipFilter = contextMenu.addAction("Apply clip filter");
        QAction* shrinkFilter = contextMenu.addAction("Apply shrink filter");
        QAction* toggleVisibility = contextMenu.addAction("Toggle visibility");
        QAction* renamePart = contextMenu.addAction("Rename part");

        QAction* selectedAction = contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
        if (!selectedAction) return;

        if (selectedAction == changeColour) {
            statusBar()->showMessage("Change colour on: " + partName);

        } else if (selectedAction == clipFilter) {
            statusBar()->showMessage("Apply clip filter on: " + partName);

        } else if (selectedAction == shrinkFilter) {
            statusBar()->showMessage("Apply shrink filter on: " + partName);

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

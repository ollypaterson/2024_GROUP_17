/**     @file ModelPart.cpp
  *
  *     EEEE2076 - Software Engineering & VR Project
  *
  *     Template for model parts that will be added as treeview items
  *
  *     P Evans 2022
  */

#include "ModelPart.h"
#include <vtkPolyDataMapper.h>
#include <vtkSTLReader.h>
#include <vtkActor.h>
#include <vtkProperty.h>

ModelPart::ModelPart(const QList<QVariant>& data, ModelPart* parent)
    : m_itemData(data), m_parentItem(parent), isVisible(true) {
    colour = vtkColor3<unsigned char>(255, 255, 255);  // Default color: white
    stlReader = vtkSmartPointer<vtkSTLReader>::New();
    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    actor = vtkSmartPointer<vtkActor>::New();
}

ModelPart::~ModelPart() {
    qDeleteAll(m_childItems);
}

void ModelPart::appendChild(ModelPart* item) {
    item->m_parentItem = this;
    m_childItems.append(item);
}

ModelPart* ModelPart::child(int row) {
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int ModelPart::childCount() const {
    return m_childItems.count();
}

int ModelPart::columnCount() const {
    return m_itemData.count();
}

QVariant ModelPart::data(int column) const {
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

void ModelPart::set(int column, const QVariant &value) {
    if (column < 0 || column >= m_itemData.size())
        return;

    m_itemData.replace(column, value);
}

ModelPart* ModelPart::parentItem() {
    return m_parentItem;
}

int ModelPart::row() const {
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModelPart*>(this));
    return 0;
}

void ModelPart::setColour(const unsigned char R, const unsigned char G, const unsigned char B) {
    colour.Set(R, G, B);
    if (actor && actor->GetProperty()) {
        actor->GetProperty()->SetColor(R / 255.0, G / 255.0, B / 255.0);
    }
}

unsigned char ModelPart::getColourR() {
    return colour.GetRed();
}

unsigned char ModelPart::getColourG() {
    return colour.GetGreen();
}

unsigned char ModelPart::getColourB() {
    return colour.GetBlue();
}

void ModelPart::setVisible(bool isVisibleFlag) {
    isVisible = isVisibleFlag;
    if (actor) {
        actor->SetVisibility(isVisible);
    }
}

bool ModelPart::visible() {
    return isVisible;
}

void ModelPart::loadSTL(QString fileName) {
    stlReader->SetFileName(fileName.toStdString().c_str());
    stlReader->Update();

    mapper->SetInputConnection(stlReader->GetOutputPort());
    actor->SetMapper(mapper);

    actor->GetProperty()->SetColor(
        colour.GetRed() / 255.0,
        colour.GetGreen() / 255.0,
        colour.GetBlue() / 255.0
    );

    actor->SetVisibility(isVisible);
}

vtkSmartPointer<vtkActor> ModelPart::getActor() {
    return actor;
}

vtkSmartPointer<vtkActor> ModelPart::getVRActor() {
    vtkSmartPointer<vtkPolyDataMapper> vrMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vrMapper->SetInputConnection(stlReader->GetOutputPort());

    vtkSmartPointer<vtkActor> vrActor = vtkSmartPointer<vtkActor>::New();
    vrActor->SetMapper(vrMapper);
    vrActor->SetProperty(actor->GetProperty());  // Link properties for sync

    return vrActor;
}

void ModelPart::linkProperties(vtkSmartPointer<vtkActor> guiActor) {
    vtkSmartPointer<vtkActor> vrActor = getVRActor();
    vrActor->SetProperty(guiActor->GetProperty());
}

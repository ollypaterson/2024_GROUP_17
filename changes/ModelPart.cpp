#include "ModelPart.h"
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkTransform.h> // Include for transforms
#include <vtkTransformFilter.h>

// Constructor
ModelPart::ModelPart(const QList<QVariant>& data, ModelPart* parent)
    : m_itemData(data), m_parentItem(parent), isVisible(true)
{
    // Initialize VTK components
    stlReader = vtkSmartPointer<vtkSTLReader>::New();
    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    actor = vtkSmartPointer<vtkActor>::New();

    // Default color if not set later
    actor->GetProperty()->SetColor(1.0, 1.0, 0.0); // Bright yellow
    actor->SetVisibility(true);
}

// Destructor
ModelPart::~ModelPart()
{
    qDeleteAll(m_childItems);
}

// Add a child part to this item
void ModelPart::appendChild(ModelPart* item)
{
    item->m_parentItem = this;
    m_childItems.append(item);
}

// Get pointer to child at row index
ModelPart* ModelPart::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

// Get number of children
int ModelPart::childCount() const
{
    return m_childItems.count();
}

// Get number of columns (properties)
int ModelPart::columnCount() const
{
    return m_itemData.count();
}

// Get the data item at a particular column for this item.
QVariant ModelPart::data(int column) const
{
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

// Set the data item at a particular column for this item.
bool ModelPart::setData(int column, const QVariant& value)
{
    if (column < 0 || column >= m_itemData.size())
        return false;
    m_itemData[column] = value;
    return true;
}

// Get the parent of this item
ModelPart* ModelPart::parent()
{
    return m_parentItem;
}

// Return the main VTK actor used for GUI rendering
vtkSmartPointer<vtkActor> ModelPart::getActor()
{
    return actor;
}

// Return a new VTK actor specifically for VR rendering
vtkSmartPointer<vtkActor> ModelPart::getVRActor()
{
    // Create a new mapper for VR
    vtkSmartPointer<vtkPolyDataMapper> vrMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vrMapper->SetInputConnection(stlReader->GetOutputPort());

    // Create a new actor for VR
    vtkSmartPointer<vtkActor> vrActor = vtkSmartPointer<vtkActor>::New();
    vrActor->SetMapper(vrMapper);

     // Sync all properties from the GUI actor to the VR actor
    if (actor && vrActor) {
        vrActor->GetProperty()->SetColor(actor->GetProperty()->GetColor());
        vrActor->GetProperty()->SetOpacity(actor->GetProperty()->GetOpacity());
        vrActor->SetPosition(actor->GetPosition());
        vrActor->SetOrientation(actor->GetOrientation());
        vrActor->SetScale(actor->GetScale());
        vrActor->SetVisibility(actor->GetVisibility());
    }
    return vrActor;
}

// Link properties from GUI actor to VR actor
void ModelPart::linkProperties(vtkSmartPointer<vtkActor> guiActor)
{
     // Ensure we have both GUI and our actor
    if (!actor || !guiActor)
        return;

    // Copy *all* relevant properties from the GUI actor to our actor
    actor->GetProperty()->SetColor(guiActor->GetProperty()->GetColor());
    actor->GetProperty()->SetOpacity(guiActor->GetProperty()->GetOpacity());
    actor->SetPosition(guiActor->GetPosition());
    actor->SetOrientation(guiActor->GetOrientation());
    actor->SetScale(guiActor->GetScale());
    actor->SetVisibility(guiActor->GetVisibility());

    vtkSmartPointer<vtkActor> vrActor = getVRActor();
     if (vrActor)
     {
        vrActor->GetProperty()->SetColor(guiActor->GetProperty()->GetColor());
        vrActor->GetProperty()->SetOpacity(guiActor->GetProperty()->GetOpacity());
        vrActor->SetPosition(guiActor->GetPosition());
        vrActor->SetOrientation(guiActor->GetOrientation());
        vrActor->SetScale(guiActor->GetScale());
        vrActor->SetVisibility(guiActor->GetVisibility());
     }
}

void ModelPart::loadSTL(QString fileName)
{
    stlReader->SetFileName(fileName.toStdString().c_str());
    stlReader->Update();

    mapper->SetInputConnection(stlReader->GetOutputPort());
    actor->SetMapper(mapper);
}

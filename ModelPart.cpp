#include "ModelPart.h"
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>

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

// Get data from column
QVariant ModelPart::data(int column) const
{
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

// Set data in column
void ModelPart::set(int column, const QVariant& value)
{
    if (column < 0 || column >= m_itemData.size())
        return;

    m_itemData.replace(column, value);
}

// Get parent item
ModelPart* ModelPart::parentItem()
{
    return m_parentItem;
}

// Get row index relative to parent
int ModelPart::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModelPart*>(this));
    return 0;
}

// Set colour of the model part
void ModelPart::setColour(const unsigned char R, const unsigned char G, const unsigned char B)
{
    double r = R / 255.0;
    double g = G / 255.0;
    double b = B / 255.0;

    actor->GetProperty()->SetColor(r, g, b);

    // Optionally sync with VR actor if it exists
    vtkSmartPointer<vtkActor> vrActor = getVRActor();
    if (vrActor) {
        vrActor->GetProperty()->SetColor(r, g, b);
    }
}

// Get red component of current colour
unsigned char ModelPart::getColourR()
{
    double rgb[3];
    actor->GetProperty()->GetColor(rgb);
    return static_cast<unsigned char>(rgb[0] * 255);
}

// Get green component of current colour
unsigned char ModelPart::getColourG()
{
    double rgb[3];
    actor->GetProperty()->GetColor(rgb);
    return static_cast<unsigned char>(rgb[1] * 255);
}

// Get blue component of current colour
unsigned char ModelPart::getColourB()
{
    double rgb[3];
    actor->GetProperty()->GetColor(rgb);
    return static_cast<unsigned char>(rgb[2] * 255);
}

// Set visibility of the actor
void ModelPart::setVisible(bool visible)
{
    isVisible = visible;
    if (actor) {
        actor->SetVisibility(visible);
    }

    // Also update VR actor if available
    vtkSmartPointer<vtkActor> vrActor = getVRActor();
    if (vrActor) {
        vrActor->SetVisibility(visible);
    }
}

// Get visibility status
bool ModelPart::visible()
{
    return isVisible;
}

// Load STL file into this ModelPart
void ModelPart::loadSTL(QString fileName)
{
    stlReader->SetFileName(fileName.toStdString().c_str());
    stlReader->Update();

    mapper->SetInputConnection(stlReader->GetOutputPort());
    actor->SetMapper(mapper);
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

    // Sync material/properties from GUI actor to VR actor
    if (actor && vrActor) {
        vrActor->GetProperty()->SetMaterial(actor->GetProperty());
    }

    return vrActor;
}

// Link properties from GUI actor to VR actor
void ModelPart::linkProperties(vtkSmartPointer<vtkActor> guiActor)
{
    // Ensure we have both GUI and VR actors
    if (!actor || !guiActor)
        return;

    // Copy properties from GUI actor to our own actor
    actor->GetProperty()->SetMaterial(guiActor->GetProperty());

    // If you want to also update a VR-specific actor:
    vtkSmartPointer<vtkActor> vrActor = getVRActor();
    if (vrActor) {
        vrActor->GetProperty()->SetMaterial(guiActor->GetProperty());
    }
}

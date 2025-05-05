/**     @file ModelPart.cpp
  *
  *     EEEE2076 - Software Engineering & VR Project
  *
  *     Template for model parts that will be added as treeview items
  *
  *     P Evans 2022
  */

#include "ModelPart.h"
#include <vtkProperty.h>
#include <vtkShrinkPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkPlane.h>
#include <vtkActor.h>


/* Commented out for now, will be uncommented later when you have
 * installed the VTK library
 */
#include <vtkSmartPointer.h>
#include <vtkDataSetMapper.h>




ModelPart::ModelPart(const QList<QVariant>& data, ModelPart* parent )
    : m_itemData(data), m_parentItem(parent) {

    /* You probably want to give the item a default colour */
}


ModelPart::~ModelPart() {
    qDeleteAll(m_childItems);
}


void ModelPart::appendChild( ModelPart* item ) {
    /* Add another model part as a child of this part
     * (it will appear as a sub-branch in the treeview)
     */
    item->m_parentItem = this;
    m_childItems.append(item);
}


ModelPart* ModelPart::child( int row ) {
    /* Return pointer to child item in row below this item.
     */
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int ModelPart::childCount() const {
    /* Count number of child items
     */
    return m_childItems.count();
}


int ModelPart::columnCount() const {
    /* Count number of columns (properties) that this item has.
     */
    return m_itemData.count();
}

QVariant ModelPart::data(int column) const {
    /* Return the data associated with a column of this item 
     *  Note on the QVariant type - it is a generic placeholder type
     *  that can take on the type of most Qt classes. It allows each 
     *  column or property to store data of an arbitrary type.
     */
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}


void ModelPart::set(int column, const QVariant &value) {
    /* Set the data associated with a column of this item 
     */
    if (column < 0 || column >= m_itemData.size())
        return;

    m_itemData.replace(column, value);
}


ModelPart* ModelPart::parentItem() {
    return m_parentItem;
}


int ModelPart::row() const {
    /* Return the row index of this item, relative to it's parent.
     */
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModelPart*>(this));
    return 0;
}

void ModelPart::setColour(const unsigned char R, const unsigned char G, const unsigned char B) {
    if (actor) {
        actor->GetProperty()->SetColor(R / 255.0, G / 255.0, B / 255.0);
    }
}

void ModelPart::propagateColour(unsigned char R, unsigned char G, unsigned char B) {
    setColour(R, G, B);

    for (ModelPart* child : m_childItems) {
        child->propagateColour(R, G, B);
    }
}


unsigned char ModelPart::getColourR() {
    /* This is a placeholder function that you will need to modify if you want to use it */
    
    /* As the name suggests ... */
    return 0;   // needs updating
}

unsigned char ModelPart::getColourG() {
    /* This is a placeholder function that you will need to modify if you want to use it */
    
    /* As the name suggests ... */
    return 0;   // needs updating
}


unsigned char ModelPart::getColourB() {
   /* This is a placeholder function that you will need to modify if you want to use it */
    
    /* As the name suggests ... */
    return 0;   // needs updating
}


void ModelPart::setVisible(bool visible) {
    isVisible = visible;
    if (actor) {
        actor->SetVisibility(visible);
    }
}

bool ModelPart::visible() {

    return isVisible;
}
void ModelPart::loadSTL(QString fileName) {
    // Load STL file
    file = vtkSmartPointer<vtkSTLReader>::New();
    file->SetFileName(fileName.toStdString().c_str());
    file->Update();

    // === SHRINK FILTER ===
    shrinkFilter = vtkSmartPointer<vtkShrinkPolyData>::New();
    shrinkFilter->SetInputConnection(file->GetOutputPort());
    shrinkFilter->SetShrinkFactor(1.0);  // Default: no shrink
    shrinkFilter->Update();

    // === CLIP FILTER ===
    clipPlane = vtkSmartPointer<vtkPlane>::New();

    // Center the clip plane in Z bounds
    double bounds[6];
    file->GetOutput()->GetBounds(bounds);
    double zMid = (bounds[4] + bounds[5]) / 2.0;

    clipPlane->SetOrigin(0.0, 0.0, zMid);      // Mid-height of model
    clipPlane->SetNormal(0.0, 0.0, 1.0);       // Clipping from bottom

    clipFilter = vtkSmartPointer<vtkClipPolyData>::New();
    clipFilter->SetInputConnection(shrinkFilter->GetOutputPort());
    clipFilter->SetClipFunction(clipPlane.Get());
    clipFilter->SetInsideOut(true);           // Show front half
    clipFilter->SetValue(0.0);
    clipFilter->Update();

    // === Mapper ===
    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(clipFilter->GetOutputPort());  // ✅ USE FILTERS

    // === Actor ===
    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1.0, 1.0, 0.0);
    actor->SetVisibility(true);
    isVisible = true;
}




bool ModelPart::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= m_itemData.size())
        return false;

    m_itemData[column] = value;
    return true;
}


void ModelPart::propagateVisibility(bool visible) {
    setVisible(visible);
    set(1, visible ? "true" : "false");
    for (ModelPart* child : m_childItems) {
        child->propagateVisibility(visible);
    }
}

void ModelPart::propagateClip(bool clip) {
    set(3, clip ? "true" : "false");
    for (ModelPart* child : m_childItems) {
        child->propagateClip(clip);
    }
}

void ModelPart::propagateShrink(bool shrink) {
    set(2, shrink ? "true" : "false");
    for (ModelPart* child : m_childItems) {
        child->propagateShrink(shrink);
    }
}



vtkSmartPointer<vtkActor> ModelPart::getActor() {
    return actor;
}

//vtkActor* ModelPart::getNewActor() {
    /* This is a placeholder function that you will need to modify if you want to use it
     * 
     * The default mapper/actor combination can only be used to render the part in 
     * the GUI, it CANNOT also be used to render the part in VR. This means you need
     * to create a second mapper/actor combination for use in VR - that is the role
     * of this function. */
     
     
     /* 1. Create new mapper */
     
     /* 2. Create new actor and link to mapper */
     
     /* 3. Link the vtkProperties of the original actor to the new actor. This means 
      *    if you change properties of the original part (colour, position, etc), the
      *    changes will be reflected in the GUI AND VR rendering.
      *    
      *    See the vtkActor documentation, particularly the GetProperty() and SetProperty()
      *    functions.
      */
    

    /* The new vtkActor pointer must be returned here */
//    return nullptr;
    
//}















void ModelPart::applyShrink(bool enable) {
    if (!shrinkFilter) return;
    shrinkFilter->SetShrinkFactor(enable ? 0.7 : 1.0);
    shrinkFilter->Modified();
    shrinkFilter->Update();
}

void ModelPart::applyClip(bool enable) {
    if (!clipFilter) return;
    clipFilter->SetInsideOut(false);
    clipFilter->Modified();
    clipFilter->Update();
}

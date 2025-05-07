/** * @file ModelPart.cpp
  * @brief Implementation of the ModelPart class.
  *
  * EEEE2076 - Software Engineering & VR Project
  *
  * Template for model parts that will be added as treeview items
  *
  * P Evans 2022
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




/**
 * @brief ModelPart::ModelPart
 * @param data The data for the model part.
 * @param parent The parent item.
 */
ModelPart::ModelPart(const QList<QVariant>& data, ModelPart* parent )
    : m_itemData(data), m_parentItem(parent) {

    /* You probably want to give the item a default colour */
}

/**
 * @brief ModelPart::~ModelPart
 * Destructor for the ModelPart class.
 */
ModelPart::~ModelPart() {
    qDeleteAll(m_childItems);
}

/**
 * @brief ModelPart::appendChild
 * Adds a child item to this model part.
 * @param item The child item to add.
 */
void ModelPart::appendChild( ModelPart* item ) {
    /* Add another model part as a child of this part
     * (it will appear as a sub-branch in the treeview)
     */
    item->m_parentItem = this;
    m_childItems.append(item);
}

/**
 * @brief ModelPart::child
 * Returns the child item at the specified row.
 * @param row The row of the child item.
 * @return Pointer to the child item, or nullptr if the row is invalid.
 */
ModelPart* ModelPart::child( int row ) {
    /* Return pointer to child item in row below this item.
     */
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

/**
 * @brief ModelPart::childCount
 * Returns the number of child items.
 * @return The number of child items.
 */
int ModelPart::childCount() const {
    /* Count number of child items
     */
    return m_childItems.count();
}

/**
 * @brief ModelPart::columnCount
 * Returns the number of columns (properties) of this item.
 * @return The number of columns.
 */
int ModelPart::columnCount() const {
    /* Count number of columns (properties) that this item has.
     */
    return m_itemData.count();
}

/**
 * @brief ModelPart::data
 * Returns the data associated with the specified column.
 * @param column The column index.
 * @return The data for the column, or a null QVariant if the column is invalid.
 */
QVariant ModelPart::data(int column) const {
    /* Return the data associated with a column of this item
     * Note on the QVariant type - it is a generic placeholder type
     * that can take on the type of most Qt classes. It allows each
     * column or property to store data of an arbitrary type.
     */
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

/**
 * @brief ModelPart::set
 * Sets the data associated with the specified column.
 * @param column The column index.
 * @param value The data to set.
 */
void ModelPart::set(int column, const QVariant &value) {
    /* Set the data associated with a column of this item
     */
    if (column < 0 || column >= m_itemData.size())
        return;

    m_itemData.replace(column, value);
}

/**
 * @brief ModelPart::parentItem
 * Returns the parent item.
 * @return Pointer to the parent item.
 */
ModelPart* ModelPart::parentItem() {
    return m_parentItem;
}

/**
 * @brief ModelPart::row
 * Returns the row index of this item relative to its parent.
 * @return The row index.
 */
int ModelPart::row() const {
    /* Return the row index of this item, relative to it's parent.
     */
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModelPart*>(this));
    return 0;
}

/**
 * @brief ModelPart::setColour
 * Sets the color of the actor.
 * @param R The red component of the color (0-255).
 * @param G The green component of the color (0-255).
 * @param B The blue component of the color (0-255).
 */
void ModelPart::setColour(const unsigned char R, const unsigned char G, const unsigned char B) {
    if (actor) {
        actor->GetProperty()->SetColor(R / 255.0, G / 255.0, B / 255.0);
    }
}

/**
 * @brief ModelPart::propagateColour
 * Propagates the color to this item and all its children.
 * @param R The red component of the color (0-255).
 * @param G The green component of the color (0-255).
 * @param B The blue component of the color (0-255).
 */
void ModelPart::propagateColour(unsigned char R, unsigned char G, unsigned char B) {
    setColour(R, G, B);

    for (ModelPart* child : m_childItems) {
        child->propagateColour(R, G, B);
    }
}

/**
 * @brief ModelPart::getColourR
 * Returns the red component of the actor's color.
 * @return The red component of the color.
 */
unsigned char ModelPart::getColourR() {
    /* This is a placeholder function that you will need to modify if you want to use it */

    /* As the name suggests ... */
    return 0;   // needs updating
}

/**
 * @brief ModelPart::getColourG
 * Returns the green component of the actor's color.
 * @return The green component of the color.
 */
unsigned char ModelPart::getColourG() {
    /* This is a placeholder function that you will need to modify if you want to use it */

    /* As the name suggests ... */
    return 0;   // needs updating
}

/**
 * @brief ModelPart::getColourB
 * Returns the blue component of the actor's color.
 * @return The blue component of the color.
 */
unsigned char ModelPart::getColourB() {
    /* This is a placeholder function that you will need to modify if you want to use it */

    /* As the name suggests ... */
    return 0;   // needs updating
}

/**
 * @brief ModelPart::setVisible
 * Sets the visibility of the actor.
 * @param visible True to show the actor, false to hide it.
 */
void ModelPart::setVisible(bool visible) {
    isVisible = visible;
    if (actor) {
        actor->SetVisibility(visible);
    }
}

/**
 * @brief ModelPart::visible
 * Returns the visibility status of the actor.
 * @return True if the actor is visible, false otherwise.
 */
bool ModelPart::visible() {
    return isVisible;
}

/**
 * @brief ModelPart::loadSTL
 * Loads an STL file and creates the corresponding VTK pipeline.
 * @param fileName The name of the STL file.
 */
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

/**
  * @brief ModelPart::setData
  * Sets the data for a specified column.
  * @param column The column index.
  * @param value The data to set for the column.
  * @return True if the data was successfully set, false otherwise.
  */
bool ModelPart::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= m_itemData.size())
        return false;

    m_itemData[column] = value;
    return true;
}

/**
 * @brief ModelPart::propagateVisibility
 * Propagates visibility changes to this item and all its children.
 * @param visible The visibility status to set.
 */
void ModelPart::propagateVisibility(bool visible) {
    setVisible(visible);
    set(1, visible ? "true" : "false");
    for (ModelPart* child : m_childItems) {
        child->propagateVisibility(visible);
    }
}

/**
 * @brief ModelPart::propagateClip
 * Propagates clipping changes to this item and all its children.
 * @param clip The clipping status to set.
 */
void ModelPart::propagateClip(bool clip) {
    set(3, clip ? "true" : "false");
    for (ModelPart* child : m_childItems) {
        child->propagateClip(clip);
    }
}

/**
 * @brief ModelPart::propagateShrink
 * Propagates shrinking changes to this item and all its children.
 * @param shrink The shrink status to set.
 */
void ModelPart::propagateShrink(bool shrink) {
    set(2, shrink ? "true" : "false");
    for (ModelPart* child : m_childItems) {
        child->propagateShrink(shrink);
    }
}

/**
 * @brief ModelPart::getActor
 * Returns the actor associated with this model part.
 * @return A pointer to the vtkActor.
 */
vtkSmartPointer<vtkActor> ModelPart::getActor() {
    return actor;
}

//vtkActor* ModelPart::getNewActor() {
/* This is a placeholder function that you will need to modify if you want to use it
     * * The default mapper/actor combination can only be used to render the part in
     * the GUI, it CANNOT also be used to render the part in VR. This means you need
     * to create a second mapper/actor combination for use in VR - that is the role
     * of this function. */


/* 1. Create new mapper */

/* 2. Create new actor and link to mapper */

/* 3. Link the vtkProperties of the original actor to the new actor. This means
      * if you change properties of the original part (colour, position, etc), the
      * changes will be reflected in the GUI AND VR rendering.
      * * See the vtkActor documentation, particularly the GetProperty() and SetProperty()
      * functions.
      */


/* The new vtkActor pointer must be returned here */
//    return nullptr;

//}

/**
 * @brief ModelPart::applyShrink
 * Applies a shrink filter to the model part.
 * @param enable True to enable the shrink filter, false to disable it.
 */
void ModelPart::applyShrink(bool enable) {
    if (!shrinkFilter) return;
    shrinkFilter->SetShrinkFactor(enable ? 0.7 : 1.0);
    shrinkFilter->Modified();
    shrinkFilter->Update();
}

/**
  * @brief ModelPart::applyClip
  * Applies a clip filter to the model part.
  * @param enable True to enable the clip filter, false to disable it.
  */
void ModelPart::applyClip(bool enable) {
    if (!clipFilter) return;
    clipFilter->SetInsideOut(false);
    clipFilter->Modified();
    clipFilter->Update();
}

}

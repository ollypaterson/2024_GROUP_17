/** @file ModelPart.h
  *
  * EEEE2076 - Software Engineering & VR Project
  *
  * Template for model parts that will be added as treeview items
  *
  * P Evans 2022
  */

#ifndef VIEWER_MODELPART_H
#define VIEWER_MODELPART_H

#include <QString>
#include <QList>
#include <QVariant>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkShrinkPolyData.h>
#include <vtkClipPolyData.h>


/* VTK headers - will be needed when VTK used in next worksheet,
 * commented out for now
 *
 * Note that there are a few function definitions and variables
 * commented out below - this is because you haven't yet installed
 * the VTK library which is needed.
 */
//#include <vtkSmartPointer.h>
//#include <vtkMapper.h>
//#include <vtkActor.h>
//#include <vtkSTLReader.h>
//#include <vtkColor.h>

/**
 * @class ModelPart
 * @brief Represents a model part in the CAD viewer application.
 *
 * This class stores information about a model part, such as its properties,
 * hierarchy, and rendering attributes. It is used to manage and display
 * parts of a 3D model in a treeview and the 3D scene.
 */
class ModelPart {
public:
    /**
     * @brief Constructor for the ModelPart class.
     * @param data List of properties for this item (part name and visibility).
     * @param parent The parent item in the tree.
     */
    ModelPart(const QList<QVariant>& data, ModelPart* parent = nullptr);

    /**
     * @brief Destructor for the ModelPart class.
     *
     * Frees the memory allocated for child items.
     */
    ~ModelPart();

    /**
     * @brief Adds a child item to this item.
     * @param item Pointer to the child item to add.
     */
    void appendChild(ModelPart* item);

    /**
     * @brief Returns the child item at the specified row.
     * @param row The row number of the child item.
     * @return Pointer to the child item, or nullptr if the row is invalid.
     */
    ModelPart* child(int row);

    /**
     * @brief Returns the number of children of this item.
     * @return The number of children.
     */
    int childCount() const;

    /**
     * @brief Returns the number of data items (columns) for this item.
     * @return The number of data columns.
     */
    int columnCount() const;

    /**
     * @brief Returns the data item at the specified column.
     * @param column The column index.
     * @return The data item at the specified column.
     */
    QVariant data(int column) const;

    /**
      * @brief Sets the data item at a specified column.
      * @param column The index of the property to set
      * @param value The value to apply
      * @return  True if the data was set, false otherwise
      */
    bool setData( int column, const QVariant& value );


    /**
     * @brief Returns the parent item.
     * @return Pointer to the parent item.
     */
    ModelPart* parentItem();

    /**
     * @brief Returns the row index of this item relative to its parent.
     * @return The row index.
     */
    int row() const;

    /**
      * @brief Sets the color of the model part.
      * @param R Red component (0-255).
      * @param G Green component (0-255).
      * @param B Blue component (0-255).
      */
    void setColour(const unsigned char R, const unsigned char G, const unsigned char B);

    /**
      * @brief Gets the red component of the model part's color.
      * @return Red component (0-255).
      */
    unsigned char getColourR();
    /**
      * @brief Gets the green component of the model part's color.
      * @return Green component (0-255).
      */
    unsigned char getColourG();
    /**
      * @brief Gets the blue component of the model part's color.
      * @return Blue component (0-255).
      */
    unsigned char getColourB();

    /**
      * @brief Sets the visibility of the model part.
      * @param isVisible True to make the part visible, false otherwise.
      */
    void setVisible(bool isVisible);

    /**
      * @brief Gets the visibility of the model part.
      * @return True if the part is visible, false otherwise.
      */
    bool visible();

    /**
      * @brief Loads the geometry of the model part from an STL file.
      * @param fileName The name of the STL file.
      */
    void loadSTL(QString fileName);

    /**
      * @brief Returns the actor used to render this model part.
      * @return Pointer to the vtkActor.
      */
    vtkSmartPointer<vtkActor> getActor();

    /**
      * @brief Returns a new actor for use in VR rendering.
      * @return Pointer to the new vtkActor.
      */
    vtkActor* getNewActor();

    /**
      * @brief Propagates visibility changes to this item and its children.
      * @param visible The new visibility state.
      */
    void propagateVisibility(bool visible);
    /**
      * @brief Propagates clipping changes to this item and its children.
      * @param clip The new clipping state.
      */
    void propagateClip(bool clip);
    /**
      * @brief Propagates shrinking changes to this item and its children.
      * @param shrink The new shrink state.
      */
    void propagateShrink(bool shrink);

    /**
      * @brief Propagates colour changes to this item and its children.
      * @param R Red component (0-255).
      * @param G Green component (0-255).
      * @param B Blue component (0-255).
      */
    void propagateColour(unsigned char R, unsigned char G, unsigned char B);

    /**
      * @brief Applies a shrink filter to the model part.
      * @param enable True to enable the shrink filter, false to disable it.
      */
    void applyShrink(bool enable);
    /**
      * @brief Applies a clip filter to the model part.
      * @param enable True to enable the clip filter, false to disable it.
      */
    void applyClip(bool enable);


private:
    QList<ModelPart*>                           m_childItems;       /**< List (array) of child items */
    QList<QVariant>                             m_itemData;         /**< List (array) of column data for item */
    ModelPart* m_parentItem;       /**< Pointer to parent */
    bool                                        isVisible;          /**< True/false to indicate if should be visible in model rendering */

    /* These are vtk properties that will be used to load/render a model of this part,
     * commented out for now but will be used later
     */
    vtkSmartPointer<vtkSTLReader>               file;               /**< Datafile from which part loaded */
    vtkSmartPointer<vtkMapper>                  mapper;             /**< Mapper for rendering */
    vtkSmartPointer<vtkActor>                   actor;              /**< Actor for rendering */

    vtkSmartPointer<vtkShrinkPolyData>          shrinkFilter;
    vtkSmartPointer<vtkClipPolyData>            clipFilter;
    vtkSmartPointer<vtkPlane>                   clipPlane;


    // vtkColor3<unsigned char>                    colour;             /**< User defineable colour */
};


#endif



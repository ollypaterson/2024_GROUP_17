#ifndef VIEWER_MODELPART_H
#define VIEWER_MODELPART_H

#include <QString>
#include <QList>
#include <QVariant>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

class ModelPart {
public:
    /** Constructor
     * @param data is a List (array) of strings for each property of this item (part name and visibility in our case)
     * @param parent is the parent of this item (one level up in tree)
     */
    ModelPart(const QList<QVariant>& data, ModelPart* parent = nullptr);

    /** Destructor
     * Needs to free array of child items
     */
    ~ModelPart();

    /** Add a child to this item.
     * @param item Pointer to child object (must already be allocated using new)
     */
    void appendChild(ModelPart* item);

    /** Return child at position 'row' below this item
     * @param row is the row number (below this item)
     * @return pointer to the item requested.
     */
    ModelPart* child(int row);

    /** Return number of children to this item
     * @return number of children
     */
    int childCount() const;

    /** Get number of data items (2 - part name and visibility string) in this case.
     * @return number of visible data columns
     */
    int columnCount() const;

    /** Return the data item at a particular column for this item.
     * i.e. either part name or visibility
     * @param column is the column number required
     * @return the data in the column
     */
    QVariant data(int column) const;

    /** Return the parent of this item
     * @return the parent item
     */
    ModelPart* parent();

     /** Return actor
     * @return pointer to default actor for GUI rendering
     */
    vtkSmartPointer<vtkActor> getActor();

    /** Return new actor for use in VR
     * @return pointer to new actor
     */
    vtkSmartPointer<vtkActor> getVRActor(); // 🔄 Added for VR rendering

    /** Sync GUI actor properties with VR actor
     * @param guiActor - actor from GUI thread
     */
    void linkProperties(vtkSmartPointer<vtkActor> guiActor); // 🔄 Added for property syncing

    bool setData(int column, const QVariant &value);

private:
    QList<ModelPart*>                           m_childItems;       /**< List (array) of child items */
    QList<QVariant>                             m_itemData;         /**< List (array of column data for item) */
    ModelPart* m_parentItem;       /**< Pointer to parent */

    /* These are some typical properties that I think the part will need, you might
     * want to add your own.
     */
    bool                                        isVisible;          /**< True/false to indicate if should be visible in model rendering */

    /* These are VTK properties used to load/render a model of this part */
    vtkSmartPointer<vtkSTLReader>               stlReader;          /**< Datafile from which part is loaded */
    vtkSmartPointer<vtkPolyDataMapper>          mapper;             /**< Mapper for rendering */
    vtkSmartPointer<vtkActor>                   actor;              /**< Actor for rendering */
};

#endif // VIEWER_MODELPART_H

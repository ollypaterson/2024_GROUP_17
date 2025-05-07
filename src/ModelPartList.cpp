/**
  * @file ModelPartList.cpp
  * @brief Implementation of the ModelPartList class.
  *
  * EEEE2076 - Software Engineering & VR Project
  *
  * Template for model part list that will be used to create the trewview.
  *
  * P Evans 2022
  */

#include "ModelPartList.h"
#include "ModelPart.h"

/**
 * @brief ModelPartList::ModelPartList
 * @param data The header data for the tree view.
 * @param parent The parent object.
 */
ModelPartList::ModelPartList(const QString& data, QObject* parent) : QAbstractItemModel(parent) {
    /* Have option to specify number of visible properties for each item in tree - the root item
     * acts as the column headers
     */
    rootItem = new ModelPart({tr("Part"), tr("Visible?"), tr("Shrink?"), tr("Clip?")});
}

/**
 * @brief ModelPartList::~ModelPartList
 * Destructor for the ModelPartList class.
 */
ModelPartList::~ModelPartList() {
    delete rootItem;
}

/**
 * @brief ModelPartList::columnCount
 * Returns the number of columns for the given parent index.
 * @param parent The parent index.
 * @return The number of columns.
 */
int ModelPartList::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return rootItem->columnCount();
}

/**
 * @brief ModelPartList::data
 * Returns the data for the item at the given index and role.
 * @param index The index of the item.
 * @param role The role of the data.
 * @return The data for the item at the given index and role.
 */
QVariant ModelPartList::data(const QModelIndex& index, int role) const {
    /* If the item index isn't valid, return a new, empty QVariant (QVariant is generic datatype
     * that could be any valid QT class) */
    if (!index.isValid())
        return QVariant();

    /* Role represents what this data will be used for, we only need deal with the case
     * when QT is asking for data to create and display the treeview. Return a new,
     * empty QVariant if any other request comes through. */
    if (role != Qt::DisplayRole)
        return QVariant();

    /* Get a pointer to the item referred to by the QModelIndex */
    ModelPart* item = static_cast<ModelPart*>(index.internalPointer());

    /* Each item in the tree has a number of columns ("Part" and "Visible" in this
     * initial example) return the column requested by the QModelIndex */
    return item->data(index.column());
}

/**
 * @brief ModelPartList::flags
 * Returns the flags for the item at the given index.
 * @param index The index of the item.
 * @return The flags for the item at the given index.
 */
Qt::ItemFlags ModelPartList::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

/**
 * @brief ModelPartList::headerData
 * Returns the header data for the given section, orientation, and role.
 * @param section The section number.
 * @param orientation The orientation of the header.
 * @param role The role of the data.
 * @return The header data for the given section, orientation, and role.
 */
QVariant ModelPartList::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

/**
 * @brief ModelPartList::index
 * Returns the index for the item in the given row and column under the given parent.
 * @param row The row number.
 * @param column The column number.
 * @param parent The parent index.
 * @return The index for the item in the given row and column under the given parent.
 */
QModelIndex ModelPartList::index(int row, int column, const QModelIndex& parent) const {
    ModelPart* parentItem;

    if (!parent.isValid() || !hasIndex(row, column, parent))
        parentItem = rootItem;              // default to selecting root
    else
        parentItem = static_cast<ModelPart*>(parent.internalPointer());

    ModelPart* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

/**
 * @brief ModelPartList::parent
 * Returns the parent index of the item at the given index.
 * @param index The index of the item.
 * @return The parent index of the item at the given index.
 */
QModelIndex ModelPartList::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();

    ModelPart* childItem = static_cast<ModelPart*>(index.internalPointer());
    ModelPart* parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

/**
 * @brief ModelPartList::rowCount
 * Returns the number of rows under the given parent index.
 * @param parent The parent index.
 * @return The number of rows under the given parent index.
 */
int ModelPartList::rowCount(const QModelIndex& parent) const {
    ModelPart* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ModelPart*>(parent.internalPointer());

    return parentItem->childCount();
}

/**
 * @brief ModelPartList::getRootItem
 * Returns the root item of the model.
 * @return The root item of the model.
 */
ModelPart* ModelPartList::getRootItem() {
    return rootItem;
}

/**
 * @brief ModelPartList::appendChild
 * Appends a child to the parent
 * @param parent the parent item
 * @param data the data for the child
 * @return QModelIndex
 */
QModelIndex ModelPartList::appendChild(QModelIndex& parent, const QList<QVariant>& data) {
    ModelPart* parentPart;

    if (parent.isValid())
        parentPart = static_cast<ModelPart*>(parent.internalPointer());
    else {
        parentPart = rootItem;
        parent = createIndex(0, 0, rootItem);
    }

    beginInsertRows(parent, rowCount(parent), rowCount(parent));

    ModelPart* childPart = new ModelPart(data, parentPart);

    parentPart->appendChild(childPart);

    QModelIndex child = createIndex(0, 0, childPart);

    endInsertRows();

    emit layoutChanged();

    return child;
}

/**
 * @brief ModelPartList::setData
 * Sets the data for the item at the given index and role.
 * @param index The index of the item.
 * @param value The new value for the data.
 * @param role The role of the data.
 * @return True if the data was set successfully, false otherwise.
 */
bool ModelPartList::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    ModelPart* item = static_cast<ModelPart*>(index.internalPointer());
    if (!item)
        return false;

    bool result = item->setData(index.column(), value);

    if (result) {
        emit dataChanged(index, index, {role});
        return true;
    }

    return false;
}


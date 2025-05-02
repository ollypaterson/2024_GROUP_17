/**     @file ModelPart.h
  *
  *     EEEE2076 - Software Engineering & VR Project
  *
  *     Template for model parts that will be added as treeview items
  *
  *     P Evans 2022
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
#include <vtkColor.h>
#include <vtkProperty.h>

class ModelPart {
public:
    ModelPart(const QList<QVariant>& data, ModelPart* parent = nullptr);
    ~ModelPart();

    void appendChild(ModelPart* item);
    ModelPart* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void set(int column, const QVariant& value);
    ModelPart* parentItem();
    int row() const;

    void setColour(const unsigned char R, const unsigned char G, const unsigned char B);
    unsigned char getColourR();
    unsigned char getColourG();
    unsigned char getColourB();

    void setVisible(bool isVisible);
    bool visible();
	
    void loadSTL(QString fileName);

    /** Return actor for GUI rendering
      * @return pointer to actor
      */
    vtkSmartPointer<vtkActor> getActor();

    /** Create new actor for VR rendering
      * @return pointer to VR actor
      */
    vtkSmartPointer<vtkActor> getVRActor();

    /** Link visual properties from GUI actor to VR actor
      * @param guiActor is the actor used in GUI
      */
    void linkProperties(vtkSmartPointer<vtkActor> guiActor);

private:
    QList<ModelPart*> m_childItems;
    QList<QVariant> m_itemData;
    ModelPart* m_parentItem;

    bool isVisible;

    vtkSmartPointer<vtkSTLReader> stlReader;            /**< STL file reader */
    vtkSmartPointer<vtkPolyDataMapper> mapper;          /**< Mapper for GUI rendering */
    vtkSmartPointer<vtkActor> actor;                    /**< Actor for GUI rendering */
    vtkColor3<unsigned char> colour;                    /**< RGB colour */
};

#endif // VIEWER_MODELPART_H

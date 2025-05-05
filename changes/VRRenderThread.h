#ifndef VRRENDERTHREAD_H
#define VRRENDERTHREAD_H

#include <QThread>
#include <vtkSmartPointer.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <QList> // Include for QList
#include <QMutex>
#include <QWaitCondition>

class VRRenderThread : public QThread {
    Q_OBJECT

public:
    VRRenderThread(QObject *parent = nullptr);
    ~VRRenderThread();

    void setRenderWindowAndRenderer(vtkSmartPointer<vtkOpenVRRenderWindow> rw, vtkSmartPointer<vtkRenderer> ren);
    void addActor(vtkSmartPointer<vtkActor> actor); // Adds an actor to the VR renderer
    void removeActor(vtkSmartPointer<vtkActor> actor);
    void setVRActors(const QList<vtkSmartPointer<vtkActor>>& actors); // Set list of actors
    void stopVR(); // Stops the VR rendering thread

protected:
    void run() override;

private:
    vtkSmartPointer<vtkOpenVRRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> renderer;
    bool running;
    QList<vtkSmartPointer<vtkActor>> vrActors; // Store actor pointers
    QMutex *mutex;
    QWaitCondition *waitCondition;
};

#endif // VRRENDERTHREAD_H

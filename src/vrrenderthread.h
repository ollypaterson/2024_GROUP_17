/*

#ifndef VRRENDERTHREAD_H
#define VRRENDERTHREAD_H

#include <QThread>  // ✅ Required for QThread base class

// Forward declarations
template <class T> class vtkSmartPointer;
class vtkOpenVRRenderWindow;
class vtkRenderer;
class vtkActor;

class VRRenderThread : public QThread {
    Q_OBJECT

public:
    VRRenderThread(QObject *parent = nullptr);
    ~VRRenderThread();

    void addActor(vtkSmartPointer<vtkActor> actor); // Adds an actor to the VR renderer
    void stopVR(); // Stops the VR rendering thread

protected:
    void run() override;

private:
    vtkSmartPointer<vtkOpenVRRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> renderer;
    bool running;
};

#endif // VRRENDERTHREAD_H
*/

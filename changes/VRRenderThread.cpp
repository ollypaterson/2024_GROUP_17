#include "VRRenderThread.h"
#include <vtkSmartPointer.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkCamera.h> // Include for camera manipulation
#include <QMutex>       // Include for thread synchronization
#include <QWaitCondition>

VRRenderThread::VRRenderThread(QObject *parent)
    : QThread(parent), running(true), renderWindow(nullptr), renderer(nullptr) // Initialize pointers
{
     // Initialize mutex and wait condition
    mutex = new QMutex();
    waitCondition = new QWaitCondition();
}

VRRenderThread::~VRRenderThread()
{
    stopVR();
    wait(); // Ensure thread has stopped
    delete mutex;
    delete waitCondition;
}

void VRRenderThread::setRenderWindowAndRenderer(vtkSmartPointer<vtkOpenVRRenderWindow> rw, vtkSmartPointer<vtkRenderer> ren)
{
    renderWindow = rw;
    renderer = ren;
}

void VRRenderThread::addActor(vtkSmartPointer<vtkActor> actor)
{
    QMutexLocker locker(mutex); // Protect access to the renderer
    if (renderer) {
        renderer->AddActor(actor);
    }
}

void VRRenderThread::removeActor(vtkSmartPointer<vtkActor> actor)
{
     QMutexLocker locker(mutex); // Protect access to the renderer
    if (renderer) {
         renderer->RemoveActor(actor);
    }
}

void VRRenderThread::setVRActors(const QList<vtkSmartPointer<vtkActor>>& actors)
{
    QMutexLocker locker(mutex); // Protect access to the renderer
    this->vrActors = actors; // Store the actor list
    if (renderer)
    {
        renderer->RemoveAllViewProps();
        for (const auto& actor : vrActors) {
            renderer->AddActor(actor);
        }
    }
     // Wake up the thread to re-render
    waitCondition->wakeAll();
}

void VRRenderThread::stopVR()
{
    QMutexLocker locker(mutex);
    running = false;
    waitCondition->wakeAll(); // Wake up the thread to stop it
}

void VRRenderThread::run()
{
    if (!renderWindow || !renderer) {
        qCritical() << "VRRenderThread started without a valid renderWindow or renderer!";
        return;
    }
    renderWindow->Initialize();
    renderWindow->MakeCurrent();


    // Main rendering loop
    while (running) {
        {
            QMutexLocker locker(mutex);
             if (!running)
                break;
            renderWindow->Render();
            // Optionally, you can add a small sleep here to control the frame rate
            // usleep(16000); // Sleep for approximately 60 FPS (16ms) -  Consider QThread::usleep
            QThread::usleep(16000);
        }
    }
    renderWindow->Finalize();
}

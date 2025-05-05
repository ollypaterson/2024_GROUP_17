/*

#include "VRRenderThread.h"
#include <vtkSmartPointer.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>



VRRenderThread::VRRenderThread(QObject *parent)
    : QThread(parent), running(true) {
    renderWindow = vtkSmartPointer<vtkOpenVRRenderWindow>::New();
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);
}

VRRenderThread::~VRRenderThread() {
    stopVR();
}

void VRRenderThread::addActor(vtkSmartPointer<vtkActor> actor) {
    renderer->AddActor(actor);
}

void VRRenderThread::stopVR() {
    running = false;
}

void VRRenderThread::run() {
    renderWindow->Initialize();
    renderWindow->Render();

    while (running) {
        renderWindow->Render();
    }

    renderWindow->Finalize();
}
*/

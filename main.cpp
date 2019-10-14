#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "K_EGLRenderWindow.h"


int main(){

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(1.0, 0.0, 0.0);

    vtkSmartPointer<K_EGLRenderWindow> renderWindow = vtkSmartPointer<K_EGLRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->Render();


    std::cout << "Hello World" << std::endl;
    return 0;
}
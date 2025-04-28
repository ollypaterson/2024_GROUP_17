// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#ifndef vtkRenderingVRModelsObjectFactory_h
#define vtkRenderingVRModelsObjectFactory_h

#include "vtkRenderingVRModelsModule.h" // For export macro
#include "vtkObjectFactory.h"

VTK_ABI_NAMESPACE_BEGIN

class VTKRENDERINGVRMODELS_EXPORT vtkRenderingVRModelsObjectFactory : public vtkObjectFactory
{
public:
  static vtkRenderingVRModelsObjectFactory * New();
  vtkTypeMacro(vtkRenderingVRModelsObjectFactory, vtkObjectFactory);

  const char * GetDescription() override { return "vtkRenderingVRModels factory overrides."; }

  const char * GetVTKSourceVersion() override;

  void PrintSelf(ostream &os, vtkIndent indent) override;

protected:
  vtkRenderingVRModelsObjectFactory();

private:
  vtkRenderingVRModelsObjectFactory(const vtkRenderingVRModelsObjectFactory&) = delete;
  void operator=(const vtkRenderingVRModelsObjectFactory&) = delete;
};

VTK_ABI_NAMESPACE_END

#endif // vtkRenderingVRModelsObjectFactory_h

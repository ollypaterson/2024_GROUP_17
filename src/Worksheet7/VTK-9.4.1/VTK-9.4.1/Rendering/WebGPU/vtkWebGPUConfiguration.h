// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkWebGPUConfiguration
 * @brief   Create a webgpu device for use in rendering and compute pipelines.
 *
 * This class creates and maintains a single WGPUInstance object. The adapter and device
 * are created off the WGPUInstance object. It also allows selection of the graphics backend
 * and the power preference of the graphics adapter.
 *
 * @sa
 * vtkWebGPURenderWindow, vtkWebGPUComputePipeline
 */

#ifndef vtkWebGPUConfiguration_h
#define vtkWebGPUConfiguration_h

#include "vtkObject.h"

#include "vtkCommand.h"               // for vtkCommand
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtk_wgpu.h"                 // for wgpu

#include <memory> // for unique_ptr

VTK_ABI_NAMESPACE_BEGIN

class vtkWebGPURenderWindow;
class vtkWebGPUComputePipeline;
class vtkWebGPUConfigurationInternals;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUConfiguration : public vtkObject
{
public:
  static vtkWebGPUConfiguration* New();
  vtkTypeMacro(vtkWebGPUConfiguration, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * These enums have a one-one correspondence with the webgpu enums.
   * They are here so that wrapped languages can make use of them.
   */
  enum class PowerPreferenceType
  {
    Undefined,
    LowPower,
    HighPerformance
  };

  enum class BackendType
  {
    Undefined,
    Null,
    WebGPU,
    D3D11,
    D3D12,
    Metal,
    Vulkan,
    OpenGL,
    OpenGLES
  };

  ///@{
  /**
   * Set/Get the power preference of the graphics adapter.
   * Available options are:
   * - `Undefined`
   * - `LowPower`
   * - `HighPerformance`.
   * The default value is `HighPerformance`.
   */
  vtkSetEnumMacro(PowerPreference, PowerPreferenceType);
  vtkGetEnumMacro(PowerPreference, PowerPreferenceType);
  ///@}

  ///@{
  /**
   * Set/Get the graphics backend to use from the graphics adapter.
   * Available options are:
   * - `Undefined`
   * - `Null`
   * - `WebGPU`
   * - `D3D11`
   * - `D3D12`
   * - `Metal`
   * - `Vulkan`
   * - `OpenGL`
   * - `OpenGLES`
   * The default value is determined at runtime based on the operating system.
   */
  vtkSetEnumMacro(Backend, BackendType);
  vtkGetEnumMacro(Backend, BackendType);
  ///@}

  ///@{
  /**
   * Set/Get the timeout. It represents the number of milliseconds
   * to wait for device initialization.
   * Due to the asynchronous nature of WebGPU adapter and device requests,
   * the `Initialize()` method repeatedly processes pending events
   * in the WebGPU instance until the device is ready. Use this timeout to prevent
   * hangs because of the infinite loop.
   */
  vtkSetMacro(Timeout, double);
  vtkGetMacro(Timeout, double);
  ///@}

  /**
   * Use this method to customize the default value for Timeout.
   */
  static void SetDefaultTimeout(double);

  /**
   * Get the backend in use. This is useful to know when the `Backend` is set to `Undefined`.
   * When the `Backend` is set to `Undefined`, the webgpu implementation library (ex: dawn,
   * wgpu-native) automatically select the best supported graphics backend. Usually, this is Metal
   * on Apple hardware, Vulkan on Linux/Android and D3D12 on Windows.
   */
  BackendType GetBackendInUse();

  /**
   * Convenient function returns a string representation of the currently used backend.
   */
  std::string GetBackendInUseAsString();

  ///@{
  /**
   * Get handles of the WGPU adapter/device/instance.
   */
  wgpu::Adapter GetAdapter();
  wgpu::Device GetDevice();
  wgpu::Instance GetInstance();
  ///@}

  /**
   * Initializes the class.
   * 1. This method creates a `WGPUInstance`.
   *    If a `WGPUInstance` was already created by a previous call to `Initialize`
   *    without a `Finalize` from another instance of `vtkWebGPUConfiguration` then this method
   *    adds a reference to the existing `WGPUInstance`.
   * 2. After a `WGPUInstance` is available, it requests a `WGPUAdapter`
   *    with the specified `Backend` and `PowerPreference`.
   * 3. Finally, it requests a `WGPUDevice` from the `WGPUAdapter`.
   *
   * NOTE: This method blocks until a device is ready. Due to the asynchronous nature of the WebGPU
   *       API, we must block by processing webgpu events until the device is ready because VTK
   *       right now does not have a concept of an event loop. You can specify a timeout in
   *       milliseconds by calling SetTimeout().
   * @sa SetTimeout(), GetTimeout()
   */
  bool Initialize();

  /**
   * Finalizes the class.
   * This method destroys the device, adapter and releases the reference to `WGPUInstance` if not
   * already done.
   */
  void Finalize();

  /**
   * Process events on the `WGPUInstance`.
   *
   * NOTE: In WebAssembly, this method needs the application to be compiled with `-sASYNCIFY=1` or
   *       `-sJSPI=1` for `emscripten_sleep` to yield to the browsers run loop for processing
   *       webgpu callbacks.
   */
  void ProcessEvents();

  /**
   * Adjusts a given value to the nearest multiple of the specified alignment.
   * It is useful for ensuring that memory addresses or other data sizes adhere to
   * alignment requirements in webgpu, which can be critical for performance and correctness
   * of shaders.
   */
  static std::size_t Align(std::size_t value, std::size_t alignment);

  enum
  {
    AdapterRequestCompletedEvent = vtkCommand::UserEvent,
    DeviceRequestCompletedEvent,
  };

protected:
  vtkWebGPUConfiguration();
  ~vtkWebGPUConfiguration() override;

  PowerPreferenceType PowerPreference = PowerPreferenceType::HighPerformance;
  // Initialized in constructor at runtime based on the operating system.
  BackendType Backend;
  // In milliseconds
  double Timeout;

  void AcquireAdapter(vtkObject* caller, unsigned long event, void* calldata);

  void AcquireDevice(vtkObject* caller, unsigned long event, void* calldata);

private:
  vtkWebGPUConfiguration(const vtkWebGPUConfiguration&) = delete;
  void operator=(const vtkWebGPUConfiguration&) = delete;

  friend class vtkWebGPUConfigurationInternals;
  std::unique_ptr<vtkWebGPUConfigurationInternals> Internals;
};

VTK_ABI_NAMESPACE_END

#endif

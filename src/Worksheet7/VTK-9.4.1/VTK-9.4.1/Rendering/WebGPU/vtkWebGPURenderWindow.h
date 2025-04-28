// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#ifndef vtkWebGPURenderWindow_h
#define vtkWebGPURenderWindow_h

#include "vtkRenderWindow.h"

#include "vtkRenderingWebGPUModule.h"      // for export macro
#include "vtkTypeUInt8Array.h"             // for ivar
#include "vtkWebGPUComputePipeline.h"      // for the compute pipelines of this render window
#include "vtkWebGPUComputeRenderTexture.h" // for compute render textures
#include "vtk_wgpu.h"                      // for webgpu

VTK_ABI_NAMESPACE_BEGIN

class vtkWebGPUComputeOcclusionCuller;
class vtkWebGPUConfiguration;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPURenderWindow : public vtkRenderWindow
{
public:
  vtkTypeMacro(vtkWebGPURenderWindow, vtkRenderWindow);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Concrete render windows must create a platform window and initialize this->WindowId.
   * Upon success, please call WGPUInit().
   */
  virtual bool WindowSetup() = 0;

  /**
   * Create a not-off-screen window.
   */
  virtual void CreateAWindow() = 0;

  /**
   * Destroy a not-off-screen window.
   */
  virtual void DestroyWindow() = 0;

  /**
   * Creates the WebGPU context, swapchain, depth buffer, color attachment, ...
   *
   * A call to this function is necessary if a WebGPU Compute class is going to use resources of the
   * render window or its renderers (or resources of the renderers, mappers for example).
   * This is because the resources of the render window are only made available when Initialize() is
   * called.
   *
   * For example, a vtkWebGPUOcclusionCuller (which uses textures of the render window to do the
   * culling) cannot be setup without these resources being available, hence the need for the
   * Initialize() call.
   */
  void Initialize() override;

  void Start() override;

  /**
   * Update the system, if needed, at end of render process
   */
  void End() override;

  /**
   * Handle opengl specific code and calls superclass
   */
  void Render() override;

  /**
   * Intermediate method performs operations required between the rendering
   * of the left and right eye.
   */
  void StereoMidpoint() override;
  void Frame() override;

  const char* GetRenderingBackend() override;

  /**
   * Reads pixels into the `CachedPixelBytes` variable.
   */
  void ReadPixels();

  ///@{
  /**
   * Set/Get the pixel data of an image, transmitted as RGBRGB...
   * front in this context indicates that the read should come from the
   * display buffer versus the render buffer
   */
  unsigned char* GetPixelData(int x, int y, int x2, int y2, int front, int right) override;
  int GetPixelData(
    int x, int y, int x2, int y2, int front, vtkUnsignedCharArray* data, int right) override;
  int SetPixelData(
    int x, int y, int x2, int y2, unsigned char* data, int front, int right) override;
  int SetPixelData(
    int x, int y, int x2, int y2, vtkUnsignedCharArray* data, int front, int right) override;
  ///@}

  ///@{
  /**
   * Set/Get the pixel data of an image, transmitted as RGBARGBA...
   */
  float* GetRGBAPixelData(int x, int y, int x2, int y2, int front, int right = 0) override;
  int GetRGBAPixelData(
    int x, int y, int x2, int y2, int front, vtkFloatArray* data, int right = 0) override;
  int SetRGBAPixelData(
    int x, int y, int x2, int y2, float* data, int front, int blend = 0, int right = 0) override;
  int SetRGBAPixelData(int x, int y, int x2, int y2, vtkFloatArray* data, int front, int blend = 0,
    int right = 0) override;
  void ReleaseRGBAPixelData(float* data) override;
  unsigned char* GetRGBACharPixelData(
    int x, int y, int x2, int y2, int front, int right = 0) override;
  int GetRGBACharPixelData(
    int x, int y, int x2, int y2, int front, vtkUnsignedCharArray* data, int right = 0) override;
  int SetRGBACharPixelData(int x, int y, int x2, int y2, unsigned char* data, int front,
    int blend = 0, int right = 0) override;
  int SetRGBACharPixelData(int x, int y, int x2, int y2, vtkUnsignedCharArray* data, int front,
    int blend = 0, int right = 0) override;
  ///@}

  ///@{
  /**
   * Set/Get the zbuffer data from an image
   */
  float* GetZbufferData(int x1, int y1, int x2, int y2) override;
  int GetZbufferData(int x1, int y1, int x2, int y2, float* z) override;
  int GetZbufferData(int x1, int y1, int x2, int y2, vtkFloatArray* buffer) override;
  int SetZbufferData(int x1, int y1, int x2, int y2, float* buffer) override;
  int SetZbufferData(int x1, int y1, int x2, int y2, vtkFloatArray* buffer) override;
  ///@}

  /**
   * Get the size of the color buffer.
   * Returns 0 if not able to determine otherwise sets R G B and A into buffer.
   */
  int GetColorBufferSizes(int* rgba) override;
  /**
   * Block the thread until work queue completes all submitted work.
   */
  void WaitForCompletion() override;

  /**
   * Does this render window support OpenGL? 0-false, 1-true
   */
  int SupportsOpenGL() override;

  /**
   * Get report of capabilities for the render window
   */
  const char* ReportCapabilities() override;

  /**
   * Initialize the render window from the information associated
   * with the currently activated OpenGL context.
   */
  bool InitializeFromCurrentContext() override;

  /**
   * Free up any graphics resources associated with this window
   * a value of NULL means the context may already be destroyed
   */
  void ReleaseGraphicsResources(vtkWindow*) override;

  void SetWGPUConfiguration(vtkWebGPUConfiguration* config);
  vtkGetSmartPointerMacro(WGPUConfiguration, vtkWebGPUConfiguration);

  /**
   * Create a new render pass encoder on the webgpu device.
   */
  wgpu::RenderPassEncoder NewRenderPass(wgpu::RenderPassDescriptor& descriptor);

  /**
   * Create a new render bundle encoder on the webgpu device. More performant for large number of
   * actors.
   */
  wgpu::RenderBundleEncoder NewRenderBundleEncoder(wgpu::RenderBundleEncoderDescriptor& descriptor);

  /**
   * Get the currently used command encoder. Use this to prepare draw commands which eventually
   * get submitted in `Frame()`
   */
  wgpu::CommandEncoder GetCommandEncoder();

  /**
   * Initializes a new command encoder
   */
  void CreateCommandEncoder();

  /**
   * Sends a given command buffer to the device queue
   */
  void FlushCommandBuffers(vtkTypeUInt32 count, wgpu::CommandBuffer* buffers);

  /**
   * Get a view of the color attachment used in the offscreen render target.
   */
  wgpu::TextureView GetOffscreenColorAttachmentView();

  /**
   * Get a view of the depth-stencil attachment used in the offscreen render target.
   */
  wgpu::TextureView GetDepthStencilView();

  /**
   * Get the texture format of the depth-stencil attachment.
   */
  wgpu::TextureFormat GetDepthStencilFormat();

  /**
   * Whether the offscreen render target has stencil capabilities.
   */
  bool HasStencil();

  /**
   * Get the webgpu device.
   */
  wgpu::Device GetDevice();

  /**
   * Get the webgpu adapter.
   */
  wgpu::Adapter GetAdapter();

  /**
   * Get the texture format preferred for the swapchain presentation.
   */
  wgpu::TextureFormat GetPreferredSwapChainTextureFormat();

  ///@{
  /**
   * Returns a vtkWebGPUComputeRenderTexture ready to be added to a compute pipeline using
   * vtkWebGPUComputePipeline::AddRenderTexture() that "points" to the depth buffer/color texture of
   * this vtkWebGPURenderWindow.
   *
   * One or multiple texture views will also need to be created for that render texture with a call
   * to CreateRenderTextureView(). The texture view returned can then be configured (group, binding,
   * ...) and added to the render window by calling AddRenderTextureView()
   */
  vtkSmartPointer<vtkWebGPUComputeRenderTexture> AcquireDepthBufferRenderTexture();

  vtkSmartPointer<vtkWebGPUComputeRenderTexture> AcquireFramebufferRenderTexture();
  ///@}

  /**
   * Creates a wgpu buffer with the device of this render window
   */
  wgpu::Buffer CreateDeviceBuffer(wgpu::BufferDescriptor& bufferDescriptor);

protected:
  vtkWebGPURenderWindow();
  ~vtkWebGPURenderWindow() override;

  /**
   * Construct the window title as "Visualization Toolkit - <WindowSystem> <GraphicsBackend>"
   * Ex: "Visualization Toolkit - X11 Vulkan"
   *     "Visualization Toolkit - X11 OpenGL"
   *     "Visualization Toolkit - Cocoa Metal"
   *     "Visualization Toolkit - Cocoa OpenGL"
   *     "Visualization Toolkit - Win32 D3D12"
   */
  virtual std::string MakeDefaultWindowNameWithBackend() = 0;

  bool WGPUInit();
  void WGPUFinalize();

  void CreateSwapChain();
  void DestroySwapChain();

  void CreateOffscreenColorAttachments();
  void DestroyOffscreenColorAttachments();

  void CreateDepthStencilTexture();
  void DestroyDepthStencilTexture();

  void CreateFSQGraphicsPipeline();
  void DestroyFSQGraphicsPipeline();

  void RecreateComputeRenderTextures();

  void RenderOffscreenTexture();

  bool RenderTexturesSetup = false;

  wgpu::Surface Surface;
  wgpu::CommandEncoder CommandEncoder;

  struct vtkWGPUSwapChain
  {
    wgpu::SwapChain Instance;
    wgpu::TextureView Framebuffer;
    wgpu::TextureFormat TexFormat;
    wgpu::PresentMode PresentMode;
    int Width = 0;
    int Height = 0;
  };
  vtkWGPUSwapChain SwapChain;

  struct vtkWGPUDeptStencil
  {
    wgpu::Texture Texture;
    wgpu::TextureView View;
    wgpu::TextureFormat Format;
    bool HasStencil;
  };
  vtkWGPUDeptStencil DepthStencil;

  struct vtkWGPUColorAttachment
  {
    wgpu::Texture Texture;
    wgpu::TextureView View;
    wgpu::TextureFormat Format;
    wgpu::Buffer OffscreenBuffer;
  };
  vtkWGPUColorAttachment ColorAttachment;

  struct vtkWGPUUserStagingPixelData
  {
    wgpu::Origin3D Origin;
    wgpu::Extent3D Extent;
    wgpu::TextureDataLayout Layout;
    wgpu::Buffer Buffer; // for SetPixelData
  };
  vtkWGPUUserStagingPixelData StagingPixelData;

  struct vtkWGPUFullScreenQuad
  {
    wgpu::RenderPipeline Pipeline;
    wgpu::BindGroup BindGroup;
  };

  vtkWGPUFullScreenQuad FSQ;

  struct MappingContext
  {
    vtkSmartPointer<vtkTypeUInt8Array> dst;
    wgpu::Buffer src;
    unsigned long size;
    vtkWebGPURenderWindow* window;
  } BufferMapReadContext;

  vtkNew<vtkTypeUInt8Array> CachedPixelBytes;
  vtkSmartPointer<vtkWebGPUConfiguration> WGPUConfiguration;

  int ScreenSize[2];

private:
  // For accessing SubmitCommandBuffer to submit custom prop render work
  friend class vtkWebGPUComputeOcclusionCuller;

  vtkWebGPURenderWindow(const vtkWebGPURenderWindow&) = delete;
  void operator=(const vtkWebGPURenderWindow&) = delete;

  /**
   * Sets up the compute pipeline of the vtkWebGPURenderer of this render window so that they use
   * the same Adapter and Device as this render window
   */
  void InitializeRendererComputePipelines();

  /**
   * Submits command buffers to the device queue. This allows the execution of additional custom
   * commands by the render window.
   */
  void SubmitCommandBuffer(int count, wgpu::CommandBuffer* commandBuffer);

  /**
   * Dispatches all the post-render compute pipelines of all the renderers of this render window
   */
  void PostRenderComputePipelines();

  /**
   * Renders actors of the renderers of this render window that were "deferred" to being rendered
   * after the main rasterization pass
   */
  void PostRasterizationRender();

  /**
   * Copies the current framebuffer to the offscreen buffer (used for screenshotting the render
   * window for example)
   */
  void CopyFramebufferToOffscreenBuffer();

  // Render textures acquired by the user on this render window. They are kept here in case the
  // render window is resized, in which case, we'll need to resize the render textures --> We need
  // access to the textures
  std::vector<vtkSmartPointer<vtkWebGPUComputeRenderTexture>> ComputeRenderTextures;
};

VTK_ABI_NAMESPACE_END
#endif

// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#ifndef vtkWebGPURenderer_h
#define vtkWebGPURenderer_h

#include "vtkRenderer.h"

#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkSmartPointer.h"          // for ivar
#include "vtkTypeUInt32Array.h"       // for ivar
#include "vtkWebGPUActor.h"           // for the actors rendered last frame
#include "vtkWebGPUComputePipeline.h" // for the compute pipelines used by this renderer
#include "vtk_wgpu.h"                 // for webgpu

#include <string>        // for ivar
#include <unordered_map> // for ivar
#include <unordered_set> // for the set of actors rendered last frame

class vtkAbstractMapper;
class vtkRenderState;
class vtkFrameBufferObjectBase;

VTK_ABI_NAMESPACE_BEGIN

class vtkWebGPUComputeOcclusionCuller;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPURenderer : public vtkRenderer
{
public:
  static vtkWebGPURenderer* New();
  vtkTypeMacro(vtkWebGPURenderer, vtkRenderer);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  struct RenderPipelineBatch
  {
    vtkSmartPointer<vtkPropCollection> Props;
    wgpu::RenderPipeline Pipeline;
  };

  // get the complexity of the current lights as a int
  // 0 = no lighting
  // 1 = headlight
  // 2 = directional lights
  // 3 = positional lights
  enum LightingComplexityEnum
  {
    NoLighting = 0,
    Headlight = 1,
    Directional = 2,
    Positional = 3
  };
  vtkGetMacro(LightingComplexity, int);

  void DeviceRender() override;

  /**
   * Updates / creates the various buffer necessary for the rendering of the props
   */
  void PrepareRender();

  /**
   * Ask all props to update themselves. This process should be limited
   * to wgpu::Buffer uploads, creation of bind groups, bind group layouts,
   * graphics pipeline. Basically, do everything necessary but do NOT encode
   * render pass commands.
   */
  int UpdateGeometry(vtkFrameBufferObjectBase* fbo = nullptr) override;

  /**
   * Set up the buffers of a given vtkWebGPUComputePass.
   * Loops through all the actors of this renderer. If an access to the data attributes buffer of
   * the actor was requested by the user through
   * vtkWebGPUPolyDataMapper::AcquirePointAttributeComputeRenderBuffer(), we'll have to set up the
   * WebGPU buffer to access the point data attributes (if it belongs to the right compute pass).
   */
  void ConfigureComputeRenderBuffers(vtkSmartPointer<vtkWebGPUComputePipeline> computePipeline);

  /**
   * Sets the adapter and the device of the render window of this renderer to the compute pipelines
   * of this renderer
   */
  void ConfigureComputePipelines();

  /// @{
  /**
   * Returns the list of compute pipelines of this renderer that have been setup for execution
   * before/after the rendering pass
   */
  const std::vector<vtkSmartPointer<vtkWebGPUComputePipeline>>& GetSetupPreRenderComputePipelines();
  const std::vector<vtkSmartPointer<vtkWebGPUComputePipeline>>&
  GetSetupPostRenderComputePipelines();
  /// @}

  /**
   * Request props to encode render commands.
   */
  int RenderGeometry();

  int UpdateLights() override;

  void SetEnvironmentTexture(vtkTexture* texture, bool isSRGB = false) override;

  void ReleaseGraphicsResources(vtkWindow* w) override;

  inline wgpu::RenderPassEncoder GetRenderPassEncoder() { return this->WGPURenderEncoder; }
  inline wgpu::BindGroup GetActorBindGroup() { return this->ActorBindGroup; }
  inline wgpu::BindGroup GetSceneBindGroup() { return this->SceneBindGroup; }

  inline void PopulateBindgroupLayouts(std::vector<wgpu::BindGroupLayout>& layouts)
  {
    layouts.emplace_back(this->SceneBindGroupLayout);
    layouts.emplace_back(this->ActorBindGroupLayout);
  }

  wgpu::ShaderModule HasShaderCache(const std::string& source);
  void InsertShader(const std::string& source, wgpu::ShaderModule shader);

  /// @{
  /**
   * Adds a compute pipeline to the renderer that will be executed each frame before/after the
   * rendering pass.
   */
  void AddPreRenderComputePipeline(vtkSmartPointer<vtkWebGPUComputePipeline> pipeline);
  void AddPostRenderComputePipeline(vtkSmartPointer<vtkWebGPUComputePipeline> pipeline);
  /// @}

  /**
   * Returns the list of the actors that were rendered last frame
   */
  std::unordered_set<vtkProp*> GetPropsRendered() { return this->PropsRendered; }

  ///@{
  /**
   * Set the user light transform applied after the camera transform.
   * Can be null to disable it.
   */
  void SetUserLightTransform(vtkTransform* transform);
  vtkTransform* GetUserLightTransform();
  ///@}

  ///@{
  /**
   * Set the usage of render bundles. This speeds up rendering in wasm.
   * @warning LEAKS MEMORY. See vtkWebGPURenderer::DeviceRender
   */
  vtkSetMacro(UseRenderBundles, bool);
  vtkBooleanMacro(UseRenderBundles, bool);
  vtkGetMacro(UseRenderBundles, bool);
  ///@}

protected:
  vtkWebGPURenderer();
  ~vtkWebGPURenderer() override;

  /**
   * Request mappers to run the vtkAlgorithm pipeline (if needed)
   * and consequently update device buffers corresponding to shader module bindings.
   * Ex: positions, colors, normals, indices
   */
  int UpdateOpaquePolygonalGeometry() override;
  int UpdateTranslucentPolygonalGeometry() override;

  /**
   * Request mappers to bind descriptor sets (bind groups) and encode draw commands.
   */
  void DeviceRenderOpaqueGeometry(vtkFrameBufferObjectBase* fbo) override;
  void DeviceRenderTranslucentPolygonalGeometry(vtkFrameBufferObjectBase* fbo) override;

  // Setup scene and actor bindgroups. Actor has dynamic offsets.
  void SetupBindGroupLayouts();
  // Create buffers for the bind groups.
  void CreateBuffers();
  // Update the bound buffers with data.
  std::size_t UpdateBufferData();
  // Create scene bind group.
  void SetupSceneBindGroup();
  // Create actor bind group.
  void SetupActorBindGroup();

  // Start, finish recording commands with render pass encoder
  void BeginEncoding();
  void EndEncoding();

  /**
   * Encodes the draw commands for the this->PropArrayCount props currently contained in
   * this->PropArray
   */
  void RenderProps();

  std::size_t WriteLightsBuffer(std::size_t offset = 0);
  std::size_t WriteSceneTransformsBuffer(std::size_t offset = 0);
  std::size_t WriteActorBlocksBuffer(std::size_t offset = 0);

  wgpu::RenderPassEncoder WGPURenderEncoder;
  wgpu::Buffer SceneTransformBuffer;
  wgpu::Buffer SceneLightsBuffer;
  wgpu::Buffer ActorBlocksBuffer;
  wgpu::BindGroup SceneBindGroup;
  wgpu::BindGroupLayout SceneBindGroupLayout;

  wgpu::BindGroup ActorBindGroup;
  wgpu::BindGroupLayout ActorBindGroupLayout;

#ifdef __EMSCRIPTEN__
  bool UseRenderBundles = true;
#else
  bool UseRenderBundles = false;
#endif
  // one bundle per actor. bundle gets reused every frame.
  // these bundles can be built in parallel with vtkSMPTools. holding off because not
  // sure how to get emscripten to thread.
  std::vector<wgpu::RenderBundle> Bundles;
  struct vtkWGPUPropItem
  {
    wgpu::RenderBundle Bundle = nullptr;
    vtkSmartPointer<vtkTypeUInt32Array> DynamicOffsets;
  };
  std::unordered_map<vtkProp*, vtkWGPUPropItem> PropWGPUItems;

  std::unordered_map<std::string, wgpu::ShaderModule> ShaderCache;
  std::size_t NumberOfPropsUpdated = 0;
  int LightingComplexity = 0;
  std::size_t NumberOfLightsUsed = 0;
  std::vector<std::size_t> LightIDs;

  vtkMTimeType LightingUpdateTime;
  vtkTimeStamp LightingUploadTimestamp;

  struct
  {
    uint32_t Hits = 0;
    uint32_t Misses = 0;
    uint32_t TotalRequests = 0;
  } BundleCacheStats;

  /**
   * Optional user transform for lights
   */
  vtkSmartPointer<vtkTransform> UserLightTransform;

private:
  friend class vtkWebGPUComputeOcclusionCuller;
  // For the mapper to access 'AddPostRasterizationActor'
  friend class vtkWebGPUComputePointCloudMapper;
  // For render window accessing PostRenderComputePipelines()
  friend class vtkWebGPURenderWindow;

  vtkWebGPURenderer(const vtkWebGPURenderer&) = delete;
  void operator=(const vtkWebGPURenderer&) = delete;

  /**
   * Sets the device and adapter of the render window of this renderer to the given pipeline
   */
  void InitComputePipeline(vtkSmartPointer<vtkWebGPUComputePipeline> pipeline);

  /// @{
  /**
   * Dispatches the compute pipelines attached to this renderer in the order they were added by
   * AddPreRenderComputePipeline() / AddPostRenderComputePipeline().
   *
   * This function only dispatches the compute pipelines that were given by the user to execute
   * before/after the rendering pass
   */
  void PreRenderComputePipelines();
  void PostRenderComputePipelines();
  /// @}

  /**
   * Renders actors contained in the PostRasterizationActors vector after the pass that rasterizes
   * the other actors of this renderer. This is mainly useful when some actors are rendered with
   * compute shaders (through compute pipelines) because compute shaders that write to the
   * framebuffer of the render window cannot be interleaved with rasterization pipeline render
   * commands (in-between BeginEncoding() and EndEncoding() calls).
   *
   * This method is called by the render window after the rasterization render pass has been flushed
   * to the device to make sure that all resources are up to date (depth buffer, frame buffer)
   */
  void PostRasterizationRender();

  /**
   * Adds an actor to be rendered after the main rasterization pass
   */
  void AddPostRasterizationActor(vtkActor* actor);

  /**
   * Compute pipelines (post and pre render) that have been setup and that will be dispatched
   * by the renderer before the rendering passes
   */
  std::vector<vtkSmartPointer<vtkWebGPUComputePipeline>> SetupPreRenderComputePipelines;
  std::vector<vtkSmartPointer<vtkWebGPUComputePipeline>> SetupPostRenderComputePipelines;

  /**
   * Compute pipelines (post and pre render) that have yet to be setup
   */
  std::vector<vtkSmartPointer<vtkWebGPUComputePipeline>> NotSetupPreRenderComputePipelines;
  std::vector<vtkSmartPointer<vtkWebGPUComputePipeline>> NotSetupPostRenderComputePipelines;

  /**
   * Actors that will be rendered by PostRasterizationRender() after the main rasterization pass.
   * Actors are added to this list when the Render() method of an actor is called but the mapper of
   * this actor determines that it needs to be rendered after the rasterization pass. The mapper
   * will then add the actor to this list of the renderer so that the renderer can render the actor
   * after the rasterization pass.
   */
  std::vector<vtkActor*> PostRasterizationActors;

  /**
   * Encodes a render command for rendering the given props
   */
  wgpu::CommandBuffer EncodePropListRenderCommand(vtkProp** propList, int listLength);

  /**
   * Whether the compute render buffers of the mappers of the actors of this renderer have already
   * been initialized or not
   */
  bool ComputeBuffersInitialized = false;

  /**
   * Indicates whether PrepareRender() was called already for this frame or not (and thus we do not
   * need to call it again).
   */
  bool PrepareRenderDone = false;

  /**
   * Whether to clear the depth/stencil/color buffer before rendering
   */
  bool DoClearPass = true;

  /**
   * List of the actors rendered last frame. Mainly used by the occlusion culler when we want to
   * render the actors that were rendered last frame in the first pass to build the z-buffer.
   * Using a set here to be able to efficiently run find operations on the list (the set) of actors
   * rendered. It makes no sense to have the same actor twice in the list anyway so a set is fine.
   */
  std::unordered_set<vtkProp*> PropsRendered;
};

VTK_ABI_NAMESPACE_END
#endif

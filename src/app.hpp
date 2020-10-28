#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "AppInfo.hpp"

#include "Model.hpp"
#include "Primitives.hpp"
#include "Window.hpp"
#include "queueFamilies.hpp"
#include "swapchainSupportDetails.hpp"
#include "validationLayer.hpp"
#include "vulkanHelpers.hpp"

struct SGraphicsPipelineStates;
enum class EShaderType;

class CApp
{
  private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkFormat m_format;
    VkExtent2D m_extent;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> m_framebuffers;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;
    VkSemaphore m_semaphoreRenderComplete = VK_NULL_HANDLE;
    VkSemaphore m_semaphorePresentComplete = VK_NULL_HANDLE;
    std::vector<VkFence> m_fences;
    VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_vertexMemory = VK_NULL_HANDLE;
    VkBuffer m_indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory m_indexMemory = VK_NULL_HANDLE;
    VkImage m_texImage = VK_NULL_HANDLE;
    VkImageView m_texImageView = VK_NULL_HANDLE;
    VkDeviceMemory m_texMemory = VK_NULL_HANDLE;
    VkSampler m_texSampler = VK_NULL_HANDLE;
    VkImage m_depthImage;
    VkImageView m_depthImageView;
    VkDeviceMemory m_depthImageMemory;
    VkFormat m_depthFormat;

    std::vector<VkBuffer> m_uniformBuffers;
    std::vector<VkDeviceMemory> m_uniformMemories;
    VkDescriptorPool m_uniformDescPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_descriptorSets;

    SAppInfo m_appInfo;
    SQueueFamilies m_queueFamilies;
    SSwapchainSupportDetails m_swapchainSupportDetails;

    std::unique_ptr<CValidationLayer> mp_validationLayer;
    std::unique_ptr<CWindow> mp_window;

    CModel m_vikingRoom{};
    std::vector<SVertex> m_vertices;
    std::vector<uint16_t> m_indices;
    VkDeviceSize m_verticesSize;
    VkDeviceSize m_indicesSize;
    SMVP m_mvp{};

    // Instance creation
    void CreateInstance();
    // Physical Device creation
    std::vector<VkPhysicalDevice> FindPhysicalDevices();
    std::vector<VkQueueFamilyProperties> FindQueueFamilies(const VkPhysicalDevice &device);
    bool IsDeviceSuitable(const VkPhysicalDevice &device);
    void CreatePhysicalDevice();
    // Logical Device creation
    void CreateDevice();
    // Queue creation
    void CreateQueues();
    // Swapchain creation
    void GetSwapchainSupportDetails(const VkPhysicalDevice &device);
    VkSurfaceFormatKHR GetOptimalSurfaceFormat();
    VkPresentModeKHR GetOptimalPresentMode();
    VkExtent2D GetOptimalExtent2D();
    void CreateSwapchain();
    void RecreateSwapchain();
    void CleanupSwapchain();
    bool ShouldRecreateSwapchain();
    // Image creation
    void CreateSwapchainImages();
    // ImageView creation
    void CreateImageViews();
    // Vertex and Index Buffer creation
    void CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size,
                      VkBuffer &buffer, VkDeviceMemory &memory);
    void CopyBuffer(VkBuffer &src, VkBuffer &dst, VkDeviceSize size);
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateDepthImage();
    // Texture Image creation
    void CreateTexImage();
    void CreateTexImageView();
    void CreateSampler();
    // Descriptor sets creation
    void CreateDescriptorSets(VkDescriptorSet *descriptorSets);
    void CreateDescriptorPool();
    void CreateDescriptorSetLayout(VkDescriptorSetLayout &layout);
    void CreateUniformDescriptors();
    void CreateUniformBuffers();
    void UpdateUniformBuffers(uint32_t frameIndex);
    // Graphics Pipeline creation
    VkShaderModule CreateShaderModule(const std::string &shaderFile, const EShaderType shaderType);
    VkPipelineShaderStageCreateInfo CreateShaderPipelineStage(const VkShaderModule &module,
                                                              const EShaderType shaderType);
    void CreatePipelineLayout();
    void CreateRenderPass();
    void CreateGraphicsPipeline();
    // Framebuffer creation
    void CreateFramebuffers();
    // Command pool and buffer creation
    void CreateCommandPool();
    void CreateCommandBuffers();

    VkFormat FindDepthFormat(std::vector<VkFormat> formats, VkImageTiling tiling, VkFormatFeatureFlags flags);
    uint32_t FindMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags flags);
    // Surface creation
    void CreateSurface();
    // Semaphore creation
    void CreateSemaphores();
    void CreateFences();
    // Draw Frame
    void Draw();

  public:
    explicit CApp(SAppInfo appInfo);

    void LoadObject(std::string objFile);
    void RenderLoop();
    void Cleanup();
};
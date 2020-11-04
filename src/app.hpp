#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "appInfo.hpp"

#include "CBufferImageManager.hpp"
#include "CInstance.hpp"
#include "CModel.hpp"
#include "CShaderUtils.hpp"
#include "CValidationLayer.hpp"
#include "CVulkanHelpers.hpp"
#include "CWindow.hpp"
#include "Primitives.hpp"
#include "SQueueFamilies.hpp"

struct SGraphicsPipelineStates;

class CApp
{
  private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
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
    VkImage m_texImage = VK_NULL_HANDLE;
    VkImageView m_texImageView = VK_NULL_HANDLE;
    VkDeviceMemory m_texMemory = VK_NULL_HANDLE;
    VkSampler m_texSampler = VK_NULL_HANDLE;
    VkImage m_depthImage;
    VkImageView m_depthImageView;
    VkDeviceMemory m_depthImageMemory;
    VkFormat m_depthFormat;

    SAppInfo m_appInfo;

    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_descriptorSets;

    std::unique_ptr<CWindow> mp_window;
    std::unique_ptr<CInstance> mp_instance;
    std::unique_ptr<CBufferImageManager> mp_bufferImageManager;

    std::vector<CModel> m_gameObjects{};

    CModel m_vikingRoom;

    // Instance creation
    // Physical Device creation
    // Logical Device creation
    void CreateDevice();
    // Queue creation
    void CreateQueues();
    // Swapchain creation
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
    //    void CreateVertexBuffer();
    //    void CreateIndexBuffer();
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
    // Semaphore creation
    void CreateSemaphores();
    void CreateFences();
    // Draw Frame
    void Draw();

  public:
    explicit CApp(SAppInfo appInfo);

    void RenderLoop();
    void Cleanup();
};
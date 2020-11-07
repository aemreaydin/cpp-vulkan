#pragma once

#include "CInstance.hpp"
#include "CWindow.hpp"
#include "appInfo.hpp"
#include <vector>
#include <vulkan/vulkan.h>

class CBufferImageManager;
class CDevice
{
  public:
    static CDevice &GetInstance();

    const CInstance *GetVulkanInstance()
    {
        return mp_instance;
    }

    const VkDevice GetDevice() const
    {
        return m_device;
    }

    const VkQueue GetGraphicsQueue() const
    {
        return m_graphicsQueue;
    }

    const VkQueue GetPresentQueue() const
    {
        return m_presentQueue;
    }

    const VkSwapchainKHR GetSwapchain() const
    {
        return m_swapchain;
    }

    const uint32_t GetSwapchainImageCount() const
    {
        return static_cast<uint32_t>(m_swapchainImages.size());
    }

    const VkPipelineLayout GetPipelineLayout() const
    {
        return m_pipelineLayout;
    }

    const VkCommandPool GetCommandPool() const
    {
        return m_commandPool;
    }

    const VkDescriptorPool GetDescriptorPool() const
    {
        return m_descriptorPool;
    }

    const VkDescriptorSetLayout GetDescriptorSetLayout() const
    {
        return m_descriptorLayout;
    }

    const VkCommandBuffer GetCurrentCommandBuffer() const
    {
        return m_currentCommandBuffer;
    }

    uint32_t GetCurrentImageIndex() const
    {
        return m_currentImageIndex;
    }

    VkExtent2D GetExtent() const
    {
        return m_extent;
    }

    const CBufferImageManager &GetBufferImageManager() const
    {
        return *mp_bufferImageManager;
    }

    void InitDevice(CWindow *window, CInstance *vkInstance, CBufferImageManager *bufferImageManager, SAppInfo appInfo);
    bool DrawBegin();
    bool DrawEnd();
    void Cleanup();

    uint32_t FindMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags flags);

  private:
    void CreateDevice(SAppInfo appInfo);
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
    void CreateDepthImage();
    // ImageView creation
    void CreateImageViews();
    void CreatePipelineLayout();
    void CreateRenderPass();
    void CreateGraphicsPipeline();
    // Framebuffer creation
    void CreateFramebuffers();
    // Command pool and buffer creation
    void CreateCommandPool();
    void CreateCommandBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSetLayout();
    void CreateSemaphores();
    void CreateFences();

    VkFormat FindDepthFormat(std::vector<VkFormat> formats, VkImageTiling tiling, VkFormatFeatureFlags flags);

  private:
    CInstance *mp_instance;
    CWindow *mp_window;
    CBufferImageManager *mp_bufferImageManager;

    VkCommandBuffer m_currentCommandBuffer;
    uint32_t m_currentImageIndex;

    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    VkFormat m_format;
    VkExtent2D m_extent;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_imageViews;
    VkImage m_depthImage;
    VkImageView m_depthImageView;
    VkDeviceMemory m_depthImageMemory;
    VkFormat m_depthFormat;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> m_framebuffers;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorLayout = VK_NULL_HANDLE;
    VkSemaphore m_semaphoreRenderComplete = VK_NULL_HANDLE;
    VkSemaphore m_semaphorePresentComplete = VK_NULL_HANDLE;
    std::vector<VkFence> m_fences;
};

#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "appInfo.hpp"

#include "queueFamilies.hpp"
#include "swapchainSupportDetails.hpp"
#include "vertex.hpp"

class CValidationLayer;
class CWindow;
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

    SAppInfo m_appInfo;
    SQueueFamilies m_queueFamilies;
    SSwapchainSupportDetails m_swapchainSupportDetails;

    std::unique_ptr<CValidationLayer> mp_validationLayer;
    std::unique_ptr<CWindow> mp_window;

    std::vector<SVertex> m_vertices = {{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}},
                                       {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f}},
                                       {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}}};
    VkDeviceSize m_verticesSize = sizeof(SVertex) * m_vertices.size();

    // Instance creation
    void CreateInstance();
    // Physical Device creation
    std::vector<VkPhysicalDevice> FindPhysicalDevices();
    std::vector<VkQueueFamilyProperties> FindQueueFamilise(const VkPhysicalDevice &device);
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
    void CreateVertexBuffer();
    void CreateVertexMemory();
    // Graphics Pipeline creation
    VkShaderModule CreateShaderModule(const std::string &shaderFile);
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

    void RenderLoop();
    void Cleanup();
};
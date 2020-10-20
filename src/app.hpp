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

class CValidationLayer;
class CWindow;
struct SGraphicsPipelineStates;
enum class EShaderType;

class CApp
{
  private:
    VkInstance m_instance;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VkSurfaceKHR m_surface;
    VkFormat m_format;
    VkExtent2D m_extent;
    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    VkPipelineLayout m_pipelineLayout;
    VkRenderPass m_renderPass;
    VkPipeline m_graphicsPipeline;
    std::vector<VkFramebuffer> m_framebuffers;

    SAppInfo m_appInfo;
    SQueueFamilies m_queueFamilies;
    SSwapchainSupportDetails m_swapchainSupportDetails;

    std::unique_ptr<CValidationLayer> mp_validationLayer;
    std::unique_ptr<CWindow> mp_window;

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
    // Image creation
    void CreateSwapchainImages();
    // ImageView creation
    void CreateImageViews();
    // Graphics Pipeline creation
    VkShaderModule CreateShaderModule(const std::string &shaderFile);
    VkPipelineShaderStageCreateInfo CreateShaderPipelineStage(const VkShaderModule &module,
                                                              const EShaderType shaderType);
    void CreatePipelineLayout();
    void CreateRenderPass();
    void CreateGraphicsPipeline();
    // Framebuffer creation
    void CreateFramebuffers();
    // Surface creation
    void CreateSurface();

  public:
    explicit CApp(SAppInfo appInfo);
    void Cleanup();
};
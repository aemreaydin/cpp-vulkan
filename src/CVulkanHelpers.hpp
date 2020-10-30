#pragma once

#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

struct SSwapchainSupport
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> surfacePresentModes;
};

class CVulkanHelpers
{
  public:
    static std::vector<const char *> GetVulkanInstanceExtensions();
    static std::vector<VkExtensionProperties> GetVulkanDeviceExtensions(const VkPhysicalDevice &physicalDevice);
    static std::vector<VkLayerProperties> GetVulkanLayers();
    static bool CheckForVulkanInstanceExtensions(const VkPhysicalDevice &physicalDevice,
                                                 const std::vector<const char *> &extensionsToCheck);
    static bool CheckForVulkanLayers(const std::vector<const char *> &layersToCheck);

    static void GetSupportForSwapchain(const VkPhysicalDevice &device, const VkSurfaceKHR &surface,
                                       SSwapchainSupport &swapchainSupport);
};
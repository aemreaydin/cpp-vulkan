#pragma once

#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

class CValidationLayer
{
    VkDebugUtilsMessengerEXT m_debugMessenger;
    // Enumerate the vulkan layers and check if the ones we need exist
    void SetupDebugMessenger(const VkInstance &instance);

  public:
    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    explicit CValidationLayer(const VkInstance &instance, const std::vector<const char *> layersToCheck);
    void DestroyDebugMessenger(const VkInstance &instance);
};

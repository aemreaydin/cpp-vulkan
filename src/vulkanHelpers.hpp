#pragma once

#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

enum class EShaderType
{
    Frag,
    Vert
};

class CVulkanHelpers
{
  public:
    static std::vector<const char *> GetVulkanInstanceExtensions()
    {
        if (glfwVulkanSupported() == GLFW_FALSE)
            throw std::runtime_error("Vulkan is not supported.");
        uint32_t glfwExtensionCount;
        const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> vExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        // TODO add debug check
        vExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return vExtensions;
    }
    static std::vector<VkExtensionProperties> GetVulkanDeviceExtensions(const VkPhysicalDevice &physicalDevice)
    {
        uint32_t extPropsCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extPropsCount, nullptr);
        std::vector<VkExtensionProperties> extensionProps(extPropsCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extPropsCount, extensionProps.data());
        return extensionProps;
    }
    static std::vector<VkLayerProperties> GetVulkanLayers()
    {
        uint32_t layerPropsCount;
        vkEnumerateInstanceLayerProperties(&layerPropsCount, nullptr);
        std::vector<VkLayerProperties> layerProps(layerPropsCount);
        vkEnumerateInstanceLayerProperties(&layerPropsCount, layerProps.data());
        return layerProps;
    }
    static bool CheckForVulkanExtensions(const VkPhysicalDevice &physicalDevice,
                                         const std::vector<const char *> extensionsToCheck)
    {
        const auto extensions = GetVulkanDeviceExtensions(physicalDevice);

        for (const auto &vulkanLayer : extensionsToCheck)
        {
            auto found = false;
            for (const auto &extension : extensions)
            {
                if (strcmp(vulkanLayer, extension.extensionName) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                return false;
        }

        return true;
    }
    static bool CheckForVulkanLayers(const std::vector<const char *> layersToCheck)
    {
        const auto layerProps = GetVulkanLayers();

        for (const auto &vulkanLayer : layersToCheck)
        {
            auto found = false;
            for (const auto &layer : layerProps)
            {
                if (strcmp(vulkanLayer, layer.layerName) == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                return false;
        }

        return true;
    }
};
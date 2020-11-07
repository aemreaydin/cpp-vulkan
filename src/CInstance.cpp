#include "CInstance.hpp"

#include "vkStructs.hpp"

CInstance::CInstance(GLFWwindow *window, SAppInfo appInfo) : m_appInfo(appInfo)
{
    CreateInstance();
    mp_validationLayer = std::make_unique<CValidationLayer>(m_instance, appInfo.layers);

    CreateSurface(window);
    CreatePhysicalDevice();
}

void CInstance::CreateInstance()
{
    if (!CVulkanHelpers::CheckForVulkanLayers(m_appInfo.layers))
        throw std::runtime_error("Validation layers not available.");

    const auto applicationInfo = vkTools::ApplicationInfo();
    const auto extensions = CVulkanHelpers::GetVulkanInstanceExtensions();
    VkDebugUtilsMessengerCreateInfoEXT debugInfo;
    CValidationLayer::PopulateDebugMessengerCreateInfo(debugInfo);
    const auto instanceInfo = vkTools::InstanceCreateInfo(applicationInfo, extensions, m_appInfo.layers, debugInfo);

    if (const auto res = vkCreateInstance(&instanceInfo, nullptr, &m_instance); res != VK_SUCCESS)
        throw std::runtime_error("Failed to create instance.");
}

void CInstance::CreateSurface(GLFWwindow *window)
{
    if (!window)
        throw std::runtime_error("Window doesn't exist, create it first.");

    const auto res = glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface);
    if (res != VK_SUCCESS)
        throw std::runtime_error("Failed to create surface.");
}

void CInstance::CreatePhysicalDevice()
{
    const auto devices = FindPhysicalDevices();
    for (const auto &device : devices)
    {
        const auto queueFamilies = FindQueueFamiliesForDevice(device);

        uint32_t index = 0;
        for (const auto &family : queueFamilies)
        {
            if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                m_queueFamilies.graphicsFamilyIndex = index;
            }

            VkBool32 isSupported;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, m_surface, &isSupported);
            if (isSupported)
            {
                m_queueFamilies.presentFamilyIndex = index;
            }

            // If device has the necessary conditions use it
            if (m_queueFamilies.HaveValues())
            {
                break;
            }

            ++index;
        }

        if (CheckIfDeviceSuitable(device))
        {
            m_physicalDevice = device;
            break;
        }
    }
}

std::vector<VkPhysicalDevice> CInstance::FindPhysicalDevices()
{
    uint32_t deviceCount;
    if (vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr) != VK_SUCCESS)
        throw std::runtime_error("Failed to get any physical device.");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    return devices;
}

std::vector<VkQueueFamilyProperties> CInstance::FindQueueFamiliesForDevice(const VkPhysicalDevice device)
{
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    return queueFamilies;
}

bool CInstance::CheckIfDeviceSuitable(const VkPhysicalDevice device)
{
    const auto extensionsValid = CVulkanHelpers::CheckForVulkanInstanceExtensions(device, m_appInfo.deviceExtensions);
    CVulkanHelpers::GetSupportForSwapchain(device, m_surface, m_swapchainSupport);

    return m_queueFamilies.HaveValues() && extensionsValid && !m_swapchainSupport.surfaceFormats.empty() &&
           !m_swapchainSupport.surfacePresentModes.empty();
}

void CInstance::UpdateSwaphchainSupport()
{
    CVulkanHelpers::GetSupportForSwapchain(m_physicalDevice, m_surface, m_swapchainSupport);
}

CInstance::~CInstance()
{
    mp_validationLayer->DestroyDebugMessenger(m_instance);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

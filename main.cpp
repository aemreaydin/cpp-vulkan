#include <iostream>
#include <string>
#include <vector>

#include "window.hpp"

#if NDEBUG
const bool kEnableValidationLayers = false
#else
const bool kEnableValidationLayers = true;
#endif

const std::vector<const char*> gVulkanLayers = { "VK_LAYER_KHRONOS_validation" };


auto CreateDebugUtilsMessengerEXT(const VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) -> VkResult {
    const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(const VkInstance instance, const VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
    );
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}


std::vector<const char*> GetVulkanExtensions()
{
    if (glfwVulkanSupported() == GLFW_FALSE) throw std::runtime_error("Vulkan is not supported.");
    uint32_t glfwExtensionCount;
    const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    //uint32_t instanceExtensionCount;
    //const auto enumExtensions = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    //std::vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount);
    //vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions.data());

    std::vector<const char*> vExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if(kEnableValidationLayers)
        vExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return vExtensions;
}

bool CheckForVulkanLayers()
{
    uint32_t layerPropsCount;
    vkEnumerateInstanceLayerProperties(&layerPropsCount, nullptr);
    std::vector<VkLayerProperties> layerProps(layerPropsCount);
    vkEnumerateInstanceLayerProperties(&layerPropsCount, layerProps.data());

    for(const auto& vulkanLayer : gVulkanLayers)
    {
        auto found = false;
        for(const auto& layer : layerProps)
        {
            if(strcmp(vulkanLayer, layer.layerName))
            {
                found = true;
            }
        }
        if(!found) return false;
    }

    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::string severity;
    if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) severity = "Error";
    else if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) severity = "Warning";
    else severity = "Info";

    std::string type;
    if(messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) type = "General";
    else if(messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) type = "Validation";
    else type = "Performance";

    fprintf_s(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT ? stderr : stdout, "%s : %s - %s\n", severity.c_str(), type.c_str(), pCallbackData->pMessage);

    return VK_FALSE;
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT  | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
}

VkDebugUtilsMessengerEXT SetupDebugMessenger(const VkInstance& instance) {
    VkDebugUtilsMessengerEXT debugMessenger;
    if (!kEnableValidationLayers) return debugMessenger;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }

    return debugMessenger;
}

VkInstance CreateInstance()
{
    if(kEnableValidationLayers && !CheckForVulkanLayers())
    {
        throw std::runtime_error("Validation layers not available.");
    }

    auto appInfo = VkApplicationInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan 00";
    appInfo.applicationVersion = VK_API_VERSION_1_0;
    appInfo.pEngineName = "Vulkan 00";
    appInfo.engineVersion = 1;
    // apiVersion => Highest version of vulkan that the app is designed to use
    // pNext => NULL or a pointer to a structure extending this structure


    const auto extensions = GetVulkanExtensions();
    auto instanceCreateInfo = VkInstanceCreateInfo{};
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    // Structure to specify the parameters for a new instance
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    if(kEnableValidationLayers)
    {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(gVulkanLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = gVulkanLayers.data();

        // PopulateDebugMessengerCreateInfo(debugCreateInfo);
        // instanceCreateInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo);
    }
    else
    {
        instanceCreateInfo.enabledLayerCount = 0;
    }
    // pNext => nullptr or a pointer to a structure extending this structure
    // flags => reserved for future use

    VkInstance instance;

    if(vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create instance.");

    std::cout << "VkInstance created.\n";

    return instance;
}

std::vector<VkPhysicalDevice> GetPhysicalDevices(const VkInstance& instance)
{
    // Get the physical devices
    uint32_t physicalDeviceCount;
    auto res = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if (res != VK_SUCCESS || physicalDeviceCount == 0)
        throw std::runtime_error(
            "Failed to find any physical devices that support Vulkan.");

    std::vector<VkPhysicalDevice> vPhysicalDevices(physicalDeviceCount);
    res = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, vPhysicalDevices.data());
    if (res != VK_SUCCESS) throw std::runtime_error("Failed to obtain physical devices.");

    return vPhysicalDevices;
}

std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties(const VkPhysicalDevice& physicalDevice)
{
    // Get the queue families and look for a specific queue family with Graphics, compute and transfer bits set
    // This is a pretty common pattern in Vulkan
    // First get the count with array nullptr
    // Then use the count to initialize the array and call the function one more time
    uint32_t queueFamilyPropsCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropsCount, nullptr);

    std::vector<VkQueueFamilyProperties> vQueueFamilyProps(queueFamilyPropsCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropsCount, vQueueFamilyProps.data());

    return vQueueFamilyProps;
}

bool IsPhysicalDeviceSuitable(const VkPhysicalDevice& physicalDevice, const VkQueueFlags requiredQueueFlags)
{
    const auto queueFamilyProperties = GetQueueFamilyProperties(physicalDevice);
    // Iterate over the queue family properties and find the one with the required queue flags
    for (const auto& iter : queueFamilyProperties)
    {
        if (iter.queueFlags & requiredQueueFlags)
        {
            return true;
        }
    }
    return false;
}

VkPhysicalDevice FindSuitablePhysicalDevice(const VkInstance& instance, const VkQueueFlags requiredQueueFlags)
{
    const auto physicalDevices = GetPhysicalDevices(instance);
    // Iterate over the devices and find the one that has the required queue flags
    for (const auto& iter : physicalDevices)
    {
        if (IsPhysicalDeviceSuitable(iter, requiredQueueFlags))
        {
            return iter;
        }
    }
    throw std::runtime_error("Failed to find a suitable GPU.");
}

uint32_t GetQueueFamilyIndex(const VkPhysicalDevice& physicalDevice, const VkQueueFlags requiredQueueFlags)
{
    const auto queueFamilyProperties = GetQueueFamilyProperties(physicalDevice);
    // Iterate over the queue family properties and find the one with the required queue flags
    auto index = 0;
    for (const auto& iter : queueFamilyProperties)
    {
        if (iter.queueFlags & requiredQueueFlags)
        {
            return index;
        }
        ++index;
    }
    throw std::runtime_error("Failed to find a suitable queueFamilyIndex.");
}

uint32_t GetPresentationQueueIndex(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) {
    const auto familyQueues = GetQueueFamilyProperties(physicalDevice);
    for(auto index = 0; index != familyQueues.size(); ++index) {
        VkBool32 isSupported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &isSupported);
        if(isSupported == VK_TRUE) {
            return index;
        }
    }
    throw std::runtime_error("Failed to find a presentation queue.");
}

VkDevice CreateLogicalDevice(const VkPhysicalDevice& physicalDevice, const uint32_t queueFamilyIndex)
{
    const auto queuePriority = 1.0f;
    VkDevice logicalDevice;

    // Create the device queue using the queueFamily we selected from the physicalDevice
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    // flags => Behaviour of the queue, probably not needed often.
    // pNext => Pointer to a structure extending this structure

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    // pNext
    // flags => Reserved for future use
    // enabledLayerCount => Deprecated
    // ppEnabledLayerNames => Deprecated
    // enabledExtensionCount => number of device extensions to enable
    // ppEnabledExtensionNames => names of the extensions to enable for the device
    // pEnabledFeatures => nullptr or a pointer to VkPhysicalDeviceFeatures of all the features to be enabled.


    const auto res = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);
    if (res != VK_SUCCESS) throw std::runtime_error("Failed to create logical device.");

    return logicalDevice;
}

VkQueue GetDeviceQueue(const VkDevice& device, const uint32_t queueFamilyIndex)
{
    VkQueue queue;
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

    return queue;
}


int main()
{
    const auto window = CWindow(1024, 768);
    const auto instance = CreateInstance();
    const auto debugMessenger = SetupDebugMessenger(instance);
    const auto surface = window.CreateSurface(instance);

    const auto queueFamilyFlags = VK_QUEUE_GRAPHICS_BIT;
    const auto physicalDevice = FindSuitablePhysicalDevice(instance, queueFamilyFlags);
    const auto queueFamilyIndex = GetQueueFamilyIndex(physicalDevice, queueFamilyFlags);

    const auto logicalDevice = CreateLogicalDevice(physicalDevice, queueFamilyIndex);
    const auto graphicsQueue = GetDeviceQueue(logicalDevice, queueFamilyIndex);
    const auto presentationQueueIndex = GetPresentationQueueIndex(physicalDevice, surface);
    const auto presentQueue = GetDeviceQueue(logicalDevice, presentationQueueIndex);

    while (!glfwWindowShouldClose(window.GetWindow()))
    {
        glfwPollEvents();
    }

    if(kEnableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyDevice(logicalDevice, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    window.Terminate();

    return 0;
}

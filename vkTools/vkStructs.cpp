#include "vkStructs.hpp"

namespace vkTools
{
namespace vkStructs
{

VkApplicationInfo ApplicationInfo(const char *appName, uint32_t appVersion, const char *engineName,
                                  uint32_t engineVersion)
{
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = appName;
    applicationInfo.applicationVersion = appVersion;
    applicationInfo.pEngineName = engineName;
    applicationInfo.engineVersion = engineVersion;
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    return applicationInfo;
}
VkInstanceCreateInfo InstanceCreateInfo(VkApplicationInfo applicationInfo,
                                        const std::vector<const char *> &vecExtensions,
                                        const std::vector<const char *> &vecLayers,
                                        VkDebugUtilsMessengerCreateInfoEXT debugInfo)
{
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = vecExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = vecExtensions.data();
#ifndef NDEBUG
    instanceCreateInfo.enabledLayerCount = vecLayers.size();
    instanceCreateInfo.ppEnabledLayerNames = vecLayers.data();
    instanceCreateInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT *>(&debugInfo);
#endif
    return instanceCreateInfo;
}

} // namespace vkStructs
} // namespace vkTools

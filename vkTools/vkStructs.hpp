#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace vkTools
{
VkApplicationInfo ApplicationInfo(const char *appName = "VULKAN APP", uint32_t appVersion = 1,
                                  const char *engineName = "VULKAN ENGINE", uint32_t engineVersion = 1);
VkInstanceCreateInfo InstanceCreateInfo(VkApplicationInfo applicationInfo,
                                        const std::vector<const char *> &vecExtensions,
                                        const std::vector<const char *> &vecLayers,
                                        VkDebugUtilsMessengerCreateInfoEXT debugInfo);
} // namespace vkTools

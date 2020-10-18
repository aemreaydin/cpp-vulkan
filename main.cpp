#include <vector>

#include "app.hpp"
#include "validationLayer.hpp"
#include "vulkanHelpers.hpp"
#include "window.hpp"

const std::vector<const char *> gVulkanLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char *> gDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

int main()
{
    SAppInfo appInfo(1024, 768, gVulkanLayers, gDeviceExtensions);
    auto app = CApp(appInfo);

    app.Cleanup();
    return 0;
}

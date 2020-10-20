#include <vector>

#include "app.hpp"
#include "validationLayer.hpp"
#include "vulkanHelpers.hpp"
#include "window.hpp"

const std::vector<const char *> gVulkanLayers = {"VK_LAYER_KHRONOS_validation" /*,  "VK_LAYER_LUNARG_api_dump" */};

const std::vector<const char *> gDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
constexpr uint32_t WIDTH = 1024;
constexpr uint32_t HEIGHT = 768;
int main()
{
    SAppInfo appInfo(WIDTH, HEIGHT, gVulkanLayers, gDeviceExtensions);
    auto app = CApp(appInfo);

    app.RenderLoop();

    app.Cleanup();
    return 0;
}

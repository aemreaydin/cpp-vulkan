#include <vector>

#include "app.hpp"

const std::vector<const char *> gVulkanLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char *> gDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
constexpr uint32_t WIDTH = 1920;
constexpr uint32_t HEIGHT = 1080;

int main()
{
    SAppInfo appInfo(WIDTH, HEIGHT, gVulkanLayers, gDeviceExtensions);
    auto app = CApp(appInfo);

    app.RenderLoop();

    app.Cleanup();
    return 0;
}

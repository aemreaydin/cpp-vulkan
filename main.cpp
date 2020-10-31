#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

#include <vector>

#include "app.hpp"

const std::vector<const char *> gVulkanLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char *> gDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
constexpr uint32_t WIDTH = 1920;
constexpr uint32_t HEIGHT = 1080;

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    SAppInfo appInfo(WIDTH, HEIGHT, gVulkanLayers, gDeviceExtensions);
    auto app = CApp(appInfo);

    char *x = new char('a');
    app.RenderLoop();

    app.Cleanup();

    return 0;
}

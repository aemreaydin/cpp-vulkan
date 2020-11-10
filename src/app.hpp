#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "appInfo.hpp"

#include "CBufferImageManager.hpp"
#include "CDevice.hpp"
#include "CGameObject.hpp"
#include "CGui.hpp"
#include "CInstance.hpp"
#include "CShaderUtils.hpp"
#include "CValidationLayer.hpp"
#include "CVulkanHelpers.hpp"
#include "CWindow.hpp"

#include "CLightObject.hpp"
#include "SQueueFamilies.hpp"

class CApp
{
  private:
    SAppInfo m_appInfo;

    std::unique_ptr<CWindow> mp_window;
    std::unique_ptr<CInstance> mp_instance;
    std::unique_ptr<CBufferImageManager> mp_bufferImageManager;
    std::unique_ptr<CGui> mp_gui;
    CDevice *m_deviceInstance;

    std::vector<std::unique_ptr<CGameObject>> m_vecGameObjects{};
    std::vector<std::unique_ptr<CLightObject>> m_vecLightObjects{};

    void Draw();
  public:
    explicit CApp(SAppInfo appInfo);

    void RenderLoop();
    void Cleanup();
};
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
#include "CInstance.hpp"
#include "CModel.hpp"
#include "CShaderUtils.hpp"
#include "CValidationLayer.hpp"
#include "CVulkanHelpers.hpp"
#include "CWindow.hpp"

#include "SQueueFamilies.hpp"

class CApp
{
  private:
    SAppInfo m_appInfo;

    std::unique_ptr<CWindow> mp_window;
    std::unique_ptr<CInstance> mp_instance;
    std::unique_ptr<CBufferImageManager> mp_bufferImageManager;
    CDevice *m_deviceInstance;

    std::vector<std::unique_ptr<CModel>> m_vecGameObjects{};

    void Draw();
  public:
    explicit CApp(SAppInfo appInfo);

    void RenderLoop();
    void Cleanup();
};
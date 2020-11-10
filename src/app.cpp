#include "app.hpp"
#include "CImageLoader.hpp"
#include <imgui.h>

CApp::CApp(SAppInfo appInfo) : m_appInfo(appInfo)
{
    // Create GLFW window
    mp_window = std::make_unique<CWindow>(appInfo.width, appInfo.height);
    // Create Vulkan instance, surface, physical device, queue families and validation layers
    mp_instance = std::make_unique<CInstance>(mp_window->Window(), appInfo);
    mp_bufferImageManager = std::make_unique<CBufferImageManager>(mp_instance->PhysicalDevice());

    m_deviceInstance = &CDevice::GetInstance();
    m_deviceInstance->InitDevice(mp_window.get(), mp_instance.get(), mp_bufferImageManager.get(), appInfo);

    SModelProps vikingProps{};
    vikingProps.modelName = "Viking Room";
    vikingProps.objectFile = "../assets/models/viking_room.obj";
    vikingProps.textureFile = "../assets/textures/viking_room.png";
    vikingProps.modelTransform.translate = glm::vec3(0.0f, -3.0f, 0.0f);
    vikingProps.modelTransform.scale = glm::vec3(3.0f, 3.0f, 3.0f);
    m_vecGameObjects.emplace_back(std::make_unique<CGameObject>(vikingProps));

    SModelProps cubeProps{};
    cubeProps.modelName = "Cube";
    cubeProps.objectFile = "../assets/models/cube.obj";
    cubeProps.textureFile = "../assets/textures/texture.jpg";
    cubeProps.modelTransform.translate = glm::vec3(0.0f, 3.0f, 0.0f);
    m_vecGameObjects.emplace_back(std::make_unique<CGameObject>(cubeProps));

    m_vecLightObjects.emplace_back(std::make_unique<CLightObject>());

    mp_gui = std::make_unique<CGui>();
}

void CApp::Draw()
{
    if (!m_deviceInstance->DrawBegin())
    {
        for (auto &lightObject : m_vecLightObjects)
        {
            lightObject->CleanupGraphicsPipeline();
            lightObject->RecreateGraphicsPipeline();
        }
        return;
    }

    for (const auto &gameObject : m_vecGameObjects)
    {
        gameObject->UpdateUniformBuffers();
        gameObject->Draw();
    }
    // TODO This has to go after gameobjects because they're using the same render pass
    mp_gui->Draw();

    for (auto &lightObject : m_vecLightObjects)
    {
        lightObject->UpdateUniformBuffers();
        lightObject->Draw();
    }

    if (!m_deviceInstance->DrawEnd())
    {
        for (auto &lightObject : m_vecLightObjects)
        {
            lightObject->CleanupGraphicsPipeline();
            lightObject->RecreateGraphicsPipeline();
        }
        return;
    }
}

void CApp::RenderLoop()
{
    while (!glfwWindowShouldClose(mp_window->Window()))
    {
        glfwPollEvents();
        Draw();
    }
}

void CApp::Cleanup()
{
    for (auto &gameObject : m_vecGameObjects)
    {
        gameObject->ObjectCleanup();
    }
    for (auto &lightObject : m_vecLightObjects)
    {
        lightObject->ObjectCleanup();
    }
    mp_gui->Cleanup();
    m_deviceInstance->Cleanup();
}
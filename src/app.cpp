#include "app.hpp"
#include "CImageLoader.hpp"

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
    vikingProps.objectFile = "../assets/models/viking_room.obj";
    vikingProps.textureFile = "../assets/textures/viking_room.png";
    vikingProps.modelTransform.translate = glm::vec3(0.0f, -3.0f, 0.0f);
    vikingProps.modelTransform.scale = glm::vec3(3.0f, 3.0f, 3.0f);
    m_vecGameObjects.emplace_back(std::make_unique<CModel>(vikingProps));

    SModelProps cubeProps{};
    cubeProps.objectFile = "../assets/models/cube.obj";
    cubeProps.textureFile = "../assets/textures/texture.jpg";
    cubeProps.modelTransform.translate = glm::vec3(0.0f, 3.0f, 0.0f);
    m_vecGameObjects.emplace_back(std::make_unique<CModel>(cubeProps));
}


void CApp::Draw()
{
    if (!m_deviceInstance->DrawBegin())
        return;

    for (const auto &gameObject : m_vecGameObjects)
    {
        gameObject->UpdateUniformBuffers();
        gameObject->Draw();
    }

    m_deviceInstance->DrawEnd();
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
        gameObject->ModelCleanup();
    }
    m_deviceInstance->Cleanup();
}
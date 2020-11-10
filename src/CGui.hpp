#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>

class CGui
{
  public:
    CGui();

    void Draw();
    void Cleanup();

  private:
    void InitImGui();
    void CreateImGuiDescriptorPool();
    VkDescriptorPool m_guiPool;
};

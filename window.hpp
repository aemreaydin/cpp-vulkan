#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class CWindow {
    uint32_t m_width;
    uint32_t m_height;
    GLFWwindow* mp_window;

    void InitWindow();
  public:
    CWindow(uint32_t width, uint32_t height);

    VkSurfaceKHR CreateSurface(const VkInstance& instance) const;

    GLFWwindow* GetWindow() const { return mp_window; }

    void Terminate() const;
};
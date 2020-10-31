#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class CWindow
{
    uint32_t m_width;
    uint32_t m_height;
    GLFWwindow *mp_window;

    void InitWindow();
    void Terminate() const;

  public:
    CWindow(uint32_t width, uint32_t height);
    ~CWindow();

    inline GLFWwindow *Window() const
    {
        return mp_window;
    }
};
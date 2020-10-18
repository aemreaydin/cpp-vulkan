#include "window.hpp"
#include <iostream>

void ErrorCallback(int error, const char* desc)
{
    fprintf_s(stderr, "Error: %s\n", desc);
}

CWindow::CWindow(const uint32_t width, const uint32_t height) : m_width(width), m_height(height)
{
    InitWindow();
}

void CWindow::InitWindow()
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize glfw.");
    }
    glfwSetErrorCallback(ErrorCallback);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mp_window = glfwCreateWindow(m_width, m_height, "App", nullptr, nullptr);
}

VkSurfaceKHR CWindow::CreateSurface(const VkInstance& instance) const
{
    if (!mp_window)
    {
        throw std::runtime_error("Failed to create window.");
    }

    VkSurfaceKHR surface;
    const auto res = glfwCreateWindowSurface(instance, mp_window, nullptr, &surface);
    if (res != VK_SUCCESS) throw std::runtime_error("Failed to create surface.");

    return surface;
}

void CWindow::Terminate() const
{
    glfwDestroyWindow(mp_window);
    glfwTerminate();
}

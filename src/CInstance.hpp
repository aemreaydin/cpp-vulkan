#include <vector>
#include <vulkan/vulkan.h>

#include "CValidationLayer.hpp"
#include "CVulkanHelpers.hpp"
#include "SQueueFamilies.hpp"
#include "appInfo.hpp"

class GLFWwindow;
class CInstance
{
  public:
    explicit CInstance(SAppInfo appInfo);
    ~CInstance();
    void CreateInstance();
    void CreatePhysicalDevice();
    void CreateSurface(GLFWwindow *window);
    void UpdateSwaphchainSupport();

    inline const VkPhysicalDevice PhysicalDevice() const
    {
        return m_physicalDevice;
    }

    inline const VkSurfaceKHR Surface() const
    {
        return m_surface;
    }

    inline SQueueFamilies QueueFamilies() const
    {
        return m_queueFamilies;
    }

    inline SSwapchainSupport SwapchainSupport() const
    {
        return m_swapchainSupport;
    }

  private:
    std::vector<VkPhysicalDevice> FindPhysicalDevices();
    std::vector<VkQueueFamilyProperties> FindQueueFamiliesForDevice(const VkPhysicalDevice device);
    bool CheckIfDeviceSuitable(const VkPhysicalDevice device);

  private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    std::unique_ptr<CValidationLayer> mp_validationLayer;
    SQueueFamilies m_queueFamilies{};
    SSwapchainSupport m_swapchainSupport;
    SAppInfo m_appInfo;
};
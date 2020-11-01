#pragma once

#include <vulkan/vulkan.h>

struct SBufferHandles
{
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

class CBufferImageManager
{
  public:
    CBufferImageManager(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue, VkCommandPool commandPool);
    void CreateBuffer(const VkBufferCreateInfo createInfo, VkMemoryPropertyFlags memFlags,
                      SBufferHandles &bufferHandles) const;

    void CopyBuffer(const VkBuffer src, const VkBufferCopy copyRegions, VkBuffer &dst) const;
    void MapMemory(const SBufferHandles &bufferHandles, const VkDeviceSize offset, const VkDeviceSize size,
                   const void *pData) const;

    void DestroyBufferHandles(SBufferHandles &bufferHandles) const;

  private:
    uint32_t FindMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags flags) const;

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_queue = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
};

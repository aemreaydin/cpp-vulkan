#pragma once

#include "CDevice.hpp"
#include <vulkan/vulkan.h>

struct SBufferHandles
{
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

struct SImageHandles
{
    VkImage image = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

class CBufferImageManager
{
  public:
    explicit CBufferImageManager(VkPhysicalDevice physicalDevice);
    void CreateBuffer(const VkBufferCreateInfo createInfo, VkMemoryPropertyFlags memFlags,
                      SBufferHandles &bufferHandles) const;

    void CreateImage(const VkImageCreateInfo createInfo, VkMemoryPropertyFlags memFlags,
                     SImageHandles &imageHandles) const;

    void CreateImageView(SImageHandles &imageHandles) const;

    void CopyBuffer(const VkBuffer src, const VkBufferCopy copyRegions, VkBuffer &dst) const;
    void MapMemory(const SBufferHandles &bufferHandles, const VkDeviceSize offset, const VkDeviceSize size,
                   const void *pData) const;

    void DestroyBufferHandles(SBufferHandles &bufferHandles) const;

  private:
    uint32_t FindMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags flags) const;

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
};

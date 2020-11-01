#include "CBufferImageManager.hpp"
#include <iostream>

CBufferImageManager::CBufferImageManager(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue queue,
                                         VkCommandPool commandPool)
    : m_physicalDevice(physicalDevice), m_device(device), m_queue(queue), m_commandPool(commandPool)
{
}

void CBufferImageManager::CreateBuffer(const VkBufferCreateInfo createInfo, VkMemoryPropertyFlags memFlags,
                                       SBufferHandles &bufferHandles) const
{
    if (const auto res = vkCreateBuffer(m_device, &createInfo, nullptr, &bufferHandles.buffer); res != VK_SUCCESS)
        throw std::runtime_error("Failed to create buffer.");

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(m_device, bufferHandles.buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, memFlags);
    memoryAllocateInfo.allocationSize = memoryRequirements.size;

    if (const auto res = vkAllocateMemory(m_device, &memoryAllocateInfo, nullptr, &bufferHandles.memory);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to create buffer device memory.");

    if (const auto res = vkBindBufferMemory(m_device, bufferHandles.buffer, bufferHandles.memory, 0); res != VK_SUCCESS)
        throw std::runtime_error("Failed to bind buffer to memory.");
}

void CBufferImageManager::CopyBuffer(const VkBuffer src, const VkBufferCopy copyRegions, VkBuffer &dst) const
{
    // Create and allocate a command buffer
    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = m_commandPool;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    if (const auto res = vkAllocateCommandBuffers(m_device, &allocateInfo, &commandBuffer); res != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffer.");

    // Begin command buffer for the copy
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (const auto res = vkBeginCommandBuffer(commandBuffer, &beginInfo); res != VK_SUCCESS)
        throw std::runtime_error("Failed to begin command buffer.");

    // Copy the src to dst
    vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegions);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (const auto res = vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE); res != VK_SUCCESS)
        throw std::runtime_error("Failed to submit queue.");

    vkQueueWaitIdle(m_queue);

    vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

void CBufferImageManager::MapMemory(const SBufferHandles &bufferHandles, const VkDeviceSize offset,
                                    const VkDeviceSize size, const void *pData) const
{
    void *data;
    vkMapMemory(m_device, bufferHandles.memory, offset, size, 0, &data);
    memcpy(data, pData, size);
    vkUnmapMemory(m_device, bufferHandles.memory);
}

void CBufferImageManager::DestroyBufferHandles(SBufferHandles &bufferHandles) const
{
    vkDestroyBuffer(m_device, bufferHandles.buffer, nullptr);
    vkFreeMemory(m_device, bufferHandles.memory, nullptr);
}

uint32_t CBufferImageManager::FindMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags flags) const
{
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &deviceMemoryProperties);

    for (auto i = 0; i != deviceMemoryProperties.memoryTypeCount; ++i)
    {
        if (memoryTypeBits & (1 << i))
        {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
            {
                return i;
            }
        }
    }
    throw std::runtime_error("Failed to find a suitable memory type.");
}

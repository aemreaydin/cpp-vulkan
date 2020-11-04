#include "CBufferImageManager.hpp"
#include <iostream>

CBufferImageManager::CBufferImageManager(VkPhysicalDevice physicalDevice) : m_physicalDevice(physicalDevice)
{
}

void CBufferImageManager::CreateBuffer(const VkBufferCreateInfo createInfo, VkMemoryPropertyFlags memFlags,
                                       SBufferHandles &bufferHandles) const
{
    if (const auto res =
            vkCreateBuffer(CDevice::GetInstance().GetDevice(), &createInfo, nullptr, &bufferHandles.buffer);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to create buffer.");

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(CDevice::GetInstance().GetDevice(), bufferHandles.buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, memFlags);
    memoryAllocateInfo.allocationSize = memoryRequirements.size;

    if (const auto res =
            vkAllocateMemory(CDevice::GetInstance().GetDevice(), &memoryAllocateInfo, nullptr, &bufferHandles.memory);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to create buffer device memory.");

    if (const auto res =
            vkBindBufferMemory(CDevice::GetInstance().GetDevice(), bufferHandles.buffer, bufferHandles.memory, 0);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to bind buffer to memory.");
}

void CBufferImageManager::CreateImage(const VkImageCreateInfo createInfo, VkMemoryPropertyFlags memFlags,
                                      SImageHandles &imageHandles) const
{
    if (const auto res = vkCreateImage(CDevice::GetInstance().GetDevice(), &createInfo, nullptr, &imageHandles.image);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to create image.");

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(CDevice::GetInstance().GetDevice(), imageHandles.image, &memReq);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memReq.size;
    allocateInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, memFlags);

    if (const auto res =
            vkAllocateMemory(CDevice::GetInstance().GetDevice(), &allocateInfo, nullptr, &imageHandles.memory);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate image memory.");

    if (const auto res =
            vkBindImageMemory(CDevice::GetInstance().GetDevice(), imageHandles.image, imageHandles.memory, 0);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to bind image memory.");

    CreateImageView(imageHandles);
}

void CBufferImageManager::CreateImageView(SImageHandles &imageHandles) const
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = imageHandles.image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (const auto res =
            vkCreateImageView(CDevice::GetInstance().GetDevice(), &createInfo, nullptr, &imageHandles.imageView);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to create image view.");
}

void CBufferImageManager::CopyBuffer(const VkBuffer src, const VkBufferCopy copyRegions, VkBuffer &dst) const
{
    // Create and allocate a command buffer
    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = CDevice::GetInstance().GetCommandPool();
    allocateInfo.commandBufferCount = 1;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    if (const auto res = vkAllocateCommandBuffers(CDevice::GetInstance().GetDevice(), &allocateInfo, &commandBuffer);
        res != VK_SUCCESS)
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

    if (const auto res = vkQueueSubmit(CDevice::GetInstance().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to submit queue.");

    vkQueueWaitIdle(CDevice::GetInstance().GetGraphicsQueue());

    vkFreeCommandBuffers(CDevice::GetInstance().GetDevice(), CDevice::GetInstance().GetCommandPool(), 1,
                         &commandBuffer);
}

void CBufferImageManager::CopyBufferToImage(const VkBuffer src, const VkBufferImageCopy copyRegions, VkImage &dst) const
{
    VkCommandBuffer cmdBuffer;
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = CDevice::GetInstance().GetCommandPool();
    allocateInfo.commandBufferCount = 1;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    if (const auto res = vkAllocateCommandBuffers(CDevice::GetInstance().GetDevice(), &allocateInfo, &cmdBuffer);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate cmd buffer to copy buffer to image.");

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmdBuffer, &beginInfo);

    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = dst;
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &imageMemoryBarrier);

    vkCmdCopyBufferToImage(cmdBuffer, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegions);

    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0,
                         nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    if (const auto res = vkQueueSubmit(CDevice::GetInstance().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to submit queue.");

    vkQueueWaitIdle(CDevice::GetInstance().GetGraphicsQueue());

    vkFreeCommandBuffers(CDevice::GetInstance().GetDevice(), CDevice::GetInstance().GetCommandPool(), 1, &cmdBuffer);
}

void CBufferImageManager::MapMemory(const SBufferHandles &bufferHandles, const VkDeviceSize offset,
                                    const VkDeviceSize size, const void *pData) const
{
    void *data;
    vkMapMemory(CDevice::GetInstance().GetDevice(), bufferHandles.memory, offset, size, 0, &data);
    memcpy(data, pData, size);
    vkUnmapMemory(CDevice::GetInstance().GetDevice(), bufferHandles.memory);
}

void CBufferImageManager::DestroyBufferHandles(SBufferHandles &bufferHandles) const
{
    vkDestroyBuffer(CDevice::GetInstance().GetDevice(), bufferHandles.buffer, nullptr);
    vkFreeMemory(CDevice::GetInstance().GetDevice(), bufferHandles.memory, nullptr);
}

void CBufferImageManager::DestroyImagesHandles(SImageHandles &bufferHandles) const
{
    vkDestroyImage(CDevice::GetInstance().GetDevice(), bufferHandles.image, nullptr);
    vkDestroyImageView(CDevice::GetInstance().GetDevice(), bufferHandles.imageView, nullptr);
    vkFreeMemory(CDevice::GetInstance().GetDevice(), bufferHandles.memory, nullptr);
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


#include "CGameObject.hpp"
#include "CImageLoader.hpp"
#include "CModelLoader.hpp"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

CGameObject::CGameObject(SModelProps modelProps) : m_modelProps(modelProps)
{
    mp_deviceInstance = &CDevice::GetInstance();
    m_mesh = CModelLoader::LoadObjModel(modelProps.objectFile);

    InitObject();
}

void CGameObject::InitObject()
{
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffers();
    CreateTextureImage();
    CreateTextureSampler();
    CreateDescriptorSets();
}

void CGameObject::UpdateUniformBuffers()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    const auto currentTime = std::chrono::high_resolution_clock::now();
    const auto time = std::chrono::duration<float, std::chrono::seconds ::period>(currentTime - startTime).count();

    m_mvp.model = glm::translate(glm::mat4(1.0f), m_modelProps.modelTransform.translate);
    m_mvp.model = glm::scale(m_mvp.model, m_modelProps.modelTransform.scale);
    m_mvp.model = glm::rotate(m_mvp.model, time * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_mvp.view = glm::lookAt(glm::vec3(10.0f, 0.01f, 10.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_mvp.projection = glm::perspective(
        glm::radians(45.0f),
        mp_deviceInstance->GetExtent().width / static_cast<float>(mp_deviceInstance->GetExtent().height), 0.1f, 100.0f);
    //    m_mvp.projection = glm::ortho(-2.0f, 2.0f, 2.0f, -2.0f, 0.1f, 10.0f);
    m_mvp.projection[1][1] *= -1;

    void *data;
    vkMapMemory(mp_deviceInstance->GetDevice(),
                m_vecUniformBufferHandles[mp_deviceInstance->GetCurrentImageIndex()].memory, 0, sizeof(m_mvp), 0,
                &data);
    memcpy(data, &m_mvp, sizeof(m_mvp));
    vkUnmapMemory(mp_deviceInstance->GetDevice(),
                  m_vecUniformBufferHandles[mp_deviceInstance->GetCurrentImageIndex()].memory);
}

void CGameObject::Draw() const
{
    VkCommandBuffer cmdBuffer = mp_deviceInstance->GetCurrentCommandBuffer();

    VkDeviceSize offsets = {0};
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &m_vertexBufferHandles.buffer, &offsets);

    vkCmdBindIndexBuffer(cmdBuffer, m_indexBufferHandles.buffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mp_deviceInstance->GetPipelineLayout(), 0, 1,
                            &m_vecDescriptorSets[mp_deviceInstance->GetCurrentImageIndex()], 0, nullptr);

    vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_mesh.indices.size()), 1, 0, 0, 0);
}

void CGameObject::CreateVertexBuffer()
{
    SBufferHandles stagingHandles{};
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = GetVerticesSize();
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    mp_deviceInstance->GetBufferImageManager().CreateBuffer(
        createInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, stagingHandles);

    mp_deviceInstance->GetBufferImageManager().MapMemory(stagingHandles, 0, GetVerticesSize(), m_mesh.vertices.data());

    // Create the vertex buffer and copy the staged buffer to it
    createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    mp_deviceInstance->GetBufferImageManager().CreateBuffer(createInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                            m_vertexBufferHandles);

    VkBufferCopy regions{};
    regions.size = GetVerticesSize();
    mp_deviceInstance->GetBufferImageManager().CopyBuffer(stagingHandles.buffer, regions, m_vertexBufferHandles.buffer);

    // Destroy the staging buffer
    mp_deviceInstance->GetBufferImageManager().DestroyBufferHandles(stagingHandles);
}

void CGameObject::CreateIndexBuffer()
{
    SBufferHandles stagingHandles{};
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = GetVerticesSize();
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    mp_deviceInstance->GetBufferImageManager().CreateBuffer(
        createInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, stagingHandles);

    mp_deviceInstance->GetBufferImageManager().MapMemory(stagingHandles, 0, GetIndicesSize(), m_mesh.indices.data());

    // Create the vertex buffer and copy the staged buffer to it
    createInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    mp_deviceInstance->GetBufferImageManager().CreateBuffer(createInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                            m_indexBufferHandles);

    VkBufferCopy regions{};
    regions.size = GetIndicesSize();
    mp_deviceInstance->GetBufferImageManager().CopyBuffer(stagingHandles.buffer, regions, m_indexBufferHandles.buffer);

    // Destroy the staging buffer
    mp_deviceInstance->GetBufferImageManager().DestroyBufferHandles(stagingHandles);
}

void CGameObject::CreateUniformBuffers()
{
    m_vecUniformBufferHandles.resize(mp_deviceInstance->GetSwapchainImageCount());

    for (auto &handle : m_vecUniformBufferHandles)
    {
        VkBufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = GetUniformSize();
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        mp_deviceInstance->GetBufferImageManager().CreateBuffer(
            createInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, handle);
    }
}

void CGameObject::CreateDescriptorSets()
{
    m_vecDescriptorSets.resize(mp_deviceInstance->GetSwapchainImageCount());

    std::vector<VkDescriptorSetLayout> layouts(mp_deviceInstance->GetSwapchainImageCount(),
                                               mp_deviceInstance->GetDescriptorSetLayout());
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = mp_deviceInstance->GetDescriptorPool();
    allocateInfo.descriptorSetCount = layouts.size();
    allocateInfo.pSetLayouts = layouts.data();
    if (const auto res =
            vkAllocateDescriptorSets(mp_deviceInstance->GetDevice(), &allocateInfo, m_vecDescriptorSets.data());
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor set.");

    for (auto i = 0; i != m_vecDescriptorSets.size(); ++i)
    {
        VkDescriptorBufferInfo mvpInfo{};
        mvpInfo.buffer = m_vecUniformBufferHandles[i].buffer;
        mvpInfo.range = GetUniformSize();
        mvpInfo.offset = 0;

        VkDescriptorImageInfo texInfo{};
        texInfo.imageView = m_textureImageHandles.imageView;
        texInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        texInfo.sampler = m_textureSampler;

        std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};
        writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSets[0].descriptorCount = 1;
        writeDescriptorSets[0].dstSet = m_vecDescriptorSets[i];
        writeDescriptorSets[0].dstBinding = 0;
        writeDescriptorSets[0].dstArrayElement = 0;
        writeDescriptorSets[0].pBufferInfo = &mvpInfo;

        writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSets[1].descriptorCount = 1;
        writeDescriptorSets[1].dstSet = m_vecDescriptorSets[i];
        writeDescriptorSets[1].dstBinding = 1;
        writeDescriptorSets[1].dstArrayElement = 0;
        writeDescriptorSets[1].pImageInfo = &texInfo;

        vkUpdateDescriptorSets(mp_deviceInstance->GetDevice(), writeDescriptorSets.size(), writeDescriptorSets.data(),
                               0, nullptr);
    }
}

void CGameObject::CreateTextureImage()
{
    int width, height, channels;
    const auto imageData = CImageLoader::Load2DImage(m_modelProps.textureFile, width, height, channels);
    const auto imageSize = width * height * 4;
    // Stage image to buffer
    SBufferHandles stagingHandles{};
    VkBufferCreateInfo stagingInfo{};
    stagingInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingInfo.size = imageSize;
    stagingInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    mp_deviceInstance->GetBufferImageManager().CreateBuffer(
        stagingInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, stagingHandles);

    mp_deviceInstance->GetBufferImageManager().MapMemory(stagingHandles, 0, imageSize, imageData);

    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.extent.width = width;
    createInfo.extent.height = height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = mp_deviceInstance->GetVulkanInstance()->QueueFamilies().GraphicsFamily();
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    mp_deviceInstance->GetBufferImageManager().CreateImage(createInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                           m_textureImageHandles);

    // Copy the buffer to image
    VkImageSubresourceLayers layers{};
    layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    layers.mipLevel = 0;
    layers.baseArrayLayer = 0;
    layers.layerCount = 1;

    VkBufferImageCopy copyRegions{};
    copyRegions.imageExtent.width = width;
    copyRegions.imageExtent.height = height;
    copyRegions.imageExtent.depth = 1;
    copyRegions.imageSubresource = layers;
    mp_deviceInstance->GetBufferImageManager().CopyBufferToImage(stagingHandles.buffer, copyRegions,
                                                                 m_textureImageHandles.image);

    CImageLoader::FreeImage(imageData);
    mp_deviceInstance->GetBufferImageManager().DestroyBufferHandles(stagingHandles);
}

void CGameObject::CreateTextureSampler()
{
    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.minLod = 0.0f;
    createInfo.maxLod = 0.0f;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.mipLodBias = 0.0f;
    createInfo.anisotropyEnable = VK_TRUE;
    createInfo.maxAnisotropy = 16.0f;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;

    if (const auto res = vkCreateSampler(mp_deviceInstance->GetDevice(), &createInfo, nullptr, &m_textureSampler);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to create sampler.");
}

void CGameObject::ObjectCleanup()
{
    mp_deviceInstance->GetBufferImageManager().DestroyBufferHandles(m_vertexBufferHandles);
    mp_deviceInstance->GetBufferImageManager().DestroyBufferHandles(m_indexBufferHandles);
    for (auto &handle : m_vecUniformBufferHandles)
    {
        mp_deviceInstance->GetBufferImageManager().DestroyBufferHandles(handle);
    }
    vkDestroySampler(mp_deviceInstance->GetDevice(), m_textureSampler, nullptr);
    mp_deviceInstance->GetBufferImageManager().DestroyImagesHandles(m_textureImageHandles);
}

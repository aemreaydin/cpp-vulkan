#include "CModel.hpp"
#include "CModelLoader.hpp"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

CModel::CModel(std::string objFile, glm::vec3 pos) : m_pos(pos)
{
    mp_deviceInstance = &CDevice::GetInstance();
    m_mesh = CModelLoader::LoadObjModel(objFile);
    InitModel();
}

void CModel::InitModel()
{
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffers();
    CreateDescriptorSets();
    CreateTextureImage();
}

void CModel::UpdateUniformBuffers()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    const auto currentTime = std::chrono::high_resolution_clock::now();
    const auto time = std::chrono::duration<float, std::chrono::seconds ::period>(currentTime - startTime).count();

    m_mvp.model = glm::translate(glm::mat4(1.0f), m_pos);
    m_mvp.model = glm::scale(m_mvp.model, glm::vec3(1.0f));
    m_mvp.model = glm::rotate(m_mvp.model, time * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_mvp.view = glm::lookAt(glm::vec3(10.0f, 0.0f, 0.01f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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

void CModel::Draw() const
{
    VkCommandBuffer cmdBuffer = mp_deviceInstance->GetCurrentCommandBuffer();

    VkDeviceSize offsets = {0};
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &m_vertexBufferHandles.buffer, &offsets);

    vkCmdBindIndexBuffer(cmdBuffer, m_indexBufferHandles.buffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mp_deviceInstance->GetPipelineLayout(), 0, 1,
                            &m_vecDescriptorSets[mp_deviceInstance->GetCurrentImageIndex()], 0, nullptr);

    vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_mesh.indices.size()), 1, 0, 0, 0);
}

void CModel::CreateVertexBuffer()
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

void CModel::CreateIndexBuffer()
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

void CModel::CreateUniformBuffers()
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

void CModel::CreateDescriptorSets()
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

        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.dstSet = m_vecDescriptorSets[i];
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.pBufferInfo = &mvpInfo;

        vkUpdateDescriptorSets(mp_deviceInstance->GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
    }
}

void CModel::CreateTextureImage()
{
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.extent.width = mp_deviceInstance->GetExtent().width;
    createInfo.extent.height = mp_deviceInstance->GetExtent().height;
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
}

void CModel::ModelCleanup()
{
    vkDestroyBuffer(mp_deviceInstance->GetDevice(), m_vertexBufferHandles.buffer, nullptr);
    vkFreeMemory(mp_deviceInstance->GetDevice(), m_vertexBufferHandles.memory, nullptr);
    vkDestroyBuffer(mp_deviceInstance->GetDevice(), m_indexBufferHandles.buffer, nullptr);
    vkFreeMemory(mp_deviceInstance->GetDevice(), m_indexBufferHandles.memory, nullptr);
    for (auto &handle : m_vecUniformBufferHandles)
    {
        vkDestroyBuffer(mp_deviceInstance->GetDevice(), handle.buffer, nullptr);
        vkFreeMemory(mp_deviceInstance->GetDevice(), handle.memory, nullptr);
    }
    vkDestroyImage(mp_deviceInstance->GetDevice(), m_textureImageHandles.image, nullptr);
    vkDestroyImageView(mp_deviceInstance->GetDevice(), m_textureImageHandles.imageView, nullptr);
    vkFreeMemory(mp_deviceInstance->GetDevice(), m_textureImageHandles.memory, nullptr);
}

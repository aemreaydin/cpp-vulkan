#include "CModel.hpp"
#include "CModelLoader.hpp"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

CModel::CModel(std::string objFile, glm::vec3 pos) : m_pos(pos)
{
    m_mesh = CModelLoader::LoadObjModel(objFile);
}

void CModel::InitModel(const CBufferImageManager &manager, uint32_t numImages)
{
    CreateVertexBuffer(manager);
    CreateIndexBuffer(manager);
    CreateUniformBuffers(manager, numImages);
}

void CModel::Draw(const VkDevice device, const VkExtent2D extent, uint32_t frameIndex)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    const auto currentTime = std::chrono::high_resolution_clock::now();
    const auto time = std::chrono::duration<float, std::chrono::seconds ::period>(currentTime - startTime).count();

    m_mvp.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
    m_mvp.model = glm::scale(m_mvp.model, glm::vec3(1.0f));
    m_mvp.model = glm::rotate(m_mvp.model, time * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_mvp.view = glm::lookAt(glm::vec3(10.0f, 0.0f, 0.01f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_mvp.projection =
        glm::perspective(glm::radians(45.0f), extent.width / static_cast<float>(extent.height), 0.1f, 100.0f);
    //    m_mvp.projection = glm::ortho(-2.0f, 2.0f, 2.0f, -2.0f, 0.1f, 10.0f);
    m_mvp.projection[1][1] *= -1;

    void *data;
    vkMapMemory(device, m_vecUniformBufferHandles[frameIndex].memory, 0, sizeof(m_mvp), 0, &data);
    memcpy(data, &m_mvp, sizeof(m_mvp));
    vkUnmapMemory(device, m_vecUniformBufferHandles[frameIndex].memory);
}

void CModel::CreateVertexBuffer(const CBufferImageManager &manager)
{
    SBufferHandles stagingHandles{};
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = GetVerticesSize();
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    manager.CreateBuffer(createInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                         stagingHandles);

    manager.MapMemory(stagingHandles, 0, GetVerticesSize(), m_mesh.vertices.data());

    // Create the vertex buffer and copy the staged buffer to it
    createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    manager.CreateBuffer(createInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBufferHandles);

    VkBufferCopy regions{};
    regions.size = GetVerticesSize();
    manager.CopyBuffer(stagingHandles.buffer, regions, vertexBufferHandles.buffer);

    // Destroy the staging buffer
    manager.DestroyBufferHandles(stagingHandles);
}

void CModel::CreateIndexBuffer(const CBufferImageManager &manager)
{
    SBufferHandles stagingHandles{};
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = GetVerticesSize();
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    manager.CreateBuffer(createInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                         stagingHandles);

    manager.MapMemory(stagingHandles, 0, GetIndicesSize(), m_mesh.indices.data());

    // Create the vertex buffer and copy the staged buffer to it
    createInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    manager.CreateBuffer(createInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBufferHandles);

    VkBufferCopy regions{};
    regions.size = GetIndicesSize();
    manager.CopyBuffer(stagingHandles.buffer, regions, indexBufferHandles.buffer);

    // Destroy the staging buffer
    manager.DestroyBufferHandles(stagingHandles);
}

void CModel::CreateUniformBuffers(const CBufferImageManager &manager, uint32_t numImages)
{
    m_vecUniformBufferHandles.resize(numImages);

    for (auto &handle : m_vecUniformBufferHandles)
    {
        VkBufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = GetUniformSize();
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        manager.CreateBuffer(createInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                             handle);
    }
}

void CModel::DestroyModel(const VkDevice device)
{
    vkDestroyBuffer(device, vertexBufferHandles.buffer, nullptr);
    vkFreeMemory(device, vertexBufferHandles.memory, nullptr);
    vkDestroyBuffer(device, indexBufferHandles.buffer, nullptr);
    vkFreeMemory(device, indexBufferHandles.memory, nullptr);
    for (auto &handle : m_vecUniformBufferHandles)
    {
        vkDestroyBuffer(device, handle.buffer, nullptr);
        vkFreeMemory(device, handle.memory, nullptr);
    }
}

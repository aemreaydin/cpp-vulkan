#include "CModel.hpp"
#include "CModelLoader.hpp"

CModel::CModel(std::string objFile, const CBufferImageManager &manager)
{
    m_mesh = CModelLoader::LoadObjModel(objFile);

    CreateVertexBuffer(manager);
    CreateIndexBuffer(manager);
}

uint32_t CModel::GetVerticesSize() const
{
    return static_cast<uint32_t>(sizeof(SVertex) * m_mesh.vertices.size());
}

uint16_t CModel::GetIndicesSize() const
{
    return static_cast<uint16_t>(sizeof(uint16_t) * m_mesh.indices.size());
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
void CModel::DestroyModel(const VkDevice device)
{
    vkDestroyBuffer(device, vertexBufferHandles.buffer, nullptr);
    vkFreeMemory(device, vertexBufferHandles.memory, nullptr);
    vkDestroyBuffer(device, indexBufferHandles.buffer, nullptr);
    vkFreeMemory(device, indexBufferHandles.memory, nullptr);
}

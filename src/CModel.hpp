#pragma once
#include "CBufferImageManager.hpp"
#include "Primitives.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

class CModel
{
  public:
    CModel() = default;
    CModel(std::string objFile, glm::vec3 pos = glm::vec3(0.0f));

    void InitModel(const CBufferImageManager &manager, uint32_t numImages);

    void Draw(const VkDevice device, const VkExtent2D extent, uint32_t frameIndex);

    uint32_t GetVerticesSize() const
    {
        return static_cast<uint32_t>(sizeof(SVertex) * m_mesh.vertices.size());
    }
    uint16_t GetIndicesSize() const
    {
        return static_cast<uint16_t>(sizeof(uint16_t) * m_mesh.indices.size());
    }
    uint32_t GetUniformSize() const
    {
        return static_cast<uint32_t>(sizeof(m_mvp));
    }

    const VkBuffer GetVertexBuffer() const
    {
        return vertexBufferHandles.buffer;
    }

    const VkBuffer GetIndexBuffer() const
    {
        return indexBufferHandles.buffer;
    }

    const VkBuffer GetUniformBuffer(uint32_t frameIndex) const
    {
        return m_vecUniformBufferHandles[frameIndex].buffer;
    }

    void DestroyModel(const VkDevice device);

  private:
    void CreateVertexBuffer(const CBufferImageManager &manager);
    void CreateIndexBuffer(const CBufferImageManager &manager);
    void CreateUniformBuffers(const CBufferImageManager &manager, uint32_t numImages);

    SBufferHandles vertexBufferHandles{};
    SBufferHandles indexBufferHandles{};
    std::vector<SBufferHandles> m_vecUniformBufferHandles{};

    SMesh m_mesh;
    SMVP m_mvp{};
    glm::vec3 m_pos;
};
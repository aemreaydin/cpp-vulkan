#pragma once
#include "CBufferImageManager.hpp"
#include "CDevice.hpp"
#include "Primitives.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

class CModel
{
  public:
    CModel() = default;
    explicit CModel(std::string objFile, glm::vec3 pos = glm::vec3(0.0f));

    void InitModel();

    void UpdateUniformBuffers();
    void Draw() const;

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
        return m_vertexBufferHandles.buffer;
    }

    const VkBuffer GetIndexBuffer() const
    {
        return m_indexBufferHandles.buffer;
    }

    const VkBuffer GetUniformBuffer(uint32_t frameIndex) const
    {
        return m_vecUniformBufferHandles[frameIndex].buffer;
    }

    const VkDescriptorSet GetDescriptorSet(uint32_t frameIndex) const
    {
        return m_vecDescriptorSets[frameIndex];
    }

    void ModelCleanup();

  private:
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateUniformBuffers();
    void CreateDescriptorSets();
    void CreateTextureImage();

    CDevice *mp_deviceInstance;

    SBufferHandles m_vertexBufferHandles{};
    SBufferHandles m_indexBufferHandles{};
    SImageHandles m_textureImageHandles{};
    std::vector<SBufferHandles> m_vecUniformBufferHandles{};
    std::vector<VkDescriptorSet> m_vecDescriptorSets;

    SMesh m_mesh;
    SMVP m_mvp{};
    glm::vec3 m_pos;
};
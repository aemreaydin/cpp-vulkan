#pragma once
#include "CBufferImageManager.hpp"
#include "CDevice.hpp"
#include "vkPrimitives.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

struct SModelTransform
{
    glm::vec3 translate{1.0f};
    glm::vec3 rotate{1.0f};
    glm::vec3 scale{1.0f};
};

struct SModelProps
{
    std::string modelName;
    std::string objectFile;
    std::string textureFile;
    SModelTransform modelTransform{};
};

class CModel
{
  public:
    CModel() = default;
    explicit CModel(SModelProps modelProps);

    void InitModel();

    void UpdateUniformBuffers();
    void Draw() const;

    uint32_t GetVerticesSize() const
    {
        return static_cast<uint32_t>(sizeof(vkTools::vkPrimitives::SVertex) * m_mesh.vertices.size());
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
    void CreateTextureSampler();

    CDevice *mp_deviceInstance;

    SBufferHandles m_vertexBufferHandles{};
    SBufferHandles m_indexBufferHandles{};
    std::vector<SBufferHandles> m_vecUniformBufferHandles{};
    std::vector<VkDescriptorSet> m_vecDescriptorSets;
    SImageHandles m_textureImageHandles{};
    VkSampler m_textureSampler;

    vkTools::vkPrimitives::SMesh m_mesh;
    vkTools::vkPrimitives::SMVP m_mvp{};
    SModelProps m_modelProps{};
};
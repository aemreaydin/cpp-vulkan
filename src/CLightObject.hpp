#pragma once

#include "CBufferImageManager.hpp"
#include "CObject.hpp"
#include "vkPrimitives.hpp"

class CLightObject : public CObject
{
  public:
    explicit CLightObject(vkTools::vkPrimitives::STransform transform = {glm::vec3(1.0f), glm::vec3(0.25f),
                                                                         glm::vec3(1.0f)});

    void CleanupGraphicsPipeline();
    void RecreateGraphicsPipeline();
    void UpdateUniformBuffers() override;
    void Draw() const override;
    void ObjectCleanup() override;

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

  private:
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateUniformBuffers();
    void CreateDescriptorSets();
    void CreateGraphicsPipeline();

    CDevice *mp_deviceInstance;

    SBufferHandles m_vertexBufferHandles{};
    SBufferHandles m_indexBufferHandles{};
    std::vector<SBufferHandles> m_vecUniformBufferHandles{};
    std::vector<VkDescriptorSet> m_vecDescriptorSets;

    VkPipelineLayout m_graphicsPipelineLayout;
    VkPipeline m_graphicsPipeline;

    vkTools::vkPrimitives::SMesh m_mesh;
    vkTools::vkPrimitives::SMVP m_mvp{};
    vkTools::vkPrimitives::STransform m_transform;
};

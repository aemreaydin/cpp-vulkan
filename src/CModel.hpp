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
    CModel(std::string objFile, const CBufferImageManager &manager);
    uint32_t GetVerticesSize() const;
    uint16_t GetIndicesSize() const;

    const VkBuffer GetVertexBuffer() const
    {
        return vertexBufferHandles.buffer;
    }

    const VkBuffer GetIndexBuffer() const
    {
        return indexBufferHandles.buffer;
    }

    void DestroyModel(const VkDevice device);

  private:
    void CreateVertexBuffer(const CBufferImageManager &manager);
    void CreateIndexBuffer(const CBufferImageManager &manager);

    SBufferHandles vertexBufferHandles{};
    SBufferHandles indexBufferHandles{};

    SMesh m_mesh;
};
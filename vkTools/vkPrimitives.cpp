#include "vkPrimitives.hpp"

namespace vkTools
{
namespace vkPrimitives
{
VkVertexInputBindingDescription SVertex::GetInputBindingDescription()
{

    VkVertexInputBindingDescription description{};
    description.binding = 0;
    description.stride = sizeof(SVertex);
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return description;
}

std::array<VkVertexInputAttributeDescription, 3> SVertex::GetAttributeBindingDescription()
{
    VkVertexInputAttributeDescription posDesc{};
    posDesc.binding = 0;
    posDesc.location = 0;
    posDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
    posDesc.offset = offsetof(SVertex, position);

    VkVertexInputAttributeDescription normalDesc{};
    normalDesc.binding = 0;
    normalDesc.location = 1;
    normalDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
    normalDesc.offset = offsetof(SVertex, normal);

    VkVertexInputAttributeDescription texDesc{};
    texDesc.binding = 0;
    texDesc.location = 2;
    texDesc.format = VK_FORMAT_R32G32_SFLOAT;
    texDesc.offset = offsetof(SVertex, uv);

    return {posDesc, normalDesc, texDesc};
}
} // namespace vkPrimitives
} // namespace vkTools

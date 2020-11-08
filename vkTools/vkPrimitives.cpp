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

    VkVertexInputAttributeDescription texDesc{};
    texDesc.binding = 0;
    texDesc.location = 1;
    texDesc.format = VK_FORMAT_R32G32_SFLOAT;
    texDesc.offset = offsetof(SVertex, uv);

    VkVertexInputAttributeDescription colorDesc{};
    colorDesc.binding = 0;
    colorDesc.location = 2;
    colorDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
    colorDesc.offset = offsetof(SVertex, color);

    return {posDesc, texDesc, colorDesc};
}
} // namespace vkPrimitives
} // namespace vkTools

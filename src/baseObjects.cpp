#include "baseObjects.hpp"

VkVertexInputBindingDescription SVertex::GetInputBindingDescription()
{

    VkVertexInputBindingDescription description{};
    description.binding = 0;
    description.stride = sizeof(SVertex);
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return description;
}

std::array<VkVertexInputAttributeDescription, 2> SVertex::GetAttributeBindingDescription()
{
    VkVertexInputAttributeDescription posDecs{};
    posDecs.binding = 0;
    posDecs.location = 0;
    posDecs.format = VK_FORMAT_R32G32B32_SFLOAT;
    posDecs.offset = offsetof(SVertex, position);

    VkVertexInputAttributeDescription colDecs{};
    colDecs.binding = 0;
    colDecs.location = 1;
    colDecs.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    colDecs.offset = offsetof(SVertex, color);

    return {posDecs, colDecs};
}
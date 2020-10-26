#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <array>
#include <iostream>
#include <vector>

struct SVertex
{
    glm::vec3 position;
    glm::vec2 uv;

    static VkVertexInputBindingDescription GetInputBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeBindingDescription();
};

struct SMVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

struct SObject
{
    std::string name;
    std::vector<SVertex> vertices;
    std::vector<uint16_t> indices;
};
#pragma once
#include "Primitives.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

struct CModel
{
    std::string name;
    std::vector<SVertex> vertices;
    std::vector<uint16_t> indices;
};
#pragma once

#include <glm/glm.hpp>
#include <iostream>

#include "vkPrimitives.hpp"

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

class CObject
{
};

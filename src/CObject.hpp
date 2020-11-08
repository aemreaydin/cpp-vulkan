#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vulkan/vulkan.h>

#include "vkPrimitives.hpp"

class CObject
{
  public:
    virtual void UpdateUniformBuffers() = 0;
    virtual void Draw() const = 0;
    virtual void ObjectCleanup() = 0;
};

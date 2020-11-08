#pragma once
#include "vkPrimitives.hpp"

#include <iostream>
#include <vector>

class CModelLoader
{
  public:
    static vkTools::vkPrimitives::SMesh LoadObjModel(std::string objFile);
};
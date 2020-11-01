#pragma once

#include <iostream>
#include <vector>

struct SMesh;

class CModelLoader
{
  public:
    static SMesh LoadObjModel(std::string objFile);
};
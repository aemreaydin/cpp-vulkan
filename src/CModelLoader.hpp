#pragma once

#include <iostream>
#include <vector>

class CModel;

class CModelLoader
{
  public:
    static CModel LoadObjModel(std::string objFile);
};
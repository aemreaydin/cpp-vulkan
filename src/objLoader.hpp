#pragma once

#include <iostream>
#include <vector>

class CModel;

class CObjLoader
{
  public:
    static CModel LoadObj(std::string objFile);
};
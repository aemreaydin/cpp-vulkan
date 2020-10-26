#pragma once

#include <iostream>
#include <vector>
struct SObject;

class CObjLoader
{
  public:
    static SObject LoadObj(std::string objFile);
};
#pragma once

#include <iostream>

class CImageLoader
{
  public:
    static unsigned char *Load2DImage(const std::string imageFile, int &width, int &height, int &numChannels);
    static void FreeImage(void *data);
};
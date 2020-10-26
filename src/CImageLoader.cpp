#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "CImageLoader.hpp"
unsigned char *CImageLoader::Load2DImage(const std::string imageFile, int &width, int &height, int &numChannels)
{
    int w, h, channels;
    const auto data = stbi_load(imageFile.c_str(), &w, &h, &channels, STBI_rgb_alpha);
    if (!data)
        throw std::runtime_error(stbi_failure_reason());

    width = w;
    height = h;
    numChannels = channels;
    return data;
}
void CImageLoader::FreeImage(void *data)
{
    stbi_image_free(data);
}

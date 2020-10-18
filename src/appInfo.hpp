struct SAppInfo
{
    uint32_t width;
    uint32_t height;
    const std::vector<const char *> layers;
    const std::vector<const char *> deviceExtensions;

    SAppInfo(uint32_t width, uint32_t height, const std::vector<const char *> layers,
             const std::vector<const char *> deviceExtensions)
        : width(width), height(height), layers(layers), deviceExtensions(deviceExtensions)
    {
    }
};
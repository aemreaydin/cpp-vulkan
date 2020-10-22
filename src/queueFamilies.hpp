#include <optional>
#include <set>
#include <vulkan/vulkan.h>

struct SQueueFamilies
{
    std::optional<uint32_t> graphicsFamilyIndex;
    std::optional<uint32_t> presentFamilyIndex;
    std::optional<uint32_t> transferFamilyIndex;

    std::set<uint32_t> GetUniqueQueueFamilies()
    {
        return std::set<uint32_t>{graphicsFamilyIndex.value(), presentFamilyIndex.value(), transferFamilyIndex.value()};
    }

    bool HaveValues()
    {
        return graphicsFamilyIndex.has_value() && presentFamilyIndex.has_value() && transferFamilyIndex.has_value();
    }
};
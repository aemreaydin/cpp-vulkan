#pragma once

#include <optional>
#include <set>
#include <vulkan/vulkan.h>

struct SQueueFamilies
{
    std::optional<uint32_t> graphicsFamilyIndex;
    std::optional<uint32_t> presentFamilyIndex;

    inline std::set<uint32_t> GetUniqueQueueFamilies() const
    {
        return std::set<uint32_t>{graphicsFamilyIndex.value(), presentFamilyIndex.value()};
    }

    inline bool HaveValues() const
    {
        return graphicsFamilyIndex.has_value() && presentFamilyIndex.has_value();
    }

    inline const uint32_t *GraphicsFamily() const
    {
        return &graphicsFamilyIndex.value();
    }

    inline const uint32_t *PresentFamily() const
    {
        return &presentFamilyIndex.value();
    }
};
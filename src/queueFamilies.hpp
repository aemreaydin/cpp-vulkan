#pragma once

#include <optional>
#include <set>
#include <vulkan/vulkan.h>

struct SQueueFamilies
{
    std::optional<uint32_t> graphicsFamilyIndex;
    std::optional<uint32_t> presentFamilyIndex;

    std::set<uint32_t> GetUniqueQueueFamilies()
    {
        return std::set<uint32_t>{graphicsFamilyIndex.value(), presentFamilyIndex.value()};
    }

    bool HaveValues()
    {
        return graphicsFamilyIndex.has_value() && presentFamilyIndex.has_value();
    }
};
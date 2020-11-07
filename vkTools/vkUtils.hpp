#pragma once
#include <vulkan/vulkan.hpp>

#define VK_CHECK_RESULT(f)                                                                                             \
    {                                                                                                                  \
        VkResult res = (f);                                                                                            \
        if (res != VK_SUCCESS)                                                                                         \
        {                                                                                                              \
            std::cout << "Fatal : Error in " << __FILE__ << " at line " << __LINE__ << "\n";                           \
            assert(res == VK_SUCCESS);                                                                                 \
        }                                                                                                              \
    }

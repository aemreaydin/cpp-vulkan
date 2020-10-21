#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <array>

struct SVertex
{
    glm::vec3 position;
    glm::vec4 color;

    static VkVertexInputBindingDescription GetInputBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeBindingDescription();
};
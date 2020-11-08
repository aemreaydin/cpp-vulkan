#pragma once
#include <vector>

#include "vkPrimitives.hpp"
#include "vkUtils.hpp"

namespace vkTools
{
namespace vkStructs
{
// Instance
VkApplicationInfo ApplicationInfo(const char *appName = "VULKAN APP", uint32_t appVersion = 1,
                                  const char *engineName = "VULKAN ENGINE", uint32_t engineVersion = 1);
VkInstanceCreateInfo InstanceCreateInfo(VkApplicationInfo applicationInfo,
                                        const std::vector<const char *> &vecExtensions,
                                        const std::vector<const char *> &vecLayers,
                                        VkDebugUtilsMessengerCreateInfoEXT debugInfo);

// Graphics Pipeline
VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout> &vecLayouts = {},
                                                    const std::vector<VkPushConstantRange> &vecPushConstantRanges = {});
VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo(
    const VkVertexInputBindingDescription &inputDesc,
    const std::array<VkVertexInputAttributeDescription, 3> &attributeDesc);
VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo();
VkPipelineTessellationStateCreateInfo TessellationStateCreateInfo();
VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo(VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL);
VkPipelineColorBlendStateCreateInfo ColorBlendStateCreateInfo(
    const VkPipelineColorBlendAttachmentState &attachmentState);
VkPipelineMultisampleStateCreateInfo MultisampleStateCreateInfo();
VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo();
VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo(const std::vector<VkDynamicState> &vecDynamicStates = {});
VkPipelineViewportStateCreateInfo ViewportCreateInfo(const std::vector<VkViewport> &vecViewports,
                                                     const std::vector<VkRect2D> &vecScissors);

VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(
    const std::vector<VkPipelineShaderStageCreateInfo> &vecShaderStagesInfos,
    const VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
    const VkPipelineInputAssemblyStateCreateInfo &inputAssemblyInfo,
    const VkPipelineTessellationStateCreateInfo &tesselationInfo, const VkPipelineViewportStateCreateInfo &viewportInfo,
    const VkPipelineRasterizationStateCreateInfo &rasterizationInfo,
    const VkPipelineMultisampleStateCreateInfo &multisampleInfo,
    const VkPipelineDepthStencilStateCreateInfo &depthStencilInfo,
    const VkPipelineColorBlendStateCreateInfo &colorBlendInfo, const VkPipelineDynamicStateCreateInfo &dynamicInfo,
    const VkPipelineLayout &pipelineLayout, const VkRenderPass &renderPass, VkPipelineCreateFlags flags = 0);
} // namespace vkStructs
} // namespace vkTools

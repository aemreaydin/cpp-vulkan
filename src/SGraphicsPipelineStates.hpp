#pragma once

#include <array>
#include <vulkan/vulkan.h>

struct SGraphicsPipelineStates
{
  public:
    VkPipelineVertexInputStateCreateInfo vertexInputState;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
    VkPipelineRasterizationStateCreateInfo rasterizationState;
    VkPipelineColorBlendStateCreateInfo colorBlendState;
    VkPipelineMultisampleStateCreateInfo multisampleState;
    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    VkPipelineDynamicStateCreateInfo dynamicState;
    VkPipelineViewportStateCreateInfo viewportState;

    explicit SGraphicsPipelineStates(const VkPipelineColorBlendAttachmentState &attachmentState,
                                     const VkViewport &viewport, const VkRect2D &scissors,
                                     const VkVertexInputBindingDescription &inputBindingDesc,
                                     const std::array<VkVertexInputAttributeDescription, 2> &attributeDesc);

  private:
    static VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo(
        const VkVertexInputBindingDescription &inputBindingDesc,
        const std::array<VkVertexInputAttributeDescription, 2> &attributeDesc);

    static VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo();

    static VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo();

    static VkPipelineColorBlendStateCreateInfo ColorBlendStateCreateInfo(
        const VkPipelineColorBlendAttachmentState &attachmentState);

    static VkPipelineMultisampleStateCreateInfo MultisampleStateCreateInfo();

    static VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo();

    static VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo();

    static VkPipelineViewportStateCreateInfo ViewportCreateInfo(const VkViewport &viewport, const VkRect2D &scissors);
};
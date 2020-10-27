#include "graphicsPipelineStates.hpp"
#include "Primitives.hpp"

SGraphicsPipelineStates::SGraphicsPipelineStates(const VkPipelineColorBlendAttachmentState &attachmentState,
                                                 const VkViewport &viewport, const VkRect2D &scissors,
                                                 const VkVertexInputBindingDescription &inputBindingDesc,
                                                 const std::array<VkVertexInputAttributeDescription, 2> &attributeDesc)
{
    vertexInputState = VertexInputStateCreateInfo(inputBindingDesc, attributeDesc);
    inputAssemblyState = InputAssemblyStateCreateInfo();
    rasterizationState = RasterizationStateCreateInfo();
    colorBlendState = ColorBlendStateCreateInfo(attachmentState);
    multisampleState = MultisampleStateCreateInfo();
    depthStencilState = DepthStencilStateCreateInfo();
    dynamicState = DynamicStateCreateInfo();
    viewportState = ViewportCreateInfo(viewport, scissors);
}
VkPipelineVertexInputStateCreateInfo SGraphicsPipelineStates::VertexInputStateCreateInfo(
    const VkVertexInputBindingDescription &inputBindingDesc,
    const std::array<VkVertexInputAttributeDescription, 2> &attributeDesc)
{
    VkPipelineVertexInputStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.vertexBindingDescriptionCount = 1;
    createInfo.pVertexBindingDescriptions = &inputBindingDesc;
    createInfo.vertexAttributeDescriptionCount = attributeDesc.size();
    createInfo.pVertexAttributeDescriptions = attributeDesc.data();

    return createInfo;
}
VkPipelineInputAssemblyStateCreateInfo SGraphicsPipelineStates::InputAssemblyStateCreateInfo()
{
    VkPipelineInputAssemblyStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    createInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    createInfo.primitiveRestartEnable = VK_FALSE;

    return createInfo;
}
VkPipelineRasterizationStateCreateInfo SGraphicsPipelineStates::RasterizationStateCreateInfo()
{
    VkPipelineRasterizationStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    createInfo.depthClampEnable = VK_FALSE;
    createInfo.rasterizerDiscardEnable = VK_FALSE;
    createInfo.polygonMode = VK_POLYGON_MODE_FILL;
    createInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    createInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    createInfo.depthBiasEnable = VK_FALSE;
    createInfo.depthBiasConstantFactor = 0.0f;
    createInfo.depthBiasClamp = 0.0f;
    createInfo.depthBiasSlopeFactor = 0.0f;
    createInfo.lineWidth = 1.0f;

    return createInfo;
}
VkPipelineColorBlendStateCreateInfo SGraphicsPipelineStates::ColorBlendStateCreateInfo(
    const VkPipelineColorBlendAttachmentState &attachmentState)
{
    VkPipelineColorBlendStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &attachmentState;

    return createInfo;
}
VkPipelineMultisampleStateCreateInfo SGraphicsPipelineStates::MultisampleStateCreateInfo()
{
    VkPipelineMultisampleStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.sampleShadingEnable = VK_FALSE;

    return createInfo;
}
VkPipelineDepthStencilStateCreateInfo SGraphicsPipelineStates::DepthStencilStateCreateInfo()
{
    VkPipelineDepthStencilStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    createInfo.depthTestEnable = VK_FALSE;

    return createInfo;
}
VkPipelineDynamicStateCreateInfo SGraphicsPipelineStates::DynamicStateCreateInfo()
{
    VkPipelineDynamicStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    return createInfo;
}
VkPipelineViewportStateCreateInfo SGraphicsPipelineStates::ViewportCreateInfo(const VkViewport &viewport,
                                                                              const VkRect2D &scissors)
{
    VkPipelineViewportStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    createInfo.viewportCount = 1;
    createInfo.pViewports = &viewport;
    createInfo.scissorCount = 1;
    createInfo.pScissors = &scissors;

    return createInfo;
}

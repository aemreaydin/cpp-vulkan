#include "vkStructs.hpp"

namespace vkTools
{
namespace vkStructs
{

VkApplicationInfo ApplicationInfo(const char *appName, uint32_t appVersion, const char *engineName,
                                  uint32_t engineVersion)
{
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = appName;
    applicationInfo.applicationVersion = appVersion;
    applicationInfo.pEngineName = engineName;
    applicationInfo.engineVersion = engineVersion;
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    return applicationInfo;
}
VkInstanceCreateInfo InstanceCreateInfo(VkApplicationInfo applicationInfo,
                                        const std::vector<const char *> &vecExtensions,
                                        const std::vector<const char *> &vecLayers,
                                        VkDebugUtilsMessengerCreateInfoEXT debugInfo)
{
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = vecExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = vecExtensions.data();
#ifndef NDEBUG
    instanceCreateInfo.enabledLayerCount = vecLayers.size();
    instanceCreateInfo.ppEnabledLayerNames = vecLayers.data();
    instanceCreateInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT *>(&debugInfo);
#endif
    return instanceCreateInfo;
}
VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout> &vecLayouts,
                                                    const std::vector<VkPushConstantRange> &vecPushConstantRanges)
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = vecLayouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = vecLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = vecPushConstantRanges.size();
    pipelineLayoutCreateInfo.pPushConstantRanges = vecPushConstantRanges.data();

    return pipelineLayoutCreateInfo;
}
VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo(
    const VkVertexInputBindingDescription &inputDesc,
    const std::array<VkVertexInputAttributeDescription, 3> &attributeDesc)
{
    VkPipelineVertexInputStateCreateInfo inputStateCreateInfo{};
    inputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputStateCreateInfo.vertexBindingDescriptionCount = 1;
    inputStateCreateInfo.pVertexBindingDescriptions = &inputDesc;
    inputStateCreateInfo.vertexAttributeDescriptionCount = attributeDesc.size();
    inputStateCreateInfo.pVertexAttributeDescriptions = attributeDesc.data();

    return inputStateCreateInfo;
}
VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    return inputAssemblyStateCreateInfo;
}
VkPipelineTessellationStateCreateInfo TessellationStateCreateInfo()
{
    return VkPipelineTessellationStateCreateInfo();
}
VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo(VkPolygonMode polygonMode)
{
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = polygonMode;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    rasterizationStateCreateInfo.lineWidth = 1.0f;

    return rasterizationStateCreateInfo;
}
VkPipelineColorBlendStateCreateInfo ColorBlendStateCreateInfo(
    const VkPipelineColorBlendAttachmentState &attachmentState)
{
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &attachmentState;

    return colorBlendStateCreateInfo;
}
VkPipelineMultisampleStateCreateInfo MultisampleStateCreateInfo()
{
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;

    return multisampleStateCreateInfo;
}
VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo()
{
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

    return depthStencilStateCreateInfo;
}
VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo(const std::vector<VkDynamicState> &vecDynamicStates)
{
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = vecDynamicStates.size();
    dynamicStateCreateInfo.pDynamicStates = vecDynamicStates.data();
    return dynamicStateCreateInfo;
}
VkPipelineViewportStateCreateInfo ViewportCreateInfo(const std::vector<VkViewport> &vecViewports,
                                                     const std::vector<VkRect2D> &vecScissors)
{
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = vecViewports.size();
    viewportStateCreateInfo.pViewports = vecViewports.data();
    viewportStateCreateInfo.scissorCount = vecScissors.size();
    viewportStateCreateInfo.pScissors = vecScissors.data();

    return viewportStateCreateInfo;
}
VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo(
    const std::vector<VkPipelineShaderStageCreateInfo> &vecShaderStagesInfos,
    const VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
    const VkPipelineInputAssemblyStateCreateInfo &inputAssemblyInfo,
    const VkPipelineTessellationStateCreateInfo &tesselationInfo, const VkPipelineViewportStateCreateInfo &viewportInfo,
    const VkPipelineRasterizationStateCreateInfo &rasterizationInfo,
    const VkPipelineMultisampleStateCreateInfo &multisampleInfo,
    const VkPipelineDepthStencilStateCreateInfo &depthStencilInfo,
    const VkPipelineColorBlendStateCreateInfo &colorBlendInfo, const VkPipelineDynamicStateCreateInfo &dynamicInfo,
    const VkPipelineLayout &pipelineLayout, VkRenderPass const &renderPass, VkPipelineCreateFlags flags)
{
    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = vecShaderStagesInfos.size();
    graphicsPipelineCreateInfo.pStages = vecShaderStagesInfos.data();
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
    graphicsPipelineCreateInfo.pTessellationState = &tesselationInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicInfo;
    graphicsPipelineCreateInfo.layout = pipelineLayout;
    graphicsPipelineCreateInfo.renderPass = renderPass;
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

    return graphicsPipelineCreateInfo;
}

} // namespace vkStructs
} // namespace vkTools

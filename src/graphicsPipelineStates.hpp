#include <vulkan/vulkan.h>

class SGraphicsPipelineStates
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
                                     const VkViewport &viewport, const VkRect2D &scissors)
    {
        vertexInputState = VertexInputStateCreateInfo();
        inputAssemblyState = InputAssemblyStateCreateInfo();
        rasterizationState = RasterizationStateCreateInfo();
        colorBlendState = ColorBlendStateCreateInfo(attachmentState);
        multisampleState = MultisampleStateCreateInfo();
        depthStencilState = DepthStencilStateCreateInfo();
        dynamicState = DynamicStateCreateInfo();
        viewportState = ViewportCreateInfo(viewport, scissors);
    }

  private:
    static VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo()
    {
        VkPipelineVertexInputStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        createInfo.vertexAttributeDescriptionCount = 0;
        createInfo.pVertexAttributeDescriptions = nullptr;
        createInfo.vertexBindingDescriptionCount = 0;
        createInfo.pVertexBindingDescriptions = nullptr;

        return createInfo;
    }

    static VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCreateInfo()
    {
        VkPipelineInputAssemblyStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        createInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        createInfo.primitiveRestartEnable = VK_FALSE;

        return createInfo;
    }

    static VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo()
    {
        VkPipelineRasterizationStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        createInfo.depthClampEnable = VK_FALSE;
        createInfo.rasterizerDiscardEnable = VK_FALSE;
        createInfo.polygonMode = VK_POLYGON_MODE_FILL;
        createInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        createInfo.depthBiasEnable = VK_FALSE;
        createInfo.depthBiasConstantFactor = 0.0f;
        createInfo.depthBiasClamp = 0.0f;
        createInfo.depthBiasSlopeFactor = 0.0f;
        createInfo.lineWidth = 1.0f;

        return createInfo;
    }

    static VkPipelineColorBlendStateCreateInfo ColorBlendStateCreateInfo(
        const VkPipelineColorBlendAttachmentState &attachmentState)
    {
        VkPipelineColorBlendStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        createInfo.logicOpEnable = VK_FALSE;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &attachmentState;

        return createInfo;
    }

    static VkPipelineMultisampleStateCreateInfo MultisampleStateCreateInfo()
    {
        VkPipelineMultisampleStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        createInfo.sampleShadingEnable = VK_FALSE;

        return createInfo;
    }

    static VkPipelineDepthStencilStateCreateInfo DepthStencilStateCreateInfo()
    {
        VkPipelineDepthStencilStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        createInfo.depthTestEnable = VK_FALSE;

        return createInfo;
    }

    static VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo()
    {
        VkPipelineDynamicStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        return createInfo;
    }

    static VkPipelineViewportStateCreateInfo ViewportCreateInfo(const VkViewport &viewport, const VkRect2D &scissors)
    {
        VkPipelineViewportStateCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        createInfo.viewportCount = 1;
        createInfo.pViewports = &viewport;
        createInfo.scissorCount = 1;
        createInfo.pScissors = &scissors;

        return createInfo;
    }
};
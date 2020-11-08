#include "CLightObject.hpp"
#include "CImageLoader.hpp"
#include "CModelLoader.hpp"
#include "CShaderUtils.hpp"
#include "vkStructs.hpp"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

using namespace vkTools;
using namespace vkTools;

CLightObject::CLightObject(vkPrimitives::STransform transform) : m_transform(transform)
{
    mp_deviceInstance = &CDevice::GetInstance();
    m_mesh = CModelLoader::LoadObjModel("../assets/models/cube.obj");

    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffers();
    CreateDescriptorSets();
    CreateGraphicsPipeline();
}

void CLightObject::CleanupGraphicsPipeline()
{
    vkDestroyPipelineLayout(mp_deviceInstance->GetDevice(), m_graphicsPipelineLayout, nullptr);
    vkDestroyPipeline(mp_deviceInstance->GetDevice(), m_graphicsPipeline, nullptr);
}
void CLightObject::RecreateGraphicsPipeline()
{
    CreateGraphicsPipeline();
}

void CLightObject::UpdateUniformBuffers()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    const auto currentTime = std::chrono::high_resolution_clock::now();
    const auto time = std::chrono::duration<float, std::chrono::seconds ::period>(currentTime - startTime).count();

    m_mvp.model = glm::translate(glm::mat4(1.0f), m_transform.translate);
    m_mvp.model = glm::scale(m_mvp.model, m_transform.scale);
    m_mvp.model = glm::rotate(m_mvp.model, time * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_mvp.view = glm::lookAt(glm::vec3(10.0f, 0.01f, 10.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_mvp.projection = glm::perspective(
        glm::radians(45.0f),
        mp_deviceInstance->GetExtent().width / static_cast<float>(mp_deviceInstance->GetExtent().height), 0.1f, 100.0f);
    //    m_mvp.projection = glm::ortho(-2.0f, 2.0f, 2.0f, -2.0f, 0.1f, 10.0f);
    m_mvp.projection[1][1] *= -1;

    void *data;
    vkMapMemory(mp_deviceInstance->GetDevice(),
                m_vecUniformBufferHandles[mp_deviceInstance->GetCurrentImageIndex()].memory, 0, sizeof(m_mvp), 0,
                &data);
    memcpy(data, &m_mvp, sizeof(m_mvp));
    vkUnmapMemory(mp_deviceInstance->GetDevice(),
                  m_vecUniformBufferHandles[mp_deviceInstance->GetCurrentImageIndex()].memory);
}

void CLightObject::Draw() const
{
    VkCommandBuffer cmdBuffer = mp_deviceInstance->GetCurrentCommandBuffer();

    VkDeviceSize offsets = {0};

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &m_vertexBufferHandles.buffer, &offsets);

    vkCmdBindIndexBuffer(cmdBuffer, m_indexBufferHandles.buffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mp_deviceInstance->GetPipelineLayout(), 0, 1,
                            &m_vecDescriptorSets[mp_deviceInstance->GetCurrentImageIndex()], 0, nullptr);

    vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_mesh.indices.size()), 1, 0, 0, 0);
}

void CLightObject::CreateVertexBuffer()
{
    SBufferHandles stagingHandles{};
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = GetVerticesSize();
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    mp_deviceInstance->GetBufferImageManager().CreateBuffer(
        createInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, stagingHandles);

    mp_deviceInstance->GetBufferImageManager().MapMemory(stagingHandles, 0, GetVerticesSize(), m_mesh.vertices.data());

    // Create the vertex buffer and copy the staged buffer to it
    createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    mp_deviceInstance->GetBufferImageManager().CreateBuffer(createInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                            m_vertexBufferHandles);

    VkBufferCopy regions{};
    regions.size = GetVerticesSize();
    mp_deviceInstance->GetBufferImageManager().CopyBuffer(stagingHandles.buffer, regions, m_vertexBufferHandles.buffer);

    // Destroy the staging buffer
    mp_deviceInstance->GetBufferImageManager().DestroyBufferHandles(stagingHandles);
}

void CLightObject::CreateIndexBuffer()
{
    SBufferHandles stagingHandles{};
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = GetVerticesSize();
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    mp_deviceInstance->GetBufferImageManager().CreateBuffer(
        createInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, stagingHandles);

    mp_deviceInstance->GetBufferImageManager().MapMemory(stagingHandles, 0, GetIndicesSize(), m_mesh.indices.data());

    // Create the vertex buffer and copy the staged buffer to it
    createInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    mp_deviceInstance->GetBufferImageManager().CreateBuffer(createInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                            m_indexBufferHandles);

    VkBufferCopy regions{};
    regions.size = GetIndicesSize();
    mp_deviceInstance->GetBufferImageManager().CopyBuffer(stagingHandles.buffer, regions, m_indexBufferHandles.buffer);

    // Destroy the staging buffer
    mp_deviceInstance->GetBufferImageManager().DestroyBufferHandles(stagingHandles);
}

void CLightObject::CreateUniformBuffers()
{
    m_vecUniformBufferHandles.resize(mp_deviceInstance->GetSwapchainImageCount());

    for (auto &handle : m_vecUniformBufferHandles)
    {
        VkBufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.size = GetUniformSize();
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        mp_deviceInstance->GetBufferImageManager().CreateBuffer(
            createInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, handle);
    }
}

void CLightObject::CreateDescriptorSets()
{
    m_vecDescriptorSets.resize(mp_deviceInstance->GetSwapchainImageCount());

    std::vector<VkDescriptorSetLayout> layouts(mp_deviceInstance->GetSwapchainImageCount(),
                                               mp_deviceInstance->GetDescriptorSetLayout());
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = mp_deviceInstance->GetDescriptorPool();
    allocateInfo.descriptorSetCount = layouts.size();
    allocateInfo.pSetLayouts = layouts.data();
    if (const auto res =
            vkAllocateDescriptorSets(mp_deviceInstance->GetDevice(), &allocateInfo, m_vecDescriptorSets.data());
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor set.");

    for (auto i = 0; i != m_vecDescriptorSets.size(); ++i)
    {
        VkDescriptorBufferInfo mvpInfo{};
        mvpInfo.buffer = m_vecUniformBufferHandles[i].buffer;
        mvpInfo.range = GetUniformSize();
        mvpInfo.offset = 0;

        std::array<VkWriteDescriptorSet, 1> writeDescriptorSets{};
        writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSets[0].descriptorCount = 1;
        writeDescriptorSets[0].dstSet = m_vecDescriptorSets[i];
        writeDescriptorSets[0].dstBinding = 0;
        writeDescriptorSets[0].dstArrayElement = 0;
        writeDescriptorSets[0].pBufferInfo = &mvpInfo;

        vkUpdateDescriptorSets(mp_deviceInstance->GetDevice(), writeDescriptorSets.size(), writeDescriptorSets.data(),
                               0, nullptr);
    }
}

void CLightObject::CreateGraphicsPipeline()
{
    const auto vertModule = CShaderUtils::CreateShaderModule(mp_deviceInstance->GetDevice(),
                                                             "../assets/shaders/light.vert", EShaderType::Vert);
    const auto vertStageInfo = CShaderUtils::ShaderPipelineStageCreateInfo(vertModule, EShaderType::Vert);

    const auto fragModule = CShaderUtils::CreateShaderModule(mp_deviceInstance->GetDevice(),
                                                             "../assets/shaders/light.frag", EShaderType::Frag);
    const auto fragStageInfo = CShaderUtils::ShaderPipelineStageCreateInfo(fragModule, EShaderType::Frag);

    const std::vector<VkPipelineShaderStageCreateInfo> vecShaderStages{vertStageInfo, fragStageInfo};

    const auto vertexInputInfo = vkStructs::VertexInputStateCreateInfo(
        vkPrimitives::SVertex::GetInputBindingDescription(), vkPrimitives::SVertex::GetAttributeBindingDescription());
    const auto inputAssemblyInfo = vkStructs::InputAssemblyStateCreateInfo();
    const auto tessellationInfo = vkStructs::TessellationStateCreateInfo();
    const auto rasterizationInfo = vkStructs::RasterizationStateCreateInfo(VK_POLYGON_MODE_LINE);

    VkPipelineColorBlendAttachmentState attachmentState{};
    attachmentState.blendEnable = VK_FALSE;
    attachmentState.colorWriteMask =
        VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_R_BIT;
    const auto colorBlendInfo = vkStructs::ColorBlendStateCreateInfo(attachmentState);
    const auto multisampleInfo = vkStructs::MultisampleStateCreateInfo();
    const auto depthStencilInfo = vkStructs::DepthStencilStateCreateInfo();
    const auto dynamicInfo = vkStructs::DynamicStateCreateInfo();

    std::vector<VkViewport> vecViewports{{0.0f, 0.0f, static_cast<float>(mp_deviceInstance->GetExtent().width),
                                          static_cast<float>(mp_deviceInstance->GetExtent().height), 0.0f, 1.0f}};
    std::vector<VkRect2D> vecScissors{{{0, 0}, mp_deviceInstance->GetExtent()}};
    const auto viewportinfo = vkStructs::ViewportCreateInfo(vecViewports, vecScissors);

    std::vector<VkDescriptorSetLayout> vecLayouts{mp_deviceInstance->GetDescriptorSetLayout()};
    const auto pipelineInfo = vkStructs::PipelineLayoutCreateInfo(vecLayouts);
    VK_CHECK_RESULT(
        vkCreatePipelineLayout(mp_deviceInstance->GetDevice(), &pipelineInfo, nullptr, &m_graphicsPipelineLayout))

    const auto graphicsPipelineInfo = vkStructs::GraphicsPipelineCreateInfo(
        vecShaderStages, vertexInputInfo, inputAssemblyInfo, tessellationInfo, viewportinfo, rasterizationInfo,
        multisampleInfo, depthStencilInfo, colorBlendInfo, dynamicInfo, m_graphicsPipelineLayout,
        mp_deviceInstance->GetRenderPass());

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(mp_deviceInstance->GetDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineInfo,
                                              nullptr, &m_graphicsPipeline))

    vkDestroyShaderModule(mp_deviceInstance->GetDevice(), vertModule, nullptr);
    vkDestroyShaderModule(mp_deviceInstance->GetDevice(), fragModule, nullptr);
}

void CLightObject::ObjectCleanup()
{
    mp_deviceInstance->GetBufferImageManager().DestroyBufferHandles(m_vertexBufferHandles);
    mp_deviceInstance->GetBufferImageManager().DestroyBufferHandles(m_indexBufferHandles);
    for (auto &handle : m_vecUniformBufferHandles)
    {
        mp_deviceInstance->GetBufferImageManager().DestroyBufferHandles(handle);
    }

    CleanupGraphicsPipeline();
}

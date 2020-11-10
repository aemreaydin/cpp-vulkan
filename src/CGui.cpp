#include "CDevice.hpp"
#include "vkStructs.hpp"

using namespace vkTools;

#include "CGui.hpp"
CGui::CGui()
{
    CreateImGuiDescriptorPool();
    InitImGui();
}

void CGui::InitImGui()
{
    const auto &deviceInstance = CDevice::GetInstance();

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(deviceInstance.GetWindow(), true);

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = deviceInstance.GetVulkanInstance()->GetInstance();
    initInfo.PhysicalDevice = deviceInstance.GetVulkanInstance()->GetPhysicalDevice();
    initInfo.Device = deviceInstance.GetDevice();
    initInfo.Queue = deviceInstance.GetGraphicsQueue();
    initInfo.DescriptorPool = m_guiPool;
    initInfo.MinImageCount = deviceInstance.GetSwapchainImageCount();
    initInfo.ImageCount = deviceInstance.GetSwapchainImageCount();

    ImGui_ImplVulkan_Init(&initInfo, deviceInstance.GetRenderPass());

    VkCommandBuffer cmdBuffer;
    const auto cmdAllocateInfo = vkStructs::CommandBufferAllocateInfo(deviceInstance.GetCommandPool());
    VK_CHECK_RESULT(vkAllocateCommandBuffers(deviceInstance.GetDevice(), &cmdAllocateInfo, &cmdBuffer))

    const auto cmdBeginInfo = vkStructs::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VK_CHECK_RESULT(vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo))

    ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer);

    VK_CHECK_RESULT(vkEndCommandBuffer(cmdBuffer))

    const auto submitInfo = vkStructs::SubmitInfo(1, cmdBuffer);
    VK_CHECK_RESULT(vkQueueSubmit(deviceInstance.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE))

    vkQueueWaitIdle(deviceInstance.GetGraphicsQueue());

    vkFreeCommandBuffers(deviceInstance.GetDevice(), deviceInstance.GetCommandPool(), 1, &cmdBuffer);
}

void CGui::CreateImGuiDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes{{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                                {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                                {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                                {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                                {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
    const auto createInfo = vkStructs::DescriptorPoolCreateInfo(1000, poolSizes);

    VK_CHECK_RESULT(vkCreateDescriptorPool(CDevice::GetInstance().GetDevice(), &createInfo, nullptr, &m_guiPool))
}

void CGui::Draw()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), CDevice::GetInstance().GetCurrentCommandBuffer());
}

void CGui::Cleanup()
{
    vkDestroyDescriptorPool(CDevice::GetInstance().GetDevice(), m_guiPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
}

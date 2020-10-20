#include "app.hpp"
#include "fileOps.hpp"
#include "graphicsPipelineStates.hpp"
#include "validationLayer.hpp"
#include "vulkanHelpers.hpp"
#include "window.hpp"

#include <array>

CApp::CApp(SAppInfo appInfo) : m_appInfo(appInfo)
{
    mp_window = std::make_unique<CWindow>(appInfo.width, appInfo.height);
    CreateInstance();
    mp_validationLayer = std::make_unique<CValidationLayer>(m_instance, m_appInfo.layers);
    CreateSurface();

    CreatePhysicalDevice();
    CreateDevice();
    CreateQueues();
    CreateSwapchain();
    CreateSwapchainImages();
    CreateImageViews();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandPool();
    CreateCommandBuffers();
}

void CApp::CreateInstance()
{
    if (!CVulkanHelpers::CheckForVulkanLayers(m_appInfo.layers))
    {
        throw std::runtime_error("Validation layers not available.");
    }

    auto appInfo = VkApplicationInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan 00";
    appInfo.applicationVersion = VK_API_VERSION_1_0;
    appInfo.pEngineName = "Vulkan 00";
    appInfo.engineVersion = 1;

    const auto extensions = CVulkanHelpers::GetVulkanInstanceExtensions();
    auto instanceCreateInfo = VkInstanceCreateInfo{};
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    // Structure to specify the parameters for a new instance
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    // TODO add debug check
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_appInfo.layers.size());
    instanceCreateInfo.ppEnabledLayerNames = m_appInfo.layers.data();

    CValidationLayer::PopulateDebugMessengerCreateInfo(debugCreateInfo);
    instanceCreateInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT *>(&debugCreateInfo);

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create instance.");

    std::cout << "VkInstance created.\n";
}
void CApp::CreateSurface()
{
    if (!mp_window)
    {
        throw std::runtime_error("Window doesn't exist, create it first.");
    }

    const auto res = glfwCreateWindowSurface(m_instance, mp_window->Window(), nullptr, &m_surface);
    if (res != VK_SUCCESS)
        throw std::runtime_error("Failed to create surface.");
}

std::vector<VkPhysicalDevice> CApp::FindPhysicalDevices()
{
    uint32_t deviceCount;
    if (vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr) != VK_SUCCESS)
        throw std::runtime_error("Failed to get any physical device.");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    return devices;
}

std::vector<VkQueueFamilyProperties> CApp::FindQueueFamilise(VkPhysicalDevice const &device)
{
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    return queueFamilies;
}

bool CApp::IsDeviceSuitable(const VkPhysicalDevice &device)
{
    const auto extensionsValid = CVulkanHelpers::CheckForVulkanExtensions(device, m_appInfo.deviceExtensions);
    GetSwapchainSupportDetails(device);

    return m_queueFamilies.HaveValues() && extensionsValid && !m_swapchainSupportDetails.surfaceFormats.empty() &&
           !m_swapchainSupportDetails.surfacePresentModes.empty();
}

void CApp::CreatePhysicalDevice()
{
    const auto devices = FindPhysicalDevices();
    for (const auto &device : devices)
    {
        const auto queueFamilies = FindQueueFamilise(device);

        uint32_t index = 0;
        for (const auto &family : queueFamilies)
        {
            if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                m_queueFamilies.graphicsFamilyIndex = index;
            }

            VkBool32 isSupported;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, m_surface, &isSupported);
            if (isSupported)
            {
                m_queueFamilies.presentFamilyIndex = index;
            }

            // If device has the necessary conditions use it
            if (m_queueFamilies.HaveValues())
            {
                break;
            }

            ++index;
        }

        if (IsDeviceSuitable(device))
            m_physicalDevice = device;
    }
}

void CApp::CreateDevice()
{
    float queuePriorities = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (const auto queueIndex : m_queueFamilies.GetUniqueQueueFamilies())
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriorities;
        queueCreateInfo.queueFamilyIndex = queueIndex;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    VkPhysicalDeviceFeatures features{};
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.enabledExtensionCount = m_appInfo.deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = m_appInfo.deviceExtensions.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &features;

    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create device.");
}

void CApp::CreateQueues()
{
    // Graphics Queue
    vkGetDeviceQueue(m_device, m_queueFamilies.graphicsFamilyIndex.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, m_queueFamilies.presentFamilyIndex.value(), 0, &m_presentQueue);
}

void CApp::GetSwapchainSupportDetails(const VkPhysicalDevice &device)
{
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &m_swapchainSupportDetails.surfaceCapabilities) !=
        VK_SUCCESS)
        throw std::runtime_error("Failed to get surface capabilities");

    uint32_t formatCount;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr) != VK_SUCCESS)
        throw std::runtime_error("Failed to get surface formats.");
    m_swapchainSupportDetails.surfaceFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount,
                                         m_swapchainSupportDetails.surfaceFormats.data());

    uint32_t presentModeCount;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr) != VK_SUCCESS)
        throw std::runtime_error("Failed to get surface present modes.");

    m_swapchainSupportDetails.surfacePresentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount,
                                              m_swapchainSupportDetails.surfacePresentModes.data());
}

VkSurfaceFormatKHR CApp::GetOptimalSurfaceFormat()
{
    for (const auto &format : m_swapchainSupportDetails.surfaceFormats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }
    return m_swapchainSupportDetails.surfaceFormats[0];
}

VkPresentModeKHR CApp::GetOptimalPresentMode()
{
    for (const auto &presentMode : m_swapchainSupportDetails.surfacePresentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return presentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D CApp::GetOptimalExtent2D()
{
    if (m_swapchainSupportDetails.surfaceCapabilities.currentExtent.width != UINT32_MAX)
    {
        return m_swapchainSupportDetails.surfaceCapabilities.currentExtent;
    }

    const auto minExtent = m_swapchainSupportDetails.surfaceCapabilities.minImageExtent;
    const auto maxExtent = m_swapchainSupportDetails.surfaceCapabilities.maxImageExtent;

    VkExtent2D extent{m_appInfo.width, m_appInfo.height};
    extent.width = std::clamp(extent.width, minExtent.width, maxExtent.width);
    extent.height = std::clamp(extent.height, minExtent.height, maxExtent.height);

    return extent;
}

void CApp::CreateSwapchain()
{
    const auto extent = GetOptimalExtent2D();
    const auto format = GetOptimalSurfaceFormat();
    const auto presentMode = GetOptimalPresentMode();
    uint32_t queueFamilyIndices[]{m_queueFamilies.graphicsFamilyIndex.value(),
                                  m_queueFamilies.presentFamilyIndex.value()};

    // Triple-buffering if possible
    const auto imageCount = std::clamp(m_swapchainSupportDetails.surfaceCapabilities.minImageCount,
                                       m_swapchainSupportDetails.surfaceCapabilities.minImageCount + 1,
                                       m_swapchainSupportDetails.surfaceCapabilities.maxImageCount);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.surface = m_surface;
    createInfo.presentMode = presentMode;
    createInfo.imageFormat = format.format;
    createInfo.imageColorSpace = format.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    // We're rendering directly into the swapchain, if we wanted post-processing this would be something else
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.minImageCount = imageCount;
    createInfo.preTransform = m_swapchainSupportDetails.surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    if (m_queueFamilies.graphicsFamilyIndex.value() == m_queueFamilies.presentFamilyIndex.value())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swapchain.");

    m_extent = extent;
    m_format = format.format;
}

void CApp::CreateSwapchainImages()
{
    uint32_t imageCount;
    if (vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr) != VK_SUCCESS)
        throw std::runtime_error("Failed to get swapchain images.");

    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_images.data());
}

void CApp::CreateImageViews()
{
    m_imageViews.resize(m_images.size());

    auto index = 0;
    for (const auto &image : m_images)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device, &createInfo, nullptr, &m_imageViews[index++]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create image view.");
    }
}

VkShaderModule CApp::CreateShaderModule(const std::string &shaderFile)
{
    const auto shader = CFileOps::ReadShader(shaderFile);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shader.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(shader.data());

    VkShaderModule shaderModule;
    vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule);

    return shaderModule;
}
VkPipelineShaderStageCreateInfo CApp::CreateShaderPipelineStage(const VkShaderModule &module,
                                                                const EShaderType shaderType)
{
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.module = module;
    createInfo.pName = "main";
    // pSpecializationInfo creates a specialization constant
    switch (shaderType)
    {
    case EShaderType::Frag:
        createInfo.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        break;
    case EShaderType::Vert:
        createInfo.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
        break;
    default:
        throw std::runtime_error("Shader Type doesn't exist.");
    }

    return createInfo;
}

void CApp::CreatePipelineLayout()
{
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = 0;
    createInfo.pSetLayouts = nullptr;
    createInfo.pushConstantRangeCount = 0;
    createInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(m_device, &createInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout.");
}

void CApp::CreateRenderPass()
{
    VkAttachmentDescription description{};
    description.format = m_format;
    description.samples = VK_SAMPLE_COUNT_1_BIT;
    description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference reference{};
    reference.attachment = 0;
    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &reference;

    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &description;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpassDescription;

    if (vkCreateRenderPass(m_device, &createInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        throw std::runtime_error("Failed to create render pass.");
}

void CApp::CreateGraphicsPipeline()
{
    // Create the shader modules
    const auto vertModule = CreateShaderModule("../shaders/simpleVertex.spv");
    const auto vertStageInfo = CreateShaderPipelineStage(vertModule, EShaderType::Vert);

    const auto fragModule = CreateShaderModule("../shaders/simpleFrag.spv");
    const auto fragStageInfo = CreateShaderPipelineStage(fragModule, EShaderType::Frag);

    const std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{vertStageInfo, fragStageInfo};

    // Create the pipeline layout
    CreatePipelineLayout();

    // Create the pipeline states
    VkPipelineColorBlendAttachmentState attachmentState{};
    attachmentState.blendEnable = VK_FALSE;
    attachmentState.colorWriteMask =
        VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_R_BIT;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_extent.width);
    viewport.height = static_cast<float>(m_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissors = {};
    scissors.extent = m_extent;
    scissors.offset = {0, 0};
    const auto graphicsPipelineStates = SGraphicsPipelineStates(attachmentState, viewport, scissors);
    // Create the renderpass
    CreateRenderPass();

    // Create the graphics pipeline
    VkGraphicsPipelineCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = shaderStages.size();
    createInfo.pStages = shaderStages.data();
    createInfo.pVertexInputState = &graphicsPipelineStates.vertexInputState;
    createInfo.pInputAssemblyState = &graphicsPipelineStates.inputAssemblyState;
    //    createInfo.pTessellationState
    createInfo.pViewportState = &graphicsPipelineStates.viewportState;
    createInfo.pRasterizationState = &graphicsPipelineStates.rasterizationState;
    createInfo.pMultisampleState = &graphicsPipelineStates.multisampleState;
    //    createInfo.pDepthStencilState = &graphicsPipelineStates.depthStencilState;
    createInfo.pColorBlendState = &graphicsPipelineStates.colorBlendState;
    //    createInfo.pDynamicState = &graphicsPipelineStates.dynamicState;
    createInfo.layout = m_pipelineLayout;
    createInfo.renderPass = m_renderPass;
    createInfo.subpass = 0;
    createInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
        throw std::runtime_error("Failed to create graphics pipeline.");
    // Destroy the shader modules after they are added to the pipeline
    vkDestroyShaderModule(m_device, vertModule, nullptr);
    vkDestroyShaderModule(m_device, fragModule, nullptr);
}

void CApp::CreateFramebuffers()
{
    m_framebuffers.resize(m_imageViews.size());

    auto index = 0;
    for (auto &framebuffer : m_framebuffers)
    {
        VkImageView attachments[] = {m_imageViews[index]};
        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = m_renderPass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = attachments;
        createInfo.width = m_extent.width;
        createInfo.height = m_extent.height;
        createInfo.layers = 1;

        if (vkCreateFramebuffer(m_device, &createInfo, nullptr, &m_framebuffers[index]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create framebuffer.");

        index++;
    }
}

void CApp::CreateCommandPool()
{
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = m_queueFamilies.graphicsFamilyIndex.value();

    if (vkCreateCommandPool(m_device, &createInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create command pool.");
}

void CApp::CreateCommandBuffers()
{
    // Allocate command buffers
    m_commandBuffers.resize(m_framebuffers.size());

    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = m_commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

    if (vkAllocateCommandBuffers(m_device, &allocateInfo, m_commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffer.");

    for (auto index = 0; index != m_commandBuffers.size(); ++index)
    {
        VkCommandBufferBeginInfo commandBufferBeginInfo{};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // Start the command buffer
        if (vkBeginCommandBuffer(m_commandBuffers[index], &commandBufferBeginInfo) != VK_SUCCESS)
            throw std::runtime_error("Failed to begin command buffer.");

        // Begin the renderpass
        VkClearValue clearValue{};
        clearValue.color = {0.0f, 0.0f, 0.0f, 0.0f};
        VkRenderPassBeginInfo renderPassBeginInfo;
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.framebuffer = m_framebuffers[index];
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = m_extent;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(m_commandBuffers[index], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Bind the graphics pipeline
        vkCmdBindPipeline(m_commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
        // Draw
        vkCmdDraw(m_commandBuffers[index], 3, 1, 0, 0);

        vkCmdEndRenderPass(m_commandBuffers[index]);

        if (vkEndCommandBuffer(m_commandBuffers[index]) != VK_SUCCESS)
            throw std::runtime_error("Failed to end command buffer.");
    }
}

void CApp::Cleanup()
{
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    for (auto &framebuffer : m_framebuffers)
    {
        vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }
    vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    for (auto &imageView : m_imageViews)
    {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    vkDestroyDevice(m_device, nullptr);

    mp_validationLayer->DestroyDebugMessenger(m_instance);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);

    mp_window->Terminate();
}

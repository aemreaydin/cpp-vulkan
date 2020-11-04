#include "app.hpp"
#include "CImageLoader.hpp"
#include "CModelLoader.hpp"
#include "SGraphicsPipelineStates.hpp"

#include <array>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <limits>

CApp::CApp(SAppInfo appInfo) : m_appInfo(appInfo)
{

    // Create GLFW window
    mp_window = std::make_unique<CWindow>(appInfo.width, appInfo.height);
    // Create Vulkan instance, surface, physical device, queue families and validation layers
    mp_instance = std::make_unique<CInstance>(appInfo);
    mp_instance->CreateSurface(mp_window->Window());
    mp_instance->CreatePhysicalDevice();
    // Create Vulkan device and the rest
    CreateDevice();
    CreateQueues();
    CreateCommandPool();
    CreateSwapchain();
    CreateSwapchainImages();
    CreateImageViews();
    // Create the buffer and image manager
    mp_bufferImageManager =
        std::make_unique<CBufferImageManager>(mp_instance->PhysicalDevice(), m_device, m_graphicsQueue, m_commandPool);
    // Create the game objects

    m_vikingRoom = CModel("../assets/models/viking_room.obj");
    m_vikingRoom.InitModel(*mp_bufferImageManager, m_images.size());

    CreateDescriptorPool();
    CreateTexImage();
    CreateTexImageView();
    CreateSampler();
    CreateDepthImage();

    CreateUniformDescriptors();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandBuffers();
    CreateSemaphores();
    CreateFences();
}

void CApp::CreateDevice()
{
    float queuePriorities = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    for (const auto queueIndex : mp_instance->QueueFamilies().GetUniqueQueueFamilies())
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
    features.fillModeNonSolid = VK_TRUE;
    features.samplerAnisotropy = VK_TRUE;
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.enabledExtensionCount = m_appInfo.deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = m_appInfo.deviceExtensions.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &features;

    if (vkCreateDevice(mp_instance->PhysicalDevice(), &createInfo, nullptr, &m_device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create device.");
}

void CApp::CreateQueues()
{
    // Graphics Queue
    vkGetDeviceQueue(m_device, mp_instance->QueueFamilies().graphicsFamilyIndex.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, mp_instance->QueueFamilies().presentFamilyIndex.value(), 0, &m_presentQueue);
}

VkSurfaceFormatKHR CApp::GetOptimalSurfaceFormat()
{
    for (const auto &format : mp_instance->SwapchainSupport().surfaceFormats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }
    return mp_instance->SwapchainSupport().surfaceFormats[0];
}

VkPresentModeKHR CApp::GetOptimalPresentMode()
{
    for (const auto &presentMode : mp_instance->SwapchainSupport().surfacePresentModes)
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
    if (mp_instance->SwapchainSupport().surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return mp_instance->SwapchainSupport().surfaceCapabilities.currentExtent;
    }

    const auto minExtent = mp_instance->SwapchainSupport().surfaceCapabilities.minImageExtent;
    const auto maxExtent = mp_instance->SwapchainSupport().surfaceCapabilities.maxImageExtent;

    auto width = 0;
    auto height = 0;
    glfwGetFramebufferSize(mp_window->Window(), &width, &height);
    VkExtent2D extent{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    extent.width = std::clamp(extent.width, minExtent.width, maxExtent.width);
    extent.height = std::clamp(extent.height, minExtent.height, maxExtent.height);

    return extent;
}

void CApp::CreateSwapchain()
{
    const auto extent = GetOptimalExtent2D();
    const auto format = GetOptimalSurfaceFormat();
    const auto presentMode = GetOptimalPresentMode();
    std::array<uint32_t, 2> queueFamilyIndices{mp_instance->QueueFamilies().graphicsFamilyIndex.value(),
                                               mp_instance->QueueFamilies().presentFamilyIndex.value()};

    // Triple-buffering if possible
    const auto imageCount = std::clamp(mp_instance->SwapchainSupport().surfaceCapabilities.minImageCount,
                                       mp_instance->SwapchainSupport().surfaceCapabilities.minImageCount + 1,
                                       mp_instance->SwapchainSupport().surfaceCapabilities.maxImageCount);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.surface = mp_instance->Surface();
    createInfo.presentMode = presentMode;
    createInfo.imageFormat = format.format;
    createInfo.imageColorSpace = format.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    // We're rendering directly into the swapchain, if we wanted post-processing this would be something else
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.minImageCount = imageCount;
    createInfo.preTransform = mp_instance->SwapchainSupport().surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    if (mp_instance->QueueFamilies().graphicsFamilyIndex.value() ==
        mp_instance->QueueFamilies().presentFamilyIndex.value())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }

    if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swapchain.");

    m_extent = extent;
    m_format = format.format;
}
void CApp::RecreateSwapchain()
{
    vkDeviceWaitIdle(m_device);

    CleanupSwapchain();

    mp_instance->UpdateSwaphchainSupport();
    CreateSwapchain();
    CreateSwapchainImages();
    CreateImageViews();
    CreateTexImageView();
    CreateDepthImage();
    CreateDescriptorPool();
    CreateDescriptorSets(m_descriptorSets.data());
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandBuffers();
}

void CApp::CleanupSwapchain()
{
    for (auto &framebuffer : m_framebuffers)
    {
        vkDestroyFramebuffer(m_device, framebuffer, nullptr);
    }
    vkFreeCommandBuffers(m_device, m_commandPool, m_commandBuffers.size(), m_commandBuffers.data());

    vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    vkDestroyImageView(m_device, m_texImageView, nullptr);
    for (auto &imageView : m_imageViews)
    {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    vkDestroyImageView(m_device, m_depthImageView, nullptr);
    vkFreeMemory(m_device, m_depthImageMemory, nullptr);
    vkDestroyImage(m_device, m_depthImage, nullptr);
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
}

bool CApp::ShouldRecreateSwapchain()
{
    auto width = 0;
    auto height = 0;
    glfwGetFramebufferSize(mp_window->Window(), &width, &height);

    return static_cast<uint32_t>(width) != m_extent.width || static_cast<uint32_t>(height) != m_extent.height;
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

VkShaderModule CApp::CreateShaderModule(const std::string &shaderFile, const EShaderType shaderType)
{
    const auto spirv = CShaderUtils::ConvertGlslToSpirv(shaderFile, shaderType);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirv.size() * sizeof(uint32_t);
    createInfo.pCode = spirv.data();

    VkShaderModule shaderModule;
    if (const auto res = vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule); res != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module.");

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
    createInfo.setLayoutCount = 1;
    createInfo.pSetLayouts = &m_descriptorLayout;
    createInfo.pushConstantRangeCount = 0;
    createInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(m_device, &createInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout.");
}

void CApp::CreateRenderPass()
{
    std::array<VkAttachmentDescription, 2> attachmentDescriptions{};
    attachmentDescriptions[0].format = m_format;
    attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachmentDescriptions[1].format = m_depthFormat;
    attachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference{};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference{};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = attachmentDescriptions.size();
    createInfo.pAttachments = attachmentDescriptions.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpassDescription;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &subpassDependency;

    if (vkCreateRenderPass(m_device, &createInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        throw std::runtime_error("Failed to create render pass.");
}

void CApp::CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size,
                        VkBuffer &buffer, VkDeviceMemory &memory)
{
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usageFlags;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = mp_instance->QueueFamilies().GraphicsFamily();

    if (vkCreateBuffer(m_device, &createInfo, nullptr, &buffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to create buffer.");

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(m_device, buffer, &memReq);

    const auto memoryType = FindMemoryType(memReq.memoryTypeBits, memoryPropertyFlags);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memReq.size;
    allocateInfo.memoryTypeIndex = memoryType;

    if (vkAllocateMemory(m_device, &allocateInfo, nullptr, &memory) != VK_SUCCESS)
        throw std::runtime_error("Failed to create vertex memory.");

    if (vkBindBufferMemory(m_device, buffer, memory, 0) != VK_SUCCESS)
        throw std::runtime_error("Failed to bind vertex buffer to memory.");
}

void CApp::CopyBuffer(VkBuffer &src, VkBuffer &dst, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.commandPool = m_commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkCommandBuffer stagingCmd;
    if (vkAllocateCommandBuffers(m_device, &allocateInfo, &stagingCmd) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffer.");

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(stagingCmd, &beginInfo);

    VkBufferCopy bufferCopy{};
    bufferCopy.size = size;

    vkCmdCopyBuffer(stagingCmd, src, dst, 1, &bufferCopy);

    vkEndCommandBuffer(stagingCmd);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &stagingCmd;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue);

    vkFreeCommandBuffers(m_device, m_commandPool, 1, &stagingCmd);
}

// void CApp::CreateVertexBuffer()
//{
//    VkBuffer stagingBuffer;
//    VkDeviceMemory stagingMemory;
//    CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
//                 m_vikingRoom.GetVerticesSize(), stagingBuffer, stagingMemory);
//
//    void *data;
//    vkMapMemory(m_device, stagingMemory, 0, m_vikingRoom.GetVerticesSize(), 0, &data);
//    memcpy(data, m_vikingRoom.m_vertices.data(), m_vikingRoom.GetVerticesSize());
//    vkUnmapMemory(m_device, stagingMemory);
//
//    CreateBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
//                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vikingRoom.GetVerticesSize(), m_vertexBuffer, m_vertexMemory);
//
//    CopyBuffer(stagingBuffer, m_vertexBuffer, m_vikingRoom.GetVerticesSize());
//
//    vkFreeMemory(m_device, stagingMemory, nullptr);
//    vkDestroyBuffer(m_device, stagingBuffer, nullptr);
//}
//
// void CApp::CreateIndexBuffer()
//{
//    VkBuffer stagingBuffer;
//    VkDeviceMemory stagingMemory;
//    CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
//                 m_vikingRoom.GetIndicesSize(), stagingBuffer, stagingMemory);
//
//    void *data;
//    vkMapMemory(m_device, stagingMemory, 0, m_vikingRoom.GetIndicesSize(), 0, &data);
//    memcpy(data, m_vikingRoom.m_indices.data(), m_vikingRoom.GetIndicesSize());
//    vkUnmapMemory(m_device, stagingMemory);
//
//    CreateBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
//                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vikingRoom.GetIndicesSize(), m_indexBuffer, m_indexMemory);
//
//    CopyBuffer(stagingBuffer, m_indexBuffer, m_vikingRoom.GetIndicesSize());
//
//    vkFreeMemory(m_device, stagingMemory, nullptr);
//    vkDestroyBuffer(m_device, stagingBuffer, nullptr);
//}

void CApp::CreateDepthImage()
{
    m_depthFormat = FindDepthFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT},
                                    VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.format = m_depthFormat;
    createInfo.extent.width = m_extent.width;
    createInfo.extent.height = m_extent.height;
    createInfo.extent.depth = 1.0f;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = mp_instance->QueueFamilies().GraphicsFamily();
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (const auto res = vkCreateImage(m_device, &createInfo, nullptr, &m_depthImage); res != VK_SUCCESS)
        throw std::runtime_error("Failed to create depth image.");

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(m_device, m_depthImage, &memReq);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    allocateInfo.allocationSize = memReq.size;

    if (const auto res = vkAllocateMemory(m_device, &allocateInfo, nullptr, &m_depthImageMemory); res != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate depth memory.");

    if (const auto res = vkBindImageMemory(m_device, m_depthImage, m_depthImageMemory, 0); res != VK_SUCCESS)
        throw std::runtime_error("Failed to bind depth image memory.");

    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = m_depthImage;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = m_depthFormat;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if (const auto res = vkCreateImageView(m_device, &imageViewCreateInfo, nullptr, &m_depthImageView);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to create depth image view.");
}

VkFormat CApp::FindDepthFormat(std::vector<VkFormat> formats, VkImageTiling tiling, VkFormatFeatureFlags flags)
{
    for (const auto &format : formats)
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(mp_instance->PhysicalDevice(), format, &formatProperties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (flags & formatProperties.linearTilingFeatures) == flags)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (flags & formatProperties.optimalTilingFeatures) == flags)
        {
            return format;
        }
    }
    throw std::runtime_error("Failed to find a suitable depth format.");
}

void CApp::CreateTexImage()
{
    int width, height, channels;
    const auto imageData = CImageLoader::Load2DImage("../assets/textures/viking_room.png", width, height, channels);
    const auto imageSize = width * height * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, imageSize, stagingBuffer,
                 stagingMemory);

    void *data;
    vkMapMemory(m_device, stagingMemory, 0, imageSize, 0, &data);
    memcpy(data, imageData, imageSize);
    vkUnmapMemory(m_device, stagingMemory);

    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.extent.width = width;
    createInfo.extent.height = height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 1;
    createInfo.pQueueFamilyIndices = mp_instance->QueueFamilies().GraphicsFamily();
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (const auto res = vkCreateImage(m_device, &createInfo, nullptr, &m_texImage); res != VK_SUCCESS)
        throw std::runtime_error("Failed to create tex image.");

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(m_device, m_texImage, &memReq);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    allocateInfo.allocationSize = memReq.size;
    vkAllocateMemory(m_device, &allocateInfo, nullptr, &m_texMemory);
    vkBindImageMemory(m_device, m_texImage, m_texMemory, 0);

    VkCommandBufferAllocateInfo cbAllocateInfo{};
    cbAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbAllocateInfo.commandBufferCount = 1;
    cbAllocateInfo.commandPool = m_commandPool;
    cbAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkCommandBuffer commandBuffer;
    if (const auto res = vkAllocateCommandBuffers(m_device, &cbAllocateInfo, &commandBuffer); res != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffer.");

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkImageMemoryBarrier memoryBarrier{};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.srcAccessMask = 0;
    memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.image = m_texImage;
    memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    memoryBarrier.subresourceRange.baseMipLevel = 0;
    memoryBarrier.subresourceRange.levelCount = 1;
    memoryBarrier.subresourceRange.baseArrayLayer = 0;
    memoryBarrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &memoryBarrier);

    VkImageSubresourceLayers layers{};
    layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    layers.mipLevel = 0;
    layers.baseArrayLayer = 0;
    layers.layerCount = 1;

    VkBufferImageCopy imageCopy{};
    imageCopy.bufferOffset = 0;
    imageCopy.bufferRowLength = 0;
    imageCopy.bufferImageHeight = 0;
    imageCopy.imageExtent.width = width;
    imageCopy.imageExtent.height = height;
    imageCopy.imageExtent.depth = 1;
    imageCopy.imageSubresource = layers;

    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, m_texImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &imageCopy);

    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &memoryBarrier);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_graphicsQueue);

    vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);

    vkDestroyBuffer(m_device, stagingBuffer, nullptr);
    vkFreeMemory(m_device, stagingMemory, nullptr);

    CImageLoader::FreeImage(imageData);
}

void CApp::CreateTexImageView()
{
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = m_texImage;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if (const auto res = vkCreateImageView(m_device, &imageViewCreateInfo, nullptr, &m_texImageView); res != VK_SUCCESS)
        throw std::runtime_error("Failed to create tex image view.");
}

void CApp::CreateSampler()
{
    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.minLod = 0.0f;
    createInfo.maxLod = 0.0f;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.mipLodBias = 0.0f;
    createInfo.anisotropyEnable = VK_TRUE;
    createInfo.maxAnisotropy = 16.0f;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;

    if (const auto res = vkCreateSampler(m_device, &createInfo, nullptr, &m_texSampler); res != VK_SUCCESS)
        throw std::runtime_error("Failed to create sampler.");
}

uint32_t CApp::FindMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags flags)
{
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(mp_instance->PhysicalDevice(), &deviceMemoryProperties);

    for (auto i = 0; i != deviceMemoryProperties.memoryTypeCount; ++i)
    {
        if (memoryTypeBits & (1 << i))
        {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
            {
                return i;
            }
        }
    }
    throw std::runtime_error("Failed to find a suitable memory type.");
}

void CApp::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> pools{};

    pools[0].descriptorCount = static_cast<uint32_t>(m_images.size());
    pools[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    pools[1].descriptorCount = static_cast<uint32_t>(m_images.size());
    pools[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.maxSets = static_cast<uint32_t>(m_images.size());
    createInfo.poolSizeCount = pools.size();
    createInfo.pPoolSizes = pools.data();

    if (vkCreateDescriptorPool(m_device, &createInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor pool");
}

void CApp::CreateDescriptorSetLayout(VkDescriptorSetLayout &layout)
{
    std::array<VkDescriptorSetLayoutBinding, 2> bindings{};
    bindings[0].descriptorCount = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings[0].binding = 0;

    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[1].binding = 1;

    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = bindings.size();
    createInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(m_device, &createInfo, nullptr, &layout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor set layout.");
}

void CApp::CreateDescriptorSets(VkDescriptorSet *descriptorSets)
{
    std::vector<VkDescriptorSetLayout> layouts(m_images.size(), m_descriptorLayout);
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pSetLayouts = layouts.data();
    allocateInfo.descriptorPool = m_descriptorPool;
    allocateInfo.descriptorSetCount = static_cast<uint32_t>(m_images.size());

    if (auto res = vkAllocateDescriptorSets(m_device, &allocateInfo, descriptorSets); res != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set.");
    }

    for (auto i = 0; i != m_descriptorSets.size(); ++i)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_vikingRoom.GetUniformBuffer(i);
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(SMVP);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = m_texSampler;
        imageInfo.imageView = m_texImageView;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        std::array<VkWriteDescriptorSet, 2> sets{};
        sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        sets[0].dstSet = m_descriptorSets[i];
        sets[0].dstBinding = 0;
        sets[0].dstArrayElement = 0;
        sets[0].descriptorCount = 1;
        sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        sets[0].pBufferInfo = &bufferInfo;

        sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        sets[1].dstSet = m_descriptorSets[i];
        sets[1].dstBinding = 1;
        sets[1].dstArrayElement = 0;
        sets[1].descriptorCount = 1;
        sets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sets[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_device, sets.size(), sets.data(), 0, nullptr);
    }
}


void CApp::CreateUniformDescriptors()
{
    m_descriptorSets.resize(m_images.size());
    CreateDescriptorSetLayout(m_descriptorLayout);
    CreateDescriptorSets(m_descriptorSets.data());
}

void CApp::CreateGraphicsPipeline()
{
    // Create the shader modules
    const auto vertModule = CreateShaderModule("../assets/shaders/simple.vert", EShaderType::Vert);
    const auto vertStageInfo = CreateShaderPipelineStage(vertModule, EShaderType::Vert);

    const auto fragModule = CreateShaderModule("../assets/shaders/simple.frag", EShaderType::Frag);
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

    const auto inputBindingsDesc = SVertex::GetInputBindingDescription();
    const auto attributeDesc = SVertex::GetAttributeBindingDescription();
    const auto graphicsPipelineStates =
        SGraphicsPipelineStates(attachmentState, viewport, scissors, inputBindingsDesc, attributeDesc);
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
    createInfo.pDepthStencilState = &graphicsPipelineStates.depthStencilState;
    createInfo.pColorBlendState = &graphicsPipelineStates.colorBlendState;
    createInfo.pDynamicState = &graphicsPipelineStates.dynamicState;
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

    for (auto index = 0; index != m_framebuffers.size(); ++index)
    {
        std::array<VkImageView, 2> attachments = {m_imageViews[index], m_depthImageView};
        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = m_renderPass;
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = attachments.data();
        createInfo.width = m_extent.width;
        createInfo.height = m_extent.height;
        createInfo.layers = 1;

        if (vkCreateFramebuffer(m_device, &createInfo, nullptr, &m_framebuffers[index]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create framebuffer.");
    }
}

void CApp::CreateCommandPool()
{
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = *mp_instance->QueueFamilies().GraphicsFamily();

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
        std::array<VkClearValue, 2> clearValues;
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.framebuffer = m_framebuffers[index];
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = m_extent;
        renderPassBeginInfo.clearValueCount = clearValues.size();
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_commandBuffers[index], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Bind the graphics pipeline
        vkCmdBindPipeline(m_commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

        // Bind the vertex buffer
        VkBuffer vertexBuffers[] = {m_vikingRoom.GetVertexBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(m_commandBuffers[index], 0, 1, vertexBuffers, offsets);

        // Bind the index buffer
        vkCmdBindIndexBuffer(m_commandBuffers[index], m_vikingRoom.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

        // Bind the uniform buffer
        vkCmdBindDescriptorSets(m_commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1,
                                &m_descriptorSets[index], 0, nullptr);

        // Draw
        vkCmdDrawIndexed(m_commandBuffers[index], m_vikingRoom.GetIndicesSize(), 1, 0, 0, 0);

        vkCmdEndRenderPass(m_commandBuffers[index]);

        if (vkEndCommandBuffer(m_commandBuffers[index]) != VK_SUCCESS)
            throw std::runtime_error("Failed to end command buffer.");
    }
}

void CApp::CreateSemaphores()
{
    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(m_device, &createInfo, nullptr, &m_semaphoreRenderComplete) != VK_SUCCESS ||
        vkCreateSemaphore(m_device, &createInfo, nullptr, &m_semaphorePresentComplete) != VK_SUCCESS)
        throw std::runtime_error("Failed to create semaphores.");
}

void CApp::CreateFences()
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    m_fences.resize(m_images.size());
    for (auto &fence : m_fences)
    {
        if (vkCreateFence(m_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
            throw std::runtime_error("Failed to create fences.");
    }
}

void CApp::Draw()
{
    // get the next image
    uint32_t imageIndex;
    const auto acquireRes = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_semaphoreRenderComplete,
                                                  VK_NULL_HANDLE, &imageIndex);
    if (acquireRes == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return;
    }
    else if (acquireRes != VK_SUCCESS && acquireRes != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire next image.");
    }
    // Wait for the current fence
    vkWaitForFences(m_device, 1, &m_fences[imageIndex], VK_TRUE, UINT64_MAX);
    vkResetFences(m_device, 1, &m_fences[imageIndex]);

    m_vikingRoom.Draw(m_device, m_extent, imageIndex);

    // start rendering the image by submitting the render
    std::array<VkPipelineStageFlags, 1> flags{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    std::array<VkSemaphore, 1> waitSemaphores{m_semaphoreRenderComplete};
    std::array<VkSemaphore, 1> signalSemaphores{m_semaphorePresentComplete};
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = flags.data();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = signalSemaphores.size();
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_fences[imageIndex]) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit queue.");

    // after render finishes start presenting the image
    std::array<VkSwapchainKHR, 1> swapchains{m_swapchain};
    VkPresentInfoKHR presentInfoKhr{};
    presentInfoKhr.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfoKhr.waitSemaphoreCount = signalSemaphores.size();
    presentInfoKhr.pWaitSemaphores = signalSemaphores.data();
    presentInfoKhr.swapchainCount = swapchains.size();
    presentInfoKhr.pSwapchains = swapchains.data();
    presentInfoKhr.pImageIndices = &imageIndex;

    const auto presentRes = vkQueuePresentKHR(m_presentQueue, &presentInfoKhr);
    if (presentRes == VK_ERROR_OUT_OF_DATE_KHR || presentRes == VK_SUBOPTIMAL_KHR || ShouldRecreateSwapchain())
    {
        RecreateSwapchain();
        return;
    }
    else if (presentRes != VK_SUCCESS)
        throw std::runtime_error("Failed to present image");
}


void CApp::RenderLoop()
{
    while (!glfwWindowShouldClose(mp_window->Window()))
    {
        glfwPollEvents();
        Draw();
    }

    vkDeviceWaitIdle(m_device);
}

void CApp::Cleanup()
{
    CleanupSwapchain();
    for (auto &fence : m_fences)
    {
        vkDestroyFence(m_device, fence, nullptr);
    }
    vkDestroySemaphore(m_device, m_semaphoreRenderComplete, nullptr);
    vkDestroySemaphore(m_device, m_semaphorePresentComplete, nullptr);

    m_vikingRoom.DestroyModel(m_device);

    vkDestroyImage(m_device, m_texImage, nullptr);
    vkFreeMemory(m_device, m_texMemory, nullptr);

    vkDestroySampler(m_device, m_texSampler, nullptr);
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);

    vkDestroyDescriptorSetLayout(m_device, m_descriptorLayout, nullptr);

    vkDestroyDevice(m_device, nullptr);
}
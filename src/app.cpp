#include "app.hpp"
#include "CImageLoader.hpp"
CApp::CApp(SAppInfo appInfo) : m_appInfo(appInfo)
{

    // Create GLFW window
    mp_window = std::make_unique<CWindow>(appInfo.width, appInfo.height);
    // Create Vulkan instance, surface, physical device, queue families and validation layers
    mp_instance = std::make_unique<CInstance>(mp_window->Window(), appInfo);
    mp_bufferImageManager = std::make_unique<CBufferImageManager>(mp_instance->PhysicalDevice());

    m_deviceInstance = &CDevice::GetInstance();
    m_deviceInstance->InitDevice(mp_window.get(), mp_instance.get(), mp_bufferImageManager.get(), appInfo);

    SModelProps vikingProps{};
    vikingProps.objectFile = "../assets/models/viking_room.obj";
    vikingProps.textureFile = "../assets/textures/viking_room.png";
    vikingProps.modelTransform.translate = glm::vec3(0.0f, -3.0f, 0.0f);
    vikingProps.modelTransform.scale = glm::vec3(3.0f, 3.0f, 3.0f);
    m_vecGameObjects.emplace_back(std::make_unique<CModel>(vikingProps));

    SModelProps cubeProps{};
    cubeProps.objectFile = "../assets/models/cube.obj";
    cubeProps.textureFile = "../assets/textures/texture.jpg";
    cubeProps.modelTransform.translate = glm::vec3(0.0f, 3.0f, 0.0f);
    m_vecGameObjects.emplace_back(std::make_unique<CModel>(cubeProps));

    // Create Vulkan device and the rest
    //    CreateDevice();
    //    CreateQueues();
    //    CreateCommandPool();
    //    CreateSwapchain();
    //    CreateSwapchainImages();
    //    CreateImageViews();
    //    // Create the buffer and image manager
    //    // Create the game objects
    //
    //    m_vikingRoom = CModel("../assets/models/viking_room.obj");
    //    m_vikingRoom.InitModel(*mp_bufferImageManager, m_images.size());
    //
    //    CreateDescriptorPool();
    //    CreateTexImage();
    //    CreateTexImageView();
    //    CreateSampler();
    //    CreateDepthImage();
    //
    //    CreateUniformDescriptors();
    //    CreateGraphicsPipeline();
    //    CreateFramebuffers();
    //    CreateCommandBuffers();
    //    CreateSemaphores();
    //    CreateFences();
}

void CApp::CreateTexImage()
{
    //    int width, height, channels;
    //    const auto imageData = CImageLoader::Load2DImage("../assets/textures/viking_room.png", width, height,
    //    channels); const auto imageSize = width * height * 4;
    //
    //    SBufferHandles stagingHandles{};
    //    VkBufferCreateInfo stagingCreateInfo{};
    //    stagingCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    //    stagingCreateInfo.size = imageSize;
    //    stagingCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //    stagingCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    //    mp_bufferImageManager->CreateBuffer(
    //        stagingCreateInfo, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    //        stagingHandles);
    //
    //    void *data;
    //    vkMapMemory(m_deviceInstance->GetDevice(), stagingHandles.memory, 0, imageSize, 0, &data);
    //    memcpy(data, imageData, imageSize);
    //    vkUnmapMemory(m_deviceInstance->GetDevice(), stagingHandles.memory);
    //
    //    VkImageCreateInfo createInfo{};
    //    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    //    createInfo.imageType = VK_IMAGE_TYPE_2D;
    //    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    //    createInfo.extent.width = width;
    //    createInfo.extent.height = height;
    //    createInfo.extent.depth = 1;
    //    createInfo.mipLevels = 1;
    //    createInfo.arrayLayers = 1;
    //    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    //    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    //    createInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    //    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //    createInfo.queueFamilyIndexCount = 1;
    //    createInfo.pQueueFamilyIndices = mp_instance->QueueFamilies().GraphicsFamily();
    //    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //
    //    if (const auto res = vkCreateImage(m_deviceInstance->GetDevice(), &createInfo, nullptr, &m_texImage);
    //        res != VK_SUCCESS)
    //        throw std::runtime_error("Failed to create tex image.");
    //
    //    VkMemoryRequirements memReq;
    //    vkGetImageMemoryRequirements(m_deviceInstance->GetDevice(), m_texImage, &memReq);
    //
    //    VkMemoryAllocateInfo allocateInfo{};
    //    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //    allocateInfo.memoryTypeIndex =
    //        m_deviceInstance->FindMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    //    allocateInfo.allocationSize = memReq.size;
    //    vkAllocateMemory(m_deviceInstance->GetDevice(), &allocateInfo, nullptr, &m_texMemory);
    //    vkBindImageMemory(m_deviceInstance->GetDevice(), m_texImage, m_texMemory, 0);
    //
    //    VkCommandBufferAllocateInfo cbAllocateInfo{};
    //    cbAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    //    cbAllocateInfo.commandBufferCount = 1;
    //    cbAllocateInfo.commandPool = m_deviceInstance->GetCommandPool();
    //    cbAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    //
    //    VkCommandBuffer commandBuffer;
    //    if (const auto res = vkAllocateCommandBuffers(m_deviceInstance->GetDevice(), &cbAllocateInfo, &commandBuffer);
    //        res != VK_SUCCESS)
    //        throw std::runtime_error("Failed to allocate command buffer.");
    //
    //    VkCommandBufferBeginInfo beginInfo{};
    //    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    //
    //    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    //
    //    VkImageMemoryBarrier memoryBarrier{};
    //    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //    memoryBarrier.srcAccessMask = 0;
    //    memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    //    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //    memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //    memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //    memoryBarrier.image = m_texImage;
    //    memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //    memoryBarrier.subresourceRange.baseMipLevel = 0;
    //    memoryBarrier.subresourceRange.levelCount = 1;
    //    memoryBarrier.subresourceRange.baseArrayLayer = 0;
    //    memoryBarrier.subresourceRange.layerCount = 1;
    //
    //    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr,
    //    0,
    //                         nullptr, 1, &memoryBarrier);
    //
    //    VkImageSubresourceLayers layers{};
    //    layers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //    layers.mipLevel = 0;
    //    layers.baseArrayLayer = 0;
    //    layers.layerCount = 1;
    //
    //    VkBufferImageCopy imageCopy{};
    //    imageCopy.bufferOffset = 0;
    //    imageCopy.bufferRowLength = 0;
    //    imageCopy.bufferImageHeight = 0;
    //    imageCopy.imageExtent.width = width;
    //    imageCopy.imageExtent.height = height;
    //    imageCopy.imageExtent.depth = 1;
    //    imageCopy.imageSubresource = layers;
    //
    //    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, m_texImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
    //                           &imageCopy);
    //
    //    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr,
    //    0,
    //                         nullptr, 1, &memoryBarrier);
    //
    //    vkEndCommandBuffer(commandBuffer);
    //
    //    VkSubmitInfo submitInfo{};
    //    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //    submitInfo.commandBufferCount = 1;
    //    submitInfo.pCommandBuffers = &commandBuffer;
    //
    //    vkQueueSubmit(m_deviceInstance->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    //    vkQueueWaitIdle(m_deviceInstance->GetGraphicsQueue());
    //
    //    vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
    //
    //    vkDestroyBuffer(m_device, stagingBuffer, nullptr);
    //    vkFreeMemory(m_device, stagingMemory, nullptr);
    //
    //    CImageLoader::FreeImage(imageData);
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

    if (const auto res =
            vkCreateImageView(m_deviceInstance->GetDevice(), &imageViewCreateInfo, nullptr, &m_texImageView);
        res != VK_SUCCESS)
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

    if (const auto res = vkCreateSampler(m_deviceInstance->GetDevice(), &createInfo, nullptr, &m_texSampler);
        res != VK_SUCCESS)
        throw std::runtime_error("Failed to create sampler.");
}
//
// void CApp::CreateDescriptorSets(VkDescriptorSet *descriptorSets)
//{
//    std::vector<VkDescriptorSetLayout> layouts(m_deviceInstance->GetSwapchainImageCount(),
//                                               m_deviceInstance->GetDescriptorSetLayout());
//    VkDescriptorSetAllocateInfo allocateInfo{};
//    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//    allocateInfo.pSetLayouts = layouts.data();
//    allocateInfo.descriptorPool = m_deviceInstance->GetDescriptorPool();
//    allocateInfo.descriptorSetCount = static_cast<uint32_t>(m_deviceInstance->GetSwapchainImageCount());
//
//    if (auto res = vkAllocateDescriptorSets(m_deviceInstance->GetDevice(), &allocateInfo, descriptorSets);
//        res != VK_SUCCESS)
//    {
//        throw std::runtime_error("Failed to create descriptor set.");
//    }
//
//    for (auto i = 0; i != m_descriptorSets.size(); ++i)
//    {
//        VkDescriptorBufferInfo bufferInfo{};
//        bufferInfo.buffer = m_vikingRoom.GetUniformBuffer(i);
//        bufferInfo.offset = 0;
//        bufferInfo.range = sizeof(SMVP);
//
//        VkDescriptorImageInfo imageInfo{};
//        imageInfo.sampler = m_texSampler;
//        imageInfo.imageView = m_texImageView;
//        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//
//        std::array<VkWriteDescriptorSet, 2> sets{};
//        sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        sets[0].dstSet = m_descriptorSets[i];
//        sets[0].dstBinding = 0;
//        sets[0].dstArrayElement = 0;
//        sets[0].descriptorCount = 1;
//        sets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//        sets[0].pBufferInfo = &bufferInfo;
//
//        sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        sets[1].dstSet = m_descriptorSets[i];
//        sets[1].dstBinding = 1;
//        sets[1].dstArrayElement = 0;
//        sets[1].descriptorCount = 1;
//        sets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//        sets[1].pImageInfo = &imageInfo;
//
//        vkUpdateDescriptorSets(m_deviceInstance->GetDevice(), sets.size(), sets.data(), 0, nullptr);
//    }
//}

void CApp::Draw()
{
    if (!m_deviceInstance->DrawBegin())
        return;

    for (const auto &gameObject : m_vecGameObjects)
    {
        gameObject->UpdateUniformBuffers();
        gameObject->Draw();
    }

    m_deviceInstance->DrawEnd();
}

void CApp::RenderLoop()
{
    while (!glfwWindowShouldClose(mp_window->Window()))
    {
        glfwPollEvents();
        Draw();
    }
}

void CApp::Cleanup()
{
    for (auto &gameObject : m_vecGameObjects)
    {
        gameObject->ModelCleanup();
    }
    m_deviceInstance->Cleanup();
}
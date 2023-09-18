#include "resources.h"
#include "Renderer.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_raii.hpp"
#include "vulkan/vulkan_structs.hpp"

static std::vector<vk::raii::Framebuffer> createFrameBuffers(const vk::raii::Device &device, const vk::raii::RenderPass &renderPass, const Swapchain &swapchain) {
    std::vector<vk::raii::Framebuffer> tempFrameBuffers{};
    tempFrameBuffers.reserve(swapchain.imageviews.size());
    for (const auto &image : swapchain.imageviews) {
        vk::FramebufferCreateInfo createInfo{};
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &*image; // Implicit type conversion by deref
        createInfo.layers = 1;
        createInfo.width = swapchain.extend.width;
        createInfo.height = swapchain.extend.height;
        createInfo.renderPass = *renderPass;
        tempFrameBuffers.push_back(device.createFramebuffer(createInfo));
    }
    // VERY IMPORTANT it will NOT die when freeing the old frameBuffers ptr
    return tempFrameBuffers;
}

static vk::raii::CommandPool createCommandPool(const vk::raii::Device& device) {
    vk::CommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.queueFamilyIndex = 0;
    commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    return device.createCommandPool(commandPoolInfo);
}

static vk::raii::CommandBuffers createCommandBuffer(const vk::raii::Device &device, const vk::raii::CommandPool& cmd_pool) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = *cmd_pool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;
    return {device, allocInfo};
}

Resources::Resources(const vk::raii::Device &device, const vk::raii::RenderPass &renderpass, const Swapchain &swapchain) {
    frameBuffers = std::move(createFrameBuffers(device, renderpass, swapchain));
    commandPool =  std::move(createCommandPool(device));
    commandBuffers = std::move(createCommandBuffer(device, commandPool));

    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    inFlightFences = std::move(device.createFence(fenceInfo));

    finishedRenderingSemaphores = std::move(device.createSemaphore({}));
    imageAvailableSemaphores = std::move(device.createSemaphore({}));
}

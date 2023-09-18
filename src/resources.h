#include "vulkan/vulkan_raii.hpp"
#include <vulkan/vulkan.hpp>

struct Swapchain;

struct Resources {
    std::vector<vk::raii::Framebuffer> frameBuffers{};
    vk::raii::CommandPool commandPool{nullptr};
    vk::raii::CommandBuffers commandBuffers{nullptr};

    vk::raii::Semaphore imageAvailableSemaphores{nullptr};
    vk::raii::Semaphore finishedRenderingSemaphores{nullptr};
    vk::raii::Fence inFlightFences{nullptr};

    Resources(const vk::raii::Device &device, const vk::raii::RenderPass &renderpass, const Swapchain &swapchain);

};
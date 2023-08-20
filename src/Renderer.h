#pragma once
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pCallback);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks *pAllocator);

struct Window {
    GLFWwindow *window{nullptr};

    Window(size_t width, size_t height);
    ~Window();

    const std::vector<const char *> getRequiredVkExtensions() const;

    int shouldClose() const;
};

struct Renderer {
    constexpr static std::array<const char *, 1> validationLayers{"VK_LAYER_KHRONOS_validation"};
    constexpr static std::array<const char *, 1> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    constexpr static size_t Width{600};
    constexpr static size_t Height{600};

    // glfw window
    Window window{Width, Height};

    // vulkan handles
    vk::raii::Context context{};

    // Interface to the vulkan driver
    vk::raii::Instance instance{nullptr};

    // A unified interface to the device
    vk::raii::Device device{nullptr};

    Renderer();
    ~Renderer();

    void run();

    void initVulkan();
    void createInstance();
    const vk::raii::PhysicalDevice pickPhysicalDevice() const;
    void createLogicalDevice();
    void mainLoop();

    // Validation layer callback
    void setupDebugCallback();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
    VkDebugUtilsMessengerEXT callback{};
};
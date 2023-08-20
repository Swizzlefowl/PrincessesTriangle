#include <fmt/core.h>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
// remember to include glfw after vulkan and DONT remove the comment
#include <GLFW/glfw3.h>
#include <memory>

struct Window {
    GLFWwindow *window{nullptr};

    Window(size_t width, size_t height);
    ~Window();

    const std::vector<const char *> getRequiredVkExtensions() const;

    vk::raii::SurfaceKHR attachSurface(vk::raii::Instance &instance);

    std::pair<int, int> getFrameBufferSize() const;

    int shouldClose() const;
};

#define BESTIE UINT64_MAX

struct Swapchain {
    struct SwapChainCapablities {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;

        vk::SurfaceFormatKHR getSurfaceFormat();
        vk::PresentModeKHR getPresentMode();
        vk::Extent2D getExtend(Window &window);

        SwapChainCapablities(vk::raii::PhysicalDevice &physicalDevice, vk::raii::SurfaceKHR &surface);
        SwapChainCapablities() = default;
    };

    vk::SurfaceFormatKHR surfaceFormat{};
    vk::PresentModeKHR presentMode{};
    vk::Extent2D extend{};

    SwapChainCapablities capabilities;

    vk::raii::SurfaceKHR surface{nullptr};
    vk::raii::SwapchainKHR swapchain{nullptr};
    std::vector<vk::raii::Image> image{};
    std::vector<vk::raii::ImageView> imageviews{};

    Swapchain(Window &window, vk::raii::Instance &instance, vk::raii::Device &device, vk::raii::PhysicalDevice &physicalDevice);
    ~Swapchain() = default;

    void createSwapChain(const vk::raii::Device &device);
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pCallback);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks *pAllocator);

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

    // A unified interface to the physicalDevice
    vk::raii::PhysicalDevice physicalDevice{nullptr};
    vk::raii::Device device{nullptr};
    vk::raii::Queue queue{nullptr};

    std::unique_ptr<Swapchain> swapchain{nullptr};

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
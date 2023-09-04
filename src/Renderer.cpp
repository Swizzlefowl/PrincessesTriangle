#include "Renderer.h"
#include "fmt/core.h"
#include "vulkan/vulkan_raii.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <memory>
#include <utility>

#define yeet(x) throw(std::runtime_error(x))

Renderer::Renderer() {}

Renderer::~Renderer() {
    DestroyDebugUtilsMessengerEXT(*instance, callback, nullptr);
}

void Renderer::initVulkan() {
    createInstance();
    setupDebugCallback();
    createLogicalDevice();
    swapchain = std::make_unique<Swapchain>(this->window, this->instance, this->device, this->physicalDevice);
    graphics = std::make_unique<Graphics>(this->device, this->swapchain->surfaceFormat);
}

void Renderer::mainLoop() {
    while (!window.shouldClose()) {
        glfwPollEvents();
    }
}

void Renderer::run() {
    initVulkan();
    mainLoop();
}

void Renderer::createInstance() {
    vk::ApplicationInfo appInfo{};
    appInfo.pApplicationName = "princess vulkanneers";
    appInfo.pEngineName = "delusional64";
    appInfo.apiVersion = VK_API_VERSION_1_2;

    vk::InstanceCreateInfo instanceInfo{};
    instanceInfo.sType = vk::StructureType::eInstanceCreateInfo;
    instanceInfo.pApplicationInfo = &appInfo;

    // Configure the extensions GLFW requires
    const auto requiredExtensions = this->window.getRequiredVkExtensions();
    instanceInfo.enabledExtensionCount = requiredExtensions.size();
    instanceInfo.ppEnabledExtensionNames = requiredExtensions.data();

    // Configure the validation layers
    instanceInfo.enabledLayerCount = Renderer::validationLayers.size();
    instanceInfo.ppEnabledLayerNames = Renderer::validationLayers.data();
    instance = context.createInstance(instanceInfo);
}

const vk::raii::PhysicalDevice Renderer::pickPhysicalDevice() const {
    for (const auto &device : instance.enumeratePhysicalDevices()) {
        if (device.getProperties().deviceType == vk::PhysicalDeviceType::eIntegratedGpu) {
            return device;
        }
    }

    fmt::println("no physical device found better be safe out there my friend");
    return instance.enumeratePhysicalDevices()[0];
}

void Renderer::createLogicalDevice() {
    physicalDevice = pickPhysicalDevice();
    bool queueInitialized{false};
    uint32_t queueIndex{0};

    // Pick a queue family
    for (const auto &queue : physicalDevice.getQueueFamilyProperties()) {
        if (queue.queueFlags & vk::QueueFlagBits::eGraphics) {
            queueInitialized = true;
            break;
        }
        queueIndex++;
    }

    if (!queueInitialized) {
        yeet("no queues either bitch std::move_if_no_exfcept() get a better gpu");
    }

    // Configure our queue with the specified family
    vk::DeviceQueueCreateInfo queueInfo{};
    queueInfo.queueFamilyIndex = queueIndex;
    const float queuePriority[1] = {1.0f}; // NOTE: array size must match queueCount
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriority;

    vk::DeviceCreateInfo deviceInfo{};
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;

    // Request the device extensions we wanna use
    deviceInfo.enabledExtensionCount = deviceExtensions.size();
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Request the device validation layers we wanna use
    deviceInfo.enabledLayerCount = validationLayers.size();
    deviceInfo.ppEnabledLayerNames = validationLayers.data();

    vk::PhysicalDeviceFeatures deviceFeatures{}; // We dont need any features, for now
    deviceInfo.pEnabledFeatures = &deviceFeatures;

    device = physicalDevice.createDevice(deviceInfo);
    queue = device.getQueue(queueIndex, 0);
};

/*
    Validation layer callback configuration, do not look for ur own sanity
*/

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pCallback) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pCallback);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
        func(instance, callback, pAllocator);
}

void Renderer::setupDebugCallback() {
    auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(vk::DebugUtilsMessengerCreateFlagsEXT(), vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError, vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance, debugCallback, nullptr);
    if (CreateDebugUtilsMessengerEXT(*instance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT *>(&createInfo), nullptr, &callback) != VK_SUCCESS)
        throw std::runtime_error("failed to set up debug callback!");
}

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

Swapchain::Swapchain(Window &window, vk::raii::Instance &instance, vk::raii::Device &device, vk::raii::PhysicalDevice &physicalDevice) {
    surface = window.attachSurface(instance);

    capabilities = SwapChainCapablities(physicalDevice, surface);
    extend = capabilities.getExtend(window);
    surfaceFormat = capabilities.getSurfaceFormat();
    // ask ken to finally go to his fucking job god damn freeloader deliver the mail bucko
    presentMode = capabilities.getPresentMode();

    createSwapChain(device);
    createImageViews(device);
}

void Swapchain::createSwapChain(const vk::raii::Device &device) {
    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.imageExtent = extend;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.presentMode = presentMode;

    uint32_t imageCount{capabilities.capabilities.minImageCount + 1};
    if (capabilities.capabilities.maxImageCount > 0 && imageCount > capabilities.capabilities.maxImageCount)
        imageCount = capabilities.capabilities.maxImageCount;
    createInfo.minImageCount = imageCount;
    createInfo.preTransform = capabilities.capabilities.currentTransform;

    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.imageArrayLayers = 1; // 1D array
    createInfo.clipped = true;

    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;

    createInfo.oldSwapchain = nullptr;
    createInfo.surface = *surface;

    swapchain = device.createSwapchainKHR(createInfo);
    fmt::println("created the swapchains gamerz");
}

vk::SurfaceFormatKHR Swapchain::SwapChainCapablities::getSurfaceFormat() {
    for (const auto &format : formats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }
    return formats[0];
}

vk::PresentModeKHR Swapchain::SwapChainCapablities::getPresentMode() {
    for (const auto &ken : presentModes) {
        if (ken == vk::PresentModeKHR::eMailbox) {
            // my main man having another hard days of work babyyyyy
            return ken;
        }
    }
    return presentModes[0];
}

vk::Extent2D Swapchain::SwapChainCapablities::getExtend(Window &window) {
    if (capabilities.currentExtent != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        const auto size = window.getFrameBufferSize();
        vk::Extent2D actualExtent{static_cast<uint32_t>(size.first), static_cast<uint32_t>(size.second)};
        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualExtent;
    }
    yeet("aha bestiee");
    return vk::Extent2D();
}

Swapchain::SwapChainCapablities::SwapChainCapablities(vk::raii::PhysicalDevice &device, vk::raii::SurfaceKHR &surface) {
    capabilities = device.getSurfaceCapabilitiesKHR(*surface);
    formats = device.getSurfaceFormatsKHR(*surface);
    presentModes = device.getSurfacePresentModesKHR(*surface);
}

void Swapchain::createImageViews(const vk::raii::Device& device) {
    auto images = swapchain.getImages();
    std::vector<vk::raii::ImageView> tempImageViews{};
    for (auto &image : images) {
        vk::ImageViewCreateInfo info{};
        info.image = image;
        info.format = surfaceFormat.format;
        info.viewType = vk::ImageViewType::e2D;

        // base	MipmapLevel = 0, levelcount = 1, baseArrayLayer = 0, layerCount
        // =
        // UINTKEN_MAX_SLAYYY
        info.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
        tempImageViews.push_back(device.createImageView(info));
    }
    // VERY IMPORTANT DO NOT REMOVE STD_MOVE_IF_NOEXCEPT
    imageviews = std::move_if_noexcept(tempImageViews);
    fmt::println("barbie is buliding all them images");
}

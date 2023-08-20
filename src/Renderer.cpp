#include "Renderer.h"

#define yeet(x) throw(std::exception(x))

Window::Window(size_t width, size_t height) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    this->window = glfwCreateWindow(width, height, "Princess Aliance Vulaknner", nullptr, nullptr);
}

Window::~Window() {
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

int Window::shouldClose() const {
    return glfwWindowShouldClose(this->window);
};

const std::vector<const char *> Window::getRequiredVkExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return extensions;
}

vk::raii::SurfaceKHR Window::attachSurface(vk::raii::Instance &instance) {
    VkSurfaceKHR c_surface{};
    if (glfwCreateWindowSurface(*instance, this->window, nullptr, &c_surface) != VK_SUCCESS) {
        yeet("no window surafacers...");
    }
    return vk::raii::SurfaceKHR{instance, c_surface};
}

std::pair<int, int> Window::getFrameBufferSize() const {
    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);
    return {width, height};
}

Renderer::Renderer() {}

Renderer::~Renderer() {
    DestroyDebugUtilsMessengerEXT(*instance, callback, nullptr);
}

void Renderer::initVulkan() {
    createInstance();
    setupDebugCallback();
    createLogicalDevice();
    swapchain = std::make_unique<Swapchain>(this->window, this->instance, this->device, this->physicalDevice);
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
    appInfo.apiVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);

    vk::InstanceCreateInfo instanceInfo{};
    // instanceInfo.sType = vk::StructureType::eInstanceCreateInfo;
    instanceInfo.pApplicationInfo = &appInfo;

    // Configure the extensions GLFW requires
    const auto requiredExtensions = this->window.getRequiredVkExtensions();
    instanceInfo.enabledExtensionCount = requiredExtensions.size();
    instanceInfo.ppEnabledExtensionNames = requiredExtensions.data();

    // Configure the validation layers
    instanceInfo.enabledLayerCount = Renderer::validationLayers.size();
    instanceInfo.ppEnabledLayerNames = Renderer::validationLayers.data();

    instance = context.createInstance(instanceInfo);
    // yeet("no chance betch");
}

const vk::raii::PhysicalDevice Renderer::pickPhysicalDevice() const {
    for (const auto &device : instance.enumeratePhysicalDevices()) {
        if (device.getProperties().deviceType == vk::PhysicalDeviceType::eIntegratedGpu) {
            return device;
        }
    }

    throw("no physical device found");
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
        throw("no queues either bitch std::move_if_no_exfcept() get a better gpu");
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

    // NOTE: Vulkan-hpp has methods for this, but they trigger linking errors...
    // instance->createDebugUtilsMessengerEXT(createInfo);
    // instance->createDebugUtilsMessengerEXTUnique(createInfo);

    // NOTE: reinterpret_cast is also used by vulkan.hpp internally for all these
    // structs
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
    std::cout << "hello  gamers";
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
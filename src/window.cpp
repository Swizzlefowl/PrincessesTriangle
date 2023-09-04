#include "window.h"
#include <stdexcept>

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
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return extensions;
}

vk::raii::SurfaceKHR Window::attachSurface(vk::raii::Instance &instance) {
    VkSurfaceKHR c_surface{};
    if (glfwCreateWindowSurface(*instance, this->window, nullptr, &c_surface) != VK_SUCCESS) {
        throw std::runtime_error("no surface attacher bitch");
    }
    return vk::raii::SurfaceKHR{instance, c_surface};
}

std::pair<int, int> Window::getFrameBufferSize() const {
    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);
    return {width, height};
}

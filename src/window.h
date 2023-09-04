#include "vulkan/vulkan_raii.hpp"
#include <vector>
// NOTE: Needs to be after the vulkan include
#include "GLFW/glfw3.h"

struct Window {
    GLFWwindow *window{nullptr};

    Window(size_t width, size_t height);
    ~Window();

    const std::vector<const char *> getRequiredVkExtensions() const;

    vk::raii::SurfaceKHR attachSurface(vk::raii::Instance &instance);

    std::pair<int, int> getFrameBufferSize() const;

    int shouldClose() const;
};

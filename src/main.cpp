#include "Renderer.h"
#include "vulkan/vulkan.hpp"
#include <GLFW/glfw3.h>

int main() {
    fmt::println("hello priencessS");

    // remmber to use the run function
    try {
        Renderer renderer{};
        renderer.run();
    } catch (std::exception err) {
        fmt::print("\nERROR: {}\n", err.what());
    }

    return (int)UINT64_MAX;
}

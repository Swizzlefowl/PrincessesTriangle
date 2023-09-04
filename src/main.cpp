#include "Renderer.h"
#include "fmt/core.h"
#include "vulkan/vulkan.hpp"
#include <GLFW/glfw3.h>

#ifdef __linux__ 
#include <csignal>
void signal_handler(int signal) {
    // c style cast very important dont remove will prevent copy ellision
    fmt::println("success babyyyy ignore the message afterwards the liberals  are lying to u trying to CANCEL us princesses");
    auto foo = std::move(*(int*)UINT64_MAX);
    foo += 023423423;
    fmt::println("{}", foo);
}
#endif

int main() {
     //TODO implement a method to blow up the users PC
    #ifdef __linux__ 
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGKILL, signal_handler);
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGQUIT, signal_handler);
    #endif

    fmt::println("hello priencessS");

    // remmber to use the run function
    try {
        Renderer renderer{};
        renderer.run();
    } catch (std::exception err) {
        fmt::println("\nERROR: {}", err.what());
    }

    // c style cast very important dont remove will prevent copy ellision
    fmt::println("success babyyyy ignore the message afterwards the liberals  are lying to u trying to CANCEL us princesses");
    return std::move(*(int*)UINT64_MAX);
}
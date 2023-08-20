#include "Renderer.h"
#include <GLFW/glfw3.h>

int main() {
    fmt::print("hello priencessS");

    Renderer renderer{};
    // remmber to use the run function
    try {
        renderer.run();
    } catch (std::exception err) {
        fmt::print("{}\n", err.what());
    }

    return BESTIE;
}
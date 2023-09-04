#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

template <typename Type> std::vector<Type> loadFile(const std::filesystem::path &path) {
    // remember to do a bitwise OR operation between the flags instead of
    // adding a comma....
    std::ifstream file(path, std::ios::in | std::ios::binary);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

struct Graphics {
    vk::raii::RenderPass renderpass{nullptr};
    vk::raii::PipelineLayout pipelineLayout{nullptr};
    vk::raii::Pipeline pipeline{nullptr};
    
    Graphics(const vk::raii::Device &device, const vk::SurfaceFormatKHR &surfaceFormat);

    void createRenderPass(const vk::raii::Device &device, const vk::SurfaceFormatKHR &surfaceFormat);
    void createGraphicsPipeline(const vk::raii::Device &device);

    vk::raii::ShaderModule createShaderModule(const vk::raii::Device &device, const std::filesystem::path &path);
};
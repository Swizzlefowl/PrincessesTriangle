#include "graphics.h"
#include "fmt/core.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_raii.hpp"
#include <cstdint>

using ShaderArray = const std::array<vk::PipelineShaderStageCreateInfo, 2>;
using DynamicStateArray = const std::array<vk::DynamicState, 2>;

Graphics::Graphics(const vk::raii::Device &device, const vk::SurfaceFormatKHR &surfaceFormat) {
    createRenderPass(device, surfaceFormat);
    createGraphicsPipeline(device);
    fmt::println("barbie is so proud of u besties the pipelines are there");
}

void Graphics::createRenderPass(const vk::raii::Device &device, const vk::SurfaceFormatKHR &surfaceFormat) {
    // the color attachment refers to the framebuffer attachment which
    // will be binded to our renderpass at drawing time
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = surfaceFormat.format;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    // ken does not give a shit
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    vk::SubpassDescription subpass{};

    vk::AttachmentReference colorAttachmentRef{0 /* Color attachment #0 */, vk::ImageLayout::eColorAttachmentOptimal};
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.colorAttachmentCount = 1;
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.subpassCount = 1;
    // zero dependencies suckless lifestyle babyyy
    renderPassInfo.pDependencies = nullptr;

    renderpass = device.createRenderPass(renderPassInfo);
}

void Graphics::createGraphicsPipeline(const vk::raii::Device &device) {
    const auto vertShaderModule{createShaderModule(device, "./build/vertshader.spv")};
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = *vertShaderModule;
    vertShaderStageInfo.pName = "main";

    const auto fragShaderModule{createShaderModule(device, "./build/FragShader.spv")};
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = *fragShaderModule;
    fragShaderStageInfo.pName = "main";

    ShaderArray shaderStagesInfo{vertShaderStageInfo, fragShaderStageInfo};

    vk::PipelineViewportStateCreateInfo viewportState{};
    // NOTE: we will add these later.
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.minSampleShading = 1.0f;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    const DynamicStateArray dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.dynamicStateCount = dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

    pipelineLayout = device.createPipelineLayout({});

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStagesInfo.data();
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = *pipelineLayout;
    pipelineInfo.renderPass = *renderpass;
    pipelineInfo.basePipelineIndex = -1;
    pipelineInfo.subpass = 0; // Index 0
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    vk::PipelineInputAssemblyStateCreateInfo assemblyCreateInfo{{} /* flags */, vk::PrimitiveTopology::eTriangleList};
    pipelineInfo.pInputAssemblyState = &assemblyCreateInfo;

    pipeline = device.createGraphicsPipeline(nullptr, pipelineInfo);
}

vk::raii::ShaderModule Graphics::createShaderModule(const vk::raii::Device &device, const std::filesystem::path &path) {
    const auto data{loadFile<uint8_t>(path)}; // TODO: does this transform char -> u32 correctly????
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.codeSize = data.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(data.data());
    return device.createShaderModule(createInfo);
}
#include "Renderer.hpp"
#include <chrono>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <stdexcept>
#include "Logger.hpp"
#include "Register.hpp"
#include "imgui.h"
#include "ImGuizmo.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

void Renderer::updateUniformBuffer(uint32_t currentImage)
{
    ubo.model = glm::mat4(1.0f);
    // needs abstract to camera class

    ubo.view = glm::lookAt(cam.pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.proj = glm::perspective(glm::radians(90.0f), Swapchain::swapchainExtent.width / (float)Swapchain::swapchainExtent.height, 0.1f, 60.0f);
    ubo.proj[1][1] *= -1;
    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void Renderer::drawFrame()
{
    vkWaitForFences(VulkanInstance::device, 1, &Syncobjects::inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    uint32_t image = 0;
    VkResult res = vkAcquireNextImageKHR(VulkanInstance::device, Swapchain::swapchain, UINT64_MAX, Syncobjects::imageDoneSemaphores[currentFrame], VK_NULL_HANDLE, &image);

    if (res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        swapchain->remakeSwapchain();
        makeDepthResources();
        renderpipeline->makeFrameBuffer(depth);
        return;
    }
    else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("Failed to get next image");

    updateUniformBuffer(currentFrame);

    vkResetFences(VulkanInstance::device, 1, &Syncobjects::inFlightFences[currentFrame]);

    vkResetCommandBuffer(renderpipeline->commandBuffers[currentFrame], 0);


    ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, Swapchain::swapchainExtent.width, Swapchain::swapchainExtent.height);

    ubo.proj[1][1] *= -1.0f;
    float mat[16];
    float grid[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(TransformWindow::target->pos), glm::value_ptr(TransformWindow::target->rotation), glm::value_ptr(TransformWindow::target->scale), mat);
    ImGuizmo::Manipulate(glm::value_ptr(ubo.view), glm::value_ptr(ubo.proj), TransformWindow::op, ImGuizmo::WORLD, mat);
    // ImGuizmo::DrawGrid(glm::value_ptr(ubo.view), glm::value_ptr(ubo.proj), grid, 20);
    ImGuizmo::DecomposeMatrixToComponents(mat, glm::value_ptr(TransformWindow::target->pos), glm::value_ptr(TransformWindow::target->rotation), glm::value_ptr(TransformWindow::target->scale));
    ubo.proj[1][1] *= -1.0f;

    //imgui commands
    transformWindow.show();
    //ImGui::ShowDemoWindow();

    // maybe abstract away
    renderpipeline->recordCommandBuffer(renderpipeline->commandBuffers[currentFrame], image, currentFrame, models);

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &renderpipeline->commandBuffers[currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &syncobjects->renderFinishedSemaphores[currentFrame];
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &syncobjects->imageDoneSemaphores[currentFrame];
    submitInfo.pWaitDstStageMask = waitStages;

    if (vkQueueSubmit(RenderPipeline::graphicsQueue, 1, &submitInfo, syncobjects->inFlightFences[currentFrame]) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit to queue");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain->swapchain;
    presentInfo.pImageIndices = &image;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &syncobjects->renderFinishedSemaphores[currentFrame];

    res = vkQueuePresentKHR(RenderPipeline::graphicsQueue, &presentInfo);

    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || frameResize)
    {
        frameResize = false;
        swapchain->remakeSwapchain();
        makeDepthResources();
        renderpipeline->makeFrameBuffer(depth);
        return;
    }
    else if (res != VK_SUCCESS)
        throw std::runtime_error("Failed to present next image");

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::makeUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        Buffer::makeBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i].buffer, uniformBuffers[i].bufferMemory);
        vkMapMemory(instance->device, uniformBuffers[i].bufferMemory, 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }

    renderpipeline->makeDescriptorSets(uniformBuffers, texture);
}

// fix this
void Renderer::makeTextureImage()
{
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load("resources/swmg.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
        throw std::runtime_error("Failed to load texture image!");

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    Buffer::makeBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void *data;
    vkMapMemory(VulkanInstance::device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(VulkanInstance::device, stagingBufferMemory);

    stbi_image_free(pixels);

    texture.makeImage(texWidth, texHeight, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    texture.transitionImageLayout(*renderpipeline, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL); // specify correct layout for transfers
    copyBufferToImage(stagingBuffer, texture.image, texWidth, texHeight);
    texture.transitionImageLayout(*renderpipeline, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); // specify correct layout for shaders

    vkDestroyBuffer(VulkanInstance::device, stagingBuffer, nullptr);
    vkFreeMemory(VulkanInstance::device, stagingBufferMemory, nullptr);
}

// fix this
void Renderer::makeDepthResources()
{
    depth.makeImage(swapchain->swapchainExtent.width, swapchain->swapchainExtent.height, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    depth.makeImageView(VK_IMAGE_ASPECT_DEPTH_BIT);
}

// needs refactor
void Renderer::clean()
{
    vkDeviceWaitIdle(VulkanInstance::device);
    std::cout << Logger::info << "Cleanup" << Logger::reset;

    // for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    // {
    //     vkDestroyBuffer(VulkanInstance::device, uniformBuffers[i], nullptr);
    //     vkFreeMemory(VulkanInstance::device, uniformBuffersMemory[i], nullptr);
    // }

    // vkDestroyImageView(VulkanInstance::device, depthImageView, nullptr);
    // vkDestroyImage(VulkanInstance::device, depthImage, nullptr);
    // vkFreeMemory(VulkanInstance::device, depthImageMemory, nullptr);

    // vkDestroySampler(VulkanInstance::device, textureSampler, nullptr);

    // vkDestroyImageView(VulkanInstance::device, textureImageView, nullptr);

    // vkDestroyImage(VulkanInstance::device, textureImage, nullptr);
    // vkFreeMemory(VulkanInstance::device, textureImageMemory, nullptr);

    // vkDestroyBuffer(VulkanInstance::device, vertexBuffer, nullptr);
    // vkFreeMemory(VulkanInstance::device, vertexBufferMemory, nullptr);

    // vkDestroyBuffer(VulkanInstance::device, indexBuffer, nullptr);
    // vkFreeMemory(VulkanInstance::device, indexBufferMemory, nullptr);

    renderpipeline.release();
    
    syncobjects.release();

    instance.release();

    window.release();
}

void Renderer::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = RenderPipeline::beginSingleTimeCommands();
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1};
    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    RenderPipeline::endSingleTimeCommands(commandBuffer);
}

// void Renderer::run()
// {
//     init();
//     loop();
//     clean();
// }

// order of member variables indicated here as logs
void Renderer::init()
{
    std::cout << Logger::info << "Engine started" << Logger::reset;
    window = std::make_unique<Window>();
    instance = std::make_unique<VulkanInstance>();

    std::cout << Logger::info << "Swapchain created" << Logger::reset;
    swapchain = std::make_unique<Swapchain>();
    /* pipeline */
    makeDepthResources();
    std::cout << Logger::info << "Renderpipeline" << Logger::reset;
    renderpipeline = std::make_unique<RenderPipeline>(depth);
    makeTextureImage();
    texture.makeImageView(VK_IMAGE_ASPECT_COLOR_BIT);
    texture.makeImageSampler();

    /* Vertex Buffer */
    std::cout << Logger::info << "Models init" << Logger::reset;
    std::vector<Renderable*> r = g_reg.getComponents<Renderable>();
    for (auto& obj : r)
    {
        models.emplace_back(obj->model_name);
        models.back().init();
    }

    std::cout << Logger::info << "Buffer initialization" << Logger::reset;
    makeUniformBuffers();

    /* Sync */
    syncobjects = std::make_unique<Syncobjects>();
    transformWindow.init();

    cam.pos = glm::vec3(8, 8, 0);
}
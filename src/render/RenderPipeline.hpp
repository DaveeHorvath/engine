#ifndef RENDERPIPELINE_HPP
#define RENDERPIPELINE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifndef MAX_FRAMES_IN_FLIGHT
# define MAX_FRAMES_IN_FLIGHT 2
#endif

#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

class RenderObject;
class Image;
class Model;
class Buffer;

struct Transform;

#include <glm/glm.hpp>

class RenderPipeline {
private:
    VkShaderModule makeShaderModule(const std::vector<char>& shader);
public:
    RenderPipeline(Image depth);
    ~RenderPipeline();
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSetLayout texutreDescriptorSetLayout;
    VkPipelineLayout pipelineLayout;


    inline static VkQueue graphicsQueue;
    inline static VkQueue presentQueue;

    inline static VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapchainFramebuffers;

    inline static VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorPool imguiPool;

    static VkCommandBuffer beginSingleTimeCommands();
    static void endSingleTimeCommands(VkCommandBuffer buffer);
    void recordCommandBuffer(VkCommandBuffer buffer, uint32_t image, uint32_t currentFrame , std::vector<Model> models);

    void makeCommandPool();
    void makeCommandBuffer();

    void makeFrameBuffer(Image depthImage);

    void makeDescriptorSetLayout();
    void makeDescriptorSets(std::vector<Buffer> uniformBuffers, Image textureImage);
    void makeDescriptorPool();

    void makeTextureDescriptorSetLayout();

    void makeRenderPass(Image depthImage);
    void makePipeline();

    void initImgui();
};

#endif
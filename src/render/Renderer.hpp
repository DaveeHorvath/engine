#ifndef APP_HPP
# define APP_HPP
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <iostream>

inline static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props, VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);
    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
    {
        if (typeFilter & (i << 1) && (memProps.memoryTypes[i].propertyFlags & props) == props)
            return i;
    }
    throw std::runtime_error("Failed to find suitable memory type");
}

struct Camera {
    glm::vec3 pos;
    float pitch;
    float tilt;
};


#include "UniformBufferObject.hpp"
#include "Window.hpp"
#include "Vulkan.hpp"
#include "Swapchain.hpp"
#include "RenderPipeline.hpp"
#include "Syncobjects.hpp"
#include "Buffer.hpp"
#include "Image.hpp"
#include "Model.hpp"
#include "TransformWindow.hpp"

#define MAX_FRAMES_IN_FLIGHT 2
#include <memory>

class Renderer {
    public:
        uint32_t currentFrame = 0;
        bool frameResize = false;
        void init();
        void clean();
        void drawFrame();
        // needs to change, tmp for access
        static inline Camera cam;
        UniformBufferObject ubo{};
    private:

        TransformWindow transformWindow;

        std::unique_ptr<Window> window;

        std::unique_ptr<VulkanInstance> instance;

        std::unique_ptr<Swapchain> swapchain;
        
        Image texture{VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT};
        Image depth{VK_FORMAT_UNDEFINED, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT};

        std::unique_ptr<RenderPipeline> renderpipeline;

        std::unique_ptr<Syncobjects> syncobjects;
        
        std::vector<Buffer> uniformBuffers;
        std::vector<void *> uniformBuffersMapped;

        std::vector<Model> models;

        void updateUniformBuffer(uint32_t currentImage);
        
        // needs to move somewhere, unsure atm
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        void makeUniformBuffers();

        void makeTextureImage();

        void makeDepthResources();
};

#endif
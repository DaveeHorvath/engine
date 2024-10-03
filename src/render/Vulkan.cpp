#include "Vulkan.hpp"
#include <iostream>
#include <cstring>
#include <set>
#include "QueueFamilyIndicies.hpp"
#include "RenderPipeline.hpp"
#include "Window.hpp"
#include "Logger.hpp"

VulkanInstance::VulkanInstance()
{
    init();
    createSurface();
    pickPhysicalDevice();
    makeLogicalDevice();
}

VulkanInstance::~VulkanInstance()
{
    vkDestroyDevice(VulkanInstance::device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

bool VulkanInstance::check_validation_layer_support()
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
    for (auto &layer : validationLayer)
    {
        bool found = false;
        for (auto &available : layers)
        {
            if (strcmp(available.layerName, layer) == 0)
            {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return true;
}

void VulkanInstance::init()
{
    std::cout << Logger::debug << "Vulkan engine: made by @dhorvath" << Logger::reset;
    if (enableValidationLayers && !check_validation_layer_support())
        throw std::runtime_error("Failed to init validation layers");
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_0;
    appInfo.pApplicationName = "Scop";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 1, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 1, 0);

    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    instanceInfo.enabledExtensionCount = glfwExtensionCount;
    instanceInfo.ppEnabledExtensionNames = glfwExtensions;

    if (enableValidationLayers)
    {
        instanceInfo.enabledLayerCount = validationLayer.size();
        instanceInfo.ppEnabledLayerNames = validationLayer.data();
    }
    else
        instanceInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create instance");

    /* extensions */
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (extensionCount != 0)
    {
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        if (false)
        {
            std::cout << "Availiable Extensions: \n";
            for (auto &ext : extensions)
                std::cout << "\t" << ext.extensionName << "\n";
        }
    }
}

/* needs to make a score of the devices */
void VulkanInstance::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
        throw std::runtime_error("No suitable device");
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    physicalDevice = physicalDevices[0];
}

void VulkanInstance::makeLogicalDevice()
{
    QueueFamilyIndicies queues = QueueFamilyIndicies::findQueueFamilyIndicies(physicalDevice, surface);
    std::set<uint32_t> uniquequeues{queues.graphicsFamily.value(), queues.presentFamily.value()};

    float queuePrio = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos{};
    for (auto &queue : uniquequeues)
    {
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.queueFamilyIndex = queue;
        deviceQueueCreateInfo.pQueuePriorities = &queuePrio;
        deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers)
    {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayer.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayer.data();
    }
    else
    {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical device");

    vkGetDeviceQueue(device, queues.graphicsFamily.value(), 0, &RenderPipeline::graphicsQueue);
    vkGetDeviceQueue(device, queues.presentFamily.value(), 0, &RenderPipeline::presentQueue);
}

void VulkanInstance::createSurface()
{
    if (glfwCreateWindowSurface(instance, Window::win, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create surface");
}
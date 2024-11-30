#include "Model.hpp"
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <string.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyObjLoaded.h>
#include <stb_image.h>

#include "Vulkan.hpp"
#include "Renderer.hpp"

Model::Model(std::string model) : name(model) {}

void Model::loadModel()
{
    std::vector<Vertex> read_verticies;
    std::vector<int> read_indicies;
    std::ifstream file{name};
    if (!file.is_open())
        throw std::runtime_error("Failed to open file" + name);
    for (std::string line; std::getline(file, line);)
    {
        std::stringstream ss{line};
        char type;
        ss >> type;
        if (type == 'v')
        {
            Vertex v;
            ss >> v.pos.x >> v.pos.y >> v.pos.z;
            v.texCoord = {v.pos.x / glm::length(v.pos), -v.pos.y / glm::length(v.pos)};
            read_verticies.push_back(v);
        }
        else if (type == 'f')
        {
            std::vector<int> face;
            face.clear();
            for (std::string item; std::getline(ss, item, ' ');)
            {
                if (item != "")
                    face.push_back(std::stoi(item));
            }
            if (face.size() != 3)
                std::cout << "weird shit detected\n";
            for (int i = 0; i + 2 < face.size(); i++)
            {
                read_indicies.push_back(face[i + 2]);
                read_indicies.push_back(face[i]);
                read_indicies.push_back(face[i + 1]);
            }
        }
    }

    for (auto &i : read_indicies)
    {
        indices.push_back(indices.size());
        vertices.push_back(read_verticies[i - 1]);
    }
}



void Model::loadExtendedModel(glm::vec3 color)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> mats;

    if (!tinyobj::LoadObj(&attrib, &shapes, &mats, nullptr, nullptr, name.c_str()))
    {
        std::runtime_error("Failed to load Model");
    }
    vertices.clear();
    indices.clear();

    for(const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex v;
            if (index.vertex_index >= 0)
            {
                v.pos = {
                    attrib.vertices[3 * index.vertex_index],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
            }
            if (index.normal_index >= 0)
            {
                v.normal = {
                    attrib.vertices[3 * index.normal_index],
                    attrib.vertices[3 * index.normal_index + 1],
                    attrib.vertices[3 * index.normal_index + 2]
                };
            }
            if (index.texcoord_index >= 0)
            {
                v.texCoord = {
                    attrib.vertices[3 * index.texcoord_index],
                    attrib.vertices[3 * index.texcoord_index + 1],
                };
            }
            else
            {
                v.texCoord = {v.pos.x / glm::length(v.pos), -v.pos.y / glm::length(v.pos)};
            }
            v.color = color;
            vertices.push_back(v);
            indices.push_back(indices.size());
        }
    }
}

void Model::makeVertexBuffer()
{
    VkDeviceSize deviceSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    Buffer::makeBuffer(deviceSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(VulkanInstance::device, stagingBufferMemory, 0, deviceSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)deviceSize);
    vkUnmapMemory(VulkanInstance::device, stagingBufferMemory);

    Buffer::makeBuffer(deviceSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer.buffer, vertexBuffer.bufferMemory);

    Buffer::copyBuffer(stagingBuffer, vertexBuffer.buffer, deviceSize);

    vkDestroyBuffer(VulkanInstance::device, stagingBuffer, nullptr);
    vkFreeMemory(VulkanInstance::device, stagingBufferMemory, nullptr);
}

// fix this
void Model::makeIndexBuffer()
{
    VkDeviceSize deviceSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    Buffer::makeBuffer(deviceSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(VulkanInstance::device, stagingBufferMemory, 0, deviceSize, 0, &data);
    memcpy(data, indices.data(), (size_t)deviceSize);
    vkUnmapMemory(VulkanInstance::device, stagingBufferMemory);

    Buffer::makeBuffer(deviceSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer.buffer, indexBuffer.bufferMemory);

    Buffer::copyBuffer(stagingBuffer, indexBuffer.buffer, deviceSize);

    vkDestroyBuffer(VulkanInstance::device, stagingBuffer, nullptr);
    vkFreeMemory(VulkanInstance::device, stagingBufferMemory, nullptr);
}

void Model::init(glm::vec3 color, RenderPipeline& renderpipeline, std::string textureName)
{
    loadExtendedModel(color);
    makeVertexBuffer();
    makeIndexBuffer();
    initImage(renderpipeline, textureName);
    initDescriptorSet(renderpipeline);
}

void Model::initImage(RenderPipeline& renderpipeline, std::string textureName)
{
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(textureName.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
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

    textureImage.makeImage(texWidth, texHeight, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    textureImage.transitionImageLayout(renderpipeline, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL); // specify correct layout for transfers
    Renderer::copyBufferToImage(stagingBuffer, textureImage.image, texWidth, texHeight);
    textureImage.transitionImageLayout(renderpipeline, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); // specify correct layout for shaders

    vkDestroyBuffer(VulkanInstance::device, stagingBuffer, nullptr);
    vkFreeMemory(VulkanInstance::device, stagingBufferMemory, nullptr);
    textureImage.makeImageView(VK_IMAGE_ASPECT_COLOR_BIT);
    textureImage.makeImageSampler();
}

void Model::initDescriptorSet(RenderPipeline& renderpipeline)
{
    VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = renderpipeline.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &renderpipeline.texutreDescriptorSetLayout;

	if (vkAllocateDescriptorSets(VulkanInstance::device, &allocInfo, &texture) != VK_SUCCESS)
		throw std::runtime_error("Failed to create texture descriptor set");

    VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = textureImage.imageView;
	imageInfo.sampler = textureImage.sampler;

    std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = texture;
	descriptorWrites[0].dstBinding = 1;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(VulkanInstance::device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

void Model::render(VkCommandBuffer commandBuffer)
{
    VkBuffer vertexBuffers[] = {vertexBuffer.buffer};
	VkDeviceSize offsets[] = {0};
    
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}
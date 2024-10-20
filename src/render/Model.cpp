#include "Model.hpp"
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <string.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyObjLoaded.h>

#include "Vulkan.hpp"

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

void Model::init(glm::vec3 color)
{
    loadExtendedModel(color);
    makeVertexBuffer();
    makeIndexBuffer();
}

void Model::render(VkCommandBuffer commandBuffer)
{
    VkBuffer vertexBuffers[] = {vertexBuffer.buffer};
	VkDeviceSize offsets[] = {0};
    
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}
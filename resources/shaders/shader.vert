#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform push_constant {
    mat4 transform;
}ps;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * ps.transform * ubo.model * vec4(inPosition, 1.0);
    vec3 l = {1,2,1};
    fragColor = inColor * max(dot(vec3(vec4(normal, 0) * ps.transform), l), 0);
    fragTexCoord = texCoord;
}
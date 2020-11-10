#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(position, 1.0);

    outPos = vec3(ubo.model * vec4(position, 1.0));
    outNormal = normal;
    outUV = uv;
}
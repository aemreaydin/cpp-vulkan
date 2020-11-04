#version 450
#extension GL_ARB_separate_shader_objects : enable

//layout (binding = 1) uniform sampler2D samplerColor;

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inClr;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(inClr, 1.0);
}
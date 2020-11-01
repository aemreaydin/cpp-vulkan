#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 1) uniform sampler2D samplerColor;
layout (binding = 2) uniform Clr {
 vec3 color;
} clr;

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 outColor;

void main() {
//    outColor = texture(samplerColor, inUV) + vec4(Color.color + 1.0);
    outColor = texture(samplerColor, inUV) + vec4(clr.color, 1.0);
}
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 1) uniform sampler2D samplerColor;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec4 outColor;

vec3 lightPos = vec3(1.0, 1.0, 1.0);
vec3 eyePos = vec3(10.0, 0.0, 10.0);
void main() {
    vec4 objColor = texture(samplerColor, inUV);
    vec4 ambColor = 0.1 * vec4(0.5, 1.0, 0.25, 1.0) * objColor;


    vec3 normLightDir = normalize(lightPos - inPos);
    vec3 normNormal = normalize(inNormal);
    vec4 difColor = max(0.0, dot(normLightDir, normNormal)) * vec4(1.0, 1.0, 1.0, 1.0) * objColor;


    vec3 viewDir = normalize(-inPos);
    vec3 lightReflection = reflect(-normLightDir, normNormal);
    vec4 spcColor = pow(max(0.0, dot(viewDir, lightReflection)), 62) * vec4(1.0) * objColor;
    outColor = ambColor + difColor + spcColor;
}
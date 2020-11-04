#pragma once

#include <fstream>
#include <shaderc/shaderc.hpp>
#include <vector>
#include <vulkan/vulkan.h>

enum class EShaderType
{
    Frag,
    Vert
};
class CShaderUtils
{
  private:
    static shaderc_shader_kind GetShaderKind(EShaderType shaderType);

  public:
    static std::vector<char> ReadGlsl(const std::string &filename);
    static std::vector<uint32_t> ConvertGlslToSpirv(const std::string &filename, EShaderType shaderType);
    static VkShaderModule CreateShaderModule(const VkDevice device, const std::string &shaderFile,
                                             const EShaderType shaderType);
    static VkPipelineShaderStageCreateInfo ShaderPipelineStageCreateInfo(const VkShaderModule &module,
                                                                         const EShaderType shaderType);
};
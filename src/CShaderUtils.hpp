#pragma once

#include <fstream>
#include <shaderc/shaderc.hpp>
#include <vector>

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
};
#include "CShaderUtils.hpp"

std::vector<char> CShaderUtils::ReadGlsl(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Failed to open the shader file.");

    const auto fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

std::vector<uint32_t> CShaderUtils::ConvertGlslToSpirv(const std::string &filename, EShaderType shaderType)
{
    const auto glsl = CShaderUtils::ReadGlsl(filename);

    shaderc::Compiler compiler;
    shaderc::CompileOptions compileOptions;
    compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);

    const auto module =
        compiler.CompileGlslToSpv(glsl.data(), glsl.size(), GetShaderKind(shaderType), "shader", compileOptions);
    if (const auto status = module.GetCompilationStatus(); status != shaderc_compilation_status_success)
        throw std::runtime_error(module.GetErrorMessage());

    return {module.cbegin(), module.cend()};
}

shaderc_shader_kind CShaderUtils::GetShaderKind(EShaderType shaderType)
{
    switch (shaderType)
    {
    case EShaderType::Frag:
        return shaderc_glsl_fragment_shader;
    case EShaderType::Vert:
        return shaderc_glsl_vertex_shader;
    default:
        throw std::runtime_error("Failed - Shader type not supported.");
    }
}

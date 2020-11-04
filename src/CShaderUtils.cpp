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

VkShaderModule CShaderUtils::CreateShaderModule(const VkDevice device, const std::string &shaderFile,
                                                const EShaderType shaderType)
{
    const auto spirv = CShaderUtils::ConvertGlslToSpirv(shaderFile, shaderType);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirv.size() * sizeof(uint32_t);
    createInfo.pCode = spirv.data();

    VkShaderModule shaderModule;
    if (const auto res = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule); res != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module.");

    return shaderModule;
}

VkPipelineShaderStageCreateInfo CShaderUtils::ShaderPipelineStageCreateInfo(const VkShaderModule &module,
                                                                            const EShaderType shaderType)
{
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.module = module;
    createInfo.pName = "main";
    // pSpecializationInfo creates a specialization constant
    switch (shaderType)
    {
    case EShaderType::Frag:
        createInfo.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        break;
    case EShaderType::Vert:
        createInfo.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
        break;
    default:
        throw std::runtime_error("Shader Type doesn't exist.");
    }

    return createInfo;
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
    {
        fprintf(stderr, "%s", module.GetErrorMessage().c_str());
        exit(-1);
    }

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

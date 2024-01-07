#include "imvkpch.h"
#include "ImVulkan/Vulkan/VulkanShaderModule.h"

#include "ImVulkan/Util/AssetManager.h"

namespace ImVulkan::VulkanShaderModule
{
    VkShaderModule Create(
        VkDevice device, 
        const char* shaderFilePath
    )
    {
		VkShaderModule shaderModule = nullptr;

		std::string shaderCode = AssetManager::GetFileContent(shaderFilePath);

		VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
		VK_ASSERT(
			vkCreateShaderModule(
				device,
				&createInfo,
				nullptr,
				&shaderModule
			),
			"Could not create shader module!"
		);

		return shaderModule;
    }
}

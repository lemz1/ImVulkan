#include "imvkpch.h"
#include "ImVulkan/Platform/Vulkan/Util/VulkanMemory.h"

namespace ImVulkan::VulkanMemory
{
	VkDeviceMemory Create(
		VkPhysicalDevice physicalDevice, 
		VkDevice device, 
		VkMemoryRequirements memoryRequirements,
		VkMemoryPropertyFlags memoryProperties
	)
	{
		VkDeviceMemory memory = nullptr;

		uint32_t memoryIndex = VulkanMemory::FindMemoryType(
			physicalDevice,
			memoryRequirements.memoryTypeBits,
			memoryProperties
		);

		VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = memoryIndex;

		VK_ASSERT(
			vkAllocateMemory(
				device,
				&allocateInfo,
				nullptr,
				&memory
			),
			"Could not allocate memory!"
		);

		return memory;
	}

	uint32_t FindMemoryType(
		VkPhysicalDevice physicalDevice, 
		uint32_t typeFilter, 
		VkMemoryPropertyFlags memoryProperties
	)
	{
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);

		for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) != 0)
			{
				if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties)
				{
					#ifdef VK_DEBUG_INFO
					IMVK_INFO("Using memory heap index: " << deviceMemoryProperties.memoryTypes[i].heapIndex);
					#endif
					return i;
				}
			}
		}

		IMVK_ASSERT(true, "Error when trying to find memory type!");
		return UINT32_MAX;
	}
}

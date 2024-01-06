#include "imvkpch.h"
#include "VulkanContext.h"

#include "ImVulkan/Core/Core.h"

#include "ImVulkan/Platform/Vulkan/VulkanQueueIndex.h"

namespace ImVulkan 
{
    VulkanContext::VulkanContext(
        uint32_t instanceExtensionCount, 
        const char** instanceExtensions, 
        uint32_t deviceExtensionCount, 
        const char** deviceExtensions
    )
    {
        const char* additionalInstanceExtensions[] = {
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME
        };
        uint32_t additionalInstanceExtensionCount = ARRAY_COUNT(additionalInstanceExtensions);

        uint32_t mergedInstanceExtensionCount = additionalInstanceExtensionCount + instanceExtensionCount;
        const char** mergedInstanceExtensions = new const char* [mergedInstanceExtensionCount];

        memcpy(
            mergedInstanceExtensions,
            additionalInstanceExtensions,
            additionalInstanceExtensionCount * sizeof(const char*)
        );

        memcpy(
            mergedInstanceExtensions + additionalInstanceExtensionCount,
            instanceExtensions,
            instanceExtensionCount * sizeof(const char*)
        );

        instance = VulkanInstance::Create(mergedInstanceExtensionCount, mergedInstanceExtensions);

        physicalDevice = VulkanPhysicalDevice::Create(instance);
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        queueFamilyIndex = VulkanQueueIndex::Create(physicalDevice);

        device = VulkanDevice::Create(physicalDevice, queueFamilyIndex, deviceExtensionCount, deviceExtensions);

        vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

        fence = VulkanFence::Create(device);
    }

    void VulkanContext::Destroy()
    {
        vkDestroyFence(device, fence, nullptr);

        vkDestroyDevice(device, nullptr);

        vkDestroyInstance(instance, nullptr);
    }
}

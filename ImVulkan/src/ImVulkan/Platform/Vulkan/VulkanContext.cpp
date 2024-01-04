#include "imvkpch.h"
#include "VulkanContext.h"

#include "ImVulkan/Core/Core.h"

namespace ImVulkan 
{
    VulkanContext::VulkanContext(
        uint32_t instanceExtensionCount,
        const char** instanceExtensions,
        uint32_t deviceExtensionCount,
        const char** deviceExtensions
    )
    {
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

            m_Instance = VulkanInstance(mergedInstanceExtensionCount, mergedInstanceExtensions);
        }

        {
            m_PhysicalDevice = VulkanPhysicalDevice(m_Instance.GetInstance());
        }

        {
            m_Device = VulkanDevice(m_PhysicalDevice.GetPhysicalDevice(), deviceExtensionCount, deviceExtensions);
        }
    }

    void VulkanContext::InitDebugMessenger()
    {
        m_DebugMessenger = VulkanDebugMessenger(m_Instance.GetInstance());
    }

    void VulkanContext::Destroy()
    {
        if (m_DebugMessenger.GetDebugMessenger())
        {
            m_DebugMessenger.Destroy(m_Instance.GetInstance());
        }

        m_Device.Destroy();

        m_Instance.Destroy();
    }
}

#include "imvkpch.h"
#include "VulkanContext.h"

#include "ImVulkan/Core/Core.h"

namespace ImVulkan 
{
    VulkanContext::VulkanContext(uint32_t instanceExtensionCount, const char** instanceExtensions, uint32_t deviceExtensionCount, const char** deviceExtensions)
    {
        m_Instance = VulkanInstance(instanceExtensionCount, instanceExtensions);

        m_PhysicalDevice = VulkanPhysicalDevice(m_Instance.GetInstance());

        m_Device = VulkanDevice(m_PhysicalDevice.GetPhysicalDevice(), deviceExtensionCount, deviceExtensions);
    }

    void VulkanContext::Destroy()
    {
        m_Device.Destroy();

        m_Instance.Destroy();
    }
}

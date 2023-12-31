#pragma once

#include "ImVulkan/Platform/Vulkan/VulkanCore.h"

namespace ImVulkan
{
	class VulkanDebugMessenger
	{
	public:
		VulkanDebugMessenger() = default;
		VulkanDebugMessenger(VkInstance instance);

		VulkanDebugMessenger(const VulkanDebugMessenger& other) noexcept = delete; // for now no copying

		VulkanDebugMessenger& operator=(const VulkanDebugMessenger& other) noexcept = delete;  // for now no copying

		VulkanDebugMessenger(VulkanDebugMessenger&& other) noexcept;

		VulkanDebugMessenger& operator=(VulkanDebugMessenger&& other) noexcept;

		void Destroy(VkInstance instance);

		const VkDebugUtilsMessengerEXT& GetDebugMessenger() const { return m_DebugMessenger; }
	public:
		static PFN_vkCreateDebugUtilsMessengerEXT CreateDebugMessenger;
		static PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugMessenger;
	private:
		static VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT severity,
			VkDebugUtilsMessageTypeFlagsEXT messageTypes,
			const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
			void* userData
		);
	private:
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
	};
}

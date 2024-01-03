#include "imvkpch.h"
#include "GLFWImGuiContext.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"

namespace ImVulkan
{
	GLFWImGuiContext::GLFWImGuiContext(
        GLFWwindow* windowHandle,
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        uint32_t queueFamilyIndex,
        VkQueue queue,
        size_t imageCount,
        VkRenderPass renderPass
    )
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags = ImGuiConfigFlags_DockingEnable;
		ImGui::StyleColorsDark();

        {
            VkDescriptorPoolSize pool_sizes[] =
            {
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
            };
            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 1;
            pool_info.poolSizeCount = ARRAY_COUNT(pool_sizes);
            pool_info.pPoolSizes = pool_sizes;
            VK_ASSERT(vkCreateDescriptorPool(device, &pool_info, nullptr, &m_DescriptorPool), "Could not create descriptor pool!");
        }

        ImGui_ImplGlfw_InitForVulkan(windowHandle, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = instance;
        init_info.PhysicalDevice = physicalDevice;
        init_info.Device = device;
        init_info.QueueFamily = queueFamilyIndex;
        init_info.Queue = queue;
        init_info.PipelineCache = nullptr;
        init_info.DescriptorPool = m_DescriptorPool;
        init_info.Subpass = 0;
        init_info.MinImageCount = 2;
        init_info.ImageCount = imageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = nullptr;
        ImGui_ImplVulkan_Init(&init_info, renderPass);
	}

    void GLFWImGuiContext::Destroy(VkDevice device)
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
    }
}

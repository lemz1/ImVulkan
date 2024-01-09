#include "ImVulkan.h"

#include "ImVulkan/Vulkan/Buffer/VulkanBuffer.h"
#include "ImVulkan/Vulkan/Buffer/VulkanCommandBuffer.h"
#include "ImVulkan/Vulkan/VulkanCommandPool.h"
#include "ImVulkan/Vulkan/VulkanPipeline.h"
#include "ImVulkan/Vulkan/VulkanMemory.h"
#include "ImVulkan/Vulkan/VulkanShaderModule.h"
#include "ImVulkan/Vulkan/VulkanPipelineLayout.h"

namespace ImVulkan
{
	class ExampleLayer : public Layer
	{
	public:
		void OnCreate() override
		{
			IMVK_INFO("Example Layer Create!");
			IMVK_WARN("Example Layer Create!");
			IMVK_ERROR("Example Layer Create!");

			auto& vulkanContext = Application::GetVulkanContext();
			auto window = Application::GetWindow();

			{
				VkShaderModule vertexModule = VulkanShaderModule::Create(vulkanContext.device, "assets/shaders/color.vert.hlsl.spv");
				VkShaderModule fragmentModule = VulkanShaderModule::Create(vulkanContext.device, "assets/shaders/color.frag.hlsl.spv");

				VkPipelineShaderStageCreateInfo shaderStages[2];
				shaderStages[0] = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
				shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
				shaderStages[0].module = vertexModule;
				shaderStages[0].pName = "main";

				shaderStages[1] = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
				shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				shaderStages[1].module = fragmentModule;
				shaderStages[1].pName = "main";

				VkVertexInputBindingDescription bindDescription = {};
				bindDescription.binding = 0;
				bindDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				bindDescription.stride = sizeof(float) * 5;

				VkVertexInputAttributeDescription attributeDescriptions[2] = {};
				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = 0;

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = sizeof(float) * 2;

				m_VulkanPipelineLayout = VulkanPipelineLayout::Create(
					vulkanContext.device,
					0,
					nullptr
				);

				m_VulkanPipeline = VulkanPipeline::Create(
					vulkanContext.device,
					ARRAY_COUNT(shaderStages),
					shaderStages,
					window->GetRenderPass(),
					2,
					attributeDescriptions,
					&bindDescription,
					m_VulkanPipelineLayout
				);

				vkDestroyShaderModule(
					vulkanContext.device,
					fragmentModule,
					nullptr
				);

				vkDestroyShaderModule(
					vulkanContext.device,
					vertexModule,
					nullptr
				);
			}

			{
				float vertexData[] = {
					-0.5f, -.5f,
					0.f, 1.f, 0.f,

					0.5f, -.5f,
					1.f, 1.f, 1.f,

					-.5f, .5f,
					1.f, 0.f, 0.f,

					.5f, .5f,
					0.f, 0.f, 1.f,
				};

				uint32_t indexData[] = {
					0, 1, 2,
					1, 3, 2
				};

				{
					m_VertexBuffer = VulkanBuffer::Create(
						vulkanContext.device,
						sizeof(vertexData),
						VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					);

					VkMemoryRequirements memoryRequirements;
					vkGetBufferMemoryRequirements(
						vulkanContext.device,
						m_VertexBuffer,
						&memoryRequirements
					);

					m_VertexMemory = VulkanMemory::Create(
						vulkanContext.physicalDevice,
						vulkanContext.device,
						memoryRequirements,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					);

					vkBindBufferMemory(
						vulkanContext.device,
						m_VertexBuffer,
						m_VertexMemory,
						0
					);

					VulkanBuffer::MapMemory(
						m_VertexBuffer,
						vulkanContext.device,
						vulkanContext.physicalDevice,
						vulkanContext.queue,
						vulkanContext.queueFamilyIndex,
						vertexData,
						sizeof(vertexData)
					);
					//m_VertexBuffer.UnmapMemory(vulkanContext.device);
				}

				{
					m_IndexBuffer = VulkanBuffer::Create(
						vulkanContext.device,
						sizeof(indexData),
						VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					);

					VkMemoryRequirements memoryRequirements;
					vkGetBufferMemoryRequirements(
						vulkanContext.device,
						m_IndexBuffer,
						&memoryRequirements
					);

					m_IndexMemory = VulkanMemory::Create(
						vulkanContext.physicalDevice,
						vulkanContext.device,
						memoryRequirements,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					);

					vkBindBufferMemory(
						vulkanContext.device,
						m_IndexBuffer,
						m_IndexMemory,
						0
					);

					VulkanBuffer::MapMemory(
						m_IndexBuffer,
						vulkanContext.device,
						vulkanContext.physicalDevice,
						vulkanContext.queue,
						vulkanContext.queueFamilyIndex,
						indexData,
						sizeof(indexData)
					);
					//m_IndexBuffer.UnmapMemory(vulkanContext.device);
				}
			}

			{
				m_CommandPool = VulkanCommandPool::Create(
					vulkanContext.device,
					vulkanContext.queueFamilyIndex
				);

				m_CommandBuffer = VulkanCommandBuffer::Create(
					vulkanContext.device,
					m_CommandPool
				);
			}
		}

		void OnDestroy() override
		{
			auto& vulkanContext = Application::GetVulkanContext();

			vkDestroyBuffer(vulkanContext.device, m_VertexBuffer, nullptr);
			vkFreeMemory(vulkanContext.device, m_VertexMemory, nullptr);
			vkDestroyBuffer(vulkanContext.device, m_IndexBuffer, nullptr);
			vkFreeMemory(vulkanContext.device, m_IndexMemory, nullptr);
			vkDestroyPipeline(vulkanContext.device, m_VulkanPipeline, nullptr);
			vkDestroyPipelineLayout(vulkanContext.device, m_VulkanPipelineLayout, nullptr);
			vkDestroyCommandPool(vulkanContext.device, m_CommandPool, nullptr);
		}

		void OnDraw() override
		{
			auto& vulkanContext = Application::GetVulkanContext();
			auto window = Application::GetWindow();

			VK_ASSERT(
				vkResetCommandPool(
					vulkanContext.device,
					m_CommandPool,
					0
				),
				"Could not reset command pool!"
			);

			{
				VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
				VK_ASSERT(
					vkBeginCommandBuffer(
						m_CommandBuffer,
						&beginInfo
					),
					"Could not begin command buffer!"
				);
			}

			{
				{ // Window Resize
					VkViewport viewport = { 0.f, 0.f, (float)window->GetWidth(), (float)window->GetHeight()};
					VkRect2D scissor = { {0, 0}, {window->GetWidth(), window->GetHeight()} };

					vkCmdSetViewport(
						m_CommandBuffer,
						0,
						1,
						&viewport
					);

					vkCmdSetScissor(
						m_CommandBuffer,
						0,
						1,
						&scissor
					);
				}

				{ // Begin RenderPass
					VkImageMemoryBarrier imageBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
					imageBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
					imageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					imageBarrier.image = window->GetCurrentImage();
					imageBarrier.oldLayout = window->GetCurrentImageLayout();
					imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					imageBarrier.subresourceRange.baseArrayLayer = 0;
					imageBarrier.subresourceRange.baseMipLevel = 0;
					imageBarrier.subresourceRange.layerCount = 1;
					imageBarrier.subresourceRange.levelCount = 1;

					vkCmdPipelineBarrier(
						m_CommandBuffer,
						VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
						VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&imageBarrier
					);

					VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
					beginInfo.renderPass = window->GetRenderPass();
					beginInfo.framebuffer = window->GetCurrentFrameBuffer();
					beginInfo.renderArea = { {0, 0}, {window->GetWidth(), window->GetHeight()} };
					
					vkCmdBeginRenderPass(
						m_CommandBuffer,
						&beginInfo,
						VK_SUBPASS_CONTENTS_INLINE
					);
				}

				{ // Clear 
					VkClearAttachment clearAttachment = {};
					clearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					clearAttachment.clearValue.color = { .1f, .1f, .1f, 1.f };
					clearAttachment.colorAttachment = 0; // Index of the color attachment

					VkClearRect clearRect = {};
					clearRect.rect = { {0, 0}, {window->GetWidth(), window->GetHeight()} };
					clearRect.baseArrayLayer = 0;
					clearRect.layerCount = 1;

					vkCmdClearAttachments(
						m_CommandBuffer,
						1,
						&clearAttachment,
						1,
						&clearRect
					);
				}

				{ // Binding Pipeline
					vkCmdBindPipeline(
						m_CommandBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						m_VulkanPipeline
					);
				}

				{ // Actual Rendering
					VkDeviceSize offset = 0;
					vkCmdBindVertexBuffers(
						m_CommandBuffer,
						0,
						1,
						&m_VertexBuffer,
						&offset
					);

					vkCmdBindIndexBuffer(
						m_CommandBuffer,
						m_IndexBuffer,
						0,
						VK_INDEX_TYPE_UINT32
					);

					vkCmdDrawIndexed(
						m_CommandBuffer,
						6,
						1,
						0,
						0,
						0
					);
				}

				vkCmdEndRenderPass(m_CommandBuffer);
			}
			vkEndCommandBuffer(m_CommandBuffer);

			Application::AddCommandBuffer(m_CommandBuffer);
		}

		void OnDrawImGui() override
		{
			ImGui::ShowDemoWindow();
		}
	private:
		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexMemory;
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexMemory;
		VkPipeline m_VulkanPipeline;
		VkPipelineLayout m_VulkanPipelineLayout;
		VkCommandBuffer m_CommandBuffer;
		VkCommandPool m_CommandPool;
	};

	Application* CreateApplication(
		int argc, 
		char** argv
	)
	{
		ApplicationSpecification spec = {};
		Application* app = Application::Create(spec);
		Application::PushLayer(new ExampleLayer());
		return app;
	}
}

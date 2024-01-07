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

			{
				VkShaderModule vertexModule = VulkanShaderModule::Create(Application::GetVulkanContext().device, "assets/shaders/color.vert.spv");
				VkShaderModule fragmentModule = VulkanShaderModule::Create(Application::GetVulkanContext().device, "assets/shaders/color.frag.spv");

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
					Application::GetVulkanContext().device,
					0,
					nullptr
				);

				m_VulkanPipeline = VulkanPipeline::Create(
					Application::GetVulkanContext().device,
					ARRAY_COUNT(shaderStages),
					shaderStages,
					Application::GetWindow()->GetRenderPass(),
					2,
					attributeDescriptions,
					&bindDescription,
					m_VulkanPipelineLayout
				);

				vkDestroyShaderModule(
					Application::GetVulkanContext().device,
					fragmentModule,
					nullptr
				);

				vkDestroyShaderModule(
					Application::GetVulkanContext().device,
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
						Application::GetVulkanContext().device,
						sizeof(vertexData),
						VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					);

					VkMemoryRequirements memoryRequirements;
					vkGetBufferMemoryRequirements(
						Application::GetVulkanContext().device,
						m_VertexBuffer,
						&memoryRequirements
					);

					m_VertexMemory = VulkanMemory::Create(
						Application::GetVulkanContext().physicalDevice,
						Application::GetVulkanContext().device,
						memoryRequirements,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					);

					vkBindBufferMemory(
						Application::GetVulkanContext().device,
						m_VertexBuffer,
						m_VertexMemory,
						0
					);

					VulkanBuffer::MapMemory(
						m_VertexBuffer,
						Application::GetVulkanContext().device,
						Application::GetVulkanContext().physicalDevice,
						Application::GetVulkanContext().queue,
						Application::GetVulkanContext().queueFamilyIndex,
						vertexData,
						sizeof(vertexData)
					);
					//m_VertexBuffer.UnmapMemory(Application::GetVulkanContext().device);
				}

				{
					m_IndexBuffer = VulkanBuffer::Create(
						Application::GetVulkanContext().device,
						sizeof(indexData),
						VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					);

					VkMemoryRequirements memoryRequirements;
					vkGetBufferMemoryRequirements(
						Application::GetVulkanContext().device,
						m_IndexBuffer,
						&memoryRequirements
					);

					m_IndexMemory = VulkanMemory::Create(
						Application::GetVulkanContext().physicalDevice,
						Application::GetVulkanContext().device,
						memoryRequirements,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
					);

					vkBindBufferMemory(
						Application::GetVulkanContext().device,
						m_IndexBuffer,
						m_IndexMemory,
						0
					);

					VulkanBuffer::MapMemory(
						m_IndexBuffer,
						Application::GetVulkanContext().device,
						Application::GetVulkanContext().physicalDevice,
						Application::GetVulkanContext().queue,
						Application::GetVulkanContext().queueFamilyIndex,
						indexData,
						sizeof(indexData)
					);
					//m_IndexBuffer.UnmapMemory(Application::GetVulkanContext().device);
				}
			}

			{
				m_CommandPool = VulkanCommandPool::Create(
					Application::GetVulkanContext().device,
					Application::GetVulkanContext().queueFamilyIndex
				);

				m_CommandBuffer = VulkanCommandBuffer::Create(
					Application::GetVulkanContext().device,
					m_CommandPool
				);
			}
		}

		void OnDestroy() override
		{
			vkDestroyBuffer(Application::GetVulkanContext().device, m_VertexBuffer, nullptr);
			vkFreeMemory(Application::GetVulkanContext().device, m_VertexMemory, nullptr);
			vkDestroyBuffer(Application::GetVulkanContext().device, m_IndexBuffer, nullptr);
			vkFreeMemory(Application::GetVulkanContext().device, m_IndexMemory, nullptr);
			vkDestroyPipeline(Application::GetVulkanContext().device, m_VulkanPipeline, nullptr);
			vkDestroyPipelineLayout(Application::GetVulkanContext().device, m_VulkanPipelineLayout, nullptr);
			vkDestroyCommandPool(Application::GetVulkanContext().device, m_CommandPool, nullptr);
		}

		void OnDraw() override
		{
			vkDeviceWaitIdle(Application::GetVulkanContext().device);

			VK_ASSERT(
				vkResetCommandPool(
					Application::GetVulkanContext().device,
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
					VkViewport viewport = { 0.f, 0.f, (float)Application::GetWindow()->GetWidth(), (float)Application::GetWindow()->GetHeight()};
					VkRect2D scissor = { {0, 0}, {Application::GetWindow()->GetWidth(), Application::GetWindow()->GetHeight()} };

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
					VkClearValue clearValue = { .1f, .1f, .1f, 1.f };
					VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
					beginInfo.renderPass = Application::GetWindow()->GetRenderPass();
					beginInfo.framebuffer = Application::GetWindow()->GetCurrentFrameBuffer();
					beginInfo.renderArea = { {0, 0}, {Application::GetWindow()->GetWidth(), Application::GetWindow()->GetHeight()} };
					beginInfo.clearValueCount = 1;
					beginInfo.pClearValues = &clearValue;

					vkCmdBeginRenderPass(
						m_CommandBuffer,
						&beginInfo,
						VK_SUBPASS_CONTENTS_INLINE
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

			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_CommandBuffer;
			VkPipelineStageFlags waitMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			submitInfo.pWaitDstStageMask = &waitMask;
			VK_ASSERT(
				vkQueueSubmit(
					Application::GetVulkanContext().queue,
					1,
					&submitInfo,
					Application::GetVulkanContext().fence
				),
				"Could not submit queue"
			);

			vkDeviceWaitIdle(Application::GetVulkanContext().device);
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

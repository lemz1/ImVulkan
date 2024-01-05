#include "ImVulkan.h"

#include "ImVulkan/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "ImVulkan/Platform/Vulkan/Buffer/VulkanCommandBuffer.h"
#include "ImVulkan/Platform/Vulkan/VulkanCommandPool.h"
#include "ImVulkan/Platform/Vulkan/VulkanPipeline.h"

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
				VkShaderModule vertexModule = VulkanPipeline::CreateShaderModule(Application::GetVulkanContext().GetDevice(), "assets/shaders/color.vert.spv");
				VkShaderModule fragmentModule = VulkanPipeline::CreateShaderModule(Application::GetVulkanContext().GetDevice(), "assets/shaders/color.frag.spv");

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

				m_VulkanPipeline = VulkanPipeline(
					Application::GetVulkanContext().GetDevice(),
					ARRAY_COUNT(shaderStages),
					shaderStages,
					Application::GetWindow()->GetRenderPass(),
					attributeDescriptions,
					2,
					&bindDescription,
					0,
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

				m_VertexBuffer = VulkanBuffer(
					Application::GetVulkanContext().GetDevice(),
					Application::GetVulkanContext().GetPhysicalDevice(),
					sizeof(vertexData),
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

				m_VertexBuffer.MapMemory(
					Application::GetVulkanContext().GetDevice(), 
					Application::GetVulkanContext().GetPhysicalDevice(),
					Application::GetVulkanContext().GetQueue(),
					Application::GetVulkanContext().GetQueueFamilyIndex(),
					vertexData, 
					sizeof(vertexData)
				);
				//m_VertexBuffer.UnmapMemory(Application::GetVulkanContext().GetDevice());

				m_IndexBuffer = VulkanBuffer(
					Application::GetVulkanContext().GetDevice(),
					Application::GetVulkanContext().GetPhysicalDevice(),
					sizeof(indexData),
					VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);
				m_IndexBuffer.MapMemory(
					Application::GetVulkanContext().GetDevice(),
					Application::GetVulkanContext().GetPhysicalDevice(),
					Application::GetVulkanContext().GetQueue(),
					Application::GetVulkanContext().GetQueueFamilyIndex(),
					indexData,
					sizeof(indexData)
				);
				//m_IndexBuffer.UnmapMemory(Application::GetVulkanContext().GetDevice());
			}

			{
				m_CommandPool = VulkanCommandPool(
					Application::GetVulkanContext().GetDevice(),
					Application::GetVulkanContext().GetQueueFamilyIndex()
				);

				m_CommandBuffer = VulkanCommandBuffer(
					Application::GetVulkanContext().GetDevice(),
					m_CommandPool.GetCommandPool()
				);
			}
		}

		void OnDestroy() override
		{
			m_VertexBuffer.Destroy(Application::GetVulkanContext().GetDevice());
			m_IndexBuffer.Destroy(Application::GetVulkanContext().GetDevice());
			m_VulkanPipeline.Destroy(Application::GetVulkanContext().GetDevice());
			m_CommandPool.Destroy(Application::GetVulkanContext().GetDevice());
		}

		void OnDraw() override
		{
			vkDeviceWaitIdle(Application::GetVulkanContext().GetDevice());

			m_CommandPool.Reset(Application::GetVulkanContext().GetDevice());

			m_CommandBuffer.BeginCommandBuffer();
			{
				{ // Window Resize
					VkViewport viewport = { 0.f, 0.f, (float)Application::GetWindow()->GetWidth(), (float)Application::GetWindow()->GetHeight()};
					VkRect2D scissor = { {0, 0}, {Application::GetWindow()->GetWidth(), Application::GetWindow()->GetHeight()} };
					m_CommandBuffer.SetViewports(0, 1, &viewport);
					m_CommandBuffer.SetScissors(0, 1, &scissor);
				}

				{ // Begin RenderPass
					VkClearValue clearValue = { .1f, .1f, .1f, 1.f };
					VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
					beginInfo.renderPass = Application::GetWindow()->GetRenderPass();
					beginInfo.framebuffer = Application::GetWindow()->GetCurrentFrameBuffer();
					beginInfo.renderArea = { {0, 0}, {Application::GetWindow()->GetWidth(), Application::GetWindow()->GetHeight()} };
					beginInfo.clearValueCount = 1;
					beginInfo.pClearValues = &clearValue;

					m_CommandBuffer.BeginRenderPass(&beginInfo);
				}

				{ // Binding Pipeline
					m_CommandBuffer.BindPipeline(m_VulkanPipeline.GetVulkanPipeline(), VK_PIPELINE_BIND_POINT_GRAPHICS);
				}

				{ // Actual Rendering
					VkDeviceSize offset = 0;
					m_CommandBuffer.BindVertexBuffers(&m_VertexBuffer.GetBuffer(), 1);
					m_CommandBuffer.BindIndexBuffer(m_IndexBuffer.GetBuffer());

					m_CommandBuffer.DrawIndexed(6);
				}

				m_CommandBuffer.EndRenderPass();
			}
			m_CommandBuffer.EndCommandBuffer();

			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_CommandBuffer.GetCommandBuffer();
			VkPipelineStageFlags waitMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			submitInfo.pWaitDstStageMask = &waitMask;
			VK_ASSERT(
				vkQueueSubmit(
					Application::GetVulkanContext().GetQueue(),
					1,
					&submitInfo,
					Application::GetVulkanContext().GetFence()
				),
				"Could not submit queue"
			);

			vkDeviceWaitIdle(Application::GetVulkanContext().GetDevice());
		}

		void OnDrawImGui() override
		{
			ImGui::ShowDemoWindow();
		}
	private:
		VulkanBuffer m_VertexBuffer;
		VulkanBuffer m_IndexBuffer;
		VulkanPipeline m_VulkanPipeline;
		VulkanCommandBuffer m_CommandBuffer;
		VulkanCommandPool m_CommandPool;
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

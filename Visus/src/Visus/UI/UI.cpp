#include "UI.h"

#include <Visus/Core/Renderer.h>
#include <Visus/Platform/VulkanGraphicsContext.h>
#include <Visus/Core/Macros.h>

#include <vulkan/vulkan.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace Motus3D {

	static struct VulkanUIData {
		VkDescriptorPool pool;
	} vk_data;
	
	namespace Vulkan {
		static void InitUI(void* windowHandle)
		{
			auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
			auto context = VulkanGraphicsContext::GetVulkanContext();
			auto glfw_window = (GLFWwindow*)windowHandle;

			VkDescriptorPoolSize pool_sizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};

			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 1000;
			pool_info.poolSizeCount = std::size(pool_sizes);
			pool_info.pPoolSizes = pool_sizes;

			VK_CHECK_RESULT(vkCreateDescriptorPool(device->GetHandle(), &pool_info, nullptr, &vk_data.pool));

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

			ImGui::StyleColorsDark();

			ImGui_ImplGlfw_InitForVulkan(glfw_window, true);

			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = context->GetInstance();
			init_info.PhysicalDevice = device->GetPhysicalDevice()->GetHandle();
			init_info.Device = device->GetHandle();
			init_info.QueueFamily = device->GetPhysicalDevice()->GetQueueFamilyIndices().graphics;
			init_info.Queue = device->GetGraphicsTransferQueue();
			init_info.DescriptorPool = vk_data.pool;
			init_info.MinImageCount = Renderer::GetConfiguration().framesInFlight;
			init_info.ImageCount = Renderer::GetConfiguration().framesInFlight;
			init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			init_info.ColorAttachmentFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
			init_info.UseDynamicRendering = true;
			ImGui_ImplVulkan_Init(&init_info, VK_NULL_HANDLE);

			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(device->GetDefaultCmdBuffer(), &begin_info);
			ImGui_ImplVulkan_CreateFontsTexture(device->GetDefaultCmdBuffer());
			vkEndCommandBuffer(device->GetDefaultCmdBuffer());

			device->ImmediateExecute(device->GetDefaultCmdBuffer(), QueueFamily::GRAPHICS_TRANSFER);

			ImGui_ImplVulkan_DestroyFontUploadObjects();

		};
		static void ShutdownUI(void* windowHandle) 
		{
			auto device = VulkanGraphicsContext::GetVulkanContext()->GetDevice();
			
			vkDeviceWaitIdle(device->GetHandle());

			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();

			vkDestroyDescriptorPool(device->GetHandle(), vk_data.pool, nullptr);
		}
		static void RenderUI(void* windowHandle) {
			
		};
	}

	namespace D3D12 {
		static void InitUI(void* windowHandle) {
			VISUS_ASSERT(false, "D3D12 is currently not supported!");
		}
		static void ShutdownUI(void* windowHandle) {
			VISUS_ASSERT(false, "D3D12 is currently not supported!");
		}
	}

	void InitUI(void* windowHandle)
	{
		switch (Renderer::GetConfiguration().api)
		{
		case GraphicsAPI::VULKAN:
			Vulkan::InitUI(windowHandle);
			break;
		case GraphicsAPI::D3D12:
			D3D12::InitUI(windowHandle);
			break;
		default:
			VISUS_ASSERT(false, "Invalid renderer configuration");
			break;
		}
	}

	void ShutdownUI(void* windowHandle)
	{
		switch (Renderer::GetConfiguration().api)
		{
		case GraphicsAPI::VULKAN:
			Vulkan::ShutdownUI(windowHandle);
			break;
		case GraphicsAPI::D3D12:
			D3D12::ShutdownUI(windowHandle);
			break;
		default:
			VISUS_ASSERT(false, "Invalid renderer configuration");
			break;
		}
	}

	void RenderUI()
	{
		Renderer::RenderImGui();
	}

	void BeginFrameUI()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EndFrameUI()
	{
		RenderUI();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	ImGuiContext* GetImGuiContext()
	{
		return ImGui::GetCurrentContext();
	}

}
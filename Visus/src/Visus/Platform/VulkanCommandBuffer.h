#include <Visus/Core/CommandBuffer.h>

#include <vulkan/vulkan.h>

namespace Motus3D {

	class VulkanCommandBuffer : public CommandBuffer {
	public:
		VulkanCommandBuffer(CommandBufferType type, uint32_t flags);
		~VulkanCommandBuffer();

		// Getters
		CommandBufferType GetType() override { return m_Type; };
		bool IsTransient() override { return m_IsTransient; }

		// Initializes command pools.
		static void InitPools();

		// Destroys command pools and frees all command buffers allocated from them.
		static void ReleasePools();

		VkCommandBuffer GetHandle();

		// Begins commands recording
		void Begin() override;

		// Ends commads recording
		void End() override;

		// Allocate command buffer.
		void Allocate(CommandBufferType type, uint32_t flags) override;

		// Resets command buffer.
		// DOES NOT lead to command buffer destruction.
		void Reset() override;

		// Frees command buffer memory. 
		// NOTE: After using this method command buffer becomes unusable! Call Allocate() to make it usable again, or call Reset()
		// to clear its commands without freeing memory (which basically destroys command buffer)
		void Release() override;

	private:
		std::vector<VkCommandBuffer> m_CommandBuffers;
		VkCommandBuffer m_CurrectCommandBuffer;

		bool m_IsTransient;
		CommandBufferType m_Type;

		// Main command pool used to allocate non-transient command buffers for graphics-transfer commands.
		static VkCommandPool m_GraphicsCommandPool;

		// Main command pool used to allocate non-transient command buffers for compute commands.
		static VkCommandPool m_ComputeCommandPool;

		// Command pool used to allocate short-living command buffers for graphics-transfer commands, which will be freed or reset soon.
		static VkCommandPool m_GraphicsTransientCommandPool;

		// Command pool used to allocate short-living command buffers for graphics-transfer commands, which will be freed or reset soon.
		static VkCommandPool m_ComputeTransientCommandPool;

	};

}
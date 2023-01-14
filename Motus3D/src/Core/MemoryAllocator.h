#include <Core/Macros.h>

namespace Motus3D {

	enum class AllocationFlags : uint64_t
	{
		AUTO = BIT(0),
		DEDICATED_BLOCK = BIT(1),
		USE_MEMORY_POOL = BIT(2)
	};

	class MOTUS_API MemoryAllocator 
	{
	public:
		static void Init();
		static void Shutdown();

		void ExtentPool(uint64_t size); // in bytes
		void* Allocate(uint64_t size, AllocationFlags flags);

	private:
		MemoryAllocator();
		~MemoryAllocator();

	private:
		static MemoryAllocator* s_Instance;
		void* m_MemoryPool;
		uint64_t m_AvailableMemory; // in bytes
		uint64_t m_AllocatedMemory; // in bytes
	};

}
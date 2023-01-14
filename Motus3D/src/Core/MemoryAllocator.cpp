#include "MemoryAllocator.h"

namespace Motus3D {

	MemoryAllocator::MemoryAllocator()
	{
		s_Instance = this;
		m_AvailableMemory = 512;
		m_AllocatedMemory = 0;
		m_MemoryPool = malloc(m_AvailableMemory);
	}

	MemoryAllocator::~MemoryAllocator()
	{
		free(m_MemoryPool);
	}

	MemoryAllocator* MemoryAllocator::s_Instance = nullptr;

	void MemoryAllocator::Init()
	{
		s_Instance = new MemoryAllocator();
	}

	void MemoryAllocator::Shutdown()
	{
		delete s_Instance;
	}

	void MemoryAllocator::ExtentPool(uint64_t size)
	{
		void* new_pool = malloc(m_AvailableMemory + size);
		memcpy(new_pool, m_MemoryPool, m_AvailableMemory);
		free(m_MemoryPool);
		m_MemoryPool = new_pool;
		m_AvailableMemory += size;
	}

	void* MemoryAllocator::Allocate(uint64_t size, AllocationFlags flags)
	{
		return malloc(size);
	}

}
#pragma once
#include <string>
#include <vector>

#include "Shader.h"

namespace Motus3D {
	constexpr static uint32_t ShaderDataSize(ShaderDataType format)
	{
		switch (format)
		{
		case ShaderDataType::INT:			return 4 * 1;
		case ShaderDataType::INT2:			return 4 * 2;
		case ShaderDataType::INT3:			return 4 * 3;
		case ShaderDataType::INT4:			return 4 * 4;
		case ShaderDataType::FLOAT:			return 4 * 1;
		case ShaderDataType::FLOAT2:		return 4 * 2;
		case ShaderDataType::FLOAT3:		return 4 * 3;
		case ShaderDataType::FLOAT4:		return 4 * 4;
		case ShaderDataType::MAT3:			return 4 * 3 * 3;
		case ShaderDataType::MAT4:			return 4 * 4 * 4;
		case ShaderDataType::BOOL:			return 1;
				
			// TODO: other data types
		}
	}

	struct VertexBufferLayoutElement
	{
		std::string name;
		ShaderDataType format;
		uint32_t size;
		uint32_t offset;

		VertexBufferLayoutElement(std::string Name, ShaderDataType Format)
			: name(Name), format(Format) {}

	};

	// The VBO layout class
	class VertexBufferLayout
	{
	public:
		VertexBufferLayout(const std::initializer_list<VertexBufferLayoutElement> list) : m_Elements(list)
		{
			for (auto& element : m_Elements)
			{
				uint32_t datasize = ShaderDataSize(element.format);
				element.offset = m_Stride;
				element.size = datasize;
				m_Stride += datasize;

			}
		};

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<VertexBufferLayoutElement>& GetElements() const { return m_Elements; }
		const std::vector<VertexBufferLayoutElement>::iterator& begin() { return m_Elements.begin(); }
		const std::vector<VertexBufferLayoutElement>::iterator& end() { return m_Elements.end(); }

	private:

		std::vector<VertexBufferLayoutElement> m_Elements;
		uint32_t m_Stride = 0;
	};
}
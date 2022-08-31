#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/VertexBuffer.h>
#include <Visus/Core/IndexBuffer.h>
#include <Visus/Core/Image.h>

#include <glm/glm.hpp>


namespace Motus3D {

	struct Vertex 
	{
		glm::vec3 position;
		glm::vec2 textureCoords;
		glm::vec3 normal;
	};

	// Represent a single submesh of a model. Stores VBO and IBO.
	// TODO: implement diffuse and specular mapping
	class VISUS_API Submesh 
	{
	public:
		Submesh(
			std::vector<Vertex> vertices,
			std::vector<uint8_t> indices
			//std::vector<Ref<Image>> textures
		);
		Submesh(
			std::vector<Vertex> vertices,
			std::vector<uint16_t> indices
			//std::vector<Ref<Image>> textures
		);
		Submesh(
			std::vector<Vertex> vertices,
			std::vector<uint32_t> indices
			//std::vector<Ref<Image>> textures
		);

		Ref<VertexBuffer> GetVertexBuffer() { return m_VBO; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IBO; }

	private:
		Ref<VertexBuffer> m_VBO;
		Ref<IndexBuffer> m_IBO;
		//std::vector<Ref<Image>> m_Textures;
	};


	// Represents a complete model with list of submeshes and textures.
	class VISUS_API Model
	{
	public:
		Model() {};
		Model(std::string_view filepath);
		void Load(std::string_view filepath);

		std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }

	private:
		// HACK: using void* to not to #include assimp in header file
		// TODO: find better solution for abstraction
		void ProcessNode(void* node, const void* scene);
		Submesh ProcessSubmesh(void* submesh, const void* scene);

	private:
		std::vector<Submesh> m_Submeshes;
		Ref<Image> m_Texture;
	};
}
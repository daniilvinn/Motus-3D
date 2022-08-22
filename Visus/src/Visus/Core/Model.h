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

	private:
		Ref<VertexBuffer> m_VBO;
		Ref<IndexBuffer> m_IBO;
		//std::vector<Ref<Image>> m_Textures;
	};


	// Represents a complete model with list of submeshes and textures.
	class VISUS_API Model
	{
	public:
		Model(std::string filepath);

	private:
		std::vector<Submesh> m_Submeshes;
		Ref<Image> m_Texture;

	};

}
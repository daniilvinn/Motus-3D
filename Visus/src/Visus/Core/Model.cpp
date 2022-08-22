#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Motus3D {

	// ============================
	// Submesh class implementation
	// ============================
	Submesh::Submesh(std::vector<Vertex> vertices, std::vector<uint8_t> indices /*std::vector<Ref<Image>> textures */)
	{
		m_VBO = VertexBuffer::Create(vertices.data(), vertices.size() * 4, 0);
		m_IBO = IndexBuffer::Create(indices.data(), indices.size(), 0, IndexType::UINT8);
	}

	Submesh::Submesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices /*std::vector<Ref<Image>> textures */)
	{
		m_VBO = VertexBuffer::Create(vertices.data(), vertices.size() * 4, 0);
		m_IBO = IndexBuffer::Create(indices.data(), indices.size() * 2, 0, IndexType::UINT16);
	}

	Submesh::Submesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices /*std::vector<Ref<Image>> textures */)
	{
		m_VBO = VertexBuffer::Create(vertices.data(), vertices.size() * 4, 0);
		m_IBO = IndexBuffer::Create(indices.data(), indices.size() * 4, 0, IndexType::UINT32);
	}
	
	// ==========================
	// Model class implementation
	// ==========================

	Model::Model(std::string filepath) 
	{
		Assimp::Importer model_importer;
		const aiScene* scene = model_importer.ReadFile(filepath, 0);
	}

}
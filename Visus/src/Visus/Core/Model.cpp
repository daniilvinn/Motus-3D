#include "Model.h"

#include <Visus/Core/Logger.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Motus3D {

	// ============================
	// Submesh class implementation
	// ============================
	Submesh::Submesh(std::vector<Vertex> vertices, std::vector<uint8_t> indices /*std::vector<Ref<Image>> textures */)
	{
		m_VBO = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Vertex), 0);
		m_IBO = IndexBuffer::Create(indices.data(), indices.size(), 0, IndexType::UINT8);
	}

	Submesh::Submesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices /*std::vector<Ref<Image>> textures */)
	{
		m_VBO = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Vertex), 0);
		m_IBO = IndexBuffer::Create(indices.data(), indices.size() * 2, 0, IndexType::UINT16);
	}

	Submesh::Submesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices /*std::vector<Ref<Image>> textures */)
	{
		m_VBO = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Vertex), 0);
		m_IBO = IndexBuffer::Create(indices.data(), indices.size() * 4, 0, IndexType::UINT32);
	}
	
	// ==========================
	// Model class implementation
	// ==========================

	Model::Model(std::string_view filepath) 
	{
		Load(filepath);
	}

	Model::~Model()
	{
		for (auto& submesh : m_Submeshes) 
		{
			submesh.GetVertexBuffer().reset();
			submesh.GetIndexBuffer().reset();
		}

	}

	void Model::Load(std::string_view filepath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath.data(),
			aiProcess_JoinIdenticalVertices |
			aiProcess_FlipUVs |
			aiProcess_Triangulate |
			aiProcess_GenNormals
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			VISUS_ERROR("Assimp: {0}", importer.GetErrorString());
		}

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::Release()
	{
		for (auto& submesh : m_Submeshes)
		{
			submesh.GetVertexBuffer()->Release();
			submesh.GetIndexBuffer()->Release();
		}
	}

	void Model::ProcessNode(void* node, const void* scene)
	{
		aiNode* ai_node = static_cast<aiNode*>(node);
		const aiScene* ai_scene = static_cast<const aiScene*>(scene);

		for (int i = 0; i < ai_node->mNumMeshes; i++) 
		{
			aiMesh* ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
			m_Submeshes.push_back(ProcessSubmesh(ai_mesh, scene));
		}

		for (int i = 0; i < ai_node->mNumChildren; i++) 
		{
			ProcessNode(ai_node->mChildren[i], scene);
		}
	}

	Submesh Model::ProcessSubmesh(void* submesh, const void* scene)
	{
		aiMesh* ai_mesh = static_cast<aiMesh*>(submesh);
		const aiScene* ai_scenedasdwa = static_cast<const aiScene*>(scene);
		std::vector<Vertex> vertices;

		glm::vec3 buffer;
		Vertex vertex;
		
		if (ai_mesh->mNumVertices < 255) // Process small size mesh
		{
			std::vector<uint8_t> indices;

			for (int i = 0; i < ai_mesh->mNumVertices; i++) 
			{
				buffer.x = ai_mesh->mVertices[i].x;
				buffer.y = ai_mesh->mVertices[i].y;
				buffer.z = ai_mesh->mVertices[i].z;

				vertex.position = buffer;

				// Process normals, if there's any.
				if (ai_mesh->HasNormals()) {
					buffer.x = ai_mesh->mNormals[i].x;
					buffer.y = ai_mesh->mNormals[i].y;
					buffer.z = ai_mesh->mNormals[i].z;
				} else {
					buffer = glm::vec3(0.0f, 0.0f, 0.0f);
				}

				vertex.normal = buffer;

				// Process texture coords, if there's any.
				if (ai_mesh->mTextureCoords[0]) 
				{
					glm::vec2 coords;
					coords.x = ai_mesh->mTextureCoords[0][i].x;
					coords.y = ai_mesh->mTextureCoords[0][i].y;

					vertex.textureCoords = coords;
				}
				else 
				{
					vertex.textureCoords = glm::vec2(0.0f, 0.0f);
				}

				vertices.push_back(vertex);

				

			}

			for (int i = 0; i < ai_mesh->mNumFaces; i++)
			{
				aiFace face = ai_mesh->mFaces[i];
				for (int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			return Submesh(vertices, indices);

		}
		else if (ai_mesh->mNumVertices < 65536) // Process medium size mesh
		{
			std::vector<uint16_t> indices;

			for (int i = 0; i < ai_mesh->mNumVertices; i++)
			{
				buffer.x = ai_mesh->mVertices[i].x;
				buffer.y = ai_mesh->mVertices[i].y;
				buffer.z = ai_mesh->mVertices[i].z;

				vertex.position = buffer;

				// Process normals, if there's any.
				if (ai_mesh->HasNormals()) {
					buffer.x = ai_mesh->mNormals[i].x;
					buffer.y = ai_mesh->mNormals[i].y;
					buffer.z = ai_mesh->mNormals[i].z;
				}
				else {
					buffer = glm::vec3(0.0f, 0.0f, 0.0f);
				}

				vertex.normal = buffer;

				// Process texture coords, if there's any.
				if (ai_mesh->mTextureCoords[0])
				{
					glm::vec2 coords;
					coords.x = ai_mesh->mTextureCoords[0][i].x;
					coords.y = ai_mesh->mTextureCoords[0][i].y;

					vertex.textureCoords = coords;
				}
				else
				{
					vertex.textureCoords = glm::vec2(0.0f, 0.0f);
				}

				vertices.push_back(vertex);



			}

			for (int i = 0; i < ai_mesh->mNumFaces; i++)
			{
				aiFace face = ai_mesh->mFaces[i];
				for (int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			return Submesh(vertices, indices);
		}
		else // Process large size mesh
		{
			std::vector<uint32_t> indices;

			for (int i = 0; i < ai_mesh->mNumVertices; i++)
			{
				buffer.x = ai_mesh->mVertices[i].x;
				buffer.y = ai_mesh->mVertices[i].y;
				buffer.z = ai_mesh->mVertices[i].z;

				vertex.position = buffer;

				// Process normals, if there's any.
				if (ai_mesh->HasNormals()) {
					buffer.x = ai_mesh->mNormals[i].x;
					buffer.y = ai_mesh->mNormals[i].y;
					buffer.z = ai_mesh->mNormals[i].z;
				}
				else {
					buffer = glm::vec3(0.0f, 0.0f, 0.0f);
				}

				vertex.normal = buffer;

				// Process texture coords, if there's any.
				if (ai_mesh->mTextureCoords[0])
				{
					glm::vec2 coords;
					coords.x = ai_mesh->mTextureCoords[0][i].x;
					coords.y = ai_mesh->mTextureCoords[0][i].y;

					vertex.textureCoords = coords;
				}
				else
				{
					vertex.textureCoords = glm::vec2(0.0f, 0.0f);
				}

				vertices.push_back(vertex);



			}

			for (int i = 0; i < ai_mesh->mNumFaces; i++)
			{
				aiFace face = ai_mesh->mFaces[i];
				for (int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			return Submesh(vertices, indices);
		}

	}
}
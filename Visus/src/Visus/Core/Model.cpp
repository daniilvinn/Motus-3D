#include "Model.h"

#include <Visus/Core/Logger.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Motus3D {

	// ============================
	// Submesh class implementation
	// ============================
	Submesh::Submesh(std::vector<float> vertices, std::vector<uint8_t> indices, Ref<Image> texture)
		: m_Texture(texture)
	{
		Load(vertices, indices, texture);
	}

	Submesh::Submesh(std::vector<float> vertices, std::vector<uint16_t> indices, Ref<Image> texture)
		: m_Texture(texture)
	{
		Load(vertices, indices, texture);
	}

	Submesh::Submesh(std::vector<float> vertices, std::vector<uint32_t> indices, Ref<Image> texture)
		: m_Texture(texture)
	{
		Load(vertices, indices, texture);
	}
	
	void Submesh::Load(std::vector<float> vertices, std::vector<uint8_t> indices, Ref<Image> texture)
	{
		m_VBO = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float), 0);
		m_IBO = IndexBuffer::Create(indices.data(), indices.size(), 0, IndexType::UINT8);
	}

	void Submesh::Load(std::vector<float> vertices, std::vector<uint16_t> indices, Ref<Image> texture)
	{
		m_VBO = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float), 0);
		m_IBO = IndexBuffer::Create(indices.data(), indices.size() * 2, 0, IndexType::UINT16);
	}

	void Submesh::Load(std::vector<float> vertices, std::vector<uint32_t> indices, Ref<Image> texture)
	{
		m_VBO = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float), 0);
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

	b8 Model::Load(std::string_view filepath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath.data(),
			aiProcess_JoinIdenticalVertices |
			aiProcess_FlipUVs |
			aiProcess_Triangulate | 
			aiProcess_GenNormals |
			aiProcess_RemoveRedundantMaterials
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			VISUS_ERROR("Assimp: {0}", importer.GetErrorString());
			return MT_FALSE;
		}

		for (int i = 0; i < scene->mNumMaterials; i++) {
			aiMaterial* mat = scene->mMaterials[i];
			aiString texture_filepath;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &texture_filepath);

			if (texture_filepath.length == 0);
			else {
				// Calculate full texture filepath
				std::string model_texture_filepath(filepath);

				auto last_slash_pos = model_texture_filepath.substr(model_texture_filepath.find_last_of('/'));
				model_texture_filepath.erase(model_texture_filepath.find(last_slash_pos));
				
				model_texture_filepath += "/";
				model_texture_filepath.append(texture_filepath.C_Str());

				bool texture_already_exists = false;
				for (int j = 0; j < m_Textures.size(); j++)
				{
					std::string_view test = m_Textures[j]->GetFilepath();
					if (m_Textures[j]->GetFilepath() == model_texture_filepath) texture_already_exists = true;
				}

				if (texture_already_exists);
				else m_Textures.push_back(Image::Create(ImageUsage::SHADER_READ_ONLY, model_texture_filepath));
			}
		}

		ProcessNode(scene->mRootNode, scene);

		return MT_TRUE;
	}

	void Model::Release()
	{
		for (auto& submesh : m_Submeshes)
		{
			submesh.GetVertexBuffer()->Release();
			submesh.GetIndexBuffer()->Release();
		}
		for (auto& image : m_Textures) {
			image->Release();
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
		const aiScene* ai_scene = static_cast<const aiScene*>(scene);
		std::vector<float> vertices;
		vertices.reserve(ai_mesh->mNumVertices * 8);

		// Calculating which texture this submesh uses.
		unsigned int texture_index = 0;
		aiMaterial* mesh_texture_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
		aiString texture_filepath;
		mesh_texture_material->GetTexture(aiTextureType_DIFFUSE, 0, &texture_filepath);

		if (texture_filepath.length == 0);
		else 
		{
			std::string std_texture_filepath(texture_filepath.C_Str()); // convert aiString to std::string, since STL's one has methods we need.
			for (int i = 0; i < m_Textures.size(); i++) 
			{
				if (m_Textures[i]->GetFilepath().find(std_texture_filepath) != std::string::npos) 
				{
					texture_index = i;
					break;
				}
			}
		}

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

				vertices.push_back(vertex.position.x);
				vertices.push_back(vertex.position.y);
				vertices.push_back(vertex.position.z);
				vertices.push_back(vertex.textureCoords.x);
				vertices.push_back(vertex.textureCoords.y);
				vertices.push_back(vertex.normal.x);
				vertices.push_back(vertex.normal.y);
				vertices.push_back(vertex.normal.z);
			}

			for (int i = 0; i < ai_mesh->mNumFaces; i++)
			{
				aiFace face = ai_mesh->mFaces[i];
				for (int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			return Submesh(vertices, indices, m_Textures[texture_index]);

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

				vertices.push_back(vertex.position.x);
				vertices.push_back(vertex.position.y);
				vertices.push_back(vertex.position.z);
				vertices.push_back(vertex.textureCoords.x);
				vertices.push_back(vertex.textureCoords.y);
				vertices.push_back(vertex.normal.x);
				vertices.push_back(vertex.normal.y);
				vertices.push_back(vertex.normal.z);

			}

			for (int i = 0; i < ai_mesh->mNumFaces; i++)
			{
				aiFace face = ai_mesh->mFaces[i];
				for (int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			return Submesh(vertices, indices, m_Textures[texture_index]);
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

				vertices.push_back(vertex.position.x);
				vertices.push_back(vertex.position.y);
				vertices.push_back(vertex.position.z);
				vertices.push_back(vertex.textureCoords.x);
				vertices.push_back(vertex.textureCoords.y);
				vertices.push_back(vertex.normal.x);
				vertices.push_back(vertex.normal.y);
				vertices.push_back(vertex.normal.z);

			}

			for (int i = 0; i < ai_mesh->mNumFaces; i++)
			{
				aiFace face = ai_mesh->mFaces[i];
				for (int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			return Submesh(vertices, indices, m_Textures[texture_index]);
		}

	}
}
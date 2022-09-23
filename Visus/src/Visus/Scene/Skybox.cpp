#include "Skybox.h"

namespace Motus3D {

	Skybox::Skybox(std::string_view filepath, Ref<Sampler> sampler)
	{
		Load(filepath, sampler);
	}

	Skybox::~Skybox()
	{

	}

	void Skybox::Load(std::string_view filepath, Ref<Sampler> sampler)
	{
		m_Cubemap = Cubemap::Create(filepath.data());

		Vertex vertex = { {}, {}, {} };
		std::vector<float> vertices = {
			// Positions				// Tex. Coord.	// Normals (do we need them?)
			// +X
			1.0f,   1.0f, -1.0f,		
			1.0f,   1.0f,  1.0f,		
			1.0f,  -1.0f, -1.0f,		
			1.0f,  -1.0f,  1.0f,		
			   
			 // -X   
			-1.0f,   1.0f, -1.0f,		
			-1.0f,   1.0f,  1.0f,		
			-1.0f,  -1.0f, -1.0f,		
			-1.0f,  -1.0f,  1.0f,		
			   
			 // +Y   
			-1.0f,  1.0f,  -1.0f,		
			 1.0f,  1.0f,  -1.0f,		
			-1.0f,  1.0f,   1.0f,		
			 1.0f,  1.0f,   1.0f,		
			   
			 //-Y   
			-1.0f,  -1.0f,  -1.0f,		
			 1.0f,  -1.0f,  -1.0f,		
			-1.0f,  -1.0f,   1.0f,		
			 1.0f,  -1.0f,   1.0f,		

			// +Z
			-1.0f,   1.0f,   1.0f,		
			 1.0f,   1.0f,   1.0f,		
			-1.0f,  -1.0f,   1.0f,		
			 1.0f,  -1.0f,   1.0f,		

			// -Z
			-1.0f,   1.0f,  -1.0f,		
			 1.0f,   1.0f,  -1.0f,		
			-1.0f,  -1.0f,  -1.0f,		
			 1.0f,  -1.0f,  -1.0f,		
		};

		std::vector<uint8_t> indices = {
			// +X
			0, 1, 2,
			2, 1, 3,

			// -X
			4, 5, 6,
			6, 5, 7,

			// +Y
			8, 9, 10,
			10, 9, 11,

			// -Y
			12, 13, 14,
			14, 13, 15,

			// +Z
			16, 17, 18,
			18, 17, 19,

			// -Z
			20, 21, 22,
			22, 21, 23
		};

		m_CubeMesh.Load(vertices, indices, RefAs<Image>(m_Cubemap));

		m_DescriptorSet = DescriptorSet::Create({
			{
				0,
				ResourceType::IMAGE,
				ShaderStage::FRAGMENT,
				1
			}
		});
		m_DescriptorSet->UpdateDescriptor(0, m_Cubemap, sampler);

	}

	void Skybox::Release()
	{
		m_Cubemap->Release();
		m_CubeMesh.GetVertexBuffer()->Release();
		m_CubeMesh.GetIndexBuffer()->Release();
		m_DescriptorSet->Release();
	}

	void Skybox::LoadFront(std::string_view filepath)
	{

	}

	void Skybox::LoadBack(std::string_view filepath)
	{

	}

	void Skybox::LoadLeft(std::string_view filepath)
	{

	}

	void Skybox::LoadRight(std::string_view filepath)
	{

	}

	void Skybox::LoadUp(std::string_view filepath)
	{

	}

	void Skybox::LoadDown(std::string_view filepath)
	{

	}

}
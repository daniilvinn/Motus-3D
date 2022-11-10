#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/Model.h>
#include <Visus/Core/Cubemap.h>

namespace Motus3D {

	/*
	* @brief represents a skybox with a static mesh with a texture coordinates and cubemap.
	*/
	class VISUS_API Skybox {
	public:
		Skybox() {};
		Skybox(std::string_view filepath, Ref<Sampler> sampler);
		~Skybox();

		Submesh* GetSubmesh() { return &m_CubeMesh; };
		Ref<Cubemap> GetTexture() { return m_Cubemap; }
		Ref<DescriptorSet> GetDescriptorSet() { return m_DescriptorSet; }

		// HACK: Directory to cubemap textures, which MUST contain 6 .jpg files with following names: posx, negx, posy, negy, posz, negz
		// Each file corresponds to each cubemap face, for example: posx will be uploaded as right (+X) face of cubemap, and so on.
		void Load(std::string_view filepath, Ref<Sampler> sampler);

		// Frees skybox and its memory
		void Release();

		/*
		* Methods to upload skybox textures.
		* Loads texture for each face of cube separately.
		*/
		void LoadFront(std::string_view filepath);
		void LoadBack(std::string_view filepath);
		void LoadLeft(std::string_view filepath);
		void LoadRight(std::string_view filepath);
		void LoadUp(std::string_view filepath);
		void LoadDown(std::string_view filepath);

	private:
		Submesh m_CubeMesh;
		Ref<Cubemap> m_Cubemap;
		Ref<DescriptorSet> m_DescriptorSet;
	};

}
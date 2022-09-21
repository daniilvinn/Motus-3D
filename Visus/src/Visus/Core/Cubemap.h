#pragma once

#include <Visus/Core/Macros.h>

#include <Visus/Core/Image.h>

namespace Motus3D {

	/*!
	* @brief Cubemap interface. To access its functionality, should be downcasted to its children (for example, to VulkanCubemap)
	*/
	class VISUS_API Cubemap : public Image
	{
	public:
		/*!
		* @return Returns reference to an Cubemap object allocated on heap.
		* @param filepath: Relative path to cubemap textures. MUST have such texture files' names: posx, posy, posz, negx, negy, negz
		*/
		static Ref<Cubemap> Create(std::string directory);
		virtual std::string_view GetFilepath() const = 0;

		virtual void Load(std::string_view filepath) = 0;

	};

}
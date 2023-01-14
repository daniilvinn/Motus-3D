#pragma once

#include <Visus/Core/Macros.h>
#include <Visus/Core/Image.h>
#include <Visus/Core/DescriptorSet.h>
#include <map>

namespace Motus3D {

	enum class MaterialProperty : uint8_t 
	{
		ALBEDO,
		NORMAL,
		ROUGHESS,
		METALLIC,
		AO,
		EMISSIVE,
		PARALLAX
	};

	/*
	* @brief represents surface material. Has metallic, roughness values. Also contains texture or albedo.
	*/
	class VISUS_API Material {
	public:
		
		bool IsPBRCompatible() const { return m_Material.size() > 1; }
		bool HasProperty(MaterialProperty property) const { m_Material[(int)property]; }
		Ref<DescriptorSet> GetDescriptorSet() { return m_DescriptorSet; }

	private:
		std::vector<Ref<Image>> m_Material; // albedo, roughess, metallic etc.
		Ref<DescriptorSet> m_DescriptorSet;

	};

}
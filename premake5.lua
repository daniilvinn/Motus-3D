workspace "Motus3D"
	architecture "x64"
	startproject "Sandbox2D"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

compileOutput = "%{cfg.system}-%{cfg.buildcfg}"
VulkanSDK = os.getenv("VULKAN_SDK")

IncludeDirectory = {}
IncludeDirectory["spdlog"] = "Motus3D/thirdparty/spdlog/include"
IncludeDirectory["GLFW"] = "Motus3D/thirdparty/GLFW/include"
IncludeDirectory["VulkanSDK"] = "%{VulkanSDK}/Include"
IncludeDirectory["glm"] = "Motus3D/thirdparty/glm/include"
IncludeDirectory["Visus"] = "Visus/src"
IncludeDirectory["VMA"] = "Motus3D/thirdparty/VulkanMemoryAllocator/include"
IncludeDirectory["VulkanSDK_Sources"] = "%{VulkanSDK}/Source"
IncludeDirectory["stb_image"] = "Motus3D/thirdparty/stb_image"
IncludeDirectory["Assimp"] = "Motus3D/thirdparty/Assimp/include"

LibraryDirectory = {}
LibraryDirectory["VulkanSDK"] = "%{VulkanSDK}/Lib"

group "Dependencies"
include "Motus3D/thirdparty/GLFW"
include "Motus3D/thirdparty/Assimp"
group ""

group "Core"
project "Visus"
	location "Visus"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"

	targetdir("bin/%{prj.name}/" .. compileOutput)
	objdir("obj/%{prj.name}/" .. compileOutput)

	flags {
		"MultiProcessorCompile"
	}

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{IncludeDirectory.VulkanSDK_Sources}/**.c",
		"%{IncludeDirectory.VulkanSDK_Sources}/**.cpp"
	}

	includedirs {
		"%{prj.name}/src/",
		"Motus3D/src/",
		"%{IncludeDirectory.VulkanSDK}",
		"%{IncludeDirectory.GLFW}",
		"%{IncludeDirectory.glm}",
		"%{IncludeDirectory.spdlog}",
		"%{IncludeDirectory.VMA}",
		"%{IncludeDirectory.VulkanSDK_Sources}",
		"%{IncludeDirectory.stb_image}",
		"%{IncludeDirectory.Assimp}",
	}

	libdirs {
		"%{LibraryDirectory.VulkanSDK}"
	}

	links {
		"GLFW",
		"vulkan-1.lib",
		"assimp"
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"VISUS_API"
	}

	postbuildcommands 
	{
		("{COPY} %{cfg.buildtarget.relpath} ../bin/Sandbox2D/" .. compileOutput)
	}

	filter "system:windows"
		staticruntime "off"
		systemversion "latest"

	filter "configurations:Debug"
		defines "VISUS_DEBUG"
		symbols "On"
		runtime "Debug"
		links 
		{
			"shaderc_sharedd.lib",
			"shaderc_utild.lib",
			"spirv-cross-cored.lib",
			"spirv-cross-glsld.lib",
			"SPIRV-Toolsd.lib"
		}

	filter "configurations:Release"
		defines "VISUS_RELEASE"
		optimize "On"
		runtime "Release"
		links 
		{
			"shaderc_shared.lib",
			"shaderc_util.lib",
			"spirv-cross-core.lib",
			"spirv-cross-glsl.lib"
		}

	filter "configurations:Dist"
		defines "VISUS_DIST"
		optimize "On"
		runtime "Release"
		links 
		{
			"shaderc_shared.lib",
			"shaderc_util.lib",
			"spirv-cross-core.lib",
			"spirv-cross-glsl.lib"
		}

project "Motus3D"
	location "Motus3D"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"

	targetdir("bin/%{prj.name}/" .. compileOutput)
	objdir("obj/%{prj.name}/" .. compileOutput)

	flags {
		"MultiProcessorCompile"
	}

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
		"%{prj.name}/src",
		"%{IncludeDirectory.spdlog}",
		"%{IncludeDirectory.GLFW}",
		"%{IncludeDirectory.VulkanSDK}",
		"%{IncludeDirectory.glm}",
		"%{IncludeDirectory.Visus}"
	}

	libdirs
	{
		"%{LibraryDirectory.VulkanSDK}"
	}

	links 
	{
		"GLFW",
		"Visus",
		"vulkan-1.lib"
	}

	defines 
	{
		"MT_DYNAMIC",
		"MT_BUILD_DLL",
		"_CRT_SECURE_NO_WARNINGS"
	}

	postbuildcommands 
	{
		("{COPY} %{cfg.buildtarget.relpath} ../bin/Sandbox2D/" .. compileOutput)
	}

	filter "system:windows"
		staticruntime "off"
		systemversion "latest"
		
		defines
		{
			"MT_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "MT_DEBUG"
		symbols "On"
		runtime "Debug"

	filter "configurations:Release"
		defines "MT_RELEASE"
		optimize "On"
		runtime "Release"

	filter "configurations:Dist"
		defines "MT_DIST"
		optimize "On"
		runtime "Release"
group ""

group "Test"
project "Sandbox2D"
	location "Sandbox2D"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	targetdir("bin/%{prj.name}/" .. compileOutput)
	objdir("obj/%{prj.name}/" .. compileOutput)
	
	flags {
		"MultiProcessorCompile"
	}

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
		"Motus3D/src",
		"Visus/src",
		"%{IncludeDirectory.spdlog}",
		"%{IncludeDirectory.glm}"
	}

	links 
	{
		"Motus3D",
		"Visus"
	}

	defines 
	{
		"MT_DYNAMIC"
	}

	filter "system:windows"
		staticruntime "off"
		systemversion "latest"

		defines
		{
			"MT_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "MT_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "MT_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "MT_DIST"
		optimize "On"
		kind "WindowedApp"
group ""
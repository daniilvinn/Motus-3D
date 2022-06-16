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
IncludeDirectory["GLFW"] = "Motus3D/thirdparty/GLFW/include"
IncludeDirectory["VulkanSDK"] = "%{VulkanSDK}/Include"

LibraryDirectory = {}
LibraryDirectory["VulkanSDK"] = "%{VulkanSDK}/Lib"

group "Core"

include "Motus3D/thirdparty/GLFW"

project "Motus3D"
	location "Motus3D"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"

	targetdir("bin/%{prj.name}/" .. compileOutput)
	objdir("obj/%{prj.name}/" .. compileOutput)

	-- pchheader "motus_pch.h"
	-- pchsource "Motus/src/motus_pch.cpp"

	flags {
		"MultiProcessorCompile"
	}

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs 
	{
		"%{prj.name}/src",
		"%{IncludeDirectory.GLFW}",
		"%{IncludeDirectory.VulkanSDK}"
	}

	libdirs
	{
		"%{LibraryDirectory.VulkanSDK}"
	}

	links 
	{
		"GLFW",
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
			"MT_PLATFORM_WINDOWS",
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
		"Motus3D/src"
	}

	links 
	{
		"Motus3D"
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
group ""
# Vulkan SDK path
set(Vulkan_SDK $ENV{VULKAN_SDK})

#
#   Motus3D dependencies' include directory
#
#0 vvv SPDLOG vvv
set(MOTUS_SPDLOG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Motus3D/thirdparty/spdlog/include")

#1 GLFW
set(MOTUS_GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Motus3D/thirdparty/GLFW/include")

#2 VulkanSDK
set(MOTUS_VULKAN_SDK_INCLUDE_DIR "${Vulkan_SDK}\\Include")

#3 glm
set(MOTUS_GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Motus3D/thirdparty/glm/include")

#4 Visus
set(MOTUS_RENDERER_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Visus")

#5 Core
set(MOTUS_CORE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Motus3D")

#5 Vulkan Memory Allocator
set(MOTUS_VMA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Motus3D/thirdparty/VulkanMemoryAllocator/include")

#6 Vulkan SDK's libraries' sources
set(MOTUS_VULKAN_SDK_SRC_INCLUDE_DIR "${Vulkan_SDK}\\Source")

#6 stb_image
set(MOTUS_STBI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Motus3D/thirdparty/stb_image")

#7 Assimp
set(MOTUS_ASSIMP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Motus3D/thirdparty/Assimp/include")

#
#   Motus3D dependencies' library directory
#
#0 VulkanSDK
set(MOTUS_VULKAN_SDK_LIBRARY_DIR "${Vulkan_SDK}\\Lib")
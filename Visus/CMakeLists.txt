set(VISUS_PROJECT Visus)

project (${VISUS_PROJECT} VERSION 1.0)

file (GLOB_RECURSE VISUS_FILES
    "src/*.h"
    "src/*.cpp"
)

file(GLOB SPIRV_REFLECT_FILES
    "${MOTUS_VULKAN_SDK_SRC_INCLUDE_DIR}\\SPIRV-Reflect\\spirv_reflect.h"
    "${MOTUS_VULKAN_SDK_SRC_INCLUDE_DIR}\\SPIRV-Reflect\\spirv_reflect.c"
)

add_library(${VISUS_PROJECT} SHARED
    ${VISUS_FILES}
    ${SPIRV_REFLECT_FILES}
)

target_include_directories(${VISUS_PROJECT} PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    "${MOTUS_CORE_INCLUDE_DIR}/src"
    ${MOTUS_VULKAN_SDK_INCLUDE_DIR}
    ${MOTUS_GLFW_INCLUDE_DIR}
    ${MOTUS_GLM_INCLUDE_DIR}
    ${MOTUS_SPDLOG_INCLUDE_DIR}
    ${MOTUS_VMA_INCLUDE_DIR}
    ${MOTUS_VULKAN_SDK_SRC_INCLUDE_DIR}
    ${MOTUS_STBI_INCLUDE_DIR}
    ${MOTUS_ASSIMP_INCLUDE_DIR}
)

target_link_directories(${VISUS_PROJECT} PRIVATE
    ${MOTUS_VULKAN_SDK_LIBRARY_DIR}
)

target_link_libraries(${VISUS_PROJECT} PRIVATE
    glfw
    vulkan-1.lib
    assimp
    debug shaderc_sharedd.lib optimized shaderc_shared.lib
    debug shaderc_utild.lib optimized shaderc_util.lib
)

target_compile_definitions(${VISUS_PROJECT} PRIVATE
    VISUS_API
    _CRT_SECURE_NO_WARNINGS
    "$<$<CONFIG:Debug>:VISUS_DEBUG>"
    "$<$<CONFIG:RelWithDebInfo>:VISUS_RELEASE>"
    "$<$<CONFIG:MinSizeRel>:VISUS_DIST>"
    "$<$<CONFIG:Release>:VISUS_DIST>"
)

set_property(TARGET ${VISUS_PROJECT} PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
)
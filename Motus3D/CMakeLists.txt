set(MOTUS_CORE_PROJECT Motus-Core)
project (${MOTUS_CORE_PROJECT} VERSION 1.0)

file (GLOB_RECURSE MOTUS_CORE_FILES
    "src/*.h"
    "src/*.cpp"
)

add_library(${MOTUS_CORE_PROJECT} SHARED
    ${MOTUS_CORE_FILES}
)

target_include_directories(${MOTUS_CORE_PROJECT} PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    ${MOTUS_SPDLOG_INCLUDE_DIR}
    ${MOTUS_GLFW_INCLUDE_DIR}
    ${MOTUS_VULKAN_SDK_INCLUDE_DIR}
    ${MOTUS_GLM_INCLUDE_DIR}
    "${MOTUS_RENDERER_INCLUDE_DIR}/src"
)

target_link_directories(${MOTUS_CORE_PROJECT} PRIVATE
    ${MOTUS_VULKAN_SDK_LIBRARY_DIR}
)

target_link_libraries(${MOTUS_CORE_PROJECT} PRIVATE
    glfw
    Visus
    vulkan-1.lib
)

target_compile_definitions(${MOTUS_CORE_PROJECT} PRIVATE
    MT_DYNAMIC
    MT_BUILD_DLL
    _CRT_SECURE_NO_WARNINGS
    "$<$<CONFIG:Debug>:MT_DEBUG>"
    "$<$<CONFIG:RelWithDebInfo>:MT_RELEASE>"
    "$<$<CONFIG:MinSizeRel>:MT_DIST>"
    "$<$<CONFIG:Release>:MT_DIST>"
)

if(WIN32)
target_compile_definitions(${MOTUS_CORE_PROJECT} PUBLIC
    MT_PLATFORM_WINDOWS
)
endif()

set_property(TARGET ${MOTUS_CORE_PROJECT} PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
)

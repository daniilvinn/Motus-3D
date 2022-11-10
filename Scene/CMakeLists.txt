set(SCENE_PROJECT "Scene")

project (${SCENE_PROJECT} VERSION 1.0)

file (GLOB_RECURSE SCENE_FILES
    "src/*.h"
    "src/*.cpp"
)

add_library(${SCENE_PROJECT} SHARED
    ${SCENE_FILES}
)

target_include_directories(${SCENE_PROJECT} PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    "${MOTUS_CORE_INCLUDE_DIR}/src"
    "${MOTUS_RENDERER_INCLUDE_DIR}/src"
    ${MOTUS_GLM_INCLUDE_DIR}
    ${MOTUS_SPDLOG_INCLUDE_DIR}
    ${MOTUS_STBI_INCLUDE_DIR}
)

target_link_libraries(${SCENE_PROJECT} PRIVATE
    Visus
    Motus-Core
)

target_compile_definitions(${SCENE_PROJECT} PRIVATE
    MT_SCENE_API
    _CRT_SECURE_NO_WARNINGS
)

set_property(TARGET ${SCENE_PROJECT} PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
)
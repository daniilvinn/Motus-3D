set(SANDBOX_PROJECT "Sandbox")

project(${SANDBOX_PROJECT} VERSION 1.0)

file (GLOB_RECURSE SANDBOX_FILES
    "src/**.h"
    "src/**.cpp"
)

add_executable(${SANDBOX_PROJECT}
    ${SANDBOX_FILES}
)

if(WIN32)
set_target_properties(${SANDBOX_PROJECT} PROPERTIES
	WIN32_EXECUTABLE $<$<CONFIG:Release>:1>
)
endif()

target_include_directories(${SANDBOX_PROJECT} PRIVATE
    "${MOTUS_CORE_INCLUDE_DIR}/src"
    "${MOTUS_RENDERER_INCLUDE_DIR}/src"
    ${MOTUS_SPDLOG_INCLUDE_DIR}
    ${MOTUS_GLM_INCLUDE_DIR}
)

target_link_libraries(${SANDBOX_PROJECT} PRIVATE
    Motus-Core
    Visus
)

target_compile_definitions(${SANDBOX_PROJECT} PRIVATE
    MT_DYNAMIC
)

set_property(TARGET ${SANDBOX_PROJECT} PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

set_target_properties(${SANDBOX_PROJECT} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
cmake_minimum_required(VERSION 3.21)

project ("Tomato_Engine"
        VERSION 0.1
        DESCRIPTION "My personal voxel game"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_UNITY_BUILD OFF)


# Note: by default ENABLE_DEVELOPER_MODE is True
# This means that all analysis (sanitizers, static analysis)
# is enabled and all warnings are treated as errors
# if you want to switch this behavior, change TRUE to FALSE
# set(ENABLE_DEVELOPER_MODE
#     TRUE
#     CACHE BOOL "Enable 'developer mode'")

# # Change this to false if you want to disable warnings_as_errors in developer mode
# set(OPT_WARNINGS_AS_ERRORS_DEVELOPER_DEFAULT TRUE)


set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(SHADER_DIR ${CMAKE_CURRENT_LIST_DIR}/shaders)
set(DATA_DIR ${CMAKE_CURRENT_LIST_DIR}/data)
set(MESH_DIR ${CMAKE_CURRENT_LIST_DIR}/assets/mesh)
set(TEXTURE_DIR ${CMAKE_CURRENT_LIST_DIR}/assets/images)
set(EXTERN_DIR ${CMAKE_CURRENT_LIST_DIR}/external)
# set(BUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/build)

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

# find_package(assimp CONFIG REQUIRED)


list(APPEND COMPILE_FLAGS
    -fp:fast
    -fp:except-
    # -FC
    # -Gm-
    -GR-
    # -GS
    # -EHsc
    # -Od
    # -Oi
    -WX
    -W4
    # -wd4201
    # -wd4100
    # -wd4189
    # -wd4505
    # -wd4127
    # -Z7
    -Zc:inline
    -Zc:wchar_t
    # -Zo
    )

list(APPEND COMPILE_DEFS
    TOM_INTERNAL
    TOM_WIN32
    )

list(APPEND LINKER_FLAGS
    # -STACK:0x100000,0x100000
    -incremental:no
    -opt:ref
    )

list(APPEND COMMON_LINKER_LIBS
    user32.lib
    gdi32.lib
    winmm.lib
    kernel32.lib
    Ole32.lib
    d3d11.lib
    d3dcompiler.lib
    )

file(GLOB SHADERS
    ${SHADER_DIR}/*.comp
    ${SHADER_DIR}/*.frag
    ${SHADER_DIR}/*.geom
    ${SHADER_DIR}/*.vert
    ${SHADER_DIR}/*.hlsl
    )

file(GLOB DATA
    ${DATA_DIR}/*.ico
    ${DATA_DIR}/*.png
    )

file(GLOB MESHES
    ${MESH_DIR}/*.obj
    )

file(GLOB TEXTURES
    ${MESH_DIR}/*.png
    )


add_library(imgui STATIC
    ${EXTERN_DIR}/imgui/imconfig.h
    ${EXTERN_DIR}/imgui/imgui.cpp
    ${EXTERN_DIR}/imgui/imgui.h
    ${EXTERN_DIR}/imgui/imgui_demo.cpp
    ${EXTERN_DIR}/imgui/imgui_draw.cpp
    ${EXTERN_DIR}/imgui/imgui_impl_win32.cpp
    ${EXTERN_DIR}/imgui/imgui_impl_win32.h
    ${EXTERN_DIR}/imgui/imgui_impl_dx11.cpp
    ${EXTERN_DIR}/imgui/imgui_impl_dx11.h
    ${EXTERN_DIR}/imgui/imgui_internal.h
    ${EXTERN_DIR}/imgui/imgui_stdlib.cpp
    ${EXTERN_DIR}/imgui/imgui_stdlib.h
    ${EXTERN_DIR}/imgui/imgui_tables.cpp
    ${EXTERN_DIR}/imgui/imgui_widgets.cpp
    ${EXTERN_DIR}/imgui/imstb_rectpack.h
    ${EXTERN_DIR}/imgui/imstb_textedit.h
    ${EXTERN_DIR}/imgui/imstb_truetype.h
    )

add_library(stb STATIC
    ${EXTERN_DIR}/stb/stb_image.h
    ${EXTERN_DIR}/stb/stb_image.cpp
    )

add_executable(tomato_engine WIN32
    ${SOURCE_DIR}/app.hpp
    ${SOURCE_DIR}/app.cpp
    ${SOURCE_DIR}/array.hpp
    ${SOURCE_DIR}/core.hpp
    ${SOURCE_DIR}/camera.hpp
    ${SOURCE_DIR}/camera.cpp
    ${SOURCE_DIR}/color.hpp
    ${SOURCE_DIR}/data.h
    ${SOURCE_DIR}/file_io.hpp
    ${SOURCE_DIR}/file_io.cpp
    ${SOURCE_DIR}/graphics.hpp
    ${SOURCE_DIR}/graphics.cpp
    ${SOURCE_DIR}/imgui.hpp
    ${SOURCE_DIR}/input.hpp
    ${SOURCE_DIR}/input.cpp
    ${SOURCE_DIR}/intrinsic.hpp
    ${SOURCE_DIR}/math.hpp
    ${SOURCE_DIR}/memory.hpp
    ${SOURCE_DIR}/mesh.hpp
    ${SOURCE_DIR}/mesh.cpp
    ${SOURCE_DIR}/noise.hpp
    ${SOURCE_DIR}/noise.cpp
    ${SOURCE_DIR}/stack.hpp
    ${SOURCE_DIR}/sound.hpp
    ${SOURCE_DIR}/sound.cpp
    ${SOURCE_DIR}/time.hpp
    ${SOURCE_DIR}/utils.hpp
    ${SOURCE_DIR}/vector.hpp
    ${SOURCE_DIR}/win32.hpp
    ${SOURCE_DIR}/win32.cpp
    ${SOURCE_DIR}/winmain.cpp
    )

target_include_directories(tomato_engine PRIVATE 
     ${EXTERN_DIR}/imgui
     ${EXTERN_DIR}/stb
    )

target_compile_options(tomato_engine  PRIVATE
    ${Compile_FLags}
    # -MTd
    )


# if(MSVC)
#     get_all_targets(all_targets)
#     set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
# endif(


target_compile_definitions(tomato_engine  PRIVATE ${COMPILE_DEFS})

target_link_libraries(tomato_engine PRIVATE ${COMMON_LINKER_LIBS} imgui stb)

target_link_options(tomato_engine PRIVATE
    ${LINKER_FLAGS}
    # -NODEFAULTLIB
    )

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
        VS_STARTUP_PROJECT tomato_engine )

add_custom_command(
    TARGET tomato_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_DIR} $<TARGET_FILE_DIR:tomato_engine>
    DEPENDS ${SHADERS})

add_custom_command(
    TARGET tomato_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_DIR} $<TARGET_FILE_DIR:tomato_engine>
    DEPENDS ${DATA})

add_custom_command(
    TARGET tomato_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${MESH_DIR} $<TARGET_FILE_DIR:tomato_engine>
    DEPENDS ${MESHES})

add_custom_command(
    TARGET tomato_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${TEXTURE_DIR} $<TARGET_FILE_DIR:tomato_engine>
    DEPENDS ${TEXTURES})

add_custom_command(
    TARGET tomato_engine PRE_BUILD
    COMMAND ctime -begin ${CMAKE_CURRENT_LIST_DIR}/data/tomato_engine.ctm)

add_custom_command(
    TARGET tomato_engine POST_BUILD
    COMMAND ctime -end ${CMAKE_CURRENT_LIST_DIR}/data/tomato_engine.ctm)

add_custom_command(
    TARGET tomato_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_LIST_DIR}/build_times.txt)

add_custom_command(
    TARGET tomato_engine POST_BUILD
    COMMAND ctime -csv ${CMAKE_CURRENT_LIST_DIR}/data/tomato_engine.ctm >> ${CMAKE_CURRENT_LIST_DIR}/build_times.txt)
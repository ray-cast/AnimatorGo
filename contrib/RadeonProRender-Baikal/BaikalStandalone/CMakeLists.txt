set(APPLICATION_SOURCES
    Application/application.cpp
    Application/application.h
    Application/app_utils.cpp
    Application/app_utils.h
    Application/cl_render.cpp
    Application/cl_render.h
    Application/gl_render.cpp
    Application/gl_render.h
    Application/uber_node.h
    Application/uber_node.cpp
    Application/uber_tree.h
    Application/uber_tree.cpp
    Application/graph_scheme.h
    Application/graph_scheme.cpp
    Application/material_explorer.h
    Application/material_explorer.cpp
    Application/scene_load_utils.h
    Application/scene_load_utils.cpp)

set(IMGUI_SORUCES
    ImGUI/imconfig.h
    ImGUI/imgui.cpp
    ImGUI/imgui.h
    ImGUI/imgui_demo.cpp
    ImGUI/imgui_draw.cpp
    ImGUI/imgui_impl_glfw_gl3.cpp
    ImGUI/imgui_impl_glfw_gl3.h
    ImGUI/imgui_internal.h
    ImGUI/stb_rect_pack.h
    ImGUI/stb_textedit.h
    ImGUI/stb_truetype.h)

set(MAIN_SOURCES
    main.cpp)

set(UTILS_SOURCES
    Utils/config_manager.cpp
    Utils/config_manager.h
    Utils/shader_manager.cpp
    Utils/shader_manager.h)

set(KERNEL_SOURCES
    Kernels/GLSL/simple.fsh
    Kernels/GLSL/simple.vsh)

set(SOURCES
    ${APPLICATION_SOURCES}
    ${IMGUI_SORUCES}
    ${MAIN_SOURCES}
    ${UTILS_SOURCES}
    ${KERNEL_SOURCES})

source_group("Application" FILES ${APPLICATION_SOURCES})
source_group("ImGUI" FILES ${IMGUI_SORUCES})
source_group("Utils" FILES ${UTILS_SOURCES})
source_group("Kernels\\GLSL" FILES ${KERNEL_SOURCES})
    
add_executable(BaikalStandalone ${SOURCES})
target_compile_features(BaikalStandalone PRIVATE cxx_std_14)

#Add project root since BaikalStandalone directly includes Baikal/* files
target_include_directories(BaikalStandalone
    PRIVATE ${Baikal_SOURCE_DIR}
    PRIVATE .)

target_link_libraries(BaikalStandalone PRIVATE Baikal BaikalIO glfw3::glfw3 OpenGL::GL GLEW::GLEW)

# Compile definitions
if (BAIKAL_ENABLE_DENOISER)
    target_compile_definitions(BaikalStandalone PUBLIC ENABLE_DENOISER)
endif(BAIKAL_ENABLE_DENOISER)

if (BAIKAL_ENABLE_GLTF)
    target_link_libraries(BaikalStandalone PRIVATE RadeonProRender RprSupport)
endif (BAIKAL_ENABLE_GLTF)

if (BAIKAL_ENABLE_DATAGENERATOR)
    target_compile_definitions(BaikalStandalone PUBLIC GENERATE_DATASET)
endif (BAIKAL_ENABLE_DATAGENERATOR)

set_target_properties(BaikalStandalone
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${Baikal_SOURCE_DIR}/BaikalStandalone FOLDER Baikal)

# Add symbolic link from binary to BaikalStandalone/Kernels directory
set(BAIKALSTANDALONE_KERNELS_DIR BaikalStandalone/Kernels)
file(TO_NATIVE_PATH ${Baikal_SOURCE_DIR}/${BAIKALSTANDALONE_KERNELS_DIR} BAIKALSTANDALONE_KERNELS_SRC)
file(TO_NATIVE_PATH ${CMAKE_BINARY_DIR}/${BAIKALSTANDALONE_KERNELS_DIR} BAIKALSTANDALONE_KERNELS_DST)

if (WIN32)
    add_custom_target(BaikalStandaloneKernelsDir ALL
        COMMAND IF NOT EXIST ${BAIKALSTANDALONE_KERNELS_DST} mklink /D ${BAIKALSTANDALONE_KERNELS_DST} ${BAIKALSTANDALONE_KERNELS_SRC}
    )
else ()
    add_custom_target(BaikalStandaloneKernelsDir ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${BAIKALSTANDALONE_KERNELS_SRC} ${BAIKALSTANDALONE_KERNELS_DST}
    )
endif ()

add_dependencies(BaikalStandalone BaikalStandaloneKernelsDir ResourcesDir Copy3rdParty)

# Install target
install(TARGETS BaikalStandalone RUNTIME DESTINATION bin)
file(TO_CMAKE_PATH ${BAIKALSTANDALONE_KERNELS_SRC} BAIKALSTANDALONE_KERNELS_SRC_CMAKE)
install(DIRECTORY ${BAIKALSTANDALONE_KERNELS_SRC_CMAKE} DESTINATION BaikalStandalone)

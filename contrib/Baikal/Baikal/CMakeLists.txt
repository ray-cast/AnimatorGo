set(CONTROLLERS_SOURCES
    Controllers/clw_scene_controller.cpp
    Controllers/clw_scene_controller.h
    Controllers/scene_controller.h
    Controllers/scene_controller.inl
    Controllers/scene_controller.cpp)
    
set(ESTIMATORS_SOURCES 
    Estimators/estimator.h
    Estimators/path_tracing_estimator.cpp
    Estimators/path_tracing_estimator.h)

set(OUTPUT_SOURCES
    Output/clwoutput.h
    Output/output.h)
    
set(POSTEFFECT_SOURCES
    PostEffects/clw_post_effect.h
    PostEffects/post_effect.h
    PostEffects/bilateral_denoiser.h
    PostEffects/wavelet_denoiser.h
    PostEffects/AreaMap33.h
    )
    
set(RENDERERS_SOURCES
    Renderers/adaptive_renderer.cpp
    Renderers/adaptive_renderer.h
    Renderers/monte_carlo_renderer.cpp
    Renderers/monte_carlo_renderer.h
    Renderers/renderer.h)

set(RENDERFACTORY_SOURCES
    RenderFactory/clw_render_factory.cpp
    RenderFactory/clw_render_factory.h
    RenderFactory/render_factory.h)

set(UTILS_SOURCES
    Utils/clw_class.h
    Utils/distribution1d.cpp
    Utils/distribution1d.h
    Utils/eLut.h
    Utils/half.cpp
    Utils/half.h
    Utils/log.h
    Utils/sh.cpp
    Utils/sh.h
    Utils/shproject.cpp
    Utils/shproject.h
    Utils/sobol.h
    Utils/toFloat.h
    Utils/version.h
    Utils/mkpath.cpp
    Utils/mkpath.h
    Utils/cl_inputmap_generator.cpp
    Utils/cl_inputmap_generator.h
    Utils/cl_program.cpp
    Utils/cl_program.h
    Utils/cl_program_manager.cpp
    Utils/cl_program_manager.h
    Utils/cl_uberv2_generator.h
    Utils/cl_uberv2_generator.cpp
    Utils/cmd_parser.h
    Utils/cmd_parser.cpp
)

set(SCENEGRAPH_SOURCES
    SceneGraph/camera.cpp
    SceneGraph/camera.h
    SceneGraph/clwscene.h
    SceneGraph/iterator.h
    SceneGraph/light.cpp
    SceneGraph/light.h
    SceneGraph/material.cpp
    SceneGraph/material.h
    SceneGraph/scene1.cpp
    SceneGraph/scene1.h
    SceneGraph/scene_object.cpp
    SceneGraph/scene_object.h
    SceneGraph/shape.cpp
    SceneGraph/shape.h
    SceneGraph/texture.cpp
    SceneGraph/texture.h
    SceneGraph/uberv2material.cpp
    SceneGraph/uberv2material.h
    SceneGraph/inputmap.h
    SceneGraph/inputmaps.h)
    
set(SCENEGRAPH_COLLECTOR_SOURCES
    SceneGraph/Collector/collector.cpp
    SceneGraph/Collector/collector.h)
       
set(XML_SOURCES
    XML/tinyxml2.cpp
    XML/tinyxml2.h)

set(KERNELS_SOURCES
    Kernels/CL/bxdf.cl
    Kernels/CL/bxdf_uberv2.cl
    Kernels/CL/bxdf_uberv2_bricks.cl
    Kernels/CL/common.cl
    Kernels/CL/denoise.cl
    Kernels/CL/disney.cl
    Kernels/CL/integrator_bdpt.cl
    Kernels/CL/isect.cl
    Kernels/CL/light.cl
    Kernels/CL/monte_carlo_renderer.cl
    Kernels/CL/normalmap.cl
    Kernels/CL/path.cl
    Kernels/CL/path_tracing_estimator.cl
    Kernels/CL/payload.cl
    Kernels/CL/ray.cl
    Kernels/CL/sampling.cl
    Kernels/CL/scene.cl
    Kernels/CL/sh.cl
    Kernels/CL/texture.cl
    Kernels/CL/utils.cl
    Kernels/CL/vertex.cl
    Kernels/CL/volumetrics.cl
    Kernels/CL/wavelet_denoise.cl
    Kernels/CL/path_tracing_estimator_uberv2.cl
    Kernels/CL/fill_aovs_uberv2.cl
    )

set(SOURCES
    ${CONTROLLERS_SOURCES}
    ${ESTIMATORS_SOURCES}
    ${OUTPUT_SOURCES}
    ${POSTEFFECT_SOURCES}
    ${RENDERERS_SOURCES}
    ${RENDERFACTORY_SOURCES}
    ${UTILS_SOURCES}
    ${SCENEGRAPH_SOURCES}
    ${SCENEGRAPH_COLLECTOR_SOURCES}
    ${SCENEGRAPH_IO_SOURCES}
    ${XML_SOURCES}
    ${KERNELS_SOURCES}
    )
    
source_group("Controllers" FILES ${CONTROLLERS_SOURCES})
source_group("Estimators" FILES ${ESTIMATORS_SOURCES})
source_group("Output" FILES ${OUTPUT_SOURCES})
source_group("Posteffect" FILES ${POSTEFFECT_SOURCES})
source_group("Renderers" FILES ${RENDERERS_SOURCES})
source_group("RenderFactory" FILES ${RENDERFACTORY_SOURCES})
source_group("Utils" FILES ${UTILS_SOURCES})
source_group("SceneGraph" FILES ${SCENEGRAPH_SOURCES})
source_group("SceneGraph\\Collector" FILES ${SCENEGRAPH_COLLECTOR_SOURCES})
source_group("XML" FILES ${XML_SOURCES})
source_group("Kernels\\CL" FILES ${KERNELS_SOURCES})

# Set CMAKE_EXPORT_ALL_SYMBOLS flag for exporting dll
# symbols without __declspec(dllimport / dllexport) specifier
if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif (UNIX)
    add_definitions(-fvisibility=default)
endif (WIN32)

add_library(Baikal STATIC ${SOURCES})

# We need to use this definition because CMAKE_EXPORT_ALL_SYMBOLS
# actually doesn't export data symbols
target_compile_definitions(Baikal PRIVATE BAIKAL_EXPORT_DATA_SYMBOLS)

if (BAIKAL_ENABLE_DENOISER)
    target_compile_definitions(Baikal PUBLIC ENABLE_DENOISER)
endif(BAIKAL_ENABLE_DENOISER)

if (BAIKAL_ENABLE_RAYMASK)
    target_compile_definitions(Baikal PUBLIC ENABLE_RAYMASK)
endif (BAIKAL_ENABLE_RAYMASK)

if (BAIKAL_EMBED_KERNELS)
    target_compile_definitions(Baikal PRIVATE BAIKAL_EMBED_KERNELS)
    set(KERNEL_HEADER "${Baikal_BINARY_DIR}/Baikal/embed_kernels.h")
    set(STRINGIFY_SCRIPT "${PROJECT_SOURCE_DIR}/Tools/scripts/baikal_stringify.py")
    add_custom_command(
        OUTPUT ${KERNEL_HEADER}
        COMMAND ${PYTHON_EXECUTABLE} ${STRINGIFY_SCRIPT} ../Baikal/Kernels/CL/ .cl opencl > ${KERNEL_HEADER}
        DEPENDS ${KERNELS_SOURCES}
        WORKING_DIRECTORY ${Baikal_SOURCE_DIR}/Baikal
        COMMENT "Generating Baikal kernel cache"
        VERBATIM)
    add_custom_target(BaikalKernelCache DEPENDS ${KERNEL_HEADER})
    add_dependencies(Baikal BaikalKernelCache)
    # Add directory where embed_kernels.h lies
    target_include_directories(Baikal PRIVATE ${Baikal_BINARY_DIR}/Baikal)
endif (BAIKAL_EMBED_KERNELS)

target_compile_features(Baikal PRIVATE cxx_std_14)
target_include_directories(Baikal PUBLIC "${Baikal_SOURCE_DIR}/Baikal")
target_link_libraries(Baikal PUBLIC RadeonRays)

# Add 'treat warnings as errors' flag and 'Wall' flag on Unix
if (WIN32)
    target_compile_options(Baikal PRIVATE /WX)
elseif (UNIX)
    target_compile_options(Baikal PRIVATE -Wall -Werror)
endif (WIN32)

# Add symbolic link from binary to Baikal/Kernels directory
set(BAIKAL_KERNELS_DIR Baikal/Kernels)
file(TO_NATIVE_PATH ${Baikal_SOURCE_DIR}/${BAIKAL_KERNELS_DIR} BAIKAL_KERNELS_SRC)
file(TO_NATIVE_PATH ${PROJECT_BINARY_DIR}/${BAIKAL_KERNELS_DIR} BAIKAL_KERNELS_DST)

if (WIN32)
    add_custom_target(BaikalKernelsDir ALL COMMAND IF NOT EXIST ${BAIKAL_KERNELS_DST} mklink /D ${BAIKAL_KERNELS_DST} ${BAIKAL_KERNELS_SRC})
else ()
    add_custom_target(BaikalKernelsDir ALL COMMAND ${CMAKE_COMMAND} -E create_symlink ${BAIKAL_KERNELS_SRC} ${BAIKAL_KERNELS_DST})
endif ()

add_dependencies(Baikal BaikalKernelsDir)

# Install target
if (WIN32)
    install(TARGETS Baikal RUNTIME DESTINATION bin)
else ()
    install(TARGETS Baikal LIBRARY DESTINATION lib)
endif ()

file(TO_CMAKE_PATH ${BAIKAL_KERNELS_SRC} BAIKAL_KERNELS_SRC_CMAKE)
install(DIRECTORY ${BAIKAL_KERNELS_SRC_CMAKE} DESTINATION Baikal)

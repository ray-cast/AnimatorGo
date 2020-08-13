SET(LIB_OUTNAME video)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(VIDEO_GRAPHICS_LIST
	${HEADER_PATH}/render_object.h
	${SOURCE_PATH}/render_object.cpp
	${HEADER_PATH}/renderer.h
	${SOURCE_PATH}/renderer.cpp
)
SOURCE_GROUP(renderer FILES ${VIDEO_GRAPHICS_LIST})

SET(VIDEO_COMMON_LIST
	${SOURCE_PATH}/output.h
	${SOURCE_PATH}/output.cpp
	${SOURCE_PATH}/estimator.h
	${SOURCE_PATH}/estimator.cpp
	${SOURCE_PATH}/pipeline.h
	${SOURCE_PATH}/pipeline.cpp
	${SOURCE_PATH}/scene_controller.h
	${SOURCE_PATH}/scene_controller.cpp
	${HEADER_PATH}/render_scene.h
	${SOURCE_PATH}/render_scene.cpp
	${HEADER_PATH}/compiled_scene.h
	${SOURCE_PATH}/compiled_scene.cpp
	${SOURCE_PATH}/render_factory.h
	${SOURCE_PATH}/render_factory.cpp
)
SOURCE_GROUP(renderer\\common FILES ${VIDEO_COMMON_LIST})

SET(VIDEO_FORWARE_LIST
	${HEADER_PATH}/forward_buffer.h
	${SOURCE_PATH}/forward_buffer.cpp
	${HEADER_PATH}/forward_output.h
	${SOURCE_PATH}/forward_output.cpp
	${HEADER_PATH}/forward_pipeline.h
	${SOURCE_PATH}/forward_pipeline.cpp
	${HEADER_PATH}/forward_scene.h
	${SOURCE_PATH}/forward_scene.cpp
	${HEADER_PATH}/forward_scene_controller.h
	${SOURCE_PATH}/forward_scene_controller.cpp
	${HEADER_PATH}/forward_render_factory.h
	${SOURCE_PATH}/forward_render_factory.cpp
	${HEADER_PATH}/forward_renderer.h
	${SOURCE_PATH}/forward_renderer.cpp
	${HEADER_PATH}/forward_material.h
	${SOURCE_PATH}/forward_material.cpp
)
SOURCE_GROUP(renderer\\forward FILES ${VIDEO_FORWARE_LIST})

SET(VIDEO_OPENCL_LIST
	${SOURCE_PATH}/clw_output.h
	${SOURCE_PATH}/clw_output.cpp
	${SOURCE_PATH}/clw_texture_output.h
	${SOURCE_PATH}/clw_texture_output.cpp
	${SOURCE_PATH}/clw_scene.h
	${SOURCE_PATH}/clw_scene.cpp
	${SOURCE_PATH}/clw_scene_controller.h
	${SOURCE_PATH}/clw_scene_controller.cpp
	${SOURCE_PATH}/clw_render_factory.h
	${SOURCE_PATH}/clw_render_factory.cpp
	${SOURCE_PATH}/clw_class.h
	${SOURCE_PATH}/clw_class.cpp
	${SOURCE_PATH}/cl_program.h
	${SOURCE_PATH}/cl_program.cpp
	${SOURCE_PATH}/cl_program_manager.h
	${SOURCE_PATH}/cl_program_manager.cpp
	${SOURCE_PATH}/rtx_manager.h
	${SOURCE_PATH}/rtx_manager.cpp
	${SOURCE_PATH}/path_tracing_estimator.h
	${SOURCE_PATH}/path_tracing_estimator.cpp
	${SOURCE_PATH}/monte_carlo_renderer.h
	${SOURCE_PATH}/monte_carlo_renderer.cpp
)
SOURCE_GROUP(renderer\\opencl FILES ${VIDEO_OPENCL_LIST})

SET(VIDEO_OFFLINE_LIST
	${SOURCE_PATH}/offline_renderer.h
	${SOURCE_PATH}/offline_renderer.cpp
)
SOURCE_GROUP(renderer\\offline FILES ${VIDEO_OFFLINE_LIST})

LIST(APPEND VIDEO_LIST ${VIDEO_COMMON_LIST} ${VIDEO_GRAPHICS_LIST} ${VIDEO_FORWARE_LIST} ${VIDEO_OPENCL_LIST} ${VIDEO_OFFLINE_LIST})
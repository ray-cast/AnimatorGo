SET(LIB_OUTNAME video)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(VIDEO_GRAPHICS_LIST
	${HEADER_PATH}/renderer.h
	${SOURCE_PATH}/renderer.cpp
	${HEADER_PATH}/render_scene.h
	${SOURCE_PATH}/render_scene.cpp
	${HEADER_PATH}/render_object.h
	${SOURCE_PATH}/render_object.cpp
)
SOURCE_GROUP(renderer FILES ${VIDEO_GRAPHICS_LIST})

SET(VIDEO_UTILS_LIST
	${HEADER_PATH}/collector.h
	${SOURCE_PATH}/collector.cpp
	${HEADER_PATH}/output.h
	${SOURCE_PATH}/output.cpp
	${HEADER_PATH}/pipeline.h
	${SOURCE_PATH}/pipeline.cpp
	${HEADER_PATH}/scene_controller.h
	${SOURCE_PATH}/scene_controller.cpp
	${HEADER_PATH}/compiled_scene.h
	${SOURCE_PATH}/compiled_scene.cpp
	${SOURCE_PATH}/render_factory.h
	${SOURCE_PATH}/render_factory.cpp
)
SOURCE_GROUP(renderer\\utils FILES ${VIDEO_UTILS_LIST})

SET(VIDEO_FORWARE_LIST
	${HEADER_PATH}/forward_renderer.h
	${SOURCE_PATH}/forward_renderer.cpp
	${HEADER_PATH}/draw_object_pass.h
	${SOURCE_PATH}/draw_object_pass.cpp
	${HEADER_PATH}/draw_selector_pass.h
	${SOURCE_PATH}/draw_selector_pass.cpp
	${HEADER_PATH}/draw_skybox_pass.h
	${SOURCE_PATH}/draw_skybox_pass.cpp
	${HEADER_PATH}/lights_shadow_caster_pass.h
	${SOURCE_PATH}/lights_shadow_caster_pass.cpp
	${HEADER_PATH}/rendering_data.h
	${SOURCE_PATH}/rendering_data.cpp
	${HEADER_PATH}/scriptable_render_buffer.h
	${SOURCE_PATH}/scriptable_render_buffer.cpp
	${HEADER_PATH}/scriptable_render_material.h
	${SOURCE_PATH}/scriptable_render_material.cpp
	${HEADER_PATH}/scriptable_render_context.h
	${SOURCE_PATH}/scriptable_render_context.cpp
	${HEADER_PATH}/scriptable_render_pass.h
	${SOURCE_PATH}/scriptable_render_pass.cpp
	${HEADER_PATH}/scriptable_scene_controller.h
	${SOURCE_PATH}/scriptable_scene_controller.cpp
)
SOURCE_GROUP(renderer\\forward FILES ${VIDEO_FORWARE_LIST})

SET(VIDEO_OPENCL_LIST
	${SOURCE_PATH}/estimator.h
	${SOURCE_PATH}/estimator.cpp
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
	${SOURCE_PATH}/config_manager.h
	${SOURCE_PATH}/config_manager.cpp
	${SOURCE_PATH}/path_tracing_estimator.h
	${SOURCE_PATH}/path_tracing_estimator.cpp
	${SOURCE_PATH}/monte_carlo_renderer.h
	${SOURCE_PATH}/monte_carlo_renderer.cpp
)
SOURCE_GROUP(renderer\\opencl FILES ${VIDEO_OPENCL_LIST})

LIST(APPEND VIDEO_LIST ${VIDEO_UTILS_LIST} ${VIDEO_GRAPHICS_LIST} ${VIDEO_FORWARE_LIST} ${VIDEO_PASS_LIST} ${VIDEO_OPENCL_LIST})
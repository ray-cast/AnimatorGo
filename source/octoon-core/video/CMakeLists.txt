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

SET(OUTPUT_LIST
	${SOURCE_PATH}/output.h
	${SOURCE_PATH}/output.cpp
)
SOURCE_GROUP(renderer\\internal\\output FILES ${OUTPUT_LIST})

SET(PIPELINE_LIST
	${SOURCE_PATH}/pipeline.h
	${SOURCE_PATH}/pipeline.cpp
)
SOURCE_GROUP(renderer\\internal\\pipeline FILES ${PIPELINE_LIST})

SET(CONTROLLER_LIST
	${SOURCE_PATH}/scene_controller.h
	${SOURCE_PATH}/scene_controller.cpp
)
SOURCE_GROUP(renderer\\internal\\controller FILES ${CONTROLLER_LIST})

SET(SCENE_LIST
	${HEADER_PATH}/render_scene.h
	${SOURCE_PATH}/render_scene.cpp
	${HEADER_PATH}/compiled_scene.h
	${SOURCE_PATH}/compiled_scene.cpp
)
SOURCE_GROUP(renderer\\internal\\scene FILES ${SCENE_LIST})

SET(FACTORY_LIST
	${SOURCE_PATH}/render_factory.h
	${SOURCE_PATH}/render_factory.cpp
)
SOURCE_GROUP(renderer\\internal\\factory FILES ${FACTORY_LIST})

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

SET(VIDEO_CLW_LIST
	${SOURCE_PATH}/clw_output.h
	${SOURCE_PATH}/clw_output.cpp
	${SOURCE_PATH}/clw_pipeline.h
	${SOURCE_PATH}/clw_pipeline.cpp
	${SOURCE_PATH}/clw_scene.h
	${SOURCE_PATH}/clw_scene.cpp
	${SOURCE_PATH}/clw_scene_controller.h
	${SOURCE_PATH}/clw_scene_controller.cpp
	${SOURCE_PATH}/clw_render_factory.h
	${SOURCE_PATH}/clw_render_factory.cpp
	${SOURCE_PATH}/clw_class.h
	${SOURCE_PATH}/cl_program.h
	${SOURCE_PATH}/cl_program.cpp
	${SOURCE_PATH}/cl_program_manager.h
	${SOURCE_PATH}/cl_program_manager.cpp
	${SOURCE_PATH}/rtx_manager.h
	${SOURCE_PATH}/rtx_manager.cpp
)
SOURCE_GROUP(renderer\\clw FILES ${VIDEO_CLW_LIST})

SET(MANAGER_LIST
	${SOURCE_PATH}/offline_renderer.h
	${SOURCE_PATH}/offline_renderer.cpp
)
SOURCE_GROUP(renderer\\offline FILES ${MANAGER_LIST})

LIST(APPEND VIDEO_LIST ${VIDEO_GRAPHICS_LIST} ${VIDEO_FORWARE_LIST} ${VIDEO_CLW_LIST} ${OFFLINE_LIST} ${OUTPUT_LIST} ${SCENE_LIST} ${FACTORY_LIST} ${CONTROLLER_LIST} ${PIPELINE_LIST} ${MANAGER_LIST})
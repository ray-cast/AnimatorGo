SET(LIB_OUTNAME video)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(VIDEO_GRAPHICS_LIST
	${HEADER_PATH}/render_object.h
	${SOURCE_PATH}/render_object.cpp
	${HEADER_PATH}/render_scene.h
	${SOURCE_PATH}/render_scene.cpp
	${HEADER_PATH}/renderer.h
	${SOURCE_PATH}/renderer.cpp
)
SOURCE_GROUP(renderer FILES ${VIDEO_GRAPHICS_LIST})

SET(VIDEO_FORWARE_LIST
	${HEADER_PATH}/render_pipeline.h
	${SOURCE_PATH}/render_pipeline.cpp
	${HEADER_PATH}/render_buffer.h
	${SOURCE_PATH}/render_buffer.cpp
	${HEADER_PATH}/render_context.h
	${SOURCE_PATH}/render_context.cpp
)
SOURCE_GROUP(renderer\\forward FILES ${VIDEO_FORWARE_LIST})

SET(OFFLINE_LIST
	${HEADER_PATH}/monte_carlo_render.h
	${SOURCE_PATH}/monte_carlo_render.cpp
)
SOURCE_GROUP(renderer\\offline FILES ${OFFLINE_LIST})

SET(OUTPUT_LIST
	${SOURCE_PATH}/output.h
	${SOURCE_PATH}/output.cpp
)
SOURCE_GROUP(renderer\\rtx\\output FILES ${OUTPUT_LIST})

SET(PIPELINE_LIST
	${SOURCE_PATH}/pipeline.h
	${SOURCE_PATH}/pipeline.cpp
)
SOURCE_GROUP(renderer\\rtx\\pipeline FILES ${PIPELINE_LIST})

SET(FACTORY_LIST
	${SOURCE_PATH}/render_factory.h
	${SOURCE_PATH}/render_factory.cpp
)
SOURCE_GROUP(renderer\\rtx\\factory FILES ${FACTORY_LIST})

SET(CLW_LIST
	${SOURCE_PATH}/clw_output.h
	${SOURCE_PATH}/clw_output.cpp
	${SOURCE_PATH}/clw_render_factory.h
	${SOURCE_PATH}/clw_render_factory.cpp
	${SOURCE_PATH}/clw_scene.h
	${SOURCE_PATH}/clw_scene.cpp
	${SOURCE_PATH}/clw_scene_controller.h
	${SOURCE_PATH}/clw_scene_controller.cpp
	${SOURCE_PATH}/clw_pipeline.h
	${SOURCE_PATH}/clw_pipeline.cpp
)
SOURCE_GROUP(renderer\\rtx\\clw FILES ${CLW_LIST})

SET(MANAGER_LIST
	${SOURCE_PATH}/rtx_manager.h
	${SOURCE_PATH}/rtx_manager.cpp
	${SOURCE_PATH}/scene_controller.h
	${SOURCE_PATH}/scene_controller.cpp
)
SOURCE_GROUP(renderer\\rtx\\manager FILES ${MANAGER_LIST})

LIST(APPEND VIDEO_LIST ${VIDEO_GRAPHICS_LIST} ${VIDEO_FORWARE_LIST} ${OFFLINE_LIST} ${OUTPUT_LIST} ${CLW_LIST} ${FACTORY_LIST} ${PIPELINE_LIST} ${MANAGER_LIST})
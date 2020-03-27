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
)
SOURCE_GROUP(renderer\\forware FILES ${VIDEO_FORWARE_LIST})

LIST(APPEND VIDEO_LIST ${VIDEO_GRAPHICS_LIST} ${VIDEO_FORWARE_LIST})
SET(LIB_OUTNAME video)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(VIDEO_GRAPHICS_LIST
	${HEADER_PATH}/render_object.h
	${SOURCE_PATH}/render_object.cpp
	${HEADER_PATH}/render_scene.h
	${SOURCE_PATH}/render_scene.cpp
	${HEADER_PATH}/render_system.h
	${SOURCE_PATH}/render_system.cpp
	${HEADER_PATH}/render_pipeline.h
	${SOURCE_PATH}/render_pipeline.cpp
	${HEADER_PATH}/basic_pipeline.h
	${SOURCE_PATH}/basic_pipeline.cpp
	${HEADER_PATH}/render_types.h
)
SOURCE_GROUP(renderer FILES ${VIDEO_GRAPHICS_LIST})

SET(VIDEO_GEOMETRY_LIST
	${HEADER_PATH}/geometry.h
	${SOURCE_PATH}/geometry.cpp
)
SOURCE_GROUP(geometry FILES ${VIDEO_GEOMETRY_LIST})

SET(VIDEO_LIGHT_LIST
	${HEADER_PATH}/light.h
	${SOURCE_PATH}/light.cpp
)
SOURCE_GROUP(light FILES ${VIDEO_LIGHT_LIST})

LIST(APPEND VIDEO_LIST ${VIDEO_GRAPHICS_LIST} ${VIDEO_GEOMETRY_LIST} ${VIDEO_LIGHT_LIST})
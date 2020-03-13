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
	${HEADER_PATH}/render_types.h
)
SOURCE_GROUP(video FILES ${VIDEO_GRAPHICS_LIST})

SET(VIDEO_CAMERA_LIST
	${HEADER_PATH}/camera.h
	${SOURCE_PATH}/camera.cpp
	${HEADER_PATH}/ortho_camera.h
	${SOURCE_PATH}/ortho_camera.cpp
	${HEADER_PATH}/perspective_camera.h
	${SOURCE_PATH}/perspective_camera.cpp
	${HEADER_PATH}/film_camera.h
	${SOURCE_PATH}/film_camera.cpp
)
SOURCE_GROUP(video\\camera FILES ${VIDEO_CAMERA_LIST})

SET(VIDEO_MATERIAL_LIST
	${HEADER_PATH}/material.h
	${SOURCE_PATH}/material.cpp
	${HEADER_PATH}/phong_material.h
	${SOURCE_PATH}/phong_material.cpp
	${HEADER_PATH}/blinn_material.h
	${SOURCE_PATH}/blinn_material.cpp
	${HEADER_PATH}/ggx_material.h
	${SOURCE_PATH}/ggx_material.cpp
	${HEADER_PATH}/line_material.h
	${SOURCE_PATH}/line_material.cpp
	${HEADER_PATH}/text_material.h
	${SOURCE_PATH}/text_material.cpp
	${HEADER_PATH}/lambert_material.h
	${SOURCE_PATH}/lambert_material.cpp
	${HEADER_PATH}/basic_material.h
	${SOURCE_PATH}/basic_material.cpp
	${HEADER_PATH}/basic_skinned_material.h
	${SOURCE_PATH}/basic_skinned_material.cpp
)
SOURCE_GROUP(video\\material FILES ${VIDEO_MATERIAL_LIST})

SET(VIDEO_GEOMETRY_LIST
	${HEADER_PATH}/geometry.h
	${SOURCE_PATH}/geometry.cpp
)
SOURCE_GROUP(video\\geometry FILES ${VIDEO_GEOMETRY_LIST})

SET(VIDEO_LIGHT_LIST
	${HEADER_PATH}/light.h
	${SOURCE_PATH}/light.cpp
)
SOURCE_GROUP(video\\light FILES ${VIDEO_LIGHT_LIST})

LIST(APPEND VIDEO_LIST ${VIDEO_GRAPHICS_LIST} ${VIDEO_CAMERA_LIST} ${VIDEO_MATERIAL_LIST} ${VIDEO_GEOMETRY_LIST} ${VIDEO_LIGHT_LIST})
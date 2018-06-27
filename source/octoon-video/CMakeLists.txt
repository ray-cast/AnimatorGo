SET(LIB_NAME video)
SET(LIB_OUTNAME octoon-${LIB_NAME})

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_NAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/${LIB_OUTNAME})

SET(VIDEO_GRAPHICS_LIST
	${HEADER_PATH}/render_object.h
	${SOURCE_PATH}/render_object.cpp
	${HEADER_PATH}/render_scene.h
	${SOURCE_PATH}/render_scene.cpp
	${HEADER_PATH}/render_system.h
	${SOURCE_PATH}/render_system.cpp
	${HEADER_PATH}/render_types.h
)
SOURCE_GROUP(${LIB_NAME}  FILES ${VIDEO_GRAPHICS_LIST})

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
SOURCE_GROUP(${LIB_NAME}\\camera  FILES ${VIDEO_CAMERA_LIST})

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
)
SOURCE_GROUP(${LIB_NAME}\\material  FILES ${VIDEO_MATERIAL_LIST})

SET(VIDEO_GEOMETRY_LIST
	${HEADER_PATH}/geometry.h
	${SOURCE_PATH}/geometry.cpp
)
SOURCE_GROUP(${LIB_NAME}\\geometry  FILES ${VIDEO_GEOMETRY_LIST})

IF(OCTOON_BUILD_SHARED_DLL)
	ADD_DEFINITIONS(-DOCTOON_BUILD_DLL_EXPORT)
	ADD_LIBRARY(${LIB_OUTNAME} SHARED 
		${VIDEO_GRAPHICS_LIST}
		${VIDEO_CAMERA_LIST}
		${VIDEO_MATERIAL_LIST}
		${VIDEO_GEOMETRY_LIST}
		${VIDEO_GRAPHICS_VIDEO_LIST}
	)
ELSE()
	ADD_DEFINITIONS(-DOCTOON_STATIC)
	ADD_LIBRARY(${LIB_OUTNAME} 
		${VIDEO_GRAPHICS_LIST}
		${VIDEO_CAMERA_LIST}
		${VIDEO_MATERIAL_LIST}
		${VIDEO_GEOMETRY_LIST}
		${VIDEO_GRAPHICS_VIDEO_LIST}
	)
ENDIF()

TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PRIVATE octoon-runtime)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PRIVATE octoon-graphics)

SET_TARGET_ATTRIBUTE(${LIB_OUTNAME} "core")
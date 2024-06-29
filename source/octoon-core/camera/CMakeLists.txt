SET(LIB_OUTNAME camera)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(CAMERA_LIST
	${HEADER_PATH}/camera.h
	${SOURCE_PATH}/camera.cpp
	${HEADER_PATH}/ortho_camera.h
	${SOURCE_PATH}/ortho_camera.cpp
	${HEADER_PATH}/perspective_camera.h
	${SOURCE_PATH}/perspective_camera.cpp
	${HEADER_PATH}/film_camera.h
	${SOURCE_PATH}/film_camera.cpp
)
SOURCE_GROUP(camera FILES ${CAMERA_LIST})
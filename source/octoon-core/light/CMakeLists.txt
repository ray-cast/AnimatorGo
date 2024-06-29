SET(LIB_OUTNAME light)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(LIGHT_LIST
	${HEADER_PATH}/light.h
	${SOURCE_PATH}/light.cpp
	${HEADER_PATH}/point_light.h
	${SOURCE_PATH}/point_light.cpp
	${HEADER_PATH}/spot_light.h
	${SOURCE_PATH}/spot_light.cpp
	${HEADER_PATH}/directional_light.h
	${SOURCE_PATH}/directional_light.cpp
	${HEADER_PATH}/rectangle_light.h
	${SOURCE_PATH}/rectangle_light.cpp
	${HEADER_PATH}/disk_light.h
	${SOURCE_PATH}/disk_light.cpp
	${HEADER_PATH}/tube_light.h
	${SOURCE_PATH}/tube_light.cpp
	${HEADER_PATH}/ambient_light.h
	${SOURCE_PATH}/ambient_light.cpp
	${HEADER_PATH}/environment_light.h
	${SOURCE_PATH}/environment_light.cpp
	${HEADER_PATH}/light_probe.h
	${SOURCE_PATH}/light_probe.cpp
)
SOURCE_GROUP(light FILES ${LIGHT_LIST})
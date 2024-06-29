SET(LIB_OUTNAME math)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(MATH_LIST
	${HEADER_PATH}/vector2.h
	${HEADER_PATH}/vector3.h
	${HEADER_PATH}/vector4.h
	${HEADER_PATH}/mat2.h
	${HEADER_PATH}/mat3.h
	${HEADER_PATH}/mat4.h
	${HEADER_PATH}/quat.h
	${HEADER_PATH}/math.h
	${HEADER_PATH}/triangle.h
	${HEADER_PATH}/trait.h
	${HEADER_PATH}/box2.h
	${HEADER_PATH}/box3.h
	${HEADER_PATH}/sphere.h
	${HEADER_PATH}/raycast.h
	${HEADER_PATH}/boundingbox.h
	${HEADER_PATH}/hammersley.h
	${HEADER_PATH}/montecarlo.h
	${HEADER_PATH}/mathfwd.h
    ${HEADER_PATH}/mathutil.h
    ${SOURCE_PATH}/mathutil.cpp
	${HEADER_PATH}/perlin_noise.h
	${SOURCE_PATH}/perlin_noise.cpp
	${HEADER_PATH}/variant.h
	${SOURCE_PATH}/variant.cpp
	${HEADER_PATH}/SH.h
)
SOURCE_GROUP("math" FILES ${MATH_LIST})
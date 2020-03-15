SET(LIB_OUTNAME material)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(MATERIAL_LIST
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
	${HEADER_PATH}/basic_skinned_material.h
	${SOURCE_PATH}/basic_skinned_material.cpp
)
SOURCE_GROUP(material  FILES ${MATERIAL_LIST})

LIST(APPEND MATERIAL_LIST ${MATERIAL_LIST})
SET(LIB_OUTNAME model)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(CONTOUER_LIST
	${HEADER_PATH}/contour.h
	${SOURCE_PATH}/contour.cpp
	${HEADER_PATH}/contour_group.h
	${SOURCE_PATH}/contour_group.cpp
	${HEADER_PATH}/path.h
	${SOURCE_PATH}/path.cpp
	${HEADER_PATH}/path_edge.h
	${SOURCE_PATH}/path_edge.cpp
	${HEADER_PATH}/path_group.h
	${SOURCE_PATH}/path_group.cpp
	${HEADER_PATH}/path_types.h
)
SOURCE_GROUP(contour  FILES ${CONTOUER_LIST})

SET(TEXT_LIST
	${HEADER_PATH}/font.h
	${SOURCE_PATH}/font.cpp
	${HEADER_PATH}/text_meshing.h
	${SOURCE_PATH}/text_meshing.cpp
	${HEADER_PATH}/font_system.h
	${SOURCE_PATH}/font_system.cpp
)
SOURCE_GROUP(text  FILES ${TEXT_LIST})

SET(SKELETON_LIST
	${HEADER_PATH}/bone.h
	${SOURCE_PATH}/bone.cpp
	${HEADER_PATH}/vertex_weight.h
)
SOURCE_GROUP(skelecton  FILES ${SKELETON_LIST})

LIST(APPEND MODEL_LIST ${LOADER_LIST} ${CONTOUER_LIST} ${TEXT_LIST} ${SKELETON_LIST})
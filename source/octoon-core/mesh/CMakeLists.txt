SET(LIB_OUTNAME mesh)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(MESH_LIST
	${HEADER_PATH}/mesh.h
	${SOURCE_PATH}/mesh.cpp
	${HEADER_PATH}/combine_mesh.h
	${SOURCE_PATH}/combine_mesh.cpp
	${HEADER_PATH}/sphere_mesh.h
	${SOURCE_PATH}/sphere_mesh.cpp
	${HEADER_PATH}/circle_mesh.h
	${SOURCE_PATH}/circle_mesh.cpp
	${HEADER_PATH}/cone_mesh.h
	${SOURCE_PATH}/cone_mesh.cpp
	${HEADER_PATH}/cube_mesh.h
	${SOURCE_PATH}/cube_mesh.cpp
	${HEADER_PATH}/cube_wireframe_mesh.h
	${SOURCE_PATH}/cube_wireframe_mesh.cpp
	${HEADER_PATH}/plane_mesh.h
	${SOURCE_PATH}/plane_mesh.cpp
	${HEADER_PATH}/ring_mesh.h
	${SOURCE_PATH}/ring_mesh.cpp
	${HEADER_PATH}/volume_mesh.h
	${SOURCE_PATH}/volume_mesh.cpp
	${HEADER_PATH}/noise_mesh.h
	${SOURCE_PATH}/noise_mesh.cpp
	${HEADER_PATH}/shape_mesh.h
	${SOURCE_PATH}/shape_mesh.cpp
)
SOURCE_GROUP(mesh FILES ${MESH_LIST})
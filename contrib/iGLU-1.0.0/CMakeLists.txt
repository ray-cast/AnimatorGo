PROJECT(iGLU C)

SET(LIB_NAME iGLU)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${OCTOON_PATH_DEPENDENCIES}/gles/include)

SET(SOURCE_LIST
	"libtess/dict.c"
	"libtess/geom.c"
	"libtess/memalloc.c"
	"libtess/mesh.c"
	"libtess/normal.c"
	"libtess/priorityq.c"
	"libtess/render.c"
	"libtess/sweep.c"
	"libtess/tess.c"
	"libtess/tessmono.c"
	"libutil/glue.c"
	"libutil/project.c"
	"libutil/registry.c"
	"libutil/error.c"
)

ADD_LIBRARY(${LIB_NAME} STATIC ${SOURCE_LIST})
SET(LIB_OUTNAME runtime)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(RUNTIME_LIST
	${HEADER_PATH}/def.h
	${HEADER_PATH}/algorithm.h
	${HEADER_PATH}/json.h
	${HEADER_PATH}/assert.h
	${HEADER_PATH}/platform.h
	${HEADER_PATH}/singleton.h
	${HEADER_PATH}/rtti.h
	${SOURCE_PATH}/rtti.cpp
	${HEADER_PATH}/rtti_interface.h
	${SOURCE_PATH}/rtti_interface.cpp
	${HEADER_PATH}/rtti_factory.h
	${SOURCE_PATH}/rtti_factory.cpp
	${HEADER_PATH}/rtti_macros.h
	${HEADER_PATH}/rtti_singleton.h
	${SOURCE_PATH}/rtti_singleton.cpp
	${HEADER_PATH}/timer.h
	${SOURCE_PATH}/timer.cpp
	${HEADER_PATH}/except.h
	${SOURCE_PATH}/except.cpp
	${HEADER_PATH}/string.h
	${SOURCE_PATH}/string.cpp
	${HEADER_PATH}/uuid.h
	${SOURCE_PATH}/uuid.cpp
	${HEADER_PATH}/sigslot.h
	${HEADER_PATH}/variant.h
)
SOURCE_GROUP("runtime" FILES ${RUNTIME_LIST})
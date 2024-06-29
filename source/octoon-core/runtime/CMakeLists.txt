SET(LIB_OUTNAME runtime)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(RUNTIME_LIST
	${HEADER_PATH}/def.h
	${HEADER_PATH}/json.h
	${HEADER_PATH}/assert.h
	${HEADER_PATH}/platform.h
	${HEADER_PATH}/singleton.h
	${HEADER_PATH}/rtti.h
	${SOURCE_PATH}/rtti.cpp
	${HEADER_PATH}/object.h
	${SOURCE_PATH}/object.cpp
	${HEADER_PATH}/rtti_factory.h
	${SOURCE_PATH}/rtti_factory.cpp
	${HEADER_PATH}/rtti_macros.h
	${HEADER_PATH}/rtti_singleton.h
	${SOURCE_PATH}/rtti_singleton.cpp
	${HEADER_PATH}/timer.h
	${SOURCE_PATH}/timer.cpp
	${HEADER_PATH}/profiling_scope.h
	${HEADER_PATH}/except.h
	${SOURCE_PATH}/except.cpp
	${HEADER_PATH}/string.h
	${SOURCE_PATH}/string.cpp
	${HEADER_PATH}/guid.h
	${SOURCE_PATH}/guid.cpp
	${HEADER_PATH}/base32.h
	${SOURCE_PATH}/base32.cpp
	${HEADER_PATH}/base64.h
	${SOURCE_PATH}/base64.cpp
	${HEADER_PATH}/md5.h
	${SOURCE_PATH}/md5.cpp
	${HEADER_PATH}/sigslot.h
)
SOURCE_GROUP("runtime" FILES ${RUNTIME_LIST})
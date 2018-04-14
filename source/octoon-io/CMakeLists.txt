SET(LIB_NAME io)
SET(LIB_OUTNAME octoon-${LIB_NAME})

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_NAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/${LIB_OUTNAME})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/contrib/zipper)

SET(IO_LIST
	${HEADER_PATH}/fcntl.h
	${HEADER_PATH}/file.h
	${SOURCE_PATH}/file.cpp
	${HEADER_PATH}/iosbase.h
	${SOURCE_PATH}/iosbase.cpp
	${HEADER_PATH}/ioserver.h
	${SOURCE_PATH}/ioserver.cpp
	${HEADER_PATH}/ori.h
	${SOURCE_PATH}/ori.cpp
	${HEADER_PATH}/stream.h
	${SOURCE_PATH}/stream.cpp
	${HEADER_PATH}/stream_buf.h
	${SOURCE_PATH}/stream_buf.cpp
	${HEADER_PATH}/istream.h
	${SOURCE_PATH}/istream.cpp
	${HEADER_PATH}/ostream.h
	${SOURCE_PATH}/ostream.cpp
	${HEADER_PATH}/iostream.h
	${SOURCE_PATH}/iostream.cpp
)
SOURCE_GROUP(${LIB_NAME}\\core FILES ${IO_LIST})

SET(STREAM_LIST
	${HEADER_PATH}/mstream.h
	${SOURCE_PATH}/mstream.cpp
	${HEADER_PATH}/fstream.h
	${SOURCE_PATH}/fstream.cpp
	${HEADER_PATH}/vstream.h
	${SOURCE_PATH}/vstream.cpp
	${HEADER_PATH}/file_buf.h
	${SOURCE_PATH}/file_buf.cpp
	${HEADER_PATH}/membuf.h
	${SOURCE_PATH}/membuf.cpp
	${HEADER_PATH}/virtual_buf.h
	${SOURCE_PATH}/virtual_buf.cpp
)
SOURCE_GROUP(${LIB_NAME}\\stream FILES ${STREAM_LIST})

SET(ARCHIVE_LIST
	${HEADER_PATH}/archive.h
	${SOURCE_PATH}/archive.cpp
	${HEADER_PATH}/zarchive.h
	${SOURCE_PATH}/zarchive.cpp
	${HEADER_PATH}/farchive.h
	${SOURCE_PATH}/farchive.cpp
)
SOURCE_GROUP(${LIB_NAME}\\archive FILES ${ARCHIVE_LIST})

SET(SERIALIZATION_LIST
	${HEADER_PATH}/endian.h
	${SOURCE_PATH}/endian.cpp
	${HEADER_PATH}/binary_reader.h
	${SOURCE_PATH}/binary_reader.cpp
	${HEADER_PATH}/binary_writer.h
	${SOURCE_PATH}/binary_writer.cpp
	${HEADER_PATH}/text_reader.h
	${SOURCE_PATH}/text_reader.cpp
	${HEADER_PATH}/text_writer.h
	${SOURCE_PATH}/text_writer.cpp
	${HEADER_PATH}/stream_reader.h
	${SOURCE_PATH}/stream_reader.cpp
	${HEADER_PATH}/stream_writer.h
	${SOURCE_PATH}/stream_writer.cpp
	${HEADER_PATH}/string_reader.h
	${SOURCE_PATH}/string_reader.cpp
	${HEADER_PATH}/string_writer.h
	${SOURCE_PATH}/string_writer.cpp
	${HEADER_PATH}/json_reader.h
	${SOURCE_PATH}/json_reader.cpp
	${HEADER_PATH}/json_writer.h
	${SOURCE_PATH}/json_writer.cpp
)
SOURCE_GROUP(${LIB_NAME}\\serialization FILES ${SERIALIZATION_LIST})

ADD_DEFINITIONS(-DOCTOON_BUILD_DLL_EXPORT)
ADD_LIBRARY(${LIB_OUTNAME} SHARED ${IO_LIST} ${STREAM_LIST} ${ARCHIVE_LIST} ${SERIALIZATION_LIST})

TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PRIVATE zipper)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PRIVATE octoon-runtime)

IF(OCTOON_BUILD_PLATFORM_ANDROID)
    TARGET_LINK_LIBRARIES (${LIB_OUTNAME} PRIVATE m)
ENDIF()

SET_TARGET_ATTRIBUTE(${LIB_OUTNAME} "core")

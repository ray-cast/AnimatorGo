SET(LIB_OUTNAME io)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(BASE_LIST
	${HEADER_PATH}/file.h
	${SOURCE_PATH}/file.cpp
	${HEADER_PATH}/iosbase.h
	${SOURCE_PATH}/iosbase.cpp
	${HEADER_PATH}/ioserver.h
	${SOURCE_PATH}/ioserver.cpp
	${HEADER_PATH}/ori.h
	${SOURCE_PATH}/ori.cpp
)
SOURCE_GROUP("io\\core" FILES ${BASE_LIST})

SET(STREAM_LIST
	${HEADER_PATH}/stream.h
	${SOURCE_PATH}/stream.cpp
	${HEADER_PATH}/istream.h
	${SOURCE_PATH}/istream.cpp
	${HEADER_PATH}/ostream.h
	${SOURCE_PATH}/ostream.cpp
	${HEADER_PATH}/iostream.h
	${SOURCE_PATH}/iostream.cpp
	${HEADER_PATH}/mstream.h
	${SOURCE_PATH}/mstream.cpp
	${HEADER_PATH}/fstream.h
	${SOURCE_PATH}/fstream.cpp
	${HEADER_PATH}/vstream.h
	${SOURCE_PATH}/vstream.cpp
	${HEADER_PATH}/http_stream.h
	${SOURCE_PATH}/http_stream.cpp
)
SOURCE_GROUP("io\\stream" FILES ${STREAM_LIST})

SET(BUFFER_LIST
	${HEADER_PATH}/stream_buf.h
	${SOURCE_PATH}/stream_buf.cpp
	${HEADER_PATH}/file_buf.h
	${SOURCE_PATH}/file_buf.cpp
	${HEADER_PATH}/membuf.h
	${SOURCE_PATH}/membuf.cpp
	${HEADER_PATH}/virtual_buf.h
	${SOURCE_PATH}/virtual_buf.cpp
	${HEADER_PATH}/http_buf.h
	${SOURCE_PATH}/http_buf.cpp
)
SOURCE_GROUP("io\\buffer" FILES ${BUFFER_LIST})

SET(ARCHIVE_LIST
	${HEADER_PATH}/archive.h
	${SOURCE_PATH}/archive.cpp
	${HEADER_PATH}/archive_buf.h
	${SOURCE_PATH}/archive_buf.cpp
	${HEADER_PATH}/iarchive.h
	${SOURCE_PATH}/iarchive.cpp
	${HEADER_PATH}/oarchive.h
	${SOURCE_PATH}/oarchive.cpp
	${HEADER_PATH}/json_reader.h
	${SOURCE_PATH}/json_reader.cpp
)
SOURCE_GROUP("io\\archive" FILES ${ARCHIVE_LIST})

SET(PACKAGE_LIST
	${HEADER_PATH}/package.h
	${SOURCE_PATH}/package.cpp
	${HEADER_PATH}/zpackage.h
	${SOURCE_PATH}/zpackage.cpp
	${HEADER_PATH}/fpackage.h
	${SOURCE_PATH}/fpackage.cpp
)
SOURCE_GROUP("io\\package" FILES ${PACKAGE_LIST})

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
)
SOURCE_GROUP("io\\serialization" FILES ${SERIALIZATION_LIST})

LIST(APPEND IO_LIST ${BASE_LIST} ${STREAM_LIST} ${BUFFER_LIST} ${PACKAGE_LIST} ${ARCHIVE_LIST} ${SERIALIZATION_LIST})
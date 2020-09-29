SET(LIB_OUTNAME audio)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(COMMON_LIST
	${HEADER_PATH}/common/audio_buffer.h
	${SOURCE_PATH}/common/audio_buffer.cpp
	${HEADER_PATH}/common/audio_device.h
	${SOURCE_PATH}/common/audio_device.cpp
	${HEADER_PATH}/common/audio_listener.h
	${SOURCE_PATH}/common/audio_listener.cpp
	${HEADER_PATH}/common/audio_reader.h
	${SOURCE_PATH}/common/audio_reader.cpp
	${HEADER_PATH}/common/audio_source.h
	${SOURCE_PATH}/common/audio_source.cpp
	${HEADER_PATH}/common/audio_source_listener.h
	${SOURCE_PATH}/common/audio_source_listener.cpp
	${HEADER_PATH}/common/audio_types.h
	${HEADER_PATH}/common/ogg_stream.h
	${SOURCE_PATH}/common/ogg_stream.cpp
)
SOURCE_GROUP("audio" FILES ${COMMON_LIST})

SET(AUDIO_OPENAL_LIST
	${HEADER_PATH}/openal/audio_device_al.h
	${SOURCE_PATH}/openal/audio_device_al.cpp
	${HEADER_PATH}/openal/audio_listener_al.h
	${SOURCE_PATH}/openal/audio_listener_al.cpp
	${HEADER_PATH}/openal/audio_source_al.h
	${SOURCE_PATH}/openal/audio_source_al.cpp
)
SOURCE_GROUP("audio\\openal" FILES ${AUDIO_OPENAL_LIST})

LIST(APPEND AUDIO_LIST ${COMMON_LIST} ${AUDIO_OPENAL_LIST})
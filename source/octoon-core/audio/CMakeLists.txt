SET(LIB_OUTNAME audio)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(AUDIO_LIST
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
)
SOURCE_GROUP("audio" FILES ${AUDIO_LIST})

LIST(APPEND AUDIO_LIST ${AUDIO_LIST})
SET(LIB_NAME audio)
SET(LIB_OUTNAME octoon-${LIB_NAME})

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_NAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/${LIB_OUTNAME})

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
SOURCE_GROUP(${LIB_NAME} FILES ${AUDIO_LIST})

ADD_DEFINITIONS(-DOCTOON_BUILD_DLL_EXPORT)
ADD_LIBRARY(${LIB_OUTNAME} STATIC ${AUDIO_LIST})

TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PRIVATE OpenAL)

IF(OCTOON_BUILD_PLATFORM_ANDROID)
    TARGET_LINK_LIBRARIES (${LIB_OUTNAME} PRIVATE m)
ENDIF()

TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${OCTOON_PATH_DEPENDENCIES}/openal-soft)

SET_TARGET_ATTRIBUTE(${LIB_OUTNAME} "core")
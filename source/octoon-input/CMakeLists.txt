SET(LIB_NAME input)
SET(LIB_OUTNAME octoon-${LIB_NAME})

OPTION(OCTOON_INPUT_BUILD_GLFW "on for use" OFF)

SET(HEADER_PATH ${OCTOON_HEADER_PATH}/${LIB_NAME})
SET(SOURCE_PATH ${OCTOON_SOURCE_PATH}/${LIB_OUTNAME})

SET(INPUT_CORE_LIST
    ${HEADER_PATH}/input_button.h
    ${SOURCE_PATH}/input_controller.cpp
    ${HEADER_PATH}/input_controller.h
    ${HEADER_PATH}/input_device.h
    ${SOURCE_PATH}/input_device.cpp
    ${SOURCE_PATH}/input_device_base.cpp
    ${HEADER_PATH}/input_device_base.h
    ${HEADER_PATH}/input_event.h
    ${SOURCE_PATH}/input_event.cpp
    ${HEADER_PATH}/input_key.h
    ${HEADER_PATH}/input_keyboard.h
    ${SOURCE_PATH}/input_keyboard.cpp
    ${HEADER_PATH}/input_keyboard_base.h
    ${SOURCE_PATH}/input_keyboard_base.cpp
    ${SOURCE_PATH}/input_mouse.cpp
    ${HEADER_PATH}/input_mouse.h
    ${HEADER_PATH}/input_mouse_base.h
    ${SOURCE_PATH}/input_mouse_base.cpp
    ${HEADER_PATH}/input.h
    ${SOURCE_PATH}/input.cpp
    ${HEADER_PATH}/input_base.h
    ${SOURCE_PATH}/input_base.cpp
    ${HEADER_PATH}/input_types.h
    ${HEADER_PATH}/input_ps3.h
    ${HEADER_PATH}/input_xbox.h
)
SOURCE_GROUP("input\\core" FILES ${INPUT_CORE_LIST})

SET(INPUT_MSW_LIST
    ${SOURCE_PATH}/msw_input_device.h
    ${SOURCE_PATH}/msw_input_device.cpp
    ${SOURCE_PATH}/msw_input_keyboard.h
    ${SOURCE_PATH}/msw_input_keyboard.cpp
    ${SOURCE_PATH}/msw_input_mouse.h
    ${SOURCE_PATH}/msw_input_mouse.cpp
)
SOURCE_GROUP("input\\msw" FILES ${INPUT_MSW_LIST})

SET(INPUT_NDK_LIST
    ${SOURCE_PATH}/ndk_input_device.h
    ${SOURCE_PATH}/ndk_input_device.cpp
    ${SOURCE_PATH}/ndk_input_keyboard.h
    ${SOURCE_PATH}/ndk_input_keyboard.cpp
    ${SOURCE_PATH}/ndk_input_mouse.h
    ${SOURCE_PATH}/ndk_input_mouse.cpp
)
SOURCE_GROUP("input\\ndk" FILES ${INPUT_NDK_LIST})

SET(INPUT_GLFW_LIST
    ${SOURCE_PATH}/glfw_input_device.h
    ${SOURCE_PATH}/glfw_input_device.cpp
    ${SOURCE_PATH}/glfw_input_keyboard.h
    ${SOURCE_PATH}/glfw_input_keyboard.cpp
    ${SOURCE_PATH}/glfw_input_mouse.h
    ${SOURCE_PATH}/glfw_input_mouse.cpp
)
SOURCE_GROUP("input\\glfw" FILES ${INPUT_GLFW_LIST})

IF(OCTOON_INPUT_BUILD_GLFW)
    ADD_LIBRARY(${LIB_OUTNAME} SHARED ${INPUT_CORE_LIST} ${INPUT_GLFW_LIST})
ELSEIF(BUILD_PLATFORM_WINDOWS)
    ADD_LIBRARY(${LIB_OUTNAME} SHARED ${INPUT_CORE_LIST} ${INPUT_MSW_LIST})
ELSEIF(BUILD_PLATFORM_LINUX)
    ADD_LIBRARY(${LIB_OUTNAME} SHARED ${INPUT_CORE_LIST})
ELSEIF(BUILD_PLATFORM_APPLE)
    ADD_LIBRARY(${LIB_OUTNAME} SHARED ${INPUT_CORE_LIST})
ENDIF()

ADD_DEFINITIONS(-DOCTOON_BUILD_DLL_EXPORT)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PUBLIC octoon-runtime)

IF(BUILD_PLATFORM_WINDOWS)
    TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PRIVATE winmm)
    TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PRIVATE imm32)
ENDIF()

IF(OCTOON_INPUT_BUILD_GLFW)
    TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PRIVATE glfw)
ENDIF()

SET_TARGET_ATTRIBUTE(${LIB_OUTNAME} "core")
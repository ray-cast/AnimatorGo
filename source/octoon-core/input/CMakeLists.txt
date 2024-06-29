SET(LIB_OUTNAME input)

SET(OCTOON_FEATURE_INPUT_API 0 CACHE STRING
"Specify the native api. Possible values:
  0 - Null
  1 - GLFW
  2 - AdnroidNDK
  3 - Windows"
)

IF(OCTOON_FEATURE_INPUT_API EQUAL 0)
    SET(OCTOON_FEATURE_INPUT_API_NULL TRUE)
    ADD_DEFINITIONS(-DOCTOON_FEATURE_INPUT_API_NULL)
ELSEIF(OCTOON_FEATURE_INPUT_API EQUAL 1)
    SET(OCTOON_FEATURE_INPUT_API_GLFW TRUE)
    ADD_DEFINITIONS(-DOCTOON_FEATURE_INPUT_API_GLFW)
ELSEIF(OCTOON_FEATURE_INPUT_API EQUAL 2)
    SET(OCTOON_FEATURE_INPUT_ANDROID TRUE)
    ADD_DEFINITIONS(-DOCTOON_FEATURE_INPUT_ANDROID)
ELSEIF(OCTOON_FEATURE_INPUT_API EQUAL 3)
    SET(OCTOON_FEATURE_INPUT_API_WINDOWS TRUE)
    ADD_DEFINITIONS(-DOCTOON_FEATURE_INPUT_API_WINDOWS)
ENDIF()

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(INPUT_CORE_LIST
	${HEADER_PATH}/input_button.h
	${SOURCE_PATH}/input_controller.cpp
	${HEADER_PATH}/input_controller.h
	${HEADER_PATH}/input_event.h
	${SOURCE_PATH}/input_event.cpp
	${HEADER_PATH}/input_key.h
	${HEADER_PATH}/input_types.h
	${HEADER_PATH}/input_ps3.h
	${HEADER_PATH}/input_xbox.h
	${HEADER_PATH}/input_map.h
	${SOURCE_PATH}/input_map.cpp
	${HEADER_PATH}/input_content.h
	${SOURCE_PATH}/input_content.cpp
	${HEADER_PATH}/input_manager.h
	${SOURCE_PATH}/input_manager.cpp
)
SOURCE_GROUP("input\\core" FILES ${INPUT_CORE_LIST})

SET(INPUT_INTERFACE_LIST
	${SOURCE_PATH}/iinput_device.cpp
	${HEADER_PATH}/iinput_device.h
	${HEADER_PATH}/iinput_keyboard.h
	${SOURCE_PATH}/iinput_keyboard.cpp
	${HEADER_PATH}/iinput_mouse.h
	${SOURCE_PATH}/iinput_mouse.cpp
	${HEADER_PATH}/iinput_listener.h
	${SOURCE_PATH}/iinput_listener.cpp
	${HEADER_PATH}/iinput.h
	${SOURCE_PATH}/iinput.cpp
)
SOURCE_GROUP("input\\interface" FILES ${INPUT_INTERFACE_LIST})

SET(INPUT_DEFAULT_LIST
	${HEADER_PATH}/input_device.h
	${SOURCE_PATH}/input_device.cpp
	${HEADER_PATH}/input_keyboard.h
	${SOURCE_PATH}/input_keyboard.cpp
	${HEADER_PATH}/input_mouse.h
	${SOURCE_PATH}/input_mouse.cpp
	${HEADER_PATH}/input.h
	${SOURCE_PATH}/input.cpp
)
SOURCE_GROUP("input\\default" FILES ${INPUT_DEFAULT_LIST})

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

IF(OCTOON_FEATURE_INPUT_API_GLFW)
	LIST(APPEND INPUT_LIST ${INPUT_CORE_LIST} ${INPUT_INTERFACE_LIST} ${INPUT_DEFAULT_LIST} ${INPUT_GLFW_LIST})
ELSEIF(OCTOON_FEATURE_INPUT_API_WINDOWS)
	LIST(APPEND INPUT_LIST ${INPUT_CORE_LIST} ${INPUT_INTERFACE_LIST} ${INPUT_DEFAULT_LIST} ${INPUT_MSW_LIST})
ELSEIF(OCTOON_FEATURE_INPUT_API_NULL)
	LIST(APPEND INPUT_LIST ${INPUT_CORE_LIST} ${INPUT_INTERFACE_LIST} ${INPUT_DEFAULT_LIST})
ELSE()
	MESSAGE("Not supported yet")
ENDIF()
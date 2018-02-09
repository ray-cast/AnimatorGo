SET(LIB_NAME octoon)

SET(HEADER_PATH ${OCTOON_HEADER_PATH})
SET(SOURCE_PATH ${OCTOON_SOURCE_PATH}/${LIB_NAME})

SET(OCTOON_LIST
    ${SOURCE_PATH}/game_application.cpp
    ${HEADER_PATH}/game_application.h
    ${SOURCE_PATH}/game_listener.cpp
    ${HEADER_PATH}/game_listener.h
    ${SOURCE_PATH}/game_object.cpp
    ${HEADER_PATH}/game_object.h
    ${SOURCE_PATH}/game_object_manager.cpp
    ${HEADER_PATH}/game_object_manager.h
    ${SOURCE_PATH}/game_component.cpp
    ${HEADER_PATH}/game_component.h
    ${SOURCE_PATH}/game_feature.cpp
    ${HEADER_PATH}/game_feature.h
    ${SOURCE_PATH}/game_server.cpp
    ${HEADER_PATH}/game_server.h
    ${SOURCE_PATH}/game_scene.cpp
    ${HEADER_PATH}/game_scene.h
    ${SOURCE_PATH}/game_scene_manager.cpp
    ${HEADER_PATH}/game_scene_manager.h
    ${HEADER_PATH}/game_types.h
)
SOURCE_GROUP("system\\app" FILES ${OCTOON_LIST})

SET(BASE_FEATURES_LIST
    ${HEADER_PATH}/transform_component.h
    ${SOURCE_PATH}/transform_component.cpp
    ${HEADER_PATH}/game_base_features.h
    ${SOURCE_PATH}/game_base_features.cpp
)
SOURCE_GROUP("system\\base" FILES ${BASE_FEATURES_LIST})

SET(INPUT_FEATURES_LIST
    ${HEADER_PATH}/input_feature.h
    ${SOURCE_PATH}/input_feature.cpp
)
SOURCE_GROUP("system\\input" FILES ${INPUT_FEATURES_LIST})

SET(GUI_FEATURES_LIST
    ${HEADER_PATH}/gui_feature.h
    ${SOURCE_PATH}/gui_feature.cpp
)
SOURCE_GROUP("system\\gui" FILES ${GUI_FEATURES_LIST})

ADD_DEFINITIONS(-DOCTOON_BUILD_DLL_EXPORT)
ADD_LIBRARY(${LIB_NAME} STATIC 
    ${OCTOON_LIST} 
    ${BASE_FEATURES_LIST} 
    ${INPUT_FEATURES_LIST} 
    ${GUI_FEATURES_LIST}
)

TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE octoon-math)
TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE octoon-input)
TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE octoon-runtime)

SET_TARGET_ATTRIBUTE(${LIB_NAME} "octoon")
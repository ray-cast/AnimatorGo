SET(LIB_NAME octoon)

SET(HEADER_PATH ${OCTOON_PATH_HEADER})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/${LIB_NAME})

SET(GAMEBASE_LIST
	${SOURCE_PATH}/game_app.cpp
	${HEADER_PATH}/game_app.h
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
SOURCE_GROUP("system\\app" FILES ${GAMEBASE_LIST})

SET(BASE_FEATURES_LIST
	${HEADER_PATH}/game_base_features.h
	${SOURCE_PATH}/game_base_features.cpp
	${HEADER_PATH}/transform_component.h
	${SOURCE_PATH}/transform_component.cpp
	${HEADER_PATH}/bone_controller_component.h
	${SOURCE_PATH}/bone_controller_component.cpp
	${HEADER_PATH}/mesh_filter_component.h
	${SOURCE_PATH}/mesh_filter_component.cpp
	${HEADER_PATH}/text_component.h
	${SOURCE_PATH}/text_component.cpp
	${HEADER_PATH}/path_component.h
	${SOURCE_PATH}/path_component.cpp
)
SOURCE_GROUP("system\\base" FILES ${BASE_FEATURES_LIST})

SET(VIDEO_FEATURES_LIST
	${HEADER_PATH}/camera_component.h
	${SOURCE_PATH}/camera_component.cpp
	${HEADER_PATH}/ortho_camera_component.h
	${SOURCE_PATH}/ortho_camera_component.cpp
	${HEADER_PATH}/perspective_camera_component.h
	${SOURCE_PATH}/perspective_camera_component.cpp
	${HEADER_PATH}/film_camera_component.h
	${SOURCE_PATH}/film_camera_component.cpp
	${HEADER_PATH}/render_component.h
	${SOURCE_PATH}/render_component.cpp
	${HEADER_PATH}/mesh_renderer_component.h
	${SOURCE_PATH}/mesh_renderer_component.cpp
	${HEADER_PATH}/skinned_mesh_renderer_component.h
	${SOURCE_PATH}/skinned_mesh_renderer_component.cpp
	${HEADER_PATH}/skinned_joint_renderer_component.h
	${SOURCE_PATH}/skinned_joint_renderer_component.cpp
	${HEADER_PATH}/video_feature.h
	${SOURCE_PATH}/video_feature.cpp
)
SOURCE_GROUP("system\\video" FILES ${VIDEO_FEATURES_LIST})

SET(INPUT_FEATURES_LIST
	${HEADER_PATH}/input_feature.h
	${SOURCE_PATH}/input_feature.cpp
	${HEADER_PATH}/editor_camera_component.h
	${SOURCE_PATH}/editor_camera_component.cpp
	${HEADER_PATH}/first_person_camera_component.h
	${SOURCE_PATH}/first_person_camera_component.cpp
)
SOURCE_GROUP("system\\input" FILES ${INPUT_FEATURES_LIST})

SET(GUI_FEATURES_LIST
	${HEADER_PATH}/gui_feature.h
	${SOURCE_PATH}/gui_feature.cpp
	${HEADER_PATH}/guizmo_component.h
	${SOURCE_PATH}/guizmo_component.cpp
)
SOURCE_GROUP("system\\gui" FILES ${GUI_FEATURES_LIST})

SET(GRAPHICS_FEATURES_LIST
	${HEADER_PATH}/hal_feature.h
	${SOURCE_PATH}/hal_feature.cpp
)
SOURCE_GROUP("system\\hal" FILES ${GRAPHICS_FEATURES_LIST})

SET(TIMER_FEATURES_LIST
	${HEADER_PATH}/timer_feature.h
	${SOURCE_PATH}/timer_feature.cpp
)
SOURCE_GROUP("system\\timer" FILES ${TIMER_FEATURES_LIST})

SET(IO_FEATURES_LIST
	${HEADER_PATH}/io_feature.h
	${SOURCE_PATH}/io_feature.cpp
	${HEADER_PATH}/game_prefabs.h
	${SOURCE_PATH}/game_prefabs.cpp
)
SOURCE_GROUP("system\\io" FILES ${IO_FEATURES_LIST})

SET(PHYSICS_FEATURES_LIST
	${HEADER_PATH}/physics_feature.h
	${SOURCE_PATH}/physics_feature.cpp
	${HEADER_PATH}/rigidbody_component.h
	${SOURCE_PATH}/rigidbody_component.cpp

	${HEADER_PATH}/collider_component.h
	${SOURCE_PATH}/collider_component.cpp
	${HEADER_PATH}/sphere_collider_component.h
	${SOURCE_PATH}/sphere_collider_component.cpp

	${HEADER_PATH}/joint_component.h
	${SOURCE_PATH}/joint_component.cpp
	${HEADER_PATH}/fixed_joint_component.h
	${SOURCE_PATH}/fixed_joint_component.cpp
	${HEADER_PATH}/configurable_joint_component.h
	${SOURCE_PATH}/configurable_joint_component.cpp
)
SOURCE_GROUP("system\\physics" FILES ${PHYSICS_FEATURES_LIST})

SET(ANIMATION_FEATURES_LIST
	${HEADER_PATH}/solver_component.h
	${SOURCE_PATH}/solver_component.cpp
	${HEADER_PATH}/animation_component.h
	${SOURCE_PATH}/animation_component.cpp
	${HEADER_PATH}/animator_component.h
	${SOURCE_PATH}/animator_component.cpp
	${HEADER_PATH}/rotation_limit_component.h
	${SOURCE_PATH}/rotation_limit_component.cpp
)
SOURCE_GROUP("system\\animation" FILES ${ANIMATION_FEATURES_LIST})

SET(OFFLINE_FEATURES_LIST
	${HEADER_PATH}/offline_camera_component.h
	${SOURCE_PATH}/offline_camera_component.cpp
	${HEADER_PATH}/offline_render_component.h
	${SOURCE_PATH}/offline_render_component.cpp
	${HEADER_PATH}/offline_mesh_renderer_component.h
	${SOURCE_PATH}/offline_mesh_renderer_component.cpp
	${HEADER_PATH}/offline_light_component.h
	${SOURCE_PATH}/offline_light_component.cpp
	${HEADER_PATH}/offline_environment_light_component.h
	${SOURCE_PATH}/offline_environment_light_component.cpp
	${HEADER_PATH}/offline_feature.h
	${SOURCE_PATH}/offline_feature.cpp
)
SOURCE_GROUP("system\\offline" FILES ${OFFLINE_FEATURES_LIST})

LIST(APPEND OCTOON_LIST ${GAMEBASE_LIST})
LIST(APPEND OCTOON_LIST ${OFFLINE_FEATURES_LIST})

IF(OCTOON_FEATURE_IO_ENABLE)
	LIST(APPEND OCTOON_LIST ${IO_FEATURES_LIST})
ENDIF()

IF(OCTOON_FEATURE_INPUT_ENABLE)
	LIST(APPEND OCTOON_LIST ${INPUT_FEATURES_LIST})
ENDIF()

IF(OCTOON_FEATURE_BASE_ENABLE)
	LIST(APPEND OCTOON_LIST ${BASE_FEATURES_LIST})
ENDIF()

IF(OCTOON_FEATURE_TIMER_ENABLE)
	LIST(APPEND OCTOON_LIST ${TIMER_FEATURES_LIST})
ENDIF()

IF(OCTOON_FEATURE_HAL_ENABLE)
	LIST(APPEND OCTOON_LIST ${GRAPHICS_FEATURES_LIST})
ENDIF()

IF(OCTOON_FEATURE_VIDEO_ENABLE)
	LIST(APPEND OCTOON_LIST ${VIDEO_FEATURES_LIST})
ENDIF()

IF(OCTOON_FEATURE_ANIMATION_ENABLE)
	LIST(APPEND OCTOON_LIST ${ANIMATION_FEATURES_LIST})
ENDIF()

IF(OCTOON_FEATURE_UI_ENABLE)
	LIST(APPEND OCTOON_LIST ${GUI_FEATURES_LIST})
ENDIF()

IF(OCTOON_FEATURE_PHYSICS_ENABLE)
	LIST(APPEND OCTOON_LIST ${PHYSICS_FEATURES_LIST})
ENDIF()

IF(OCTOON_BUILD_SHARED_DLL)
	ADD_DEFINITIONS(-DOCTOON_BUILD_DLL_EXPORT)
	ADD_LIBRARY(${LIB_NAME} SHARED ${OCTOON_LIST})
ELSE()
	ADD_DEFINITIONS(-DOCTOON_STATIC)
	ADD_LIBRARY(${LIB_NAME} STATIC ${OCTOON_LIST})
ENDIF()

TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-math)
TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-runtime)
TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-image)
TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC RadeonProRender64)

IF(OCTOON_FEATURE_ANIMATION_ENABLE)
	TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-animation)
ENDIF()

IF(OCTOON_FEATURE_MODEL_ENABLE)
	TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-model)
ENDIF()

IF(OCTOON_FEATURE_IO_ENABLE)
	TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-io)
ENDIF()

IF(OCTOON_FEATURE_INPUT_ENABLE)
	TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-input)
ENDIF()

IF(OCTOON_FEATURE_HAL_ENABLE)
	TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-hal)
ENDIF()

IF(OCTOON_FEATURE_VIDEO_ENABLE)
	TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-video)
ENDIF()

IF(OCTOON_FEATURE_UI_ENABLE)
	TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-ui)
ENDIF()

IF(OCTOON_FEATURE_PHYSICS_ENABLE)
	TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-physics)
ENDIF()

TARGET_INCLUDE_DIRECTORIES(${LIB_NAME} PRIVATE ${OCTOON_PATH_INCLUDE})
TARGET_INCLUDE_DIRECTORIES(${LIB_NAME} PRIVATE ${OCTOON_PATH_DEPENDENCIES}/json)

SET_TARGET_ATTRIBUTE(${LIB_NAME} "octoon")
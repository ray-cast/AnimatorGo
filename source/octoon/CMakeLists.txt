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
	${HEADER_PATH}/mesh_filter_component.h
	${SOURCE_PATH}/mesh_filter_component.cpp
	${HEADER_PATH}/text_component.h
	${SOURCE_PATH}/text_component.cpp
	${HEADER_PATH}/path_component.h
	${SOURCE_PATH}/path_component.cpp
)
SOURCE_GROUP("system\\base" FILES ${BASE_FEATURES_LIST})

SET(AUDIO_FEATURES_LIST
	${HEADER_PATH}/audio_feature.h
	${SOURCE_PATH}/audio_feature.cpp
	${HEADER_PATH}/audio_listener_component.h
	${SOURCE_PATH}/audio_listener_component.cpp
	${HEADER_PATH}/audio_source_component.h
	${SOURCE_PATH}/audio_source_component.cpp
)
SOURCE_GROUP("system\\audio" FILES ${AUDIO_FEATURES_LIST})

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
	${HEADER_PATH}/light_component.h
	${SOURCE_PATH}/light_component.cpp
	${HEADER_PATH}/ambient_light_component.h
	${SOURCE_PATH}/ambient_light_component.cpp
	${HEADER_PATH}/directional_light_component.h
	${SOURCE_PATH}/directional_light_component.cpp
	${HEADER_PATH}/point_light_component.h
	${SOURCE_PATH}/point_light_component.cpp
	${HEADER_PATH}/spot_light_component.h
	${SOURCE_PATH}/spot_light_component.cpp
	${HEADER_PATH}/rectangle_light_component.h
	${SOURCE_PATH}/rectangle_light_component.cpp
	${HEADER_PATH}/environment_light_component.h
	${SOURCE_PATH}/environment_light_component.cpp
	${HEADER_PATH}/video_feature.h
	${SOURCE_PATH}/video_feature.cpp
)
SOURCE_GROUP("system\\video" FILES ${VIDEO_FEATURES_LIST})

SET(HELPER_LIST
	${HEADER_PATH}/raycaster.h
	${SOURCE_PATH}/raycaster.cpp
	${HEADER_PATH}/ortho_camera_helper.h
	${SOURCE_PATH}/ortho_camera_helper.cpp
	${HEADER_PATH}/perspective_camera_helper.h
	${SOURCE_PATH}/perspective_camera_helper.cpp
	${HEADER_PATH}/film_camera_helper.h
	${SOURCE_PATH}/film_camera_helper.cpp
	${HEADER_PATH}/circle_helper.h
	${SOURCE_PATH}/circle_helper.cpp
	${HEADER_PATH}/plane_helper.h
	${SOURCE_PATH}/plane_helper.cpp
	${HEADER_PATH}/ring_helper.h
	${SOURCE_PATH}/ring_helper.cpp
	${HEADER_PATH}/sphere_helper.h
	${SOURCE_PATH}/sphere_helper.cpp
	${HEADER_PATH}/cube_helper.h
	${SOURCE_PATH}/cube_helper.cpp
	${HEADER_PATH}/cone_helper.h
	${SOURCE_PATH}/cone_helper.cpp
	${HEADER_PATH}/text_helper.h
	${SOURCE_PATH}/text_helper.cpp
	${HEADER_PATH}/volume_helper.h
	${SOURCE_PATH}/volume_helper.cpp
)
SOURCE_GROUP("system\\helper" FILES ${HELPER_LIST})

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
)
SOURCE_GROUP("system\\io" FILES ${IO_FEATURES_LIST})

SET(LOADER_LIST
	${HEADER_PATH}/mesh_loader.h
	${SOURCE_PATH}/mesh_loader.cpp
	${HEADER_PATH}/texture_loader.h
	${SOURCE_PATH}/texture_loader.cpp
	${HEADER_PATH}/vmd_loader.h
	${SOURCE_PATH}/vmd_loader.cpp
	${HEADER_PATH}/pmx_loader.h
	${SOURCE_PATH}/pmx_loader.cpp
	${HEADER_PATH}/mdl_loader.h
	${SOURCE_PATH}/mdl_loader.cpp
	${HEADER_PATH}/PMREM_loader.h
	${SOURCE_PATH}/PMREM_loader.cpp
	${HEADER_PATH}/ass_loader.h
	${SOURCE_PATH}/ass_loader.cpp
	${HEADER_PATH}/obj_loader.h
	${SOURCE_PATH}/obj_loader.cpp
	${HEADER_PATH}/audio_loader.h
	${SOURCE_PATH}/audio_loader.cpp
)
SOURCE_GROUP("system\\loader" FILES ${LOADER_LIST})

SET(PHYSICS_FEATURES_LIST
	${HEADER_PATH}/physics_feature.h
	${SOURCE_PATH}/physics_feature.cpp
	${HEADER_PATH}/rigidbody_component.h
	${SOURCE_PATH}/rigidbody_component.cpp

	${HEADER_PATH}/collider_component.h
	${SOURCE_PATH}/collider_component.cpp
	${HEADER_PATH}/sphere_collider_component.h
	${SOURCE_PATH}/sphere_collider_component.cpp
	${HEADER_PATH}/box_collider_component.h
	${SOURCE_PATH}/box_collider_component.cpp
	${HEADER_PATH}/capsule_collider_component.h
	${SOURCE_PATH}/capsule_collider_component.cpp

	${HEADER_PATH}/joint_component.h
	${SOURCE_PATH}/joint_component.cpp
	${HEADER_PATH}/fixed_joint_component.h
	${SOURCE_PATH}/fixed_joint_component.cpp
	${HEADER_PATH}/configurable_joint_component.h
	${SOURCE_PATH}/configurable_joint_component.cpp

	${HEADER_PATH}/physic_material.h
	${SOURCE_PATH}/physic_material.cpp

	${HEADER_PATH}/cloth_component.h
	${SOURCE_PATH}/cloth_component.cpp
	${HEADER_PATH}/cloth_feature.h
	${SOURCE_PATH}/cloth_feature.cpp
)
SOURCE_GROUP("system\\physics" FILES ${PHYSICS_FEATURES_LIST})

SET(ANIMATION_FEATURES_LIST
	${HEADER_PATH}/solver_component.h
	${SOURCE_PATH}/solver_component.cpp
	${HEADER_PATH}/animator_component.h
	${SOURCE_PATH}/animator_component.cpp
	${HEADER_PATH}/animation_component.h
	${SOURCE_PATH}/animation_component.cpp
	${HEADER_PATH}/rotation_limit_component.h
	${SOURCE_PATH}/rotation_limit_component.cpp
	${HEADER_PATH}/rotation_link_component.h
	${SOURCE_PATH}/rotation_link_component.cpp
	${HEADER_PATH}/rotation_link_limit_component.h
	${SOURCE_PATH}/rotation_link_limit_component.cpp
	${HEADER_PATH}/skinned_component.h
	${SOURCE_PATH}/skinned_component.cpp
	${HEADER_PATH}/skinned_morph_component.h
	${SOURCE_PATH}/skinned_morph_component.cpp
	${HEADER_PATH}/skinned_bone_component.h
	${SOURCE_PATH}/skinned_bone_component.cpp
	${HEADER_PATH}/skinned_texture_component.h
	${SOURCE_PATH}/skinned_texture_component.cpp
	${HEADER_PATH}/mesh_animation_component.h
	${SOURCE_PATH}/mesh_animation_component.cpp	
)
SOURCE_GROUP("system\\animation" FILES ${ANIMATION_FEATURES_LIST})

LIST(APPEND OCTOON_LIST ${GAMEBASE_LIST})
LIST(APPEND OCTOON_LIST ${HELPER_LIST})
LIST(APPEND OCTOON_LIST ${LOADER_LIST})

IF(OCTOON_FEATURE_IO_ENABLE)
	LIST(APPEND OCTOON_LIST ${IO_FEATURES_LIST})
ENDIF()

IF(OCTOON_FEATURE_INPUT_ENABLE)
	LIST(APPEND OCTOON_LIST ${INPUT_FEATURES_LIST})
ENDIF()

IF(OCTOON_FEATURE_BASE_ENABLE)
	LIST(APPEND OCTOON_LIST ${BASE_FEATURES_LIST})
ENDIF()

IF(OCTOON_FEATURE_AUDIO_ENABLE)
	LIST(APPEND OCTOON_LIST ${AUDIO_FEATURES_LIST})
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
	ADD_LIBRARY(${LIB_NAME} SHARED ${OCTOON_LIST})
ELSE()
	ADD_LIBRARY(${LIB_NAME} STATIC ${OCTOON_LIST})
ENDIF()

IF(OCTOON_BUILD_SHARED_DLL)
	TARGET_COMPILE_DEFINITIONS(${LIB_NAME} PRIVATE OCTOON_BUILD_DLL_EXPORT)
ELSE()
	TARGET_COMPILE_DEFINITIONS(${LIB_NAME} PRIVATE OCTOON_STATIC)
ENDIF()

FIND_PACKAGE(Threads)
# FIND_PACKAGE(IlmBase REQUIRED)
FIND_PACKAGE(Alembic REQUIRED)
FIND_PACKAGE(tinyobjloader REQUIRED)
FIND_PACKAGE(Iconv REQUIRED)
FIND_PACKAGE(Physx REQUIRED)
FIND_PACKAGE(nvcloth REQUIRED)

TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC octoon-core)

TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE ${nvcloth_LIBRARIES})
TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE ${PhysX_LIBRARIES})
TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE ${Alembic_LIBRARIES})
TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE ${tinyobjloader_LIBRARIES})
TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE ${Iconv_LIBRARIES})

TARGET_INCLUDE_DIRECTORIES(${LIB_NAME} PRIVATE ${OCTOON_PATH_INCLUDE})
TARGET_INCLUDE_DIRECTORIES(${LIB_NAME} PRIVATE ${OCTOON_PATH_DEPENDENCIES}/mdl/include)
TARGET_INCLUDE_DIRECTORIES(${LIB_NAME} PRIVATE ${PhysX_INCLUDE_DIRS})
TARGET_INCLUDE_DIRECTORIES(${LIB_NAME} PRIVATE ${nvcloth_INCLUDE_DIRS})
TARGET_INCLUDE_DIRECTORIES(${LIB_NAME} PRIVATE ${tinyobjloader_INCLUDE_DIRS})
TARGET_INCLUDE_DIRECTORIES(${LIB_NAME} PRIVATE ${Iconv_INCLUDE_DIRS})
TARGET_INCLUDE_DIRECTORIES(${LIB_NAME} PRIVATE ${Alembic_INCLUDE_DIRS})

IF(OCTOON_BUILD_PLATFORM_WINDOWS)
	TARGET_COPY_TO_OUTPUT_DIR(TARGET ${LIB_NAME} FILES 
		${OCTOON_PATH_DEPENDENCIES}/mdl/nt-x86-64/lib/dds.dll
		${OCTOON_PATH_DEPENDENCIES}/mdl/nt-x86-64/lib/libmdl_sdk.dll
		${OCTOON_PATH_DEPENDENCIES}/mdl/nt-x86-64/lib/nv_freeimage.dll
	)
ENDIF()

SET_TARGET_ATTRIBUTE(${LIB_NAME} "octoon")
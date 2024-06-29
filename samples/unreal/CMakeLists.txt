SET(LIB_NAME launcher)
SET(LIB_OUTNAME octoon-${LIB_NAME})

SET(HEADER_PATH ${OCTOON_PATH_HEADER})
SET(SOURCE_PATH ${OCTOON_PATH_SAMPLES}/unreal)

SET(CMAKE_AUTOMOC ON)

SET(MAIN_LIST
	${SOURCE_PATH}/main.cpp
	${SOURCE_PATH}/unreal_version.h
	${SOURCE_PATH}/unreal_behaviour.h
	${SOURCE_PATH}/unreal_behaviour.cpp
	${SOURCE_PATH}/unreal_component.h
	${SOURCE_PATH}/unreal_component.cpp
	${SOURCE_PATH}/unreal_profile.h
	${SOURCE_PATH}/unreal_profile.cpp
	${SOURCE_PATH}/unreal_context.h
	${SOURCE_PATH}/unreal_context.cpp
	${SOURCE_PATH}/unreal_model.h
	${SOURCE_PATH}/unreal_model.cpp
	${SOURCE_PATH}/resource.rc
)
SOURCE_GROUP("launcher" FILES ${MAIN_LIST})

SET(MODULES_LIST
	${SOURCE_PATH}/module/sound_module.h
	${SOURCE_PATH}/module/sound_module.cpp
	${SOURCE_PATH}/module/camera_module.h
	${SOURCE_PATH}/module/camera_module.cpp
	${SOURCE_PATH}/module/main_light_module.h
	${SOURCE_PATH}/module/main_light_module.cpp
	${SOURCE_PATH}/module/environment_module.h
	${SOURCE_PATH}/module/environment_module.cpp
	${SOURCE_PATH}/module/record_module.h
	${SOURCE_PATH}/module/record_module.cpp
	${SOURCE_PATH}/module/entities_module.h
	${SOURCE_PATH}/module/entities_module.cpp
	${SOURCE_PATH}/module/offline_module.h
	${SOURCE_PATH}/module/offline_module.cpp
	${SOURCE_PATH}/module/encode_module.h
	${SOURCE_PATH}/module/encode_module.cpp
	${SOURCE_PATH}/module/physics_module.h
	${SOURCE_PATH}/module/physics_module.cpp
	${SOURCE_PATH}/module/player_module.h
	${SOURCE_PATH}/module/player_module.cpp
	${SOURCE_PATH}/module/mark_module.h
	${SOURCE_PATH}/module/mark_module.cpp
	${SOURCE_PATH}/module/client_module.h
	${SOURCE_PATH}/module/client_module.cpp
	${SOURCE_PATH}/module/resource_module.h
	${SOURCE_PATH}/module/resource_module.cpp
	${SOURCE_PATH}/module/selector_module.h
	${SOURCE_PATH}/module/selector_module.cpp
	${SOURCE_PATH}/module/grid_module.h
	${SOURCE_PATH}/module/grid_module.cpp
)
SOURCE_GROUP("launcher\\module" FILES ${MODULES_LIST})


SET(WIDGETS_LIST
	${SOURCE_PATH}/widgets/draggable_list_widget.h
	${SOURCE_PATH}/widgets/draggable_list_widget.cpp
	${SOURCE_PATH}/widgets/draggable_button.h
	${SOURCE_PATH}/widgets/draggable_button.cpp
	${SOURCE_PATH}/widgets/color_dialog.h
	${SOURCE_PATH}/widgets/color_dialog.cpp
	${SOURCE_PATH}/widgets/spoiler.h
	${SOURCE_PATH}/widgets/spoiler.cpp
	${SOURCE_PATH}/widgets/udockwidget.h
	${SOURCE_PATH}/widgets/udockwidget.cpp
	${SOURCE_PATH}/widgets/utranslatable.h
	${SOURCE_PATH}/widgets/utranslatable.cpp
	${SOURCE_PATH}/widgets/upanel.h
	${SOURCE_PATH}/widgets/upanel.cpp
	${SOURCE_PATH}/widgets/uspinbox.h
	${SOURCE_PATH}/widgets/uspinbox.cpp
	${SOURCE_PATH}/widgets/udoublespinbox.h
	${SOURCE_PATH}/widgets/udoublespinbox.cpp
	${SOURCE_PATH}/widgets/uspinline.h
	${SOURCE_PATH}/widgets/uspinline.cpp
	${SOURCE_PATH}/widgets/udoublespinline.h
	${SOURCE_PATH}/widgets/udoublespinline.cpp
)
SOURCE_GROUP("launcher\\widgets" FILES ${WIDGETS_LIST})

SET(VIEWS_LIST
	${SOURCE_PATH}/views/asset_browse_dock.h
	${SOURCE_PATH}/views/asset_browse_dock.cpp
	${SOURCE_PATH}/views/asset_browse_dialog.h
	${SOURCE_PATH}/views/asset_browse_dialog.cpp
	${SOURCE_PATH}/views/splash_screen.h
	${SOURCE_PATH}/views/splash_screen.cpp
	${SOURCE_PATH}/views/main_dock.h
	${SOURCE_PATH}/views/main_dock.cpp
	${SOURCE_PATH}/views/view_dock.h
	${SOURCE_PATH}/views/view_dock.cpp
	${SOURCE_PATH}/views/main_light_dock.h
	${SOURCE_PATH}/views/main_light_dock.cpp
	${SOURCE_PATH}/views/environment_dock.h
	${SOURCE_PATH}/views/environment_dock.cpp
	${SOURCE_PATH}/views/model_dock.h
	${SOURCE_PATH}/views/model_dock.cpp
	${SOURCE_PATH}/views/texture_dock.h
	${SOURCE_PATH}/views/texture_dock.cpp
	${SOURCE_PATH}/views/motion_dock.h
	${SOURCE_PATH}/views/motion_dock.cpp
	${SOURCE_PATH}/views/toplevel_bar.h
	${SOURCE_PATH}/views/toplevel_bar.cpp
	${SOURCE_PATH}/views/tool_dock.h
	${SOURCE_PATH}/views/tool_dock.cpp
	${SOURCE_PATH}/views/status_bar.h
	${SOURCE_PATH}/views/status_bar.cpp
	${SOURCE_PATH}/views/record_dock.h
	${SOURCE_PATH}/views/record_dock.cpp
	${SOURCE_PATH}/views/camera_dock.h
	${SOURCE_PATH}/views/camera_dock.cpp
	${SOURCE_PATH}/views/material_dock.h
	${SOURCE_PATH}/views/material_dock.cpp
	${SOURCE_PATH}/views/material_edit_dock.h
	${SOURCE_PATH}/views/material_edit_dock.cpp
	${SOURCE_PATH}/views/inspector_dock.h
	${SOURCE_PATH}/views/inspector_dock.cpp
	${SOURCE_PATH}/views/light_dock.h
	${SOURCE_PATH}/views/light_dock.cpp
	${SOURCE_PATH}/views/setting_window.h
	${SOURCE_PATH}/views/setting_window.cpp
	${SOURCE_PATH}/views/title_bar.h
	${SOURCE_PATH}/views/title_bar.cpp
	${SOURCE_PATH}/views/login_window.h
	${SOURCE_PATH}/views/login_window.cpp
	${SOURCE_PATH}/views/info_window.h
	${SOURCE_PATH}/views/info_window.cpp
)
SOURCE_GROUP("launcher\\views" FILES ${VIEWS_LIST})

SET(VIEWMODEL_LIST
	${SOURCE_PATH}/viewmodel/mutable_live_data.h
)
SOURCE_GROUP("launcher\\viewmodel" FILES ${VIEWMODEL_LIST})

SET(CONTROLLERS_LIST
	${SOURCE_PATH}/controllers/camera_component.h
	${SOURCE_PATH}/controllers/camera_component.cpp
	${SOURCE_PATH}/controllers/player_component.h
	${SOURCE_PATH}/controllers/player_component.cpp
	${SOURCE_PATH}/controllers/entities_component.h
	${SOURCE_PATH}/controllers/entities_component.cpp
	${SOURCE_PATH}/controllers/environment_component.h
	${SOURCE_PATH}/controllers/environment_component.cpp
	${SOURCE_PATH}/controllers/main_light_component.h
	${SOURCE_PATH}/controllers/main_light_component.cpp
	${SOURCE_PATH}/controllers/offline_component.h
	${SOURCE_PATH}/controllers/offline_component.cpp
	${SOURCE_PATH}/controllers/record_component.h
	${SOURCE_PATH}/controllers/record_component.cpp
	${SOURCE_PATH}/controllers/sound_component.h
	${SOURCE_PATH}/controllers/sound_component.cpp
	${SOURCE_PATH}/controllers/h264_component.h
	${SOURCE_PATH}/controllers/h264_component.cpp
	${SOURCE_PATH}/controllers/h265_component.h
	${SOURCE_PATH}/controllers/h265_component.cpp
	${SOURCE_PATH}/controllers/frame_sequence_component.h
	${SOURCE_PATH}/controllers/frame_sequence_component.cpp
	${SOURCE_PATH}/controllers/mark_component.h
	${SOURCE_PATH}/controllers/mark_component.cpp
	${SOURCE_PATH}/controllers/client_component.h
	${SOURCE_PATH}/controllers/client_component.cpp
	${SOURCE_PATH}/controllers/selector_component.h
	${SOURCE_PATH}/controllers/selector_component.cpp
	${SOURCE_PATH}/controllers/grid_component.h
	${SOURCE_PATH}/controllers/grid_component.cpp
	${SOURCE_PATH}/controllers/gizmo_component.h
	${SOURCE_PATH}/controllers/gizmo_component.cpp
	${SOURCE_PATH}/controllers/light_component.h
	${SOURCE_PATH}/controllers/light_component.cpp
	${SOURCE_PATH}/controllers/material_component.h
	${SOURCE_PATH}/controllers/material_component.cpp
)
SOURCE_GROUP("launcher\\controllers" FILES ${CONTROLLERS_LIST})

SET(UTILS_LIST
	${SOURCE_PATH}/utils/pmm.h
	${SOURCE_PATH}/utils/pmm.cpp
	${SOURCE_PATH}/utils/pmm_loader.h
	${SOURCE_PATH}/utils/pmm_loader.cpp
	${SOURCE_PATH}/utils/ass_loader.h
	${SOURCE_PATH}/utils/ass_loader.cpp
	${SOURCE_PATH}/utils/asset_library.h
	${SOURCE_PATH}/utils/asset_library.cpp
	${SOURCE_PATH}/utils/material_importer.h
	${SOURCE_PATH}/utils/material_importer.cpp
)
SOURCE_GROUP("launcher\\utils" FILES ${UTILS_LIST})

SET(BINDINGS_LIST
	${SOURCE_PATH}/bindings/application.h
	${SOURCE_PATH}/bindings/application.cpp
	${SOURCE_PATH}/bindings/timer.h
)
SOURCE_GROUP("launcher\\bindings" FILES ${BINDINGS_LIST})

SET(MINIH264E_LIST 
	libs/minih264e/minih264e.h
)
SOURCE_GROUP("launcher\\libs\\minih264e" FILES ${MINIH264E_LIST})

SET(QRCODEGEN_LIST 
	libs/qrcodegen/qrcodegen.h
	libs/qrcodegen/qrcodegen.c
)
SOURCE_GROUP("launcher\\libs\\qrcodegen" FILES ${QRCODEGEN_LIST})

SET(OIDN_LIST 
	libs/oidn-1.4.3/include/OpenImageDenoise/oidn.h
	libs/oidn-1.4.3/include/OpenImageDenoise/oidn.hpp
	libs/oidn-1.4.3/include/OpenImageDenoise/config.h
)
SOURCE_GROUP("launcher\\libs\\oidn" FILES ${OIDN_LIST})

# SET(Qt6_DIR "" CACHE PATH "Directory that contains Qt for the selected compiler, e.g., ../Qt/6.3.0/msvc2017_64")
# work around
IF(POLICY CMP0057)
	cmake_policy(SET CMP0057 NEW)
ENDIF()
SET(QT_NO_CREATE_VERSIONLESS_FUNCTIONS True)

ADD_DEFINITIONS(-DQT_NO_SIGNALS_SLOTS_KEYWORDS)
FIND_PACKAGE(Qt6 REQUIRED COMPONENTS Core Network Widgets Qml)
IF(NOT ${Qt6_FOUND})
    MESSAGE(FATAL_ERROR "The dependency \"qt\" could not be resolved. Install Qt on your system or specify the 'Qt6_DIR' variable.")
ENDIF()

Qt6_ADD_RESOURCES(RESOURCES res.qrc)
SET_PROPERTY(SOURCE qrc_res.cpp PROPERTY SKIP_AUTOMOC ON)

ADD_EXECUTABLE(${LIB_OUTNAME} 
	${MAIN_LIST}
	${WIDGETS_LIST}
	${VIEWS_LIST} 
	${VIEWMODEL_LIST}
	${MODULES_LIST} 
	${CONTROLLERS_LIST}
	${UTILS_LIST}
	${MINIH264E_LIST}
	${OIDN_LIST}
	${BINDINGS_LIST}
	${QRCODEGEN_LIST}
	${RESOURCES}
)

IF(NOT OCTOON_BUILD_SHARED_DLL AND OCTOON_BUILD_PLATFORM_WINDOWS)
	TARGET_COMPILE_DEFINITIONS(${LIB_OUTNAME} PRIVATE OCTOON_STATIC)
ENDIF()

TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${SOURCE_PATH})
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${SOURCE_PATH}/libs)
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${SOURCE_PATH}/libs/oidn-1.4.3/include)
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${OCTOON_PATH_INCLUDE})

CONAN_TARGET_LINK_LIBRARIES(${LIB_OUTNAME})

TARGET_LINK_LIBRARIES(${LIB_OUTNAME} octoon)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} ${SOURCE_PATH}/libs/oidn-1.4.3/lib/tbb.lib)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} ${SOURCE_PATH}/libs/oidn-1.4.3/lib/OpenImageDenoise.lib)

TARGET_COPY_TO_OUTPUT_DIR(TARGET ${LIB_OUTNAME} FILES
		${CMAKE_CURRENT_SOURCE_DIR}/libs/oidn-1.4.3/bin/OpenImageDenoise.dll
		${CMAKE_CURRENT_SOURCE_DIR}/libs/oidn-1.4.3/bin/tbb12.dll
	)

IF(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
	SET(LINK_FLAGS "-s DISABLE_EXCEPTION_CATCHING=0 -s USE_WEBGL2=1 -s FULL_ES2=1 -s FULL_ES3=1 --preload-file ${PROJECT_SOURCE_DIR}/lib/system@/system")
	SET_TARGET_PROPERTIES(${LIB_OUTNAME} PROPERTIES LINK_FLAGS "${LINK_FLAGS}")
	SET_TARGET_PROPERTIES(${LIB_OUTNAME} PROPERTIES LINK_FLAGS_DEBUG "${LINK_FLAGS} -O0 -g4 --emrun")
	SET_TARGET_PROPERTIES(${LIB_OUTNAME} PROPERTIES LINK_FLAGS_RELEASE "${LINK_FLAGS} -O3 -g0")
	SET_TARGET_PROPERTIES(${LIB_OUTNAME} PROPERTIES LINK_FLAGS_MINSIZEREL "${LINK_FLAGS} -O3 -g0")
	SET_TARGET_PROPERTIES(${LIB_OUTNAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "${LINK_FLAGS} -O3 -g3")

	SET(runner_in ${OCTOON_PATH_BIN}/system/web/runner.html.in)

	SET(emscripten_js_path "${LIB_OUTNAME}${CMAKE_DEBUG_POSTFIX}.js")
	CONFIGURE_FILE(${runner_in} ${OCTOON_LIBRARY_OUTPUT_PATH_DEBUG}/${LIB_OUTNAME}${CMAKE_DEBUG_POSTFIX}.html @ONLY)

	SET(emscripten_js_path "${LIB_OUTNAME}${CMAKE_RELEASE_POSTFIX}.js")
	CONFIGURE_FILE(${runner_in} ${OCTOON_LIBRARY_OUTPUT_PATH_RELEASE}/${LIB_OUTNAME}${CMAKE_RELEASE_POSTFIX}.html @ONLY)

	SET(emscripten_js_path "${LIB_OUTNAME}${CMAKE_RELWITHDEBINFO_POSTFIX}.js")
	CONFIGURE_FILE(${runner_in} ${OCTOON_LIBRARY_OUTPUT_PATH_RELWITHDEBINFO}/${LIB_OUTNAME}${CMAKE_RELWITHDEBINFO_POSTFIX}.html @ONLY)

	SET(emscripten_js_path "${LIB_OUTNAME}${CMAKE_MINSIZEREL_POSTFIX}.js")
	CONFIGURE_FILE(${runner_in} ${OCTOON_LIBRARY_OUTPUT_PATH_MINSIZEREL}/${LIB_OUTNAME}${CMAKE_MINSIZEREL_POSTFIX}.html @ONLY)
ENDIF()

FIND_PROGRAM(OCTOON_DEPLOYQT_EXECUTABLE_DEBUG NAMES windeployqt macdeployqt HINTS ${CONAN_BIN_DIRS_QT_DEBUG} NO_CMAKE_PATH)
FIND_PROGRAM(OCTOON_DEPLOYQT_EXECUTABLE_RELEASE NAMES windeployqt macdeployqt HINTS ${CONAN_BIN_DIRS_QT_RELEASE} NO_CMAKE_PATH)

ADD_CUSTOM_COMMAND(TARGET ${LIB_OUTNAME}
	PRE_BUILD
	COMMAND python update_languages.py
	COMMAND python deploy_languages.py
	WORKING_DIRECTORY ${SOURCE_PATH}/languages
)

ADD_CUSTOM_COMMAND(TARGET ${LIB_OUTNAME}
	POST_BUILD
	COMMAND
	$<$<CONFIG:Debug>:${OCTOON_DEPLOYQT_EXECUTABLE_DEBUG}>$<$<CONFIG:Release>:${OCTOON_DEPLOYQT_EXECUTABLE_RELEASE}>
	$<TARGET_FILE:${LIB_OUTNAME}>
	$<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>
	--qmldir ${SOURCE_PATH}/res/qml
)

SET_TARGET_ATTRIBUTE(${LIB_OUTNAME} "samples")
SET_TARGET_PROPERTIES(${LIB_OUTNAME} PROPERTIES OUTPUT_NAME "launcher")
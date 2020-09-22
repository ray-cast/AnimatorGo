SET(LIB_NAME rabbit)
SET(LIB_OUTNAME octoon-${LIB_NAME})

SET(HEADER_PATH ${OCTOON_PATH_HEADER})
SET(SOURCE_PATH ${OCTOON_PATH_SAMPLES}/${LIB_NAME})

SET(CMAKE_AUTOMOC ON)

SET(MAIN_LIST
	${SOURCE_PATH}/main.cpp
	${SOURCE_PATH}/rabbit_behaviour.h
	${SOURCE_PATH}/rabbit_behaviour.cpp
	${SOURCE_PATH}/rabbit_component.h
	${SOURCE_PATH}/rabbit_component.cpp
	${SOURCE_PATH}/rabbit_profile.h
	${SOURCE_PATH}/rabbit_profile.cpp
	${SOURCE_PATH}/rabbit_context.h
	${SOURCE_PATH}/rabbit_context.cpp
	${SOURCE_PATH}/rabbit_model.h
	${SOURCE_PATH}/rabbit_model.cpp
	${SOURCE_PATH}/resource.rc
)
SOURCE_GROUP("rabbit" FILES ${MAIN_LIST})

SET(MODULES_LIST
	${SOURCE_PATH}/module/file_module.h
	${SOURCE_PATH}/module/file_module.cpp
	${SOURCE_PATH}/module/sun_module.h
	${SOURCE_PATH}/module/sun_module.cpp
	${SOURCE_PATH}/module/environment_module.h
	${SOURCE_PATH}/module/environment_module.cpp
	${SOURCE_PATH}/module/canvas_module.h
	${SOURCE_PATH}/module/canvas_module.cpp
	${SOURCE_PATH}/module/entities_module.h
	${SOURCE_PATH}/module/entities_module.cpp
	${SOURCE_PATH}/module/offline_module.h
	${SOURCE_PATH}/module/offline_module.cpp
	${SOURCE_PATH}/module/h265_module.h
	${SOURCE_PATH}/module/h265_module.cpp
	${SOURCE_PATH}/module/denoise_module.h
	${SOURCE_PATH}/module/denoise_module.cpp
	${SOURCE_PATH}/module/physics_module.h
	${SOURCE_PATH}/module/physics_module.cpp
	${SOURCE_PATH}/module/time_module.h
	${SOURCE_PATH}/module/time_module.cpp
	${SOURCE_PATH}/module/mark_module.h
	${SOURCE_PATH}/module/mark_module.cpp
	${SOURCE_PATH}/module/client_module.h
	${SOURCE_PATH}/module/client_module.cpp
	${SOURCE_PATH}/module/material_module.h
	${SOURCE_PATH}/module/material_module.cpp
	${SOURCE_PATH}/module/drag_module.h
	${SOURCE_PATH}/module/drag_module.cpp
	${SOURCE_PATH}/module/grid_module.h
	${SOURCE_PATH}/module/grid_module.cpp
)
SOURCE_GROUP("rabbit\\module" FILES ${MODULES_LIST})

SET(VIEWS_LIST
	${SOURCE_PATH}/views/view_window.h
	${SOURCE_PATH}/views/view_window.cpp
	${SOURCE_PATH}/views/sun_window.h
	${SOURCE_PATH}/views/sun_window.cpp
	${SOURCE_PATH}/views/environment_window.h
	${SOURCE_PATH}/views/environment_window.cpp
	${SOURCE_PATH}/views/splash_screen.h
	${SOURCE_PATH}/views/splash_screen.cpp
	${SOURCE_PATH}/views/color_dialog.h
	${SOURCE_PATH}/views/color_dialog.cpp
	${SOURCE_PATH}/views/title_window.h
	${SOURCE_PATH}/views/title_window.cpp
	${SOURCE_PATH}/views/tool_window.h
	${SOURCE_PATH}/views/tool_window.cpp
	${SOURCE_PATH}/views/hide_window.h
	${SOURCE_PATH}/views/hide_window.cpp
	${SOURCE_PATH}/views/main_window.h
	${SOURCE_PATH}/views/main_window.cpp
	${SOURCE_PATH}/views/setting_window.h
	${SOURCE_PATH}/views/setting_window.cpp
	${SOURCE_PATH}/views/title_bar.h
	${SOURCE_PATH}/views/title_bar.cpp
	${SOURCE_PATH}/views/login_window.h
	${SOURCE_PATH}/views/login_window.cpp
	${SOURCE_PATH}/views/info_window.h
	${SOURCE_PATH}/views/info_window.cpp
	${SOURCE_PATH}/views/record_window.h
	${SOURCE_PATH}/views/record_window.cpp
	${SOURCE_PATH}/views/material_window.h
	${SOURCE_PATH}/views/material_window.cpp
	${SOURCE_PATH}/views/spoiler.h
	${SOURCE_PATH}/views/spoiler.cpp
)
SOURCE_GROUP("rabbit\\views" FILES ${VIEWS_LIST})

SET(CONTROLLERS_LIST
	${SOURCE_PATH}/controllers/camera_window.h
	${SOURCE_PATH}/controllers/camera_window.cpp
	${SOURCE_PATH}/controllers/theme_manager.h
	${SOURCE_PATH}/controllers/theme_manager.cpp
	${SOURCE_PATH}/controllers/ui_component.h
	${SOURCE_PATH}/controllers/ui_component.cpp
	${SOURCE_PATH}/controllers/player_component.h
	${SOURCE_PATH}/controllers/player_component.cpp
	${SOURCE_PATH}/controllers/entities_component.h
	${SOURCE_PATH}/controllers/entities_component.cpp
	${SOURCE_PATH}/controllers/offline_component.h
	${SOURCE_PATH}/controllers/offline_component.cpp
	${SOURCE_PATH}/controllers/canvas_component.h
	${SOURCE_PATH}/controllers/canvas_component.cpp
	${SOURCE_PATH}/controllers/h264_component.h
	${SOURCE_PATH}/controllers/h264_component.cpp
	${SOURCE_PATH}/controllers/h265_component.h
	${SOURCE_PATH}/controllers/h265_component.cpp
	${SOURCE_PATH}/controllers/denoise_component.h
	${SOURCE_PATH}/controllers/denoise_component.cpp
	${SOURCE_PATH}/controllers/mark_component.h
	${SOURCE_PATH}/controllers/mark_component.cpp
	${SOURCE_PATH}/controllers/client_component.h
	${SOURCE_PATH}/controllers/client_component.cpp
	${SOURCE_PATH}/controllers/material_component.h
	${SOURCE_PATH}/controllers/material_component.cpp
	${SOURCE_PATH}/controllers/drag_component.h
	${SOURCE_PATH}/controllers/drag_component.cpp
	${SOURCE_PATH}/controllers/grid_component.h
	${SOURCE_PATH}/controllers/grid_component.cpp
)
SOURCE_GROUP("rabbit\\controllers" FILES ${CONTROLLERS_LIST})

SET(UTILS_LIST
	${SOURCE_PATH}/utils/pmm.h
	${SOURCE_PATH}/utils/pmm.cpp
)
SOURCE_GROUP("rabbit\\utils" FILES ${UTILS_LIST})

SET(MINIH264E_LIST 
	libs/minih264e/minih264e.h
)
SOURCE_GROUP("rabbit\\libs\\minih264e" FILES ${MINIH264E_LIST})

SET(BASE64_LIST 
	libs/base64/base64.h
	libs/base64/base64.cpp
)
SOURCE_GROUP("rabbit\\libs\\base64" FILES ${BASE64_LIST})

SET(QRCODEGEN_LIST 
	libs/qrcodegen/qrcodegen.h
	libs/qrcodegen/qrcodegen.c
)
SOURCE_GROUP("rabbit\\libs\\qrcodegen" FILES ${QRCODEGEN_LIST})

SET(TINYOBJ_LIST 
	libs/tinyobj/tiny_obj_loader.h
	libs/tinyobj/tiny_obj_loader.cpp
)
SOURCE_GROUP("rabbit\\libs\\tinyobj" FILES ${TINYOBJ_LIST})

SET(NATIVE_FILED_DIALOG_LIST 
	libs/nativefiledialog/nfd.h
	libs/nativefiledialog/nfd_common.c
	libs/nativefiledialog/nfd_win.cpp
)
SOURCE_GROUP("rabbit\\libs\\NativeFiledDialog" FILES ${NATIVE_FILED_DIALOG_LIST})

SET(OIDN_LIST 
	libs/oidn-1.2.3/include/OpenImageDenoise/oidn.h
	libs/oidn-1.2.3/include/OpenImageDenoise/oidn.hpp
	libs/oidn-1.2.3/include/OpenImageDenoise/version.h
)
SOURCE_GROUP("rabbit\\libs\\oidn" FILES ${OIDN_LIST})

ADD_DEFINITIONS(-DQT_NO_SIGNALS_SLOTS_KEYWORDS)
SET(Qt5_DIR "" CACHE PATH "Directory that contains Qt for the selected compiler, e.g., ../Qt/5.10.1/msvc2017_64")
FIND_PACKAGE(Qt5 COMPONENTS Quick Core Network Widgets QuickWidgets HINTS ${Qt5_DIR})
IF(NOT ${Qt5_FOUND})
    MESSAGE(FATAL_ERROR "The dependency \"qt\" could not be resolved. Install Qt on your system or specify the 'Qt5_DIR' variable.")
ENDIF()
GET_TARGET_PROPERTY(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
GET_FILENAME_COMPONENT(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
FIND_PROGRAM(DEPLOYQT_EXECUTABLE NAMES windeployqt macdeployqt HINTS "${_qt_bin_dir}")

QT5_ADD_RESOURCES(RESOURCES res.qrc)
SET_PROPERTY(SOURCE qrc_res.cpp PROPERTY SKIP_AUTOMOC ON)

ADD_EXECUTABLE(${LIB_OUTNAME} 
	${MAIN_LIST} 
	${VIEWS_LIST} 
	${MODULES_LIST} 
	${CONTROLLERS_LIST}
	${UTILS_LIST}
	${MINIH264E_LIST}
	${BASE64_LIST}
	${NATIVE_FILED_DIALOG_LIST}
	${OIDN_LIST}
	${QRCODEGEN_LIST}
	${TINYOBJ_LIST}
	${RESOURCES}
)

IF(NOT OCTOON_BUILD_SHARED_DLL AND OCTOON_BUILD_PLATFORM_WINDOWS)
	TARGET_COMPILE_DEFINITIONS(${LIB_OUTNAME} PRIVATE OCTOON_STATIC)
ENDIF()

FIND_PACKAGE(FFMPEG REQUIRED)
FIND_PACKAGE(unofficial-iconv CONFIG REQUIRED)

TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${SOURCE_PATH})
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${SOURCE_PATH}/libs)
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${SOURCE_PATH}/libs/oidn-1.2.3/include)
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${OCTOON_PATH_INCLUDE})
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})

TARGET_LINK_LIBRARIES(${LIB_OUTNAME} octoon)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} unofficial::iconv::libiconv unofficial::iconv::libcharset)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} ${FFMPEG_LIBRARIES})
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} ${SOURCE_PATH}/libs/oidn-1.2.3/lib/tbb.lib)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} ${SOURCE_PATH}/libs/oidn-1.2.3/lib/tbbmalloc.lib)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} ${SOURCE_PATH}/libs/oidn-1.2.3/lib/OpenImageDenoise.lib)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} Qt5::Core Qt5::Widgets Qt5::QuickWidgets)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} debug "../../vcpkg_installed/x64-windows/debug/lib/x264")
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} debug "../../vcpkg_installed/x64-windows/debug/lib/x265-static")
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} optimized "../../vcpkg_installed/x64-windows/lib/x264")
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} optimized "../../vcpkg_installed/x64-windows/lib/x265-static")

TARGET_COPY_TO_OUTPUT_DIR(TARGET ${LIB_OUTNAME} FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/libs/oidn-1.2.3/bin/OpenImageDenoise.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/libs/oidn-1.2.3/bin/tbb.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/libs/oidn-1.2.3/bin/tbbmalloc.dll"
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

SET_TARGET_ATTRIBUTE(${LIB_OUTNAME} "samples")
SET_TARGET_PROPERTIES(${LIB_OUTNAME} PROPERTIES OUTPUT_NAME "Rabbit Toolbox")
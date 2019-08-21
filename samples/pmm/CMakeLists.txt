SET(LIB_NAME pmm)
SET(LIB_OUTNAME octoon-${LIB_NAME})

SET(HEADER_PATH ${OCTOON_PATH_HEADER})
SET(SOURCE_PATH ${OCTOON_PATH_SAMPLES}/${LIB_NAME})

SET(MAIN_LIST
	${SOURCE_PATH}/main.cpp
	${SOURCE_PATH}/main_controller.h
	${SOURCE_PATH}/main_controller.cpp
	${SOURCE_PATH}/mysticlit_behaviour.h
	${SOURCE_PATH}/mysticlit_behaviour.cpp
	${SOURCE_PATH}/mysticlit_component.h
	${SOURCE_PATH}/mysticlit_component.cpp
	${SOURCE_PATH}/mysticlit_profile.h
	${SOURCE_PATH}/mysticlit_profile.cpp
	${SOURCE_PATH}/mysticlit_context.h
	${SOURCE_PATH}/mysticlit_context.cpp
	${SOURCE_PATH}/mysticlit_model.h
	${SOURCE_PATH}/mysticlit_model.cpp
)
SOURCE_GROUP("pmm" FILES ${MAIN_LIST})

SET(MODULES_LIST
	${SOURCE_PATH}/module/file_module.h
	${SOURCE_PATH}/module/file_module.cpp
	${SOURCE_PATH}/module/canvas_module.h
	${SOURCE_PATH}/module/canvas_module.cpp
	${SOURCE_PATH}/module/entities_module.h
	${SOURCE_PATH}/module/entities_module.cpp
	${SOURCE_PATH}/module/offline_module.h
	${SOURCE_PATH}/module/offline_module.cpp
	${SOURCE_PATH}/module/h264_module.h
	${SOURCE_PATH}/module/h264_module.cpp
	${SOURCE_PATH}/module/denoise_module.h
	${SOURCE_PATH}/module/denoise_module.cpp
	${SOURCE_PATH}/module/physics_module.h
	${SOURCE_PATH}/module/physics_module.cpp
	${SOURCE_PATH}/module/time_module.h
	${SOURCE_PATH}/module/time_module.cpp
)
SOURCE_GROUP("pmm\\module" FILES ${MODULES_LIST})

SET(VIEWS_LIST
	${SOURCE_PATH}/views/about_window.h
	${SOURCE_PATH}/views/about_window.cpp
	${SOURCE_PATH}/views/camera_window.h
	${SOURCE_PATH}/views/camera_window.cpp
	${SOURCE_PATH}/views/message_window.h
	${SOURCE_PATH}/views/message_window.cpp
	${SOURCE_PATH}/views/main_menu.h
	${SOURCE_PATH}/views/main_menu.cpp
	${SOURCE_PATH}/views/theme_manager.h
	${SOURCE_PATH}/views/theme_manager.cpp
)
SOURCE_GROUP("pmm\\views" FILES ${VIEWS_LIST})

SET(CONTROLLERS_LIST
	${SOURCE_PATH}/controllers/file_component.h
	${SOURCE_PATH}/controllers/file_component.cpp
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
	${SOURCE_PATH}/controllers/denoise_component.h
	${SOURCE_PATH}/controllers/denoise_component.cpp
)
SOURCE_GROUP("pmm\\controllers" FILES ${CONTROLLERS_LIST})

SET(UTILS_LIST
	${SOURCE_PATH}/utils/pmm.h
	${SOURCE_PATH}/utils/pmm.cpp
)
SOURCE_GROUP("pmm\\utils" FILES ${UTILS_LIST})

SET(MINIH264E_LIST 
	libs/minih264e/minih264e.h
)
SOURCE_GROUP("pmm\\libs\\minih264e" FILES ${MINIH264E_LIST})

SET(NATIVE_FILED_DIALOG_LIST 
	libs/nativefiledialog/nfd.h
	libs/nativefiledialog/nfd_common.c
	libs/nativefiledialog/nfd_win.cpp
)
SOURCE_GROUP("pmm\\libs\\NativeFiledDialog" FILES ${NATIVE_FILED_DIALOG_LIST})

SET(OIDN_LIST 
	libs/oidn-1.0.0/include/OpenImageDenoise/oidn.h
	libs/oidn-1.0.0/include/OpenImageDenoise/oidn.hpp
	libs/oidn-1.0.0/include/OpenImageDenoise/version.h
)
SOURCE_GROUP("pmm\\libs\\oidn" FILES ${OIDN_LIST})

IF(NOT OCTOON_BUILD_SHARED_DLL AND OCTOON_BUILD_PLATFORM_WINDOWS)
	ADD_DEFINITIONS(-DOCTOON_STATIC)
ENDIF()

ADD_EXECUTABLE(${LIB_OUTNAME} 
	${MAIN_LIST} 
	${VIEWS_LIST} 
	${MODULES_LIST} 
	${CONTROLLERS_LIST}
	${UTILS_LIST}
	${MINIH264E_LIST} 
	${NATIVE_FILED_DIALOG_LIST}
	${OIDN_LIST}
)

TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PRIVATE libiconv)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PRIVATE octoon)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PRIVATE octoon-c)

TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${SOURCE_PATH})
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${SOURCE_PATH}/libs)
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${SOURCE_PATH}/libs/minih264e)
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${SOURCE_PATH}/libs/oidn-1.0.0/include)
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${OCTOON_PATH_INCLUDE})

TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PUBLIC ${SOURCE_PATH}/libs/oidn-1.0.0/lib/tbb.lib)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PUBLIC ${SOURCE_PATH}/libs/oidn-1.0.0/lib/tbbmalloc.lib)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PUBLIC ${SOURCE_PATH}/libs/oidn-1.0.0/lib/OpenImageDenoise.lib)

ADD_SUBDIRECTORY(${SOURCE_PATH}/libs/oidn-1.0.0)

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

SET_PROPERTY(TARGET ${LIB_OUTNAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${OCTOON_LIBRARY_OUTPUT_PATH}_${CMAKE_CFG_INTDIR})
SET_TARGET_ATTRIBUTE(${LIB_OUTNAME} "samples")
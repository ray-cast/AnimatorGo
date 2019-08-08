#
# Build PxFoundation
#

SET(PXSHARED_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../../../src)

SET(LL_SOURCE_DIR ${PXSHARED_SOURCE_DIR}/foundation)

SET(PXFOUNDATION_LIBTYPE SHARED)

SET(PXFOUNDATION_RESOURCE_FILE
	${PXSHARED_SOURCE_DIR}/compiler/resource_${LIBPATH_SUFFIX}/PxFoundation.rc
)
SOURCE_GROUP(resource FILES ${PXFOUNDATION_RESOURCE_FILE})

SET(PXFOUNDATION_PLATFORM_HEADERS
	${PXSHARED_SOURCE_DIR}/../include/foundation/windows/PxWindowsIntrinsics.h
	${PXSHARED_SOURCE_DIR}/../include/foundation/windows/PxWindowsFoundationDelayLoadHook.h
)
SOURCE_GROUP(include\\windows FILES ${PXFOUNDATION_PLATFORM_HEADERS})

SET(PXFOUNDATION_PLATFORM_SOURCE
	${LL_SOURCE_DIR}/src/windows/PsWindowsAtomic.cpp
	${LL_SOURCE_DIR}/src/windows/PsWindowsCpu.cpp
	${LL_SOURCE_DIR}/src/windows/PsWindowsFPU.cpp
	${LL_SOURCE_DIR}/src/windows/PsWindowsMutex.cpp
	${LL_SOURCE_DIR}/src/windows/PsWindowsPrintString.cpp
	${LL_SOURCE_DIR}/src/windows/PsWindowsSList.cpp
	${LL_SOURCE_DIR}/src/windows/PsWindowsSocket.cpp
	${LL_SOURCE_DIR}/src/windows/PsWindowsSync.cpp
	${LL_SOURCE_DIR}/src/windows/PsWindowsThread.cpp
	${LL_SOURCE_DIR}/src/windows/PsWindowsTime.cpp
)
SOURCE_GROUP(src\\src\\windows FILES ${PXFOUNDATION_PLATFORM_SOURCE})

SET(PXFOUNDATION_PLATFORM_SOURCE_HEADERS
	${LL_SOURCE_DIR}/include/windows/PsWindowsAoS.h
	${LL_SOURCE_DIR}/include/windows/PsWindowsFPU.h
	${LL_SOURCE_DIR}/include/windows/PsWindowsInclude.h
	${LL_SOURCE_DIR}/include/windows/PsWindowsInlineAoS.h
	${LL_SOURCE_DIR}/include/windows/PsWindowsIntrinsics.h
	${LL_SOURCE_DIR}/include/windows/PsWindowsLoadLibrary.h
	${LL_SOURCE_DIR}/include/windows/PsWindowsTrigConstants.h
)
SOURCE_GROUP(src\\include\\windows FILES ${PXFOUNDATION_PLATFORM_SOURCE_HEADERS})


SET(PXFOUNDATION_PLATFORM_FILES
	${PXFOUNDATION_PLATFORM_SOURCE}
	${PXFOUNDATION_PLATFORM_SOURCE_HEADERS}
	${PXFOUNDATION_PLATFORM_HEADERS}
	${PXFOUNDATION_RESOURCE_FILE}
)

SET(PXFOUNDATION_PLATFORM_INCLUDES
	${LL_SOURCE_DIR}/include/windows
)

SET(PXFOUNDATION_COMPILE_DEFS
	# Common to all configurations
	${PXSHARED_WINDOWS_COMPILE_DEFS};PX_FOUNDATION_DLL=1;

	$<$<CONFIG:debug>:${PXSHARED_WINDOWS_DEBUG_COMPILE_DEFS};>
	$<$<CONFIG:checked>:${PXSHARED_WINDOWS_CHECKED_COMPILE_DEFS};>
	$<$<CONFIG:profile>:${PXSHARED_WINDOWS_PROFILE_COMPILE_DEFS};>
	$<$<CONFIG:release>:${PXSHARED_WINDOWS_RELEASE_COMPILE_DEFS};>
)
	
# include PxFoundation common
INCLUDE(../common/PxFoundation.cmake)
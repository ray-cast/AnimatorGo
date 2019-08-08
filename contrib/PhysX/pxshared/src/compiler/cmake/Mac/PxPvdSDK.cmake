#
# Build PxPvdSDK
#

SET(PXSHARED_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../../../src)

SET(LL_SOURCE_DIR ${PXSHARED_SOURCE_DIR}/pvd)

SET(PXPVDSDK_LIBTYPE SHARED)

# Use generator expressions to set config specific preprocessor definitions
SET(PXPVDSDK_COMPILE_DEFS 

	# Common to all configurations
	${PXSHARED_MAC_COMPILE_DEFS}

	$<$<CONFIG:debug>:${PXSHARED_MAC_DEBUG_COMPILE_DEFS}>
	$<$<CONFIG:checked>:${PXSHARED_MAC_CHECKED_COMPILE_DEFS}>
	$<$<CONFIG:profile>:${PXSHARED_MAC_PROFILE_COMPILE_DEFS}>
	$<$<CONFIG:release>:${PXSHARED_MAC_RELEASE_COMPILE_DEFS}>
)

# include PxPvdSDK common
INCLUDE(../common/PxPvdSDK.cmake)

# Add linked libraries
TARGET_LINK_LIBRARIES(PxPvdSDK PRIVATE PxFoundation)


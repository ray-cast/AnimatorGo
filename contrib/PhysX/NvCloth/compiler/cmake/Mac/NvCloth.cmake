#
# Build NvCloth (PROJECT not SOLUTION)
#

MESSAGE("[NvCloth]cmake/Mac/NvCloth.cmake")

SET(GW_DEPS_ROOT $ENV{GW_DEPS_ROOT})
FIND_PACKAGE(PxShared REQUIRED)


#FIND_PACKAGE(nvToolsExt REQUIRED)

SET(NVCLOTH_PLATFORM_INCLUDES
	${NVTOOLSEXT_INCLUDE_DIRS}
)

SET(NVCLOTH_PLATFORM_SOURCE_FILES
	#${PROJECT_ROOT_DIR}/src/neon/NeonCollision.cpp
	#${PROJECT_ROOT_DIR}/src/neon/NeonSelfCollision.cpp
	#${PROJECT_ROOT_DIR}/src/neon/NeonSolverKernel.cpp
	#${PROJECT_ROOT_DIR}/src/neon/SwCollisionHelpers.h
)

# Use generator expressions to set config specific preprocessor definitions
SET(NVCLOTH_COMPILE_DEFS
	NV_CLOTH_IMPORT=PX_DLL_EXPORT
	NV_CLOTH_ENABLE_DX11=0
	NV_CLOTH_ENABLE_CUDA=0

	# Common to all configurations
	${PHYSX_MAC_COMPILE_DEFS};PX_PHYSX_CORE_EXPORTS

	$<$<CONFIG:debug>:${PHYSX_MAC_DEBUG_COMPILE_DEFS};PX_PHYSX_DLL_NAME_POSTFIX=DEBUG;>
	$<$<CONFIG:checked>:${PHYSX_MAC_CHECKED_COMPILE_DEFS};PX_PHYSX_DLL_NAME_POSTFIX=CHECKED;>
	$<$<CONFIG:profile>:${PHYSX_MAC_PROFILE_COMPILE_DEFS};PX_PHYSX_DLL_NAME_POSTFIX=PROFILE;>
	$<$<CONFIG:release>:${PHYSX_MAC_RELEASE_COMPILE_DEFS};>
)

SET(NVCLOTH_LIBTYPE SHARED)

# include common PhysX settings
INCLUDE(../common/NvCloth.cmake)


# Add linked libraries
# TARGET_LINK_LIBRARIES(NvCloth PUBLIC ${NVTOOLSEXT_LIBRARIES} LowLevel LowLevelAABB LowLevelCloth LowLevelDynamics LowLevelParticles PhysXCommon PhysXGpu PxFoundation PxPvdSDK PxTask SceneQuery SimulationController)

TARGET_LINK_LIBRARIES(NvCloth PUBLIC PxFoundation)

SET_TARGET_PROPERTIES(NvCloth PROPERTIES 
	LINK_FLAGS_DEBUG ""
	LINK_FLAGS_CHECKED ""
	LINK_FLAGS_PROFILE ""
	LINK_FLAGS_RELEASE ""
)

# enable -fPIC so we can link static libs with the editor
SET_TARGET_PROPERTIES(NvCloth PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
MESSAGE("[NvCloth]cmake/Mac/NvCloth.cmake END")
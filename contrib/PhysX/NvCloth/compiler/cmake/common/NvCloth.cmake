SET( NV_CLOTH_SOURCE_LIST
	${NVCLOTH_PLATFORM_SOURCE_FILES}
	${PROJECT_SOURCE_DIR}/include/NvCloth/Allocator.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/Callbacks.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/Cloth.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/DxContextManagerCallback.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/Fabric.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/Factory.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/PhaseConfig.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/Range.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/Solver.h
	
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/Ps.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsAtomic.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsAlignedMalloc.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsAllocator.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsAllocator.cpp
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsArray.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsBasicTemplates.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsBitUtils.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsHash.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsHashInternals.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsHashMap.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsIntrinsics.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsUserAllocated.h
	${PROJECT_SOURCE_DIR}/include/NvCloth/ps/PsMathUtils.h
	
	${PROJECT_SOURCE_DIR}/src/Allocator.cpp
	${PROJECT_SOURCE_DIR}/src/BoundingBox.h
	${PROJECT_SOURCE_DIR}/src/Callbacks.cpp
	${PROJECT_SOURCE_DIR}/src/ClothBase.h
	${PROJECT_SOURCE_DIR}/src/ClothClone.h
	${PROJECT_SOURCE_DIR}/src/ClothImpl.h
	${PROJECT_SOURCE_DIR}/src/Factory.cpp
	${PROJECT_SOURCE_DIR}/src/IndexPair.h
	${PROJECT_SOURCE_DIR}/src/IterationState.h
	${PROJECT_SOURCE_DIR}/src/MovingAverage.h
	${PROJECT_SOURCE_DIR}/src/PhaseConfig.cpp
	${PROJECT_SOURCE_DIR}/src/PointInterpolator.h
	${PROJECT_SOURCE_DIR}/src/Simd.h
	${PROJECT_SOURCE_DIR}/src/StackAllocator.h
	${PROJECT_SOURCE_DIR}/src/SwCloth.cpp
	${PROJECT_SOURCE_DIR}/src/SwCloth.h
	${PROJECT_SOURCE_DIR}/src/SwClothData.cpp
	${PROJECT_SOURCE_DIR}/src/SwClothData.h
	${PROJECT_SOURCE_DIR}/src/SwCollision.cpp
	${PROJECT_SOURCE_DIR}/src/SwCollision.h
	${PROJECT_SOURCE_DIR}/src/SwCollisionHelpers.h
	${PROJECT_SOURCE_DIR}/src/SwFabric.cpp
	${PROJECT_SOURCE_DIR}/src/SwFabric.h
	${PROJECT_SOURCE_DIR}/src/SwFactory.cpp
	${PROJECT_SOURCE_DIR}/src/SwFactory.h
	${PROJECT_SOURCE_DIR}/src/SwInterCollision.cpp
	${PROJECT_SOURCE_DIR}/src/SwInterCollision.h
	${PROJECT_SOURCE_DIR}/src/SwSelfCollision.cpp
	${PROJECT_SOURCE_DIR}/src/SwSelfCollision.h
	${PROJECT_SOURCE_DIR}/src/SwSolver.cpp
	${PROJECT_SOURCE_DIR}/src/SwSolver.h
	${PROJECT_SOURCE_DIR}/src/SwSolverKernel.cpp
	${PROJECT_SOURCE_DIR}/src/SwSolverKernel.h
	${PROJECT_SOURCE_DIR}/src/TripletScheduler.cpp
	${PROJECT_SOURCE_DIR}/src/TripletScheduler.h
	${PROJECT_SOURCE_DIR}/src/Vec4T.h

	${PROJECT_SOURCE_DIR}/src/NvSimd/NvSimd4f.h
	${PROJECT_SOURCE_DIR}/src/NvSimd/NvSimd4i.h
	${PROJECT_SOURCE_DIR}/src/NvSimd/NvSimdTypes.h
	${PROJECT_SOURCE_DIR}/src/NvSimd/neon/NvNeonSimd4f.h
	${PROJECT_SOURCE_DIR}/src/NvSimd/neon/NvNeonSimd4i.h
	${PROJECT_SOURCE_DIR}/src/NvSimd/neon/NvNeonSimdTypes.h
	${PROJECT_SOURCE_DIR}/src/NvSimd/scalar/NvScalarSimd4f.h
	${PROJECT_SOURCE_DIR}/src/NvSimd/scalar/NvScalarSimd4i.h
	${PROJECT_SOURCE_DIR}/src/NvSimd/scalar/NvScalarSimdTypes.h
	${PROJECT_SOURCE_DIR}/src/NvSimd/sse2/NvSse2Simd4f.h
	${PROJECT_SOURCE_DIR}/src/NvSimd/sse2/NvSse2Simd4i.h
	${PROJECT_SOURCE_DIR}/src/NvSimd/sse2/NvSse2SimdTypes.h
	${PROJECT_SOURCE_DIR}/src/scalar/SwCollisionHelpers.h
	${PROJECT_SOURCE_DIR}/src/sse2/SwCollisionHelpers.h
	${PROJECT_SOURCE_DIR}/src/sse2/SwSolveConstraints.h
	
	${PROJECT_SOURCE_DIR}/src/ps/PsAlloca.h
	${PROJECT_SOURCE_DIR}/src/ps/PsFPU.h
	${PROJECT_SOURCE_DIR}/src/ps/PsSort.h
	${PROJECT_SOURCE_DIR}/src/ps/PsSortInternals.h
	${PROJECT_SOURCE_DIR}/src/ps/PsUtilities.h
	${PROJECT_SOURCE_DIR}/src/ps/PxIntrinsics.h
	
	${PROJECT_SOURCE_DIR}/extensions/include/NvClothExt/ClothFabricCooker.h
	${PROJECT_SOURCE_DIR}/extensions/include/NvClothExt/ClothMeshDesc.h
	${PROJECT_SOURCE_DIR}/extensions/include/NvClothExt/ClothMeshQuadifier.h
	${PROJECT_SOURCE_DIR}/extensions/include/NvClothExt/ClothTetherCooker.h
	${PROJECT_SOURCE_DIR}/extensions/src/ClothFabricCooker.cpp
	${PROJECT_SOURCE_DIR}/extensions/src/ClothGeodesicTetherCooker.cpp
	${PROJECT_SOURCE_DIR}/extensions/src/ClothMeshQuadifier.cpp
	${PROJECT_SOURCE_DIR}/extensions/src/ClothSimpleTetherCooker.cpp
)

ADD_LIBRARY(NvCloth ${NVCLOTH_LIBTYPE} ${NV_CLOTH_SOURCE_LIST})

FOREACH(source IN LISTS NV_CLOTH_SOURCE_LIST)
	STRING(LENGTH ${PROJECT_SOURCE_DIR} strlen)
	STRING(SUBSTRING ${source} ${strlen} -1 timmedSource)
	GET_FILENAME_COMPONENT(source_path "${timmedSource}" PATH)
	STRING(REPLACE "/" "\\" source_path_msvc "${source_path}")
	SOURCE_GROUP("${source_path_msvc}" FILES "${source}")
ENDFOREACH()

TARGET_INCLUDE_DIRECTORIES(NvCloth PUBLIC ${PROJECT_SOURCE_DIR}/include)
TARGET_INCLUDE_DIRECTORIES(NvCloth PUBLIC ${PROJECT_SOURCE_DIR}/extensions/include)

TARGET_INCLUDE_DIRECTORIES(NvCloth PRIVATE ${NVCLOTH_PLATFORM_INCLUDES})
TARGET_INCLUDE_DIRECTORIES(NvCloth PRIVATE ${PXSHARED_PATH}/include)
TARGET_INCLUDE_DIRECTORIES(NvCloth PRIVATE ${PXSHARED_PATH}/src/foundation/include)
TARGET_INCLUDE_DIRECTORIES(NvCloth PRIVATE ${PROJECT_SOURCE_DIR}/src)
TARGET_INCLUDE_DIRECTORIES(NvCloth PRIVATE ${PROJECT_SOURCE_DIR}/extensions/src)

TARGET_COMPILE_DEFINITIONS(NvCloth PRIVATE ${NVCLOTH_COMPILE_DEFS})

IF(NOT ${NVCLOTH_LIBTYPE} STREQUAL "OBJECT")
	SET_TARGET_PROPERTIES(NvCloth PROPERTIES 
		COMPILE_PDB_NAME_DEBUG "NvCloth${CMAKE_DEBUG_POSTFIX}"
		COMPILE_PDB_NAME_CHECKED "NvCloth${CMAKE_CHECKED_POSTFIX}"
		COMPILE_PDB_NAME_PROFILE "NvCloth${CMAKE_PROFILE_POSTFIX}"
		COMPILE_PDB_NAME_RELEASE "NvCloth${CMAKE_RELEASE_POSTFIX}"
	)
ENDIF()
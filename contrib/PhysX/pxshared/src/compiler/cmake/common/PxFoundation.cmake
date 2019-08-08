#
# Build PxFoundation common
#

SET(PXFOUNDATION_HEADERS	
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/Px.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxAllocatorCallback.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxAssert.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxBitAndData.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxBounds3.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxErrorCallback.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxErrors.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxFlags.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxFoundation.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxFoundationVersion.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxIntrinsics.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxIO.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxMat33.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxMat44.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxMath.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxMathUtils.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxMemory.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxPlane.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxPreprocessor.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxProfiler.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxQuat.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxSimpleTypes.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxStrideIterator.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxTransform.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxUnionCast.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxVec2.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxVec3.h
	${PROJECT_SOURCE_DIR}/../../../../include/foundation/PxVec4.h
)
SOURCE_GROUP(include FILES ${PXFOUNDATION_HEADERS})

SET(PXFOUNDATION_SOURCE
	${LL_SOURCE_DIR}/src/PsAllocator.cpp
	${LL_SOURCE_DIR}/src/PsAssert.cpp
	${LL_SOURCE_DIR}/src/PsFoundation.cpp
	${LL_SOURCE_DIR}/src/PsMathUtils.cpp
	${LL_SOURCE_DIR}/src/PsString.cpp
	${LL_SOURCE_DIR}/src/PsTempAllocator.cpp
	${LL_SOURCE_DIR}/src/PsUtilities.cpp
)
SOURCE_GROUP(src\\src FILES ${PXFOUNDATION_SOURCE})

SET(PXFOUNDATION_SOURCE_HEADERS
	${LL_SOURCE_DIR}/include/Ps.h
	${LL_SOURCE_DIR}/include/PsAlignedMalloc.h
	${LL_SOURCE_DIR}/include/PsAlloca.h
	${LL_SOURCE_DIR}/include/PsAllocator.h
	${LL_SOURCE_DIR}/include/PsAoS.h
	${LL_SOURCE_DIR}/include/PsArray.h
	${LL_SOURCE_DIR}/include/PsAtomic.h
	${LL_SOURCE_DIR}/include/PsBasicTemplates.h
	${LL_SOURCE_DIR}/include/PsBitUtils.h
	${LL_SOURCE_DIR}/include/PsBroadcast.h
	${LL_SOURCE_DIR}/include/PsCpu.h
	${LL_SOURCE_DIR}/include/PsFoundation.h
	${LL_SOURCE_DIR}/include/PsFPU.h
	${LL_SOURCE_DIR}/include/PsHash.h
	${LL_SOURCE_DIR}/include/PsHashInternals.h
	${LL_SOURCE_DIR}/include/PsHashMap.h
	${LL_SOURCE_DIR}/include/PsHashSet.h
	${LL_SOURCE_DIR}/include/PsInlineAllocator.h
	${LL_SOURCE_DIR}/include/PsInlineAoS.h
	${LL_SOURCE_DIR}/include/PsInlineArray.h
	${LL_SOURCE_DIR}/include/PsIntrinsics.h
	${LL_SOURCE_DIR}/include/PsMathUtils.h
	${LL_SOURCE_DIR}/include/PsMutex.h
	${LL_SOURCE_DIR}/include/PsPool.h
	${LL_SOURCE_DIR}/include/PsSList.h
	${LL_SOURCE_DIR}/include/PsSocket.h
	${LL_SOURCE_DIR}/include/PsSort.h
	${LL_SOURCE_DIR}/include/PsSortInternals.h
	${LL_SOURCE_DIR}/include/PsString.h
	${LL_SOURCE_DIR}/include/PsSync.h
	${LL_SOURCE_DIR}/include/PsTempAllocator.h
	${LL_SOURCE_DIR}/include/PsThread.h
	${LL_SOURCE_DIR}/include/PsTime.h
	${LL_SOURCE_DIR}/include/PsUserAllocated.h
	${LL_SOURCE_DIR}/include/PsUtilities.h
	${LL_SOURCE_DIR}/include/PsVecMath.h
	${LL_SOURCE_DIR}/include/PsVecMathAoSScalar.h
	${LL_SOURCE_DIR}/include/PsVecMathAoSScalarInline.h
	${LL_SOURCE_DIR}/include/PsVecMathSSE.h
	${LL_SOURCE_DIR}/include/PsVecMathUtilities.h
	${LL_SOURCE_DIR}/include/PsVecQuat.h
	${LL_SOURCE_DIR}/include/PsVecTransform.h
)
SOURCE_GROUP(src\\include FILES ${PXFOUNDATION_SOURCE_HEADERS})

ADD_LIBRARY(PxFoundation ${PXFOUNDATION_LIBTYPE} 
	${PXFOUNDATION_SOURCE}	
	${PXFOUNDATION_SOURCE_HEADERS}
	${PXFOUNDATION_HEADERS}
	
	${PXFOUNDATION_PLATFORM_FILES}
)

TARGET_INCLUDE_DIRECTORIES(PxFoundation 
	PRIVATE ${PXSHARED_SOURCE_DIR}/../include
	PRIVATE ${LL_SOURCE_DIR}/include
	
	PRIVATE ${PXFOUNDATION_PLATFORM_INCLUDES}
)

TARGET_COMPILE_DEFINITIONS(PxFoundation 
	PRIVATE ${PXFOUNDATION_COMPILE_DEFS}
)

SET_TARGET_PROPERTIES(PxFoundation PROPERTIES 
	COMPILE_PDB_NAME_DEBUG "PxFoundation${CMAKE_DEBUG_POSTFIX}"
	COMPILE_PDB_NAME_CHECKED "PxFoundation${CMAKE_CHECKED_POSTFIX}"
	COMPILE_PDB_NAME_PROFILE "PxFoundation${CMAKE_PROFILE_POSTFIX}"
	COMPILE_PDB_NAME_RELEASE "PxFoundation${CMAKE_RELEASE_POSTFIX}"
)
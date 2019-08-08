#
# Build PxPvdSDK common
#

SET(PXPVDSDK_HEADERS
	${PROJECT_SOURCE_DIR}/../../../../include/pvd/PxPvd.h
	${PROJECT_SOURCE_DIR}/../../../../include/pvd/PxPvdTransport.h
)
SOURCE_GROUP(include FILES ${PXPVDSDK_HEADERS})

SET(PXPVDSDK_SOURCE
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileBase.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileCompileTimeEventFilter.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileContextProvider.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileContextProviderImpl.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileDataBuffer.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileDataParsing.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventBuffer.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventBufferAtomic.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventBufferClient.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventBufferClientManager.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventFilter.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventHandler.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventId.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventImpl.cpp
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventMutex.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventNames.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventParser.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEvents.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventSender.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventSerialization.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileEventSystem.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileMemory.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileMemoryBuffer.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileMemoryEventBuffer.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileMemoryEventParser.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileMemoryEventRecorder.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileMemoryEventReflexiveWriter.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileMemoryEvents.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileMemoryEventSummarizer.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileMemoryEventTypes.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileScopedEvent.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileScopedMutexLock.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileZone.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileZoneImpl.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileZoneManager.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxProfileZoneManagerImpl.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvd.cpp
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdBits.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdByteStreams.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdCommStreamEvents.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdCommStreamEventSink.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdCommStreamSDKEventTypes.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdCommStreamTypes.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdDataStream.cpp
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdDefaultFileTransport.cpp
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdDefaultFileTransport.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdDefaultSocketTransport.cpp
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdDefaultSocketTransport.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdFoundation.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdImpl.cpp
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdImpl.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdInternalByteStreams.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdMarshalling.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdMemClient.cpp
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdMemClient.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdObjectModel.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdObjectModelInternalTypeDefs.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdObjectModelInternalTypes.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdObjectModelMetaData.cpp
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdObjectModelMetaData.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdObjectRegistrar.cpp
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdObjectRegistrar.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdProfileZoneClient.cpp
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdProfileZoneClient.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdUserRenderer.cpp
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdUserRenderImpl.h
	${PXSHARED_SOURCE_DIR}/pvd/src/PxPvdUserRenderTypes.h
)
SOURCE_GROUP(src\\src FILES ${PXPVDSDK_SOURCE})

SET(PXPVDSDK_INTERNAL_HEADERS
	${PXSHARED_SOURCE_DIR}/pvd/include/PsPvd.h
	${PXSHARED_SOURCE_DIR}/pvd/include/PxProfileAllocatorWrapper.h
	${PXSHARED_SOURCE_DIR}/pvd/include/PxPvdClient.h
	${PXSHARED_SOURCE_DIR}/pvd/include/PxPvdDataStream.h
	${PXSHARED_SOURCE_DIR}/pvd/include/PxPvdDataStreamHelpers.h
	${PXSHARED_SOURCE_DIR}/pvd/include/PxPvdErrorCodes.h
	${PXSHARED_SOURCE_DIR}/pvd/include/PxPvdObjectModelBaseTypes.h
	${PXSHARED_SOURCE_DIR}/pvd/include/PxPvdRenderBuffer.h
	${PXSHARED_SOURCE_DIR}/pvd/include/PxPvdUserRenderer.h
)
SOURCE_GROUP(src\\include FILES ${PXPVDSDK_INTERNAL_HEADERS})

ADD_LIBRARY(PxPvdSDK ${PXPVDSDK_LIBTYPE} 
	${PXPVDSDK_HEADERS}

	${PXPVDSDK_INTERNAL_HEADERS}
	${PXPVDSDK_SOURCE}
	
	${PXPVDSDK_PLATFORM_FILES}
)

TARGET_INCLUDE_DIRECTORIES(PxPvdSDK 
	PRIVATE ${PXSHARED_SOURCE_DIR}/../include
	PRIVATE ${PXSHARED_SOURCE_DIR}/foundation/include
	PRIVATE ${PXSHARED_SOURCE_DIR}/pvd/include
	PRIVATE ${PXSHARED_SOURCE_DIR}/filebuf/include

	PRIVATE ${PXPVDSDK_PLATFORM_INCLUDES}
	
)

TARGET_COMPILE_DEFINITIONS(PxPvdSDK 
	PRIVATE ${PXPVDSDK_COMPILE_DEFS}
)

SET_TARGET_PROPERTIES(PxPvdSDK PROPERTIES 
	COMPILE_PDB_NAME_DEBUG "PxPvdSDK${CMAKE_DEBUG_POSTFIX}"
	COMPILE_PDB_NAME_CHECKED "PxPvdSDK${CMAKE_CHECKED_POSTFIX}"
	COMPILE_PDB_NAME_PROFILE "PxPvdSDK${CMAKE_PROFILE_POSTFIX}"
	COMPILE_PDB_NAME_RELEASE "PxPvdSDK${CMAKE_RELEASE_POSTFIX}"
)
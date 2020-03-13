SET(LIB_NAME physics)
SET(LIB_OUTNAME octoon-${LIB_NAME})

PROJECT(${LIB_OUTNAME})

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_NAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/${LIB_OUTNAME})

SET(SOURCE_LIST
	${HEADER_PATH}/physics_type.h
	${HEADER_PATH}/physics_system.h
	${SOURCE_PATH}/physics_system.cpp
	${HEADER_PATH}/physics_context.h
	${SOURCE_PATH}/physics_context.cpp
	${HEADER_PATH}/physics_scene.h
	${SOURCE_PATH}/physics_scene.cpp
	${HEADER_PATH}/physics_rigidbody.h
	${SOURCE_PATH}/physics_rigidbody.cpp
	${HEADER_PATH}/physics_listener.h
	${SOURCE_PATH}/physics_listener.cpp

	${HEADER_PATH}/physics_shape.h
	${SOURCE_PATH}/physics_shape.cpp
	${HEADER_PATH}/physics_sphere_shape.h
	${SOURCE_PATH}/physics_sphere_shape.cpp
	${HEADER_PATH}/physics_box_shape.h
	${SOURCE_PATH}/physics_box_shape.cpp
	${HEADER_PATH}/physics_capsule_shape.h
	${SOURCE_PATH}/physics_capsule_shape.cpp

	${HEADER_PATH}/physics_joint.h
	${SOURCE_PATH}/physics_joint.cpp
	${HEADER_PATH}/physics_fixed_joint.h
	${SOURCE_PATH}/physics_fixed_joint.cpp
	${HEADER_PATH}/physics_configurable_joint.h
	${SOURCE_PATH}/physics_configurable_joint.cpp

	${HEADER_PATH}/physics_cloth.h
	${SOURCE_PATH}/physics_cloth.cpp
)
SOURCE_GROUP("physics" FILES ${SOURCE_LIST})

SET(PHYSX_SOURCE_LIST
	${SOURCE_PATH}/physx/physx_type.h
	${SOURCE_PATH}/physx/physx_context.h
	${SOURCE_PATH}/physx/physx_context.cpp
	${SOURCE_PATH}/physx/physx_scene.h
	${SOURCE_PATH}/physx/physx_scene.cpp
	${SOURCE_PATH}/physx/physx_rigidbody.h
	${SOURCE_PATH}/physx/physx_rigidbody.cpp

	${SOURCE_PATH}/physx/physx_shape.h
	${SOURCE_PATH}/physx/physx_shape.cpp
	${SOURCE_PATH}/physx/physx_sphere_shape.h
	${SOURCE_PATH}/physx/physx_sphere_shape.cpp
	${SOURCE_PATH}/physx/physx_box_shape.h
	${SOURCE_PATH}/physx/physx_box_shape.cpp
	${SOURCE_PATH}/physx/physx_capsule_shape.h
	${SOURCE_PATH}/physx/physx_capsule_shape.cpp

	${SOURCE_PATH}/physx/physx_joint.h
	${SOURCE_PATH}/physx/physx_joint.cpp
	${SOURCE_PATH}/physx/physx_fixed_joint.h
	${SOURCE_PATH}/physx/physx_fixed_joint.cpp
	${SOURCE_PATH}/physx/physx_configurable_joint.h
	${SOURCE_PATH}/physx/physx_configurable_joint.cpp

	${SOURCE_PATH}/physx/physx_cloth.h
	${SOURCE_PATH}/physx/physx_cloth.cpp
)
SOURCE_GROUP("PhysX" FILES ${PHYSX_SOURCE_LIST})

IF(OCTOON_BUILD_SHARED_DLL)
	ADD_DEFINITIONS(-DOCTOON_BUILD_DLL_EXPORT)
	ADD_LIBRARY(${LIB_OUTNAME} SHARED ${SOURCE_LIST} ${PHYSX_SOURCE_LIST})
ELSE()
	ADD_DEFINITIONS(-DOCTOON_STATIC)
	ADD_LIBRARY(${LIB_OUTNAME} ${SOURCE_LIST} ${PHYSX_SOURCE_LIST})
ENDIF()

TARGET_LINK_LIBRARIES(${LIB_OUTNAME} octoon-io)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} octoon-runtime)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PhysX)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PhysXCommon)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PhysXCooking)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PhysXExtensions)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PhysXCharacterKinematic)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PhysXPvdSDK)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PhysXTask)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} PhysXVehicle)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} NvCloth)
TARGET_LINK_LIBRARIES(${LIB_OUTNAME} ${PHYSX_LIBRARY})
#TARGET_LINK_LIBRARIES(${LIB_OUTNAME} debug $ENV{VCPKG_ROOT}/installed/x64-windows/debug/lib/Bullet3Common_Debug.lib)
#TARGET_LINK_LIBRARIES(${LIB_OUTNAME} debug $ENV{VCPKG_ROOT}/installed/x64-windows/debug/lib/BulletCollision_Debug.lib)
#TARGET_LINK_LIBRARIES(${LIB_OUTNAME} debug $ENV{VCPKG_ROOT}/installed/x64-windows/debug/lib/BulletDynamics_Debug.lib)
#TARGET_LINK_LIBRARIES(${LIB_OUTNAME} debug $ENV{VCPKG_ROOT}/installed/x64-windows/debug/lib/BulletInverseDynamics_Debug.lib)
#TARGET_LINK_LIBRARIES(${LIB_OUTNAME} debug $ENV{VCPKG_ROOT}/installed/x64-windows/debug/lib/BulletSoftBody_Debug.lib)
#TARGET_LINK_LIBRARIES(${LIB_OUTNAME} optimized $ENV{VCPKG_ROOT}/installed/x64-windows/lib/Bullet3Common.lib)
#TARGET_LINK_LIBRARIES(${LIB_OUTNAME} optimized $ENV{VCPKG_ROOT}/installed/x64-windows/lib/BulletCollision.lib)
#TARGET_LINK_LIBRARIES(${LIB_OUTNAME} optimized $ENV{VCPKG_ROOT}/installed/x64-windows/lib/BulletDynamics.lib)
#TARGET_LINK_LIBRARIES(${LIB_OUTNAME} optimized $ENV{VCPKG_ROOT}/installed/x64-windows/lib/BulletInverseDynamics.lib)
#TARGET_LINK_LIBRARIES(${LIB_OUTNAME} optimized $ENV{VCPKG_ROOT}/installed/x64-windows/lib/BulletSoftBody.lib)

TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${PHYSX_INCLUDE_DIR})
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE ${OCTOON_PATH_INCLUDE})
TARGET_INCLUDE_DIRECTORIES(${LIB_OUTNAME} PRIVATE $ENV{VCPKG_ROOT}/installed/x64-windows/include)

SET_TARGET_ATTRIBUTE(${LIB_OUTNAME} "core")
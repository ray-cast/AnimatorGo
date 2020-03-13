SET(LIB_OUTNAME physics)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

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
SOURCE_GROUP("physics\\PhysX" FILES ${PHYSX_SOURCE_LIST})

LIST(APPEND PHYSICS_LIST ${SOURCE_LIST} ${PHYSX_SOURCE_LIST})
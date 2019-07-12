# message("now using FindPhysX.cmake find physx lib")

if(BULLET_DIR)
    message("search BULLET_DIR in ${BULLET_DIR}")
endif()

FIND_PATH(BULLET_INCLUDE_DIR btBulletCollisionCommon.h ${BULLET_DIR}/src)

if(CMAKE_CL_64)
    SET(BULLET_LIB_LIST 
        PhysXCharacterKinematic_static_64
        PhysXCommon_64
        PhysXCooking_64
        PhysXExtensions_static_64
        PhysXFoundation_64
        PhysXPvdSDK_static_64
        PhysXTask_static_64
        PhysXVehicle_static_64
        PhysX_64
    )
else()
    SET(BULLET_LIB_LIST 
        PhysXCharacterKinematic_static
        PhysXCommon
        PhysXCooking
        PhysXExtensions_static
        PhysXFoundation
        PhysXPvdSDK_static
        PhysXTask_static
        PhysXVehicle_static
        PhysX
    )
endif()

SET(BULLET_LIBRARY "")
foreach(each_lib ${BULLET_LIB_LIST})
    UNSET(BULLET_LIB CACHE)
    FIND_LIBRARY(BULLET_LIB ${each_lib}
        HINTS ${BULLET_DIR}/bin/win.x86_64.vc141.mt/debug
        ${BULLET_DIR}/bin/win.x86_64.vc141.mt/release
        ${BULLET_DIR}/bin/win.x86_64.vc141.mt/checked
        ${BULLET_DIR}/bin/win.x86_64.vc141.mt/profile
        
        ${BULLET_DIR}/bin/win.x86_64.vc140.mt/debug
        ${BULLET_DIR}/bin/win.x86_64.vc140.mt/release
        ${BULLET_DIR}/bin/win.x86_64.vc140.mt/checked
        ${BULLET_DIR}/bin/win.x86_64.vc140.mt/profile
        
        ${BULLET_DIR}/bin/win.x86_64.vc120.mt/debug
        ${BULLET_DIR}/bin/win.x86_64.vc120.mt/release
        ${BULLET_DIR}/bin/win.x86_64.vc120.mt/checked
        ${BULLET_DIR}/bin/win.x86_64.vc120.mt/profile
        
        ${BULLET_DIR}/bin/win.x86_32.vc141.mt/debug
        ${BULLET_DIR}/bin/win.x86_32.vc141.mt/release
        ${BULLET_DIR}/bin/win.x86_32.vc141.mt/checked
        ${BULLET_DIR}/bin/win.x86_32.vc141.mt/profile

        ${BULLET_DIR}/bin/win.x86_32.vc140.mt/debug
        ${BULLET_DIR}/bin/win.x86_32.vc140.mt/release
        ${BULLET_DIR}/bin/win.x86_32.vc140.mt/checked
        ${BULLET_DIR}/bin/win.x86_32.vc140.mt/profile

        ${BULLET_DIR}/bin/win.x86_32.vc120.mt/debug
        ${BULLET_DIR}/bin/win.x86_32.vc120.mt/release
        ${BULLET_DIR}/bin/win.x86_32.vc120.mt/checked
        ${BULLET_DIR}/bin/win.x86_32.vc120.mt/profile

        ${BULLET_DIR}/bin/linux.clang/debug
        ${BULLET_DIR}/bin/linux.clang/release
        ${BULLET_DIR}/bin/linux.clang/checked
        ${BULLET_DIR}/bin/linux.clang/profile)
    # message(${BULLET_LIB})
    SET(BULLET_LIBRARY ${BULLET_LIBRARY} ${BULLET_LIB})
endforeach()


# message(${BULLET_INCLUDE_DIR})
# message(${BULLET_LIBRARY})

if(BULLET_INCLUDE_DIR AND BULLET_LIBRARY)
    set(BULLET_FOUND TRUE)
endif(BULLET_INCLUDE_DIR AND BULLET_LIBRARY)

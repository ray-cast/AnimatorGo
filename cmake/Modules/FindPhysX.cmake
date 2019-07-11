message("now using FindPhysX.cmake find physx lib")

if(PHYSX_DIR)
    message("search PHYSX_DIR in ${PHYSX_DIR}")
endif()

FIND_PATH(PHYSX_INCLUDE_DIR PxPhysics.h ${PHYSX_DIR}/include)

SET(FIND_LIBRARY_USE_LIB64_PATHS TRUE)

if(CMAKE_CL_64)
    SET(PHYSX_LIB_LIST 
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
    SET(PHYSX_LIB_LIST 
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

SET(PHYSX_LIBRARY "")
foreach(each_lib ${PHYSX_LIB_LIST})
    UNSET(PHYSX_LIB CACHE)
    FIND_LIBRARY(PHYSX_LIB ${each_lib}
        HINTS ${PHYSX_DIR}/bin/win.x86_64.vc141.mt/debug
        ${PHYSX_DIR}/bin/win.x86_64.vc141.mt/release
        ${PHYSX_DIR}/bin/win.x86_64.vc141.mt/checked
        ${PHYSX_DIR}/bin/win.x86_64.vc141.mt/profile
        
        ${PHYSX_DIR}/bin/win.x86_64.vc140.mt/debug
        ${PHYSX_DIR}/bin/win.x86_64.vc140.mt/release
        ${PHYSX_DIR}/bin/win.x86_64.vc140.mt/checked
        ${PHYSX_DIR}/bin/win.x86_64.vc140.mt/profile
        
        ${PHYSX_DIR}/bin/win.x86_64.vc120.mt/debug
        ${PHYSX_DIR}/bin/win.x86_64.vc120.mt/release
        ${PHYSX_DIR}/bin/win.x86_64.vc120.mt/checked
        ${PHYSX_DIR}/bin/win.x86_64.vc120.mt/profile
        
        ${PHYSX_DIR}/bin/win.x86_32.vc141.mt/debug
        ${PHYSX_DIR}/bin/win.x86_32.vc141.mt/release
        ${PHYSX_DIR}/bin/win.x86_32.vc141.mt/checked
        ${PHYSX_DIR}/bin/win.x86_32.vc141.mt/profile

        ${PHYSX_DIR}/bin/win.x86_32.vc140.mt/debug
        ${PHYSX_DIR}/bin/win.x86_32.vc140.mt/release
        ${PHYSX_DIR}/bin/win.x86_32.vc140.mt/checked
        ${PHYSX_DIR}/bin/win.x86_32.vc140.mt/profile

        ${PHYSX_DIR}/bin/win.x86_32.vc120.mt/debug
        ${PHYSX_DIR}/bin/win.x86_32.vc120.mt/release
        ${PHYSX_DIR}/bin/win.x86_32.vc120.mt/checked
        ${PHYSX_DIR}/bin/win.x86_32.vc120.mt/profile

        ${PHYSX_DIR}/bin/linux.clang/debug
        ${PHYSX_DIR}/bin/linux.clang/release
        ${PHYSX_DIR}/bin/linux.clang/checked
        ${PHYSX_DIR}/bin/linux.clang/profile)
    # message(${PHYSX_LIB})
    SET(PHYSX_LIBRARY ${PHYSX_LIBRARY} ${PHYSX_LIB})
endforeach()

# message(${PHYSX_INCLUDE_DIR})
# message(${PHYSX_LIBRARY})

if(PHYSX_INCLUDE_DIR AND PHYSX_LIBRARY)
    set(PHYSX_FOUND TRUE)
endif(PHYSX_INCLUDE_DIR AND PHYSX_LIBRARY)

# message("now using FindPhysX.cmake find physx lib")

if(PHYSX_DIR)
    message("search PHYSX_DIR in ${PHYSX_DIR}")
endif()

if(PHYSX_STATIC_WINCRT)
    message("physx wincrt static: ${PHYSX_STATIC_WINCRT}")
endif()

UNSET(PHYSX_INCLUDE_DIR)

SET(PHYSX_MAIN_INCLUDE_DIR ${PHYSX_DIR}/physx/include)
SET(PHYSX_SHARED_INCLUDE_DIR ${PHYSX_DIR}/pxshared/include)

SET(PHYSX_INCLUDE_DIR ${PHYSX_MAIN_INCLUDE_DIR} ${PHYSX_SHARED_INCLUDE_DIR} CACHE STRING "physx include dir")

# SET(FIND_LIBRARY_USE_LIB64_PATHS TRUE)

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

SET(PHYSX_LIBRARY)

foreach(each_lib ${PHYSX_LIB_LIST})
    UNSET(PHYSX_LIB CACHE)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(BUILD_TYPE_DIR "debug")
    elseif(CMAKE_BUILD_TYPE MATCHES "Release")
        set(BUILD_TYPE_DIR "release")
    else()
        set(BUILD_TYPE_DIR "checked")
    endif()

    if(PHYSX_STATIC_WINCRT)
        set(WINCRT_DIR "mt")
    else()
        set(WINCRT_DIR "md")
    endif()

    if(CMAKE_CL_64)
        set(MACHINE_TYPE_DIR "x86_64")
    else()
        set(MACHINE_TYPE_DIR "x86_32")
    endif()

    set(VS_VERSION_DIR "vc141")

    list(APPEND PHYSX_LIBRARY ${PHYSX_DIR}/physx/bin/win.${MACHINE_TYPE_DIR}.${VS_VERSION_DIR}.${WINCRT_DIR}/${BUILD_TYPE_DIR}/${each_lib}.lib)
    # message(${PHYSX_LIB})
endforeach()

SET(PHYSX_LIBRARY ${PHYSX_LIBRARY} CACHE STRING "physx libs")

# message(${PHYSX_LIBRARY})

# message(${PHYSX_INCLUDE_DIR})
# message(${PHYSX_LIBRARY})

if(PHYSX_INCLUDE_DIR AND PHYSX_LIBRARY)
    set(PHYSX_FOUND TRUE)
endif(PHYSX_INCLUDE_DIR AND PHYSX_LIBRARY)

set(FBX_SDK_DIR "C:/Program Files/Autodesk/FBX/FBX SDK/2017.0.1")

if (WIN32)
    set(FBX_SDK_LIB_NAME libfbxsdk-md)
else ()
    set(FBX_SDK_LIB_NAME fbxsdk)
endif (WIN32)

find_library(FBX_SDK_LIBRARY_RELEASE NAMES ${FBX_SDK_LIB_NAME} PATHS "${FBX_SDK_DIR}/lib/vs2015/x64/release")
find_library(FBX_SDK_LIBRARY_DEBUG NAMES ${FBX_SDK_LIB_NAME} PATHS "${FBX_SDK_DIR}/lib/vs2015/x64/debug")
find_path(FBX_SDK_INCLUDE_DIR NAMES fbxsdk.h PATHS "${FBX_SDK_DIR}/include")

include(SelectLibraryConfigurations)
select_library_configurations(FBX_SDK)

find_package_handle_standard_args(FBX_SDK DEFAULT_MSG FBX_SDK_LIBRARY FBX_SDK_INCLUDE_DIR)

add_library(fbxsdk::fbxsdk UNKNOWN IMPORTED)

set_target_properties(fbxsdk::fbxsdk PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${FBX_SDK_INCLUDE_DIR}")

if (OIIO_LIBRARY_RELEASE)
    set_property(TARGET fbxsdk::fbxsdk APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(fbxsdk::fbxsdk PROPERTIES IMPORTED_LOCATION_RELEASE "${FBX_SDK_LIBRARY_RELEASE}")
endif (OIIO_LIBRARY_RELEASE)

if (OIIO_LIBRARY_DEBUG)
    set_property(TARGET fbxsdk::fbxsdk APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(fbxsdk::fbxsdk PROPERTIES IMPORTED_LOCATION_DEBUG "${FBX_SDK_LIBRARY_DEBUG}")
endif (OIIO_LIBRARY_DEBUG)

if(NOT OIIO_LIBRARY_RELEASE AND NOT OIIO_LIBRARY_DEBUG)
    set_property(TARGET fbxsdk::fbxsdk APPEND PROPERTY IMPORTED_LOCATION "${FBX_SDK_LIBRARY}")
endif(NOT OIIO_LIBRARY_RELEASE AND NOT OIIO_LIBRARY_DEBUG)

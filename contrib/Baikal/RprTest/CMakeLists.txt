set(SOURCES
    main.cpp
    aov.h
    arithmetic.h
    basic.h
    camera.h
    light.h
    material.h)

add_executable(RprTest ${SOURCES})
target_compile_features(RprTest PRIVATE cxx_std_14)
target_include_directories(RprTest PRIVATE .)
target_link_libraries(RprTest PRIVATE GTest RprSupport64 RadeonProRender64)
set_target_properties(RprTest
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${Baikal_SOURCE_DIR}/RprTest)

# TODO: BaikalTest images and RprTest images directories have identical names
if (0)
# Add symbolic links to RprTest/OutputImages and RprTest/ReferenceImages directory
set(RPRTEST_REFERENCEIMAGES_DIR ReferenceImages)
file(TO_NATIVE_PATH ${Baikal_SOURCE_DIR}/RprTest/${RPRTEST_REFERENCEIMAGES_DIR} RPRTEST_REFERENCEIMAGES_SRC)
file(TO_NATIVE_PATH ${CMAKE_BINARY_DIR}/${RPRTEST_REFERENCEIMAGES_DIR} RPRTEST_REFERENCEIMAGES_DST)

set(RPRTEST_OUTPUTIMAGES_DIR OutputImages)
file(TO_NATIVE_PATH ${Baikal_SOURCE_DIR}/RprTest/${RPRTEST_OUTPUTIMAGES_DIR} RPRTEST_OUTPUTIMAGES_SRC)
file(TO_NATIVE_PATH ${CMAKE_BINARY_DIR}/${RPRTEST_OUTPUTIMAGES_DIR} RPRTEST_OUTPUTIMAGES_DST)

if (WIN32)
    add_custom_target(RprTestImagesDir ALL
        COMMAND IF NOT EXIST ${RPRTEST_REFERENCEIMAGES_DST} mklink /D ${RPRTEST_REFERENCEIMAGES_DST} ${RPRTEST_REFERENCEIMAGES_SRC}
        COMMAND IF NOT EXIST ${RPRTEST_OUTPUTIMAGES_DST} mklink /D ${RPRTEST_OUTPUTIMAGES_DST} ${RPRTEST_OUTPUTIMAGES_SRC}
    )
else ()
    add_custom_target(RprTestImagesDir ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${RPRTEST_REFERENCEIMAGES_SRC} ${RPRTEST_REFERENCEIMAGES_DST}
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${RPRTEST_OUTPUTIMAGES_SRC} ${RPRTEST_OUTPUTIMAGES_DST}
    )
endif ()

add_dependencies(RprTest RprTestImagesDir ResourcesDir)
endif ()

add_dependencies(RprTest ResourcesDir)

# Install target
install(TARGETS RprTest RUNTIME DESTINATION bin)
install(DIRECTORY DESTINATION bin/${RPRTEST_REFERENCEIMAGES_DIR})
install(DIRECTORY DESTINATION bin/${RPRTEST_OUTPUTIMAGES_DIR})

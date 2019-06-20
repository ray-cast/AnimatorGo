set(SOURCES
    aov.h
    basic.h
    camera.h
    input_maps.h
    internal.h
    light.h
    main.cpp
    material.h
    test_scenes.h
    uberv2.h)

add_executable(BaikalTest ${SOURCES})
target_compile_features(BaikalTest PRIVATE cxx_std_14)

target_include_directories(BaikalTest PRIVATE .)
target_link_libraries(BaikalTest PRIVATE Baikal BaikalIO GTest)

set_target_properties(BaikalTest
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${Baikal_SOURCE_DIR}/BaikalTest FOLDER Baikal)
target_compile_definitions(BaikalTest PRIVATE _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING=1)

# Add symbolic links to BaikalTest/OutputImages and BaikalTest/ReferenceImages directory
set(BAIKALTEST_REFERENCEIMAGES_DIR ReferenceImages)
FILE(TO_NATIVE_PATH ${Baikal_SOURCE_DIR}/BaikalTest/${BAIKALTEST_REFERENCEIMAGES_DIR} BAIKALTEST_REFERENCEIMAGES_SRC)
FILE(TO_NATIVE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BAIKALTEST_REFERENCEIMAGES_DIR} BAIKALTEST_REFERENCEIMAGES_DST)

set(BAIKALTEST_OUTPUTIMAGES_DIR OutputImages)
FILE(TO_NATIVE_PATH ${Baikal_SOURCE_DIR}/BaikalTest/${BAIKALTEST_OUTPUTIMAGES_DIR} BAIKALTEST_OUTPUTIMAGES_SRC)
FILE(TO_NATIVE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BAIKALTEST_OUTPUTIMAGES_DIR} BAIKALTEST_OUTPUTIMAGES_DST)

if (WIN32)
    add_custom_target(BaikalTestImagesDir ALL
        COMMAND IF NOT EXIST ${BAIKALTEST_REFERENCEIMAGES_DST} mklink /D ${BAIKALTEST_REFERENCEIMAGES_DST} ${BAIKALTEST_REFERENCEIMAGES_SRC}
        COMMAND IF NOT EXIST ${BAIKALTEST_OUTPUTIMAGES_DST} mklink /D ${BAIKALTEST_OUTPUTIMAGES_DST} ${BAIKALTEST_OUTPUTIMAGES_SRC}
    )
else ()
    add_custom_target(BaikalTestImagesDir ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${BAIKALTEST_REFERENCEIMAGES_SRC} ${BAIKALTEST_REFERENCEIMAGES_DST}
        COMMAND ${CMAKE_COMMAND} -E create_symlink ${BAIKALTEST_OUTPUTIMAGES_SRC} ${BAIKALTEST_OUTPUTIMAGES_DST}
    )
endif ()

add_dependencies(BaikalTest BaikalTestImagesDir ResourcesDir)

# Install target
install(TARGETS BaikalTest RUNTIME DESTINATION bin)
install(DIRECTORY DESTINATION bin/${BAIKALTEST_REFERENCEIMAGES_DIR})
install(DIRECTORY DESTINATION bin/${BAIKALTEST_OUTPUTIMAGES_DIR})

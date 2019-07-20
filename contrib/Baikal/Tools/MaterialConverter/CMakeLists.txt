SET(BAIKAL_OLD_SOURCES
    BaikalOld/SceneGraph/iterator.h
    BaikalOld/SceneGraph/material.cpp
    BaikalOld/SceneGraph/material.h
    BaikalOld/SceneGraph/scene_object.h
    BaikalOld/SceneGraph/texture.cpp
    BaikalOld/SceneGraph/texture.h)

SET(BAIKAL_OLD_IO_SOURCES
    BaikalOld/SceneGraph/IO/image_io.cpp
    BaikalOld/SceneGraph/IO/image_io.h
    BaikalOld/SceneGraph/IO/material_io.cpp
    BaikalOld/SceneGraph/IO/material_io.h)

source_group("BaikalOld\\SceneGraph" FILES ${BAIKAL_OLD_SOURCES})
source_group("BaikalOld\\SceneGraph\\IO" FILES ${BAIKAL_OLD_IO_SOURCES})

SET(SOURCES
    material_converter.cpp
    material_converter.h
    main.cpp)

add_executable(MaterialConverter ${SOURCES} ${BAIKAL_OLD_SOURCES} ${BAIKAL_OLD_IO_SOURCES})
target_compile_features(MaterialConverter PRIVATE cxx_std_14)
target_include_directories(MaterialConverter PRIVATE .)
target_link_libraries(MaterialConverter PUBLIC Baikal BaikalIO)

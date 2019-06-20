# Library

set(DATAGENERATOR_LIB_SOURCES
    Source/data_generator.h
    Source/data_generator.cpp
    Source/devices.h
    Source/devices.cpp
    Source/filesystem.h
    Source/data_generator_impl.h
    Source/data_generator_impl.cpp
    Source/utils.h
)

add_library(BaikalDataGenerator SHARED ${DATAGENERATOR_LIB_SOURCES})

target_compile_features(BaikalDataGenerator PRIVATE cxx_std_17)

target_compile_definitions(BaikalDataGenerator PRIVATE RPR_EXPORT_API)

target_include_directories(BaikalDataGenerator
    PRIVATE .
    PRIVATE ${Baikal_SOURCE_DIR}
    PRIVATE ${Baikal_SOURCE_DIR}/Rpr
    PRIVATE ${Baikal_SOURCE_DIR}/RadeonRays/include
)

target_link_libraries(BaikalDataGenerator PRIVATE RadeonProRender64 Baikal BaikalIO)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_libraries(BaikalDataGenerator PRIVATE stdc++fs)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_link_libraries(BaikalDataGenerator PRIVATE c++experimental)
endif ()

set_target_properties(BaikalDataGenerator
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${Baikal_SOURCE_DIR}/BaikalDataGenerator)

add_dependencies(BaikalDataGenerator ResourcesDir Copy3rdParty)

if (WIN32)
    install(TARGETS BaikalDataGenerator RUNTIME DESTINATION bin)
else ()
    install(TARGETS BaikalDataGenerator LIBRARY DESTINATION lib)
endif ()


# Application

set(DATAGENERATOR_APP_SOURCES
    Source/app_config.h
    Source/cmd_line_parser.h
    Source/cmd_line_parser.cpp
    Source/devices.h
    Source/devices.cpp
    Source/filesystem.h
    Source/object_loader.h
    Source/object_loader.cpp
    Source/logging.h
    Source/logging.cpp
    Source/main.cpp
)

add_executable(BaikalDataGeneratorApp ${DATAGENERATOR_APP_SOURCES})

target_compile_features(BaikalDataGeneratorApp PRIVATE cxx_std_17)

target_include_directories(BaikalDataGeneratorApp
    PRIVATE ${Baikal_SOURCE_DIR}
    PRIVATE .
)

target_link_libraries(BaikalDataGeneratorApp PRIVATE BaikalDataGenerator RadeonProRender64 Baikal)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_libraries(BaikalDataGeneratorApp PRIVATE stdc++fs)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_link_libraries(BaikalDataGeneratorApp PRIVATE c++experimental)
endif ()

set_target_properties(BaikalDataGeneratorApp
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${Baikal_SOURCE_DIR}/BaikalDataGenerator)

install(TARGETS BaikalDataGeneratorApp RUNTIME DESTINATION bin)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(HLSLcc C CXX)

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT CMAKE_BUILD_TYPE)

IF( BUILD_SHARED_LIBS)
   ADD_DEFINITIONS(-DHLSLCC_DYNLIB)
ENDIF( BUILD_SHARED_LIBS )

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin" )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../lib" )

INCLUDE(TestBigEndian)

TEST_BIG_ENDIAN(BIG_ENDIAN_SYSTEM)

IF(BIG_ENDIAN_SYSTEM)
    ADD_DEFINITIONS(-D__BIG_ENDIAN__)
ELSE()
    ADD_DEFINITIONS(-D__LITTLE_ENDIAN__)
ENDIF()

file(GLOB CBSTRING_CFILES ../src/cbstring/*.c)
file(GLOB CBSTRING_HFILES ../src/cbstring/*.h)
source_group("CBString Files" FILES ${CBSTRING_CFILES} ${CBSTRING_HFILES})

SET(libHLSLcc_SOURCES
../src/internal_includes/debug.h
../src/internal_includes/decode.h
../src/internal_includes/languages.h
../include/pstdint.h
../src/internal_includes/reflect.h
../src/internal_includes/structs.h
../include/hlslcc.h
../src/internal_includes/toGLSLDeclaration.h
../src/internal_includes/toGLSLInstruction.h
../src/internal_includes/toGLSLOperand.h
../src/internal_includes/tokens.h
../src/internal_includes/tokensDX9.h
../src/internal_includes/shaderLimits.h
../src/internal_includes/hlslcc_malloc.h
../src/internal_includes/hlslcc_malloc.c
${CBSTRING_HFILES}
../src/decode.c
../src/decodeDX9.c
../src/reflect.c
../src/toGLSL.c
../src/toGLSLInstruction.c
../src/toGLSLOperand.c
../src/toGLSLDeclaration.c
${CBSTRING_CFILES}
)
ADD_LIBRARY( libHLSLcc ${libHLSLcc_SOURCES} )

TARGET_INCLUDE_DIRECTORIES(libHLSLcc PUBLIC ../include)
TARGET_INCLUDE_DIRECTORIES(libHLSLcc PRIVATE ../src/cbstring/ ../src/)
TARGET_INCLUDE_DIRECTORIES(libHLSLcc PRIVATE ../offline/cjson/ )

file(GLOB CJSON_CFILES ../offline/cjson/*.c)
file(GLOB CJSON_HFILES ../offline/cjson/*.h)
source_group("cJSON Files" FILES ${CJSON_CFILES} ${CJSON_HFILES})

SET(HLSLcc_SOURCES
../offline/toGLSLStandalone.cpp
../offline/timer.h
../offline/timer.cpp
../offline/hash.h
../offline/serializeReflection.cpp
../offline/serializeReflection.h
${CJSON_CFILES}
${CJSON_HFILES}
)

IF(MINGW OR UNIX)
   SET_TARGET_PROPERTIES(libHLSLcc PROPERTIES OUTPUT_NAME HLSLcc)
ENDIF()

# ADD_EXECUTABLE( HLSLcc ${HLSLcc_SOURCES} )

# Compile 32-bit binaries for linux
# IF(BUILD_PLATFORM_LINUX)
#     ADD_LIBRARY( libHLSLcc-i386 ${libHLSLcc_SOURCES} )
#     #ADD_EXECUTABLE( HLSLcc-i386 ${HLSLcc_SOURCES} )
#     TARGET_LINK_LIBRARIES( HLSLcc-i386 libHLSLcc-i386)
#     SET_TARGET_PROPERTIES(libHLSLcc-i386 PROPERTIES COMPILE_FLAGS -m32 LINK_FLAGS -m32 )
#     SET_TARGET_PROPERTIES(HLSLcc-i386 PROPERTIES COMPILE_FLAGS -m32 LINK_FLAGS -m32 )
# ENDIF()
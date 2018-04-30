ADD_DEFINITIONS(-DPX_PHYSX_STATIC_LIB)

FILE(GLOB_RECURSE LOWLEVEL_HEADERS API/include/*.h
                                   API/src/*.h
                                   common/include/*.h
                                   common/src/*.h
                                   software/include/*.h
                                   software/src/*.h)

FILE(GLOB_RECURSE LOWLEVEL_SOURCES API/src/*.cpp
                                   common/src/*.cpp
                                   software/src/*.cpp)

SOURCE_GROUP("Headers" FILES ${LOWLEVEL_HEADERS})
SOURCE_GROUP("Sources" FILES ${LOWLEVEL_SOURCES})

ADD_LIBRARY(LowLevel ${LOWLEVEL_HEADERS} ${LOWLEVEL_SOURCES})
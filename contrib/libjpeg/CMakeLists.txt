project(libjpeg C)

SET(LIB_NAME libjpeg)

SET(HEADER_LIST
    jconfig.h
    jdct.h
    jerror.h
    jinclude.h
    jmemsys.h
    jmorecfg.h
    jpegint.h
    jpeglib.h
    jversion.h
    transupp.h
)
SET(SOURCE_LIST
    jaricom.c
    jcapimin.c
    jcapistd.c
    jcarith.c
    jccoefct.c
    jccolor.c
    jcdctmgr.c
    jchuff.c
    jcinit.c
    jcmainct.c
    jcmarker.c
    jcmaster.c
    jcomapi.c
    jcparam.c
    jcprepct.c
    jcsample.c
    jctrans.c
    jdapimin.c
    jdapistd.c
    jdarith.c
    jdatadst.c
    jdatasrc.c
    jdcoefct.c
    jdcolor.c
    jddctmgr.c
    jdhuff.c
    jdinput.c
    jdmainct.c
    jdmarker.c
    jdmaster.c
    jdmerge.c
    jdpostct.c
    jdsample.c
    jdtrans.c
    jerror.c
    jfdctflt.c
    jfdctfst.c
    jfdctint.c
    jidctflt.c
    jidctfst.c
    jidctint.c
    jmemmgr.c
    jmemnobs.c
    jquant1.c
    jquant2.c
    jutils.c
    transupp.c
)

SOURCE_GROUP("libjpeg" FILES ${HEADER_LIST})
SOURCE_GROUP("libjpeg" FILES ${SOURCE_LIST})

ADD_LIBRARY(${LIB_NAME} ${HEADER_LIST} ${SOURCE_LIST})
TARGET_LINK_LIBRARIES (${LIB_NAME} zlib)

IF(MINGW OR UNIX)
   SET_TARGET_PROPERTIES(libjpeg PROPERTIES OUTPUT_NAME jpeg)
ENDIF()
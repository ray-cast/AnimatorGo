SET(LIB_OUTNAME texture)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(SOURCE_LIST
    ${HEADER_PATH}/lut.h
    ${HEADER_PATH}/texture.h
    ${SOURCE_PATH}/texture.cpp
    ${HEADER_PATH}/texture_types.h
    ${HEADER_PATH}/texture_loader.h
    ${HEADER_PATH}/texture_format.h
    ${SOURCE_PATH}/texture_format.cpp
    ${HEADER_PATH}/texture_util.h
    ${SOURCE_PATH}/texture_util.cpp
)
SOURCE_GROUP("texture" FILES ${SOURCE_LIST})

SET(FORMAT_LIST
    ${SOURCE_PATH}/texture_bmp.h
    ${SOURCE_PATH}/texture_bmp.cpp
    ${SOURCE_PATH}/texture_jpeg.h
    ${SOURCE_PATH}/texture_jpeg.cpp
    ${SOURCE_PATH}/texture_png.h
    ${SOURCE_PATH}/texture_png.cpp
	${SOURCE_PATH}/texture_tga.h
	${SOURCE_PATH}/texture_tga.cpp
	${SOURCE_PATH}/texture_dds.h
	${SOURCE_PATH}/texture_dds.cpp
	${SOURCE_PATH}/texture_hdr.h
	${SOURCE_PATH}/texture_hdr.cpp
    ${SOURCE_PATH}/texture_all.h
    ${SOURCE_PATH}/texture_all.cpp
)
SOURCE_GROUP("texture\\internal" FILES ${FORMAT_LIST})

LIST(APPEND TEXTURE_LIST ${SOURCE_LIST} ${FORMAT_LIST})
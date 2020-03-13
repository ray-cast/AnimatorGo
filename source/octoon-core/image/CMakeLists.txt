SET(LIB_OUTNAME image)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(SOURCE_LIST
    ${HEADER_PATH}/lut.h
    ${HEADER_PATH}/image.h
    ${SOURCE_PATH}/image.cpp
    ${HEADER_PATH}/image_types.h
    ${HEADER_PATH}/image_loader.h
    ${HEADER_PATH}/image_format.h
    ${SOURCE_PATH}/image_format.cpp
    ${HEADER_PATH}/image_util.h
    ${SOURCE_PATH}/image_util.cpp
)
SOURCE_GROUP("image" FILES ${SOURCE_LIST})

SET(FORMAT_LIST
    ${SOURCE_PATH}/image_bmp.h
    ${SOURCE_PATH}/image_bmp.cpp
    ${SOURCE_PATH}/image_jpeg.h
    ${SOURCE_PATH}/image_jpeg.cpp
    ${SOURCE_PATH}/image_png.h
    ${SOURCE_PATH}/image_png.cpp
	${SOURCE_PATH}/image_tga.h
	${SOURCE_PATH}/image_tga.cpp
	${SOURCE_PATH}/image_dds.h
	${SOURCE_PATH}/image_dds.cpp
	${SOURCE_PATH}/image_hdr.h
	${SOURCE_PATH}/image_hdr.cpp
    ${SOURCE_PATH}/image_all.h
    ${SOURCE_PATH}/image_all.cpp
)
SOURCE_GROUP("image\\internal" FILES ${FORMAT_LIST})

LIST(APPEND IMAGE_LIST ${SOURCE_LIST} ${FORMAT_LIST})
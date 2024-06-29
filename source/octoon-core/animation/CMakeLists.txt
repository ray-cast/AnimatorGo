SET(LIB_OUTNAME animation)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/${LIB_OUTNAME})
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/octoon-core/${LIB_OUTNAME})

SET(ANIM_LIST
	${HEADER_PATH}/animation.h
	${SOURCE_PATH}/animation.cpp
	${HEADER_PATH}/animation_clip.h
	${SOURCE_PATH}/animation_clip.cpp
	${HEADER_PATH}/animation_curve.h
)
SOURCE_GROUP("animation"  FILES ${ANIM_LIST})

SET(KEYFRAME_LIST
	${HEADER_PATH}/keyframe.h
)
SOURCE_GROUP("animation\\keyframe"  FILES ${KEYFRAME_LIST})

SET(INTERPOLATOR_LIST
	${HEADER_PATH}/interpolator.h
	${HEADER_PATH}/path_interpolator.h
	${HEADER_PATH}/fixed_interpolator.h
	${HEADER_PATH}/linear_interpolator.h
)
SOURCE_GROUP("animation\\interpolator" FILES ${INTERPOLATOR_LIST})

LIST(APPEND ANIMATION_LIST ${ANIM_LIST} ${KEYFRAME_LIST} ${INTERPOLATOR_LIST} ${LOADER_LIST})
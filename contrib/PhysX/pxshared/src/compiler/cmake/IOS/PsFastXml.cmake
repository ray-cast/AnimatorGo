#
# Build PsFastXml
#

SET(PXSHARED_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../../../src)

SET(LL_SOURCE_DIR ${PXSHARED_SOURCE_DIR}/fastxml)

# Use generator expressions to set config specific preprocessor definitions
SET(PSFASTXML_COMPILE_DEFS 

	# Common to all configurations
	${PXSHARED_IOS_COMPILE_DEFS};PX_FOUNDATION_DLL=0;

	$<$<CONFIG:debug>:${PXSHARED_IOS_DEBUG_COMPILE_DEFS};>
	$<$<CONFIG:checked>:${PXSHARED_IOS_CHECKED_COMPILE_DEFS};>
	$<$<CONFIG:profile>:${PXSHARED_IOS_PROFILE_COMPILE_DEFS};>
	$<$<CONFIG:release>:${PXSHARED_IOS_RELEASE_COMPILE_DEFS};>
)

# include PsFastXml common
INCLUDE(../common/PsFastXml.cmake)
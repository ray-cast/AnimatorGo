#
# Build PsFastXml
#

SET(PXSHARED_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../../../src)

SET(LL_SOURCE_DIR ${PXSHARED_SOURCE_DIR}/fastxml)

SET(PLATFORM_INCLUDES
	$ENV{EMSCRIPTEN}/system/include
)

# Use generator expressions to set config specific preprocessor definitions
SET(PSFASTXML_COMPILE_DEFS 

	# Common to all configurations
	${PXSHARED_HTML5_COMPILE_DEFS};PX_FOUNDATION_DLL=0;

	$<$<CONFIG:debug>:${PXSHARED_HTML5_DEBUG_COMPILE_DEFS};>
	$<$<CONFIG:checked>:${PXSHARED_HTML5_CHECKED_COMPILE_DEFS};>
	$<$<CONFIG:profile>:${PXSHARED_HTML5_PROFILE_COMPILE_DEFS};>
	$<$<CONFIG:release>:${PXSHARED_HTML5_RELEASE_COMPILE_DEFS};>
)

# include PsFastXml common
INCLUDE(../common/PsFastXml.cmake)
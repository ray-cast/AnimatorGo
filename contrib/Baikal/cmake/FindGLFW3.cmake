set(GLFW3_DIR "${Baikal_SOURCE_DIR}/3rdparty/glfw")

find_library(GLFW3_LIB NAMES glfw glfw3 PATHS "${GLFW3_DIR}/lib/x64")
find_path(GLFW3_INCLUDE_DIR NAMES GLFW/glfw3.h PATHS "${GLFW3_DIR}/include")

find_package_handle_standard_args(GLFW3 DEFAULT_MSG GLFW3_LIB GLFW3_INCLUDE_DIR)

add_library(glfw3::glfw3 UNKNOWN IMPORTED)
set_target_properties(glfw3::glfw3 PROPERTIES IMPORTED_LOCATION "${GLFW3_LIB}")
set_target_properties(glfw3::glfw3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIR}")

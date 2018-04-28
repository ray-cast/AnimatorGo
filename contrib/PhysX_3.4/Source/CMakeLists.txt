cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# -----------------------------------------------------------------------------------------------------------------------------
# PROJECT
# -----------------------------------------------------------------------------------------------------------------------------

project("PhysX")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# -----------------------------------------------------------------------------------------------------------------------------
# SUB-DIRECTORIES
# -----------------------------------------------------------------------------------------------------------------------------                    

# PhysX Library
add_subdirectory("Common")
add_subdirectory("GeomUtils")
add_subdirectory("ImmediateMode")
add_subdirectory("LowLevel")
add_subdirectory("LowLevelAABB")
add_subdirectory("LowLevelCloth")
add_subdirectory("LowLevelDynamics")
add_subdirectory("LowLevelParticles")
add_subdirectory("PhysX")
add_subdirectory("PhysXCharacterKinematic")
add_subdirectory("PhysXCooking")
add_subdirectory("PhysXExtensions")
add_subdirectory("PhysXGpu")
add_subdirectory("PhysXVehicle")
add_subdirectory("SceneQuery")
add_subdirectory("SimulationController")


# Samples
# add_subdirectory("Samples")


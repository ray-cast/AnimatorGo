set(SOURCES RprSupport.cpp)
    
add_library(RprSupport64 SHARED ${SOURCES})
target_compile_definitions(RprSupport64 PRIVATE RPR_EXPORT_API)
target_compile_features(RprSupport64 PRIVATE cxx_std_14)
target_include_directories(RprSupport64 PUBLIC ${RprSupport_HEADER})
target_link_libraries(RprSupport64 PUBLIC RadeonProRender64)

# Install target
if (WIN32)
    install(TARGETS RprSupport64 RUNTIME DESTINATION bin)
else ()
    install(TARGETS RprSupport64 LIBRARY DESTINATION lib)
endif ()

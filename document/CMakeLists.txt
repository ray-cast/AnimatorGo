FIND_PACKAGE(Doxygen)
IF(NOT DOXYGEN_FOUND)
	MESSAGE(WARNING "Doxygen is needed to build the documentation.")  
else()
	SET(doxyfile_in ${OCTOON_PATH_DOCUMENT}/OctoonAPI/docs.doxyfile)
	SET(doxyfile ${OCTOON_PATH_DOCUMENT}/OctoonAPI/Doxyfile)

	CONFIGURE_FILE(${doxyfile_in} ${doxyfile} @ONLY)

	ADD_CUSTOM_TARGET(document
		COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
		WORKING_DIRECTORY ${OCTOON_PATH_DOCUMENT}/OctoonAPI
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM)

	INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/document/OctoonAPI/html DESTINATION document/OctoonAPI/html)
ENDIF()
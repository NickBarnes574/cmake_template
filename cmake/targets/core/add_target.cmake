# FILENAME: add_target.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Adds a target to the target list
# -----------------------------------------------------------------------------

function(add_target)
    # Parsing arguments simplification
    cmake_parse_arguments(PARSE_ARGV 0 ADD_TARGET "" "TARGET_NAME;ENDPOINT;TARGET_TYPE;SOURCE_DIR;DESTINATION_DIR" "LIBRARIES")
    
    set(SOURCE_PATH "${CMAKE_SOURCE_DIR}/${ADD_TARGET_SOURCE_DIR}")
    set(SOURCE_FILES_PATTERN "/${ADD_TARGET_ENDPOINT}/src/*.c")
    file(GLOB SOURCES "${SOURCE_PATH}${SOURCE_FILES_PATTERN}")
    set(INCLUDES "${SOURCE_PATH}/${ADD_TARGET_ENDPOINT}/include")

    if(ADD_TARGET_TARGET_TYPE STREQUAL "EXE")
        add_exe("${ADD_TARGET_TARGET_NAME}" "${ADD_TARGET_ENDPOINT}" "${SOURCES}" "${INCLUDES}" "${ADD_TARGET_DESTINATION_DIR}" ${ADD_TARGET_LIBRARIES})
    elseif(ADD_TARGET_TARGET_TYPE STREQUAL "LIB")
        message(WARNING "LIB target type not implemented yet.")
    else()
        message(FATAL_ERROR "Invalid target type: ${ADD_TARGET_TARGET_TYPE}. Must be 'EXE' or 'LIB'.")
    endif()
endfunction()

# *** end of file ***

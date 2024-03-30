# FILENAME: configure_target.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Configures a new target as either an executable (exe) or a
# shared object library (so)
# -----------------------------------------------------------------------------

function(configure_target)
    set(options "")
    set(singleValueArgs TARGET_NAME ENDPOINT TARGET_TYPE SOURCE_DIR DESTINATION_DIR)
    set(multiValueArgs LIBRARIES)

    # Parse the function arguments
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${singleValueArgs}" "${multiValueArgs}")

    # Display the configuration status
    message(STATUS "Configuring target: ${ARG_TARGET_NAME}")
    message(STATUS "Type: ${ARG_TARGET_TYPE}, Endpoint: ${ARG_ENDPOINT}, Source: ${ARG_SOURCE_DIR}, Destination: ${ARG_DESTINATION_DIR}")
    string(REPLACE ";" " " LIBRARIES_STR "${ARG_LIBRARIES}")
    message(STATUS "configure_target() Libraries as a string: ${LIBRARIES_STR}")
    message(STATUS "Libraries: ${ARG_LIBRARIES}")

    if ("${ARG_LIBRARIES}" STREQUAL "")
        set(ARG_LIBRARIES "NONE")
        message(STATUS "ARG_LIBRARIES: " ${ARG_LIBRARIES})
    endif()

    # Add the target
    add_target(${ARG_TARGET_NAME} ${ARG_ENDPOINT} ${ARG_TARGET_TYPE} ${ARG_SOURCE_DIR} ${ARG_DESTINATION_DIR} ${ARG_LIBRARIES})
endfunction()

# *** end of file ***
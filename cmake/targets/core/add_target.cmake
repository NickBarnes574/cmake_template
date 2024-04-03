# FILENAME: add_target.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Adds a target to the target list
# -----------------------------------------------------------------------------

function(add_target)
    set(options "")
    set(oneValueArgs TARGET_NAME ENDPOINT TARGET_TYPE SOURCE_DIR DESTINATION_DIR)
    set(multiValueArgs LIBRARIES)
    cmake_parse_arguments(ADD_TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    message(STATUS "add_target() source directory: ${SOURCE_DIR}")
    set(SOURCE "${CMAKE_SOURCE_DIR}/${ADD_TARGET_SOURCE_DIR}")

    message(STATUS "add_target() Libraries: ${ADD_TARGET_LIBRARIES}")

    # Local endpoint
    if(ADD_TARGET_ENDPOINT STREQUAL "LOCAL")
        file(GLOB SOURCES "${SOURCE}/local/src/*.c")
        message(STATUS "add_target() message: ${SOURCES}")
        set(INCLUDES ${SOURCE}/local/include)
    
    # Remote endpoint
    elseif(ADD_TARGET_ENDPOINT STREQUAL "REMOTE")
        file(GLOB SOURCES "${SOURCE}/remote/src/*.c")
        set(INCLUDES ${SOURCE}/remote/include)
    else()
        message(FATAL_ERROR "Invalid target endpoint. Must be 'LOCAL' or 'REMOTE'.")
    endif()

    # Determine the target type

    # Executable program
    if(ADD_TARGET_TARGET_TYPE STREQUAL "EXE")
        add_exe("${ADD_TARGET_TARGET_NAME}" "${ADD_TARGET_ENDPOINT}" "${SOURCES}" "${INCLUDES}" "${ADD_TARGET_DESTINATION_DIR}" ${ADD_TARGET_LIBRARIES})
    
    # Shared object library
    elseif(TARGET_TYPE STREQUAL "LIB")
        # TODO: Add LIB functionality
        message(WARNING "Shared object library functionality not implemented yet.")
    else()
        message(FATAL_ERROR "Invalid target type. Must be 'EXE' or 'LIB'.")
    endif()
endfunction()

# ------------IN PROGRESS------------

# *** end of file ***
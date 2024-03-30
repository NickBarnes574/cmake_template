# FILENAME: add_target.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Adds a target to the target list
# -----------------------------------------------------------------------------

function(add_target TARGET_NAME ENDPOINT TARGET_TYPE SOURCE_DIR DESTINATION_DIR)
    set(SOURCE "${CMAKE_SOURCE_DIR}/${SOURCE_DIR}")
    # Determine the endpoint directory

    # Use ARGN to capture all additional arguments, which include libraries
    set(LIBRARIES ${ARGN})
    message(STATUS "add_target() Libraries: ${LIBRARIES}")

    # Local endpoint
    if(ENDPOINT STREQUAL "LOCAL")
        file(GLOB SOURCES "${SOURCE}/local/src/*.c")
        set(INCLUDES ${SOURCE}/local/include)
    
    # Remote endpoint
    elseif(ENDPOINT STREQUAL "REMOTE")
        file(GLOB SOURCES "${SOURCE}/remote/src/*.c")
        set(INCLUDES ${SOURCE}/remote/include)
    else()
        message(FATAL_ERROR "Invalid target endpoint. Must be 'LOCAL' or 'REMOTE'.")
    endif()

    # Determine the target type

    # Executable program
    if(TARGET_TYPE STREQUAL "EXE")
        add_exe("${TARGET_NAME}" "${ENDPOINT}" "${SOURCES}" "${INCLUDES}" "${DESTINATION_DIR}" ${LIBRARIES})
    
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
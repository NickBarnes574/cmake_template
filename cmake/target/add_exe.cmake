# FILENAME: add_exe.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function, add_exe, for adding executable targets to 
# the project.
# -----------------------------------------------------------------------------

function(add_exe TARGET_NAME ENDPOINT SRC_FILES INCLUDE_FILES DESTINATION_DIR)
    # Use ARGN to capture all additional arguments, which include libraries
    set(LINK_LIBRARIES ${ARGN})
    set(FULL_TARGET_NAME "${TARGET_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
    add_executable(${FULL_TARGET_NAME} ${SRC_FILES})
    target_include_directories(${FULL_TARGET_NAME} PRIVATE ${INCLUDE_FILES})

    # Link libraries if any are passed in
    if(NOT LINK_LIBRARIES STREQUAL "NONE")
        foreach(LIB IN LISTS LINK_LIBRARIES)
            message(STATUS "Linking library ${LIB} to ${FULL_TARGET_NAME}")
            message(STATUS "Linking libraries: ${LINK_LIBRARIES}")
            target_link_libraries(${FULL_TARGET_NAME} PRIVATE ${LIB})
        endforeach()
    endif()
    
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        set_default_release_options(${FULL_TARGET_NAME})
        strip_target(${FULL_TARGET_NAME})
        if (ENDPOINT STREQUAL "LOCAL")
            install_target(${FULL_TARGET_NAME} "${DEST_BUILD}/${DESTINATION_DIR}/${DEST_RELEASE_LOCAL}")
        elseif(ENDPOINT STREQUAL "REMOTE")
            install_target(${FULL_TARGET_NAME} "${DEST_BUILD}/${DESTINATION_DIR}/${DEST_RELEASE_REMOTE}")
        else()
            message(FATAL_ERROR "Invalid Endpoint.")
        endif()

    elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
        set_default_debug_options(${FULL_TARGET_NAME})
        if (ENDPOINT STREQUAL "LOCAL")
            install_target(${FULL_TARGET_NAME} "${DEST_BUILD}/${DESTINATION_DIR}/${DEST_DEBUG_LOCAL}")
        elseif(ENDPOINT STREQUAL "REMOTE")
            install_target(${FULL_TARGET_NAME} "${DEST_BUILD}/${DESTINATION_DIR}/${DEST_DEBUG_REMOTE}")
        else()
            message(FATAL_ERROR "Invalid Endpoint.")
        endif()
    endif()

endfunction()

# *** end of file ***
# FILENAME: strip_target.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function to strip debugging information from an
# executable target. This is commonly used in release mode to reduce the size
# of the executable and remove unnecessary debugging information.
# -----------------------------------------------------------------------------

function(strip_target TARGET_NAME)
    get_target_property(target_type ${TARGET_NAME} TYPE)
    if (target_type STREQUAL "EXECUTABLE")
        set(NAME ${TARGET_NAME})
    elseif(target_type STREQUAL "SHARED_LIBRARY")
        set(NAME "${TARGET_NAME}.so")
    endif()

    add_custom_command(TARGET ${TARGET_NAME}
        POST_BUILD
        COMMAND ${CMAKE_STRIP} ${NAME}
        WORKING_DIRECTORY ${OUTPUT_DIRECTORY}
        COMMENT "Stripping ${NAME}"
    )
endfunction()

# *** end of file ***

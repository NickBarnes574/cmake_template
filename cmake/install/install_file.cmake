function(install_file FILE_NAME INSTALL_PATH ENDPOINT)
    # Use relative paths for installation, and consider CMAKE_INSTALL_PREFIX if installing outside the build directory
    # set(RELATIVE_INSTALL_PATH "${CMAKE_SOURCE_DIR}/build/projects/BSLE")

    if (ENDPOINT STREQUAL "LOCAL")
        set(SPECIFIC_PATH "local")
    elseif(ENDPOINT STREQUAL "REMOTE")
        set(SPECIFIC_PATH "remote")
    else()
        message(FATAL_ERROR "Invalid Endpoint.")
        return()
    endif()

    # Construct the full relative installation path
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        set(FULL_INSTALL_PATH "${INSTALL_PATH}/release/${SPECIFIC_PATH}")
    elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
        # Adjust if you have a different structure for Debug builds
        set(FULL_INSTALL_PATH "${INSTALL_PATH}/debug/${SPECIFIC_PATH}")
    else()
        message(FATAL_ERROR "Invalid Build Type.")
        return()
    endif()

    # Note: This installation is relative to CMAKE_INSTALL_PREFIX by default
    install(FILES ${FILE_NAME} DESTINATION ${FULL_INSTALL_PATH})
endfunction(install_file)
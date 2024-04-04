# FILENAME: install_file.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function to install a single file to a specified 
# destination within the build environment. The destination path is constructed 
# from the provided relative destination and endpoint, taking into account the 
# current build type for structured deployment.
# -----------------------------------------------------------------------------

function(install_file FILE_NAME RELATIVE_DESTINATION ENDPOINT)
    set(full_install_path "${RELATIVE_DESTINATION}/${CMAKE_BUILD_TYPE}/${ENDPOINT}")
    install(FILES ${FILE_NAME} DESTINATION ${full_install_path})
endfunction(install_file)

# *** end of file ***

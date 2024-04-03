function(install_file FILE_NAME RELATIVE_DESTINATION ENDPOINT)
    set(full_install_path "${RELATIVE_DESTINATION}/${CMAKE_BUILD_TYPE}/${ENDPOINT}")
    install(FILES ${FILE_NAME} DESTINATION ${full_install_path})
endfunction(install_file)


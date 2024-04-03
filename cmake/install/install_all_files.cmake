function(install_all_files)
    install_file("${CMAKE_SOURCE_DIR}/projects/BSLE/local/client.py" "projects/BSLE" "local")
    install_file("${CMAKE_SOURCE_DIR}/docs/build_system.md" "projects/BSLE" "docs")
endfunction(install_all_files)
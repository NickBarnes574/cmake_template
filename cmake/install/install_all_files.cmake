function(install_all_files)

    install_file(projects/BSLE/local/client.py "${CMAKE_SOURCE_DIR}/build/projects/BSLE" "LOCAL")
    install_file(docs/build_system.md "${CMAKE_SOURCE_DIR}/build/projects/BSLE" "DOCS")

endfunction(install_all_files)
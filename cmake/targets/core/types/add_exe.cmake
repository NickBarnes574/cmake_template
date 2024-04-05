# FILENAME: add_exe.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function, add_exe, for adding executable targets to 
# the project.
# -----------------------------------------------------------------------------

function(add_exe target_name endpoint src_files include_dirs destination_dir)
    set(link_libraries ${ARGN}) # Libraries to link with
    set(full_target_name "${target_name}_${CMAKE_SYSTEM_PROCESSOR}")

    add_executable(${full_target_name} ${src_files})
    target_include_directories(${full_target_name} PRIVATE ${include_dirs})

    if(link_libraries)
        target_link_libraries(${full_target_name} PRIVATE ${link_libraries})
    endif()

    set(target_destination "${destination_dir}/${CMAKE_BUILD_TYPE}/${endpoint}")
    install(TARGETS ${full_target_name} DESTINATION ${target_destination})
endfunction()

# *** end of file ***

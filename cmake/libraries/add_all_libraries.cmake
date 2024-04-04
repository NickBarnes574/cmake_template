# FILENAME: add_all_libraries.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function to add and link all custom libraries necessary 
# for the project.
# -----------------------------------------------------------------------------

function(add_all_libraries)
    add_user_library(Common)
    add_user_library(Math)
    add_user_library(DSA)
    add_user_library(Threading)
    add_user_library(Networking)
    
    # Link libraries
    target_link_libraries(Math PUBLIC Common)
    target_link_libraries(DSA PUBLIC Common Math)
    target_link_libraries(Threading PUBLIC Common DSA pthread)
    target_link_libraries(Networking PUBLIC Common Threading)
endfunction(add_all_libraries)

# *** end of file ***

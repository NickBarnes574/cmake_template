# FILENAME: add_all_libs.cmake
# -----------------------------------------------------------------------------
# DESCRIPTION: Defines a function to add and link all custom libraries necessary 
# for the project. This includes specifying the directories containing the 
# libraries' source and include files and setting up their inter-library dependencies.
# -----------------------------------------------------------------------------

function(add_all_libs)
    # Add libraries specifying their directories
    add_lib(Common DIRS "libraries/Common")
    add_lib(Math DIRS "libraries/Math")
    add_lib(DSA DIRS 
        "libraries/DSA/adjacency_list" 
        "libraries/DSA/adjacency_matrix"
        "libraries/DSA/hash_table"
        "libraries/DSA/linked_list"
        "libraries/DSA/queue"
        "libraries/DSA/stack"
        "libraries/DSA/vector"
    )
    add_lib(Threading DIRS "libraries/Threading")
    add_lib(Networking DIRS
        "libraries/Networking/common"
        "libraries/Networking/tcp_server"
    )
    
    # Setup inter-library dependencies
    target_link_libraries(Math PUBLIC Common)
    target_link_libraries(DSA PUBLIC Common Math)
    target_link_libraries(Threading PUBLIC Common DSA pthread)
    target_link_libraries(Networking PUBLIC Common Threading)
endfunction(add_all_libs)

# *** end of file ***

function(add_all_libraries)
    # Setup libraries
    setup_library(Common "0_Common")
    setup_library(Math "1_Math")
    setup_library(DSA "2_DataStructures")
    setup_library(Threading "3_Threading")
    setup_library(Networking "4_Networking")

    # Link libraries
    target_link_libraries(Math PUBLIC Common)
    target_link_libraries(DSA PUBLIC Common Math)
    target_link_libraries(Threading PUBLIC Common DSA pthread)
    target_link_libraries(Networking PUBLIC Common Threading)
endfunction()

# ------------IN PROGRESS------------

# *** end of file ***

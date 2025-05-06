function(enable_auto_localized_output)
    get_filename_component(CURRENT_PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    include(${CMAKE_SOURCE_DIR}/cmake/paths-config.cmake)
endfunction()

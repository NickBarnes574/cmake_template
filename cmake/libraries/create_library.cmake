function(create_library LIB_NAME LIB_SOURCES LIB_INCLUDE_DIR)
    add_library(${LIB_NAME} STATIC ${LIB_SOURCES})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_INCLUDE_DIR})
endfunction()
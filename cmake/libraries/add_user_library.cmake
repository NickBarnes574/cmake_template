function(add_user_library LIB_DIR)

    file(GLOB COMMON_LIB_SOURCES "${CMAKE_SOURCE_DIR}/libraries/${LIB_DIR}/src/*.c")
    set(COMMON_LIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libraries/${LIB_DIR}/include)

    create_library(${LIB_DIR} "${COMMON_LIB_SOURCES}" ${COMMON_LIB_INCLUDE_DIR})
endfunction()
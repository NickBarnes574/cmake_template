include(${CMAKE_SOURCE_DIR}/cmake/add-localized-app.cmake)

add_localized_app(
    PROJECT     salt_valt
    TARGET      sv_server
    SCOPE       remote
    SOURCES
        src/main.c
        src/option_handler.c
    INCLUDES
        include
)

# Link against libraries
target_link_libraries(sv_server
PUBLIC Core DSA Math Networking Threading
)


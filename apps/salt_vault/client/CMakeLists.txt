include(${CMAKE_SOURCE_DIR}/cmake/add-localized-app.cmake)

add_localized_app(
    PROJECT     salt_valt
    TARGET      sv_client
    SCOPE       local
    SOURCES
        src/client.c
    INCLUDES
        include
)

# # Link against libraries
# target_link_libraries(sv_server
# PUBLIC Core DSA Math Networking Threading
# )
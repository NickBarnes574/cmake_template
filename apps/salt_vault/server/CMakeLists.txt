# ----------------------------------------------------------------------------
# BRIEF -- apps/salt_vault/server/CMakeLists.txt
# ----------------------------------------------------------------------------
# Builds the Salt Vault Server
# ----------------------------------------------------------------------------

message_color(STATUS "Building Salt Vault Server for ARCH: ${ARCH}, Mode: ${CMAKE_BUILD_TYPE}")

add_app(
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
target_link_libraries(
    sv_server PUBLIC Core Math Networking TCPServer BlockingQueue ThreadPool
)

# *** END OF FILE ***
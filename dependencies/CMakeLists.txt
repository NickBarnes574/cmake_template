# ----------------------------------------------------------------------------
# BRIEF -- dependencies/CMakeLists.txt
# ----------------------------------------------------------------------------
# Builds dependency libraries if they are not already built
# ----------------------------------------------------------------------------

fetch_library( # Core Library
    clib-core
    git@github.com:NickBarnes574/clib-core.git
    main
)

# fetch_library( # DSA Library
#     clib-dsa
#     git@github.com:NickBarnes574/clib-dsa.git
#     main
# )

fetch_library( # Math Library
    clib-math
    git@github.com:NickBarnes574/clib-math.git
    main
)

fetch_library( # Networking Library
    clib-networking
    git@github.com:NickBarnes574/clib-networking.git
    main
)

fetch_library( # TCP Server Library
    clib-tcp-server
    git@github.com:NickBarnes574/clib-tcp-server.git
    main
)

fetch_library( # Thread Pool Library
    clib-thread-pool
    git@github.com:NickBarnes574/clib-thread-pool.git
    main
)

fetch_library( # Blocking Queue Library
    clib-blocking-queue
    git@github.com:NickBarnes574/clib-blocking-queue.git
    main
)

# *** END OF FILE ***
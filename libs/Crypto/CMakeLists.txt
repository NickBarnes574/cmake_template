# Object library
add_library(tweetnacl_obj OBJECT src/tweetnacl.c)
set_target_properties(tweetnacl_obj PROPERTIES COMPILE_FLAGS "-w")
set_source_files_properties(src/tweetnacl.c PROPERTIES SKIP_LINTING ON)
target_include_directories(tweetnacl_obj PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Create Crypto lib manually (cannot use function here due to generator expression)
add_library(Crypto
    src/encryption.c
    $<TARGET_OBJECTS:tweetnacl_obj>
)

# Include + compile flags
target_include_directories(Crypto SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_definitions(Crypto PRIVATE _GNU_SOURCE)
target_compile_features(Crypto PRIVATE c_std_17)
target_link_libraries(Crypto PUBLIC Core Warnings)

# Apply project-scoped output
set_project_output_paths("Crypto")
configure_library(Crypto internal)

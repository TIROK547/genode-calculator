# ──────────────────────────────────────────────────────────────────────────────
# Genode/NOVA Cross-Compilation Toolchain for calcirok
#
# Usage:
#   cmake -DGENODE_BUILD=ON \
#         -DGENODE_DIR=/path/to/genode \
#         -DCMAKE_TOOLCHAIN_FILE=cmake/genode_toolchain.cmake \
#         -B build && cmake --build build
# ──────────────────────────────────────────────────────────────────────────────

set(CMAKE_SYSTEM_NAME Genode)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Genode cross-compiler (built via Genode's tool/tool_chain script)
set(GENODE_CROSS_PREFIX "x86_64-pc-elf-")

find_program(CMAKE_C_COMPILER   "${GENODE_CROSS_PREFIX}gcc" REQUIRED)
find_program(CMAKE_CXX_COMPILER "${GENODE_CROSS_PREFIX}g++" REQUIRED)
find_program(CMAKE_LINKER       "${GENODE_CROSS_PREFIX}ld")
find_program(CMAKE_AR           "${GENODE_CROSS_PREFIX}ar")
find_program(CMAKE_RANLIB       "${GENODE_CROSS_PREFIX}ranlib")

# Don't try to run test programs on the host during config
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Sysroot: Genode's ABI stubs live here after 'make' in the Genode tree
if(DEFINED GENODE_DIR)
    set(CMAKE_SYSROOT "${GENODE_DIR}/build/x86_64")
    set(CMAKE_FIND_ROOT_PATH "${GENODE_DIR}/build/x86_64")
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# NOVA microkernel — lets source code conditionalize on kernel type
add_compile_definitions(NOVA_KERNEL)

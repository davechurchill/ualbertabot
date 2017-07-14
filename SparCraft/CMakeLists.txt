file(GLOB SparCraft_SOURCES "source/*.cpp")
file(GLOB_RECURSE SparCraft_HEADERS "source/*.h")

set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_SSCAIT_DIR}/include")
include_directories(
    ${BWAPI_INCLUDE_DIR}
    source
)

add_definitions(-DNOMINMAX)

add_library(SparCraft 
    ${SparCraft_SOURCES}
)

if (MSVC14)
    target_compile_options(SparCraft PRIVATE /await)
endif()

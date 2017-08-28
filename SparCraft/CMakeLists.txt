file(GLOB SparCraft_SOURCES "source/*.cpp")
file(GLOB_RECURSE SparCraft_HEADERS "source/*.h")

if (AKBOT_SSCAIT)
	set(BWAPI_LIB_DIR "$ENV{BWAPI_SSCAIT_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_SSCAIT_DIR}/include")
else()
	set(BWAPI_LIB_DIR "$ENV{BWAPI_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_DIR}/include")
endif()

include_directories(
    ${BWAPI_INCLUDE_DIR}
    source
)

add_definitions(-DNOMINMAX)

add_library(SparCraft 
    ${SparCraft_SOURCES}
	source/torch/TorchTools.cpp
	source/torch/TorchFrame.cpp
)

if (MSVC14)
    target_compile_options(SparCraft PRIVATE /await)
endif()

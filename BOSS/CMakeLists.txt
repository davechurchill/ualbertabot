file(GLOB BOSS_SOURCES "source/*.cpp")
list(REMOVE_ITEM BOSS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/StarCraftGUI.cpp")

file(GLOB_RECURSE BOSS_HEADERS "source/*.h")

if (MSVC14)
	set(BWAPI_LIB_DIR "$ENV{BWAPI_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_DIR}/include")
else()
	set(BWAPI_LIB_DIR "$ENV{BWAPI_SSCAIT_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_SSCAIT_DIR}/include")
endif()

include_directories(
    ${BWAPI_INCLUDE_DIR}
    source
    source/rapidjson
    source/CImg
)

add_definitions(-DNOMINMAX)

add_library(BOSS 
    ${BOSS_SOURCES}
)

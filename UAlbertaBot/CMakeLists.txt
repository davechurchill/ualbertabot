file(GLOB AKBot_SOURCES "source/*.cpp")
file(GLOB AKBot_DEBUG_SOURCES "source/debug/*.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/UAlbertaBotModule_dll.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/Dll.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/Dll_Arena.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/TimerManager.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/CombatData.cpp")
file(GLOB_RECURSE AKBot_HEADERS "source/*.h")

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
)

find_package(Boost 1.62.0)
include_directories(${Boost_INCLUDE_DIRS})

find_path(SDL_INCLUDE_DIR SDL.h)
include_directories(${SDL_INCLUDE_DIR}..)

link_directories(${Boost_LIBRARY_DIRS})
add_definitions(-DNOMINMAX)

add_library(AKBot 
    ${AKBot_SOURCES}
	${AKBot_DEBUG_SOURCES}
)
add_library(AKBot::Lib ALIAS AKBot)
add_library(AKBot::SparCraft ALIAS SparCraft)
add_library(AKBot::BOSS ALIAS BOSS)

add_dependencies(AKBot BOSS SparCraft)
target_link_libraries(AKBot PUBLIC AKBot::Lib AKBot::SparCraft AKBot::BOSS)

if (MSVC14)
    target_compile_options(AKBot PRIVATE /await)
endif()

link_directories(${BWAPI_LIB_DIR})
add_executable(
    AKBot_exe 
    source/main.cpp
)

add_dependencies(AKBot_exe AKBot)
target_link_libraries(AKBot_exe PUBLIC AKBot::Lib AKBot::SparCraft AKBot::BOSS BWAPI.lib BWAPIClient.lib)
if (MSVC14)
    target_compile_options(AKBot_exe PRIVATE /await)
endif()
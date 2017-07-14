file(GLOB AKBot_SOURCES "source/*.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/UAlbertaBotModule_dll.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/Dll.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/Dll_Arena.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/TimerManager.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/CombatData.cpp")
file(GLOB_RECURSE AKBot_HEADERS "source/*.h")

set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_SSCAIT_DIR}/include")
include_directories(
    ${BWAPI_INCLUDE_DIR}
    source
)

find_path(SDL_INCLUDE_DIR SDL.h)
include_directories(${SDL_INCLUDE_DIR}..)


add_definitions(-DNOMINMAX)

add_library(AKBot 
    ${AKBot_SOURCES}
)

add_dependencies(AKBot BOSS SparCraft)

if (MSVC14)
    target_compile_options(AKBot PRIVATE /await)
endif()

add_executable(
    AKBot_exe 
    source/main.cpp
)

add_dependencies(AKBot_exe AKBot)
target_link_libraries(AKBot_exe PUBLIC BWAPI.lib)
if (MSVC14)
    target_compile_options(AKBot_exe PRIVATE /await)
endif()
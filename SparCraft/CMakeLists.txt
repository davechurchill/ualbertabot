file(GLOB SparCraft_SOURCES "source/*.cpp")
file(GLOB_RECURSE SparCraft_HEADERS "source/*.h")
file(GLOB_RECURSE SparCraft_Lib "lib/*.lib")

if (AKBOT_SSCAIT)
	set(BWAPI_LIB_DIR "${BWAPI_SSCAIT_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "${BWAPI_SSCAIT_DIR}/include")
else()
	set(BWAPI_LIB_DIR "${BWAPI_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "${BWAPI_DIR}/include")
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

if(MSVC_VERSION GREATER_EQUAL 1900)
    target_compile_options(SparCraft PRIVATE /await)
endif()

link_directories(${BWAPI_LIB_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_executable(SparCraft_main
    source/gui/GUI.cpp
	source/gui/GUIGame.cpp
	source/gui/GUITools.cpp
	source/main/main.cpp
	source/main/SparCraftExperiment.cpp
	source/TutorialCode.cpp
)

find_package(sdl2 REQUIRED)
#find_package(sdl2-image REQUIRED)
target_link_libraries(SparCraft_main PRIVATE SDL2::SDL2 SDL2::SDL2main SparCraft SDL2_image opengl32)

add_dependencies(SparCraft_main SparCraft)
message(STATUS "Link dir\n" ${BWAPI_LIB_DIR})
target_link_libraries(SparCraft_main PUBLIC SparCraft)

IF(CMAKE_BUILD_TYPE MATCHES Release)
	target_link_libraries(SparCraft_main PUBLIC BWAPI.lib)
ELSE()
	message("Adding debug BWAPI libraries to SparCraft")
	target_link_libraries(SparCraft_main PUBLIC BWAPId.lib)
ENDIF()
	
#target_link_libraries(SparCraft_main PUBLIC SDL2 SDL2_image opengl32)
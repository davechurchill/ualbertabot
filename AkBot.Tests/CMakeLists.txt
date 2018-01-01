file(GLOB AKBotTests_SOURCES "*.cpp" "TestLib/*.cpp" "Shared/*.cpp")
file(GLOB_RECURSE AKBotTest_HEADERS "source/*.h")

if (AKBOT_SSCAIT)
	set(BWAPI_LIB_DIR "$ENV{BWAPI_SSCAIT_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_SSCAIT_DIR}/include")
	set(BWAPIClient_INCLUDE_DIR "$ENV{BWAPI_SSCAIT_DIR}/include/Client")
else()
	set(BWAPI_LIB_DIR "$ENV{BWAPI_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_DIR}/include")
	set(BWAPIClient_INCLUDE_DIR "$ENV{BWAPI_DIR}/include/Client")
endif()

find_package(Boost 1.62.0)
include_directories(${Boost_INCLUDE_DIRS})

find_path(SDL_INCLUDE_DIR SDL.h)
include_directories(${SDL_INCLUDE_DIR}..)

link_directories(${Boost_LIBRARY_DIRS})
add_definitions(-DNOMINMAX)

link_directories(${BWAPI_LIB_DIR})
add_definitions(-DWITH_CMAKE)

add_library(AKBotTests SHARED
    ${AKBotTests_SOURCES}
)

target_include_directories(AKBotTests BEFORE PUBLIC
	${BWAPI_INCLUDE_DIR}
	${BWAPIClient_INCLUDE_DIR}
	"../UAlbertaBot/source/"
	"TestLib"
)

add_library(AKBot::Lib ALIAS AKBot)
add_library(AKBot::SparCraft ALIAS SparCraft)
add_library(AKBot::BOSS ALIAS BOSS)


IF(CMAKE_BUILD_TYPE MATCHES Debug)
	message("Adding debug BWAPI libraries")
	target_link_libraries(AKBotTests PUBLIC BWAPId.lib BWAPIClientd.lib)
ELSE()
	target_link_libraries(AKBotTests PUBLIC BWAPI.lib BWAPIClient.lib)
ENDIF()

add_dependencies(AKBotTests AKBot)
target_link_libraries(AKBotTests PUBLIC AKBot::Lib AKBot::SparCraft AKBot::BOSS)

if (MSVC14)
    target_compile_options(AKBotTests PRIVATE /await)
endif()

link_directories(${BWAPI_LIB_DIR})

if (AKBOT_SSCAIT)
	set(BWAPI_LIB_DIR "$ENV{BWAPI_SSCAIT_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_SSCAIT_DIR}/include")
	set(BWAPIClient_INCLUDE_DIR "$ENV{BWAPI_SSCAIT_DIR}/include/Client")
else()
	set(BWAPI_LIB_DIR "$ENV{BWAPI_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_DIR}/include")
	set(BWAPIClient_INCLUDE_DIR "$ENV{BWAPI_DIR}/include/Client")
endif()
file(GLOB AKBotTests_SOURCES "../AkBot.Tests/GameHelper.cpp" "../AkBot.Tests/UnitHelper.cpp" "../AkBot.Tests/TestLib/*.cpp" "../AkBot.Tests/Shared/*.cpp")

link_directories(${BWAPI_LIB_DIR})
#add_definitions(-DWITH_CMAKE)
#find_package(BWAPI PATHS $ENV{BWAPI_DIR)
#find_library(BWAPI_LIBRARY BWAPI HINTS $ENV{BWAPI_DIR}/lib)
add_definitions(-DNOMINMAX)

add_executable(AKBot_CombatSimulation
	main.cpp
	${AKBotTests_SOURCES}
)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
#	message("Adding debug BWAPI libraries")
	#find_library(BWAPI_LIBRARY BWAPId HINTS ENV BWAPI_DIR)
	target_link_libraries(AKBot_CombatSimulation PUBLIC BWAPId.lib BWAPIClientd.lib)
ELSE()
	#find_library(BWAPI_LIBRARY BWAPI HINTS $ENV{BWAPI_DIR}/lib)
	#find_library(BWAPI_LIBRARY BWAPI HINTS ENV BWAPI_DIR)
	target_link_libraries(AKBot_CombatSimulation PUBLIC BWAPI.lib BWAPIClient.lib)
ENDIF()
#target_link_libraries(AKBot_CombatSimulation PUBLIC ${BWAPI_LIBRARY})

add_library(AKBot::Lib ALIAS AKBot)
add_dependencies(AKBot_CombatSimulation AKBot)
target_link_libraries(AKBot_CombatSimulation PUBLIC AKBot::Lib AKBot::SparCraft AKBot::BOSS)

target_include_directories(AKBot_CombatSimulation BEFORE PUBLIC
	"${BWAPI_INCLUDE_DIR}"
	#"${BWAPI_INCLUDE_DIR}/BWAPI/Client"
	"${BWAPI_SSCAIT_DIR}/Shared"
	"../UAlbertaBot/source/"
	"../AkBot.Tests/"
	"../AkBot.Tests/TestLib/"
	#"../AkBot.Tests/Shared"
)
if (MSVC14)
    target_compile_options(AKBot_CombatSimulation PRIVATE /await)
endif()
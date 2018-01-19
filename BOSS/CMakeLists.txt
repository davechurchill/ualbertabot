file(GLOB BOSS_SOURCES "source/*.cpp")
list(REMOVE_ITEM BOSS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/StarCraftGUI.cpp")

file(GLOB_RECURSE BOSS_HEADERS "source/*.h")

list(APPEND BOSS_MAIN_SOURCES 
	"source/BOSSExperiments.cpp"
	"source/BOSSParameters.cpp"
	"source/BOSSPlotBuildOrders.cpp"
	"source/CombatSearchExperiment.cpp"
	"source/BOSS_main.cpp"
	"source/BuildOrderTester.cpp"
)

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
    source/rapidjson
    source/CImg
)

add_definitions(-DNOMINMAX)

add_library(BOSS 
    ${BOSS_SOURCES}
)

include_directories(
    ${BWAPI_INCLUDE_DIR}
    source/rapidjson
)

if (AKBOT_SSCAIT)	
	set(BWAPI_INCLUDE_DIR "${BWAPI_SSCAIT_DIR}/include")
	if (MSVC14)
		# For embedding BWAPI 4.1.2 you should manually place Debug.h and starcraftver.h  in the root of BWAPI folder
		# if use installer and create svnrev.h with approximately following content
		#
		# static const int SVN_REV = 4708;
		# #include "starcraftver.h"
		# Value of SVN_REV is important, since BWAPI require match of these value with value hardcoded in the BWAPI.dll
		file(GLOB BWAPILib_SOURCES "${BWAPI_SSCAIT_DIR}/BWAPILib/Source/*.cpp")
		file(GLOB BWAPIClient_SOURCES "${BWAPI_SSCAIT_DIR}/BWAPIClient/Source/*.cpp")
		file(GLOB BWAPIShared_SOURCES "${BWAPI_SSCAIT_DIR}/Shared/*.cpp")

		add_library(BWAPILib 
			${BWAPILib_SOURCES}
			#${BWAPIClient_SOURCES}
			"${BWAPIShared_SOURCES}"
			"${BWAPI_SSCAIT_DIR}/BWAPILib/UnitCommand.cpp"
		)
		add_library(AKBot::BWAPILib ALIAS BWAPILib)

		target_include_directories(BWAPILib BEFORE PUBLIC
			"${BWAPI_INCLUDE_DIR}"
			"${BWAPI_INCLUDE_DIR}/BWAPI/Client"
			"${BWAPI_SSCAIT_DIR}/Shared"
		)

		add_library(BWAPIClient
			#${BWAPILib_SOURCES}
			${BWAPIClient_SOURCES}
			${BWAPIShared_SOURCES}
		)
		add_library(AKBot::BWAPIClient ALIAS BWAPIClient)

		target_include_directories(BWAPIClient BEFORE 
			PUBLIC "${BWAPI_INCLUDE_DIR}"
			"${BWAPI_INCLUDE_DIR}/BWAPI/Client"
			PUBLIC "${BWAPI_SSCAIT_DIR}/Shared"
			)
	else()
		set(BWAPI_LIB_DIR "$ENV{BWAPI_SSCAIT_DIR}/lib")
	endif()
else()
	set(BWAPI_LIB_DIR "${BWAPI_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "${BWAPI_DIR}/include")
endif()
link_directories(${BWAPI_LIB_DIR})

add_executable(BOSS_main
	${BOSS_MAIN_SOURCES}
)
add_dependencies(BOSS_main BOSS)
target_link_libraries(BOSS_main PUBLIC BOSS)
IF(CMAKE_BUILD_TYPE MATCHES Debug)
	message("Adding debug BWAPI libraries")
	target_link_libraries(BOSS_main PUBLIC BWAPId.lib)
ELSE()
	target_link_libraries(BOSS_main PUBLIC BWAPI.lib)
ENDIF()
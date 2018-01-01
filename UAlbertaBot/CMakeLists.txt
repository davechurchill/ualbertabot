file(GLOB AKBot_SOURCES "source/*.cpp")
file(GLOB AKBot_DEBUG_SOURCES "source/debug/*.cpp")
file(GLOB AKBot_Strategy_SOURCES "source/strategies/**/*.cpp" "source/strategies/*.cpp")
file(GLOB AKBot_Commands_SOURCES "source/commands/*.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/UAlbertaBotModule_dll.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/Dll.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/Dll_Arena.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/TimerManager.cpp")
list(REMOVE_ITEM AKBot_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/source/CombatData.cpp")
file(GLOB_RECURSE AKBot_HEADERS "source/*.h")

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

find_package(Boost 1.62.0)
include_directories(${Boost_INCLUDE_DIRS})

find_path(SDL_INCLUDE_DIR SDL.h)
include_directories(${SDL_INCLUDE_DIR}..)

link_directories(${Boost_LIBRARY_DIRS})
add_definitions(-DNOMINMAX)

add_library(AKBot 
    ${AKBot_SOURCES}
    source/commands/CommandManager.cpp
	${AKBot_DEBUG_SOURCES}
	${AKBot_Strategy_SOURCES}
	${AKBot_Commands_SOURCES}
	#${BWAPILib_SOURCES}
	#${BWAPIClient_SOURCES}
	#${BWAPIShared_SOURCES}
)
target_include_directories(AKBot BEFORE PUBLIC
	${BWAPI_INCLUDE_DIR}
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
target_link_libraries(AKBot_exe PUBLIC AKBot::Lib)
target_include_directories(AKBot_exe BEFORE PUBLIC
	${BWAPI_INCLUDE_DIR}
)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
	message("Adding debug BWAPI libraries")
	target_link_libraries(AKBot_exe PUBLIC BWAPId.lib BWAPIClientd.lib)
ELSE()
	target_link_libraries(AKBot_exe PUBLIC BWAPI.lib BWAPIClient.lib)
ENDIF()

if (MSVC14)
    target_compile_options(AKBot_exe PRIVATE /await)
endif()
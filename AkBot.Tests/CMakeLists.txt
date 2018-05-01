file(GLOB AKBotTests_Lib "TestLib/*.cpp" "Shared/*.cpp" "SupportLib/*.cpp")

if (AKBOT_SSCAIT)
	set(BWAPI_LIB_DIR "$ENV{BWAPI_SSCAIT_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_SSCAIT_DIR}/include")
	set(BWAPIClient_INCLUDE_DIR "$ENV{BWAPI_SSCAIT_DIR}/include/Client")
else()
	set(BWAPI_LIB_DIR "$ENV{BWAPI_DIR}/lib")
	set(BWAPI_INCLUDE_DIR "$ENV{BWAPI_DIR}/include")
	set(BWAPIClient_INCLUDE_DIR "$ENV{BWAPI_DIR}/include/Client")
endif()

set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost 1.62.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_path(SDL_INCLUDE_DIR SDL.h)
include_directories(${SDL_INCLUDE_DIR}..)

link_directories(${Boost_LIBRARY_DIRS})
add_definitions(-DNOMINMAX)

link_directories(${BWAPI_LIB_DIR})
#add_definitions(-DWITH_CMAKE)

add_library(AKBot::Lib ALIAS AKBot)
add_library(AKBot::SparCraft ALIAS SparCraft)
add_library(AKBot::BOSS ALIAS BOSS)

MESSAGE( STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_CONFIGURATION_TYPES} )
MESSAGE( STATUS "MSVC_VERSION: " ${MSVC_VERSION} )

link_directories(${BWAPI_LIB_DIR})

add_library(BotTestLib ${AKBotTests_Lib})
target_include_directories(BotTestLib BEFORE PUBLIC
	${BWAPI_INCLUDE_DIR}
	${BWAPIClient_INCLUDE_DIR}
	"../UAlbertaBot/source/"
	"TestLib"
)

file(GLOB BOOST_TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *Test.cpp basedetection/*Test.cpp)
foreach(testSrc ${BOOST_TEST_SRCS})
	#Extract the filename without an extension (NAME_WE)
    get_filename_component(testName ${testSrc} NAME_WE)

    #Add compile target
	add_definitions(-DWITH_CMAKE)
    add_executable(${testName}
		${testSrc})
	target_include_directories(${testName} BEFORE PUBLIC
		${BWAPI_INCLUDE_DIR}
		${BWAPIClient_INCLUDE_DIR}
		"../UAlbertaBot/source/"
		"TestLib"
	)

    #link to Boost libraries AND your targets and dependencies
    #target_link_libraries(${testName} ${Boost_LIBRARIES} [YOUR TARGETS/LIBRARIES])
	IF(CMAKE_CONFIGURATION_TYPES MATCHES Debug)
		target_link_libraries(${testName} PUBLIC BWAPId.lib BWAPIClientd.lib)
	ELSE()
		target_link_libraries(${testName} PUBLIC BWAPI.lib BWAPIClient.lib)
	ENDIF()
	target_link_libraries(${testName} PUBLIC AKBot::Lib AKBot::SparCraft AKBot::BOSS BotTestLib)

    #I like to move testing binaries into a testBin directory
    set_target_properties(${testName} PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/testBin)

    #Finally add it to test execution - 
    #Notice the WORKING_DIRECTORY and COMMAND
    add_test(NAME ${testName} 
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin 
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin/${testName})

endforeach(testSrc)

project (AKBot)
cmake_minimum_required (VERSION 2.6)
option(AKBOT_SSCAIT "Generate SSCAIT supported version")

set(BWAPI_SSCAIT_DIR "$ENV{BWAPI_SSCAIT_DIR}")
STRING(REPLACE "\\" "/" BWAPI_SSCAIT_DIR ${BWAPI_SSCAIT_DIR})

add_subdirectory (BOSS)
add_subdirectory (SparCraft) 
add_subdirectory (UAlbertaBot) 

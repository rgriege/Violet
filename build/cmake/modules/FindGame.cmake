# - Locate Game library
# This module defines
#  GAME_LIBRARIES, the name of the library to link against
#  GAME_FOUND, if false, do not try to link to Game
#  GAME_INCLUDE_DIRS, where to find engine/Game.h
#

include(FindPkgMacros)
findpkg_begin(GAME)

# construct search paths
set(GAME_LIB_SEARCH_PATH ${Violet_LIB})

set (GAME_INCLUDE_DIRS ${Violet_SRC} CACHE STRING "Game Include Dirs")

find_library(GAME_LIBRARIES NAMES Game HINTS ${GAME_LIB_SEARCH_PATH})

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(GAME
                                  REQUIRED_VARS GAME_LIBRARIES GAME_INCLUDE_DIRS)

set (GAME_FOUND ${GAME_FOUND})

mark_as_advanced (GAME_INCLUDE_DIRS GAME_LIBRARIES)

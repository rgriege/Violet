# - Locate Engine library
# This module defines
#  ENGINE_LIBRARIES, the name of the library to link against
#  ENGINE_FOUND, if false, do not try to link to Engine
#  ENGINE_INCLUDE_DIRS, where to find engine/Engine.h
#

include(FindPkgMacros)
findpkg_begin(ENGINE)

# construct search paths
set(ENGINE_LIB_SEARCH_PATH ${Violet_LIB})

set (ENGINE_INCLUDE_DIRS ${Violet_SRC} CACHE STRING "Engine Include Dirs")

find_library(ENGINE_LIBRARIES NAMES Engine HINTS ${ENGINE_LIB_SEARCH_PATH})

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(ENGINE
                                  REQUIRED_VARS ENGINE_LIBRARIES ENGINE_INCLUDE_DIRS)

set (ENGINE_FOUND ${ENGINE_FOUND})

mark_as_advanced (ENGINE_INCLUDE_DIRS ENGINE_LIBRARIES)

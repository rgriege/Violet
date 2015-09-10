# - Locate Editor library
# This module defines
#  EDITOR_LIBRARIES, the name of the library to link against
#  EDITOR_FOUND, if false, do not try to link to Editor
#  EDITOR_INCLUDE_DIRS, where to find engine/Editor.h
#

include(FindPkgMacros)
findpkg_begin(EDITOR)

# construct search paths
set(EDITOR_LIB_SEARCH_PATH ${Violet_LIB})

set (EDITOR_INCLUDE_DIRS ${Violet_SRC} CACHE STRING "Editor Include Dirs")

find_library(EDITOR_LIBRARIES NAMES Editor HINTS ${EDITOR_LIB_SEARCH_PATH})

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(EDITOR
                                  REQUIRED_VARS EDITOR_LIBRARIES EDITOR_INCLUDE_DIRS)

set (EDITOR_FOUND ${EDITOR_FOUND})

mark_as_advanced (EDITOR_INCLUDE_DIRS EDITOR_LIBRARIES)

# - Locate Freeglut library
# This module defines
#  Freeglut_LIBRARIES, the name of the library to link against
#  Freeglut_FOUND, if false, do not try to link to Freeglut
#  Freeglut_INCLUDE_DIRS, where to find Freeglut.h
#

include(FindPkgMacros)
findpkg_begin(Freeglut)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(Freeglut_HOME)

# construct search paths
set(Freeglut_PREFIX_PATH ${Freeglut_HOME} ${ENV_Freeglut_HOME})
create_search_paths(Freeglut)
# redo search if prefix path changed
clear_if_changed(Freeglut_PREFIX_PATH)

# For Freeglut, prefer static library over framework (important when referencing Freeglut source build)
set(CMAKE_FIND_FRAMEWORK "LAST")

find_path(Freeglut_INCLUDE_DIRS freeglut.h HINTS ${Freeglut_INC_SEARCH_PATH} PATH_SUFFIXES GL)

if (MSVC)
  find_library(Freeglut_LIBRARIES NAMES freeglut HINTS ${Freeglut_LIB_SEARCH_PATH} PATH_SUFFIXES x86/Debug)
else ()
  find_library(Freeglut_LIBRARIES NAMES glut HINTS ${Freeglut_LIB_SEARCH_PATH})
endif (MSVC)

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Freeglut
                                  REQUIRED_VARS Freeglut_LIBRARIES Freeglut_INCLUDE_DIRS)

set (Freeglut_FOUND ${FREEGLUT_FOUND})

mark_as_advanced (Freeglut_INCLUDE_DIRS Freeglut_LIBRARIES)

# Reset framework finding
set(CMAKE_FIND_FRAMEWORK "FIRST")
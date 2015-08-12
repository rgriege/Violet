# - Locate JsonCpp library
# This module defines
#  JSONCPP_LIBRARIES, the name of the library to link against
#  JSONCPP_FOUND, if false, do not try to link to JsonCpp
#  JSONCPP_INCLUDE_DIRS, where to find json/JsonCpp.h
#

include(FindPkgMacros)
findpkg_begin(JSONCPP)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(JSONCPP_HOME)

# construct search paths
set(JSONCPP_PREFIX_PATH ${JSONCPP_HOME} ${ENV_JSONCPP_HOME})
create_search_paths(JSONCPP)
# redo search if prefix path changed
clear_if_changed(JSONCPP_PREFIX_PATH)

# For JsonCpp, prefer static library over framework (important when referencing JsonCpp source build)
set(CMAKE_FIND_FRAMEWORK "LAST")

find_path(JSONCPP_INCLUDE_DIRS_CHILD json.h HINTS ${JSONCPP_INC_SEARCH_PATH} PATH_SUFFIXES json jsoncpp/json)
get_parent_dir(JSONCPP_INCLUDE_DIRS_CHILD)
set (JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIRS_CHILD_PARENT} CACHE STRING "JsonCpp Include Dirs")
unset (JSONCPP_INCLUDE_DIRS_CHILD CACHE)

find_library(JSONCPP_LIBRARIES NAMES jsoncpp HINTS ${JSONCPP_LIB_SEARCH_PATH})

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(JSONCPP
                                  REQUIRED_VARS JSONCPP_LIBRARIES JSONCPP_INCLUDE_DIRS)

set (JSONCPP_FOUND ${JSONCPP_FOUND})

mark_as_advanced (JSONCPP_INCLUDE_DIRS JSONCPP_LIBRARIES)

# Reset framework finding
set(CMAKE_FIND_FRAMEWORK "FIRST")

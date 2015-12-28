# Minimum cmake requirement. We should require a quite recent
# cmake for the dependency find macros etc. to be up to date.
cmake_minimum_required (VERSION 2.6)

# Set CMake library search policy
if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    cmake_policy (SET CMP0005 NEW)
    cmake_policy (SET CMP0017 OLD)
endif ()

# Create project
project (Violet)

# Use relative paths
# This is mostly to reduce path size for command-line limits on windows
if (WIN32)
  set (CMAKE_USE_RELATIVE_PATHS true)
endif ()

# Paths
set (Violet_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set (Violet_INCLUDE ${Violet_ROOT}/src)
set (Violet_SRC ${Violet_ROOT}/src)
set (Violet_BIN ${Violet_ROOT}/bin)
set (Violet_LIB ${Violet_ROOT}/bin)

# Include necessary submodules
set (CMAKE_MODULE_PATH "${Violet_ROOT}/build/cmake/modules")
include (LinkLibraries)

set (Violet_MAJOR_VERSION 0)
set (Violet_MINOR_VERSION 2)
set (Violet_PATCH_VERSION 0)
set (Violet_VERSION ${Violet_MAJOR_VERSION}.${Violet_MINOR_VERSION}.${Violet_PATCH_VERSION})

# Disable unnecessary build types
set (CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "Configurations" FORCE)


# CMake install step prefix. I assume linux users want the prefix to
# be the default /usr or /usr/local so this is only adjusted on Windows.
# - Windows: Build the INSTALL project in your solution file.
# - Linux/OSX: make install.
if (MSVC)
    set (CMAKE_INSTALL_PREFIX "${Violet_ROOT}/install")
endif ()

# Specify build paths
if (MSVC)
  foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string (TOUPPER ${CONFIG} CONFIG)
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${Violet_LIB})
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${Violet_LIB})
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${Violet_BIN})
  endforeach (CONFIG)
else ()
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${Violet_LIB})
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${Violet_LIB})
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Violet_BIN})
endif ()

# Preprocessing environment variables
if (WIN32)
  if (SDL_HOME AND NOT ENV{SDLDIR})
    set (ENV{SDLDIR} ${SDL_HOME})
  endif (SDL_HOME AND NOT ENV{SDLDIR})
  if (SDL_MIXER_HOME AND NOT ENV{SDLMIXERDIR})
    set (ENV{SDLMIXERDIR} ${SDL_MIXER_HOME})
  endif (SDL_MIXER_HOME AND NOT ENV{SDLMIXERDIR})
endif (WIN32)

# Set solution-wide flags
if (MSVC)
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
elseif (UNIX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -g")
endif ()

add_subdirectory (engine)
add_subdirectory (editor)
add_subdirectory (app)
# add_subdirectory (Server)
# option (BUILD_DESKTOP_CLIENT "Build the desktop render client" OFF)
# if (BUILD_DESKTOP_CLIENT)
  # add_subdirectory (DesktopClient)
# endif (BUILD_DESKTOP_CLIENT)

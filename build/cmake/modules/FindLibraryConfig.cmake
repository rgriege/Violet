# - Try to find LibraryCoded
# Once done, this will define
#
#  LibraryConfig_FOUND - system has LibraryConfig
#  LibraryConfig_INCLUDE_DIRS - the LibraryConfig include directories 
#  LibraryConfig_LIBRARIES - link these to use LibraryConfig

set (LibraryConfig_INCLUDE_DIR ${RemoteRenderer_ROOT}/LibraryConfig)

if (WIN32)
    set (LibraryConfig_LIBRARY_DBG ${RemoteRenderer_LIB}/LibraryConfig_d.lib)
    set (LibraryConfig_LIBRARY_REL ${RemoteRenderer_LIB}/LibraryConfig.lib)
    mark_as_advanced(LibraryConfig_BINARY_REL LibraryConfig_BINARY_DBG)
else ()
    set (LibraryConfig_LIBRARY_DBG ${RemoteRenderer_LIB}/libLibraryConfig_d.so)
    set (LibraryConfig_LIBRARY_REL ${RemoteRenderer_LIB}/libLibraryConfig.so)
endif()
make_library_set(LibraryConfig_LIBRARY)

findpkg_finish(LibraryConfig)

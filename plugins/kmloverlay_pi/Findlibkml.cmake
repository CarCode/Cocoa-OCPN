# - Find libkml
# Find the native libkml includes and library
#
#   libkml_FOUND       - True if libkml found.
#   libkml_INCLUDE_DIR - where to find tinyxml.h, etc.
#   libkml_LIBRARIES   - List of libraries when using libkml.
#

IF( libkml_INCLUDE_DIR )
    # Already in cache, be silent
    SET( libkml_FIND_QUIETLY TRUE )
ENDIF( libkml_INCLUDE_DIR )

if(NOT libkml_FIND_COMPONENTS)
    # Assume they only want dom
    set(libkml_FIND_COMPONENTS dom)
endif()

IF(WIN32)
    # required libs for libKML
    # This ease use of cmake for Windows users
    # Unices handle this as deps of libkml
    FIND_LIBRARY( libkml_MINIZIP_LIBRARY NAMES "minizip" )
    FIND_LIBRARY( libkml_ZLIB_LIBRARY NAMES "z" )
    FIND_LIBRARY( libkml_EXPAT_LIBRARY NAMES "expat" )
    FIND_LIBRARY( libkml_EXPAT_DLL NAMES "expat" )
    FIND_LIBRARY( libkml_ZLIB_DLL NAMES "zlib" )
    FIND_LIBRARY( libkml_URIPARSER_LIBRARY NAMES "uriparser" )
    SET( libkml_LIBRARIES ${libkml_MINIZIP_LIBRARY} ${libkml_ZLIB_LIBRARY} ${libkml_EXPAT_LIBRARY} ${libkml_URIPARSER_LIBRARY} )
ENDIF(WIN32)

# base is always required
FIND_LIBRARY( libkml_BASE_LIBRARY NAMES "kmlbase" )
SET( libkml_LIBRARIES ${libkml_LIBRARIES} ${libkml_BASE_LIBRARY} )

foreach(_component ${libkml_FIND_COMPONENTS})
    if(_component STREQUAL "dom")
        FIND_PATH( libkml_KML_INCLUDE_DIR "dom.h" PATH_SUFFIXES "kml" )
        FIND_LIBRARY( libkml_DOM_LIBRARY NAMES "kmldom" )
        SET( libkml_LIBRARIES ${libkml_LIBRARIES} ${libkml_DOM_LIBRARY} )
        # boost headers are required as well (in kml/dom/element.h)
        FIND_PATH( libkml_BOOST_INCLUDE_DIR "scoped_ptr.hpp" PATH_SUFFIXES "boost" )
        SET( libkml_INCLUDE_DIR ${libkml_KML_INCLUDE_DIR} ${libkml_BOOST_INCLUDE_DIR} )
    elseif(_component STREQUAL "engine")
        FIND_PATH( libkml_KML_INCLUDE_DIR "engine.h" PATH_SUFFIXES "kml" )
        FIND_LIBRARY( libkml_ENGINE_LIBRARY NAMES "kmlengine" )
        SET( libkml_LIBRARIES ${libkml_LIBRARIES} ${libkml_ENGINE_LIBRARY} )
        # boost headers are required as well (in kml/engine/kml_file.h and others)
        FIND_PATH( libkml_BOOST_INCLUDE_DIR "scoped_ptr.hpp" PATH_SUFFIXES "boost" )
        SET( libkml_INCLUDE_DIR ${libkml_KML_INCLUDE_DIR} ${libkml_BOOST_INCLUDE_DIR} )
    else()
        message(FATAL_ERROR "Unknown libkml component ${_component}")
    endif()
endforeach()

list(REMOVE_DUPLICATES libkml_INCLUDE_DIR)
list(REMOVE_DUPLICATES libkml_LIBRARIES)

# handle the QUIETLY and REQUIRED arguments and set libkml_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( "FindPackageHandleStandardArgs" )

FIND_PACKAGE_HANDLE_STANDARD_ARGS( "libkml" DEFAULT_MSG libkml_INCLUDE_DIR libkml_LIBRARIES )

MARK_AS_ADVANCED( libkml_INCLUDE_DIR libkml_LIBRARIES )

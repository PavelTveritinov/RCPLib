cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

include("./cmake/configure_project.cmake")

set( RCPLIB_TARGET "RCPLib" )
project( ${RCPLIB_TARGET} )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

if( APPLE )
    enable_language(OBJCXX)
    enable_language(OBJC)

    set(CMAKE_C_FLAGS "-x objective-c")
    set(TARGET_PLATFORM "macOS")

    set(PLATFORM "mac")
    set(SHARED_LIB_EXT ".dylib")
    set(SYMBOLS_FILE_EXT ".dylib.dSYM")
    set(SHARED_LIB_PREFIX "lib")

#    if(${CMAKE_OSX_ARCHITECTURES} STREQUAL "x86_64")
#        set(ARCH "x64")
#    else()
#        set(ARCH "arm64")
#    endif()
elseif( WIN32 )
    set(PLATFORM "win")
    set(SHARED_LIB_EXT ".dll")
    set(SYMBOLS_FILE_EXT ".pdb")
#    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "ARM64")
#        set(ARCH "arm64")
#    else()
#        set(ARCH "x64")
#    endif()
    set(DEPS_CONFIGURATION ${CMAKE_BUILD_TYPE})
endif()

include_directories( ${PROJECT_SOURCE_DIR}/include )

###############################################################
# Unicode
###############################################################
add_definitions( -DUNICODE -D_UNICODE )

###############################################################
# Output
###############################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Collect all sources files into variable SOURCES

collect_sources( ${PROJECT_SOURCE_DIR}/include )
collect_sources( ${PROJECT_SOURCE_DIR}/src )

collect_sources( ${PROJECT_SOURCE_DIR}/include/data )

collect_sources( ${PROJECT_SOURCE_DIR}/include/utils )
collect_sources( ${PROJECT_SOURCE_DIR}/src/utils )
collect_sources( ${PROJECT_SOURCE_DIR}/src/utils/${PLATFORM} )

collect_sources( ${PROJECT_SOURCE_DIR}/include/platform )
collect_sources( ${PROJECT_SOURCE_DIR}/src/platform )
collect_sources( ${PROJECT_SOURCE_DIR}/src/platform/${PLATFORM} )

collect_sources( ${PROJECT_SOURCE_DIR}/include/threads )
collect_sources( ${PROJECT_SOURCE_DIR}/src/threads )
collect_sources( ${PROJECT_SOURCE_DIR}/src/threads/${PLATFORM} )

if( WIN32 )
    collect_sources( ${PROJECT_SOURCE_DIR}/include/win )
    collect_sources( ${PROJECT_SOURCE_DIR}/src/win )
endif()


add_library( ${RCPLIB_TARGET} ${SOURCES} ${INCS} )

#target_include_directories( ${RCPLIB_TARGET} PUBLIC ${RCPLIB_TARGET}/include )


cmake_minimum_required(VERSION 3.18)
project(storm-engine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(conan)
include(StormSetup)

option(STORM_ENABLE_CRASH_REPORTS "Enable automatic crash reports" OFF)
option(STORM_ENABLE_STEAM "Enable Steam integration" OFF)
if (NOT WIN32)
option(STORM_MESA_NINE "Use Gallium Nine from Mesa (without WINE) for D3D9 API" OFF)
endif()

### Set up output paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)

set(STORM_WATERMARK_FILE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/watermark.hpp CACHE FILEPATH "Include file containing build revision, etc." FORCE)

message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
if (CMAKE_SYSTEM_PROCESSOR MATCHES "armv*")
    set(PROJECT_PLATFORM_ARM TRUE)
elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(PROJECT_PLATFORM_ARM64 TRUE)
endif()

if (PROJECT_PLATFORM_ARM)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon -fsigned-char")
endif()

### Set up third-party dependencies
set(ENV{CONAN_REVISIONS_ENABLED} 1)
conan_add_remote(NAME bincrafters
    URL https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
    VERIFY_SSL True
)
conan_add_remote(NAME storm
    URL https://storm.jfrog.io/artifactory/api/conan/conan-remote
    VERIFY_SSL True
)
normalize_booleans(STORM_ENABLE_CRASH_REPORTS STORM_ENABLE_STEAM)
conan_cmake_run(CONANFILE conanfile.py
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
    OPTIONS
        output_directory=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        watermark_file=${STORM_WATERMARK_FILE}
        crash_reports=${STORM_ENABLE_CRASH_REPORTS}
        steam=${STORM_ENABLE_STEAM}
)

if (NOT WIN32)
    find_package(SDL REQUIRED)
    message(STATUS "SDL_LIBRARIES="${SDL_LIBRARIES})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${SDL_INCLUDE_DIR}")
    message(STATUS "CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS})
endif()

if (WIN32)
    message("Using Windows D3D9 API")
    set(SDL_LIBRARIES "sdl")
elseif (STORM_MESA_NINE) # for Gallium Nine
    message("Using Gallium Nine for native D3D9 API")

    #sudo apt install libd3dadapter9-mesa-dev
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(D3D REQUIRED IMPORTED_TARGET d3d)

    add_subdirectory(nine-native)
    set(NINE_NATIVE_INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/nine-native/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/nine-native/include/D3D9"
    )
    include_directories("${NINE_NATIVE_INCLUDE_DIRS}")
else() # for DXVK Native
    message("Using DXVK-native for D3D9 API")

    include(ExternalProject)
    ExternalProject_Add(dxvk-native
        GIT_REPOSITORY    https://github.com/IonAgorria/dxvk-native
        GIT_TAG           2e91e784a86bc1355100518cd90eee48433551ac
        GIT_SHALLOW       ON
        BUILD_ALWAYS      OFF
        CONFIGURE_HANDLED_BY_BUILD ON
        CONFIGURE_COMMAND meson ../dxvk-native
        BUILD_COMMAND     ninja
        INSTALL_COMMAND   ""
    )
    ExternalProject_Get_property(dxvk-native SOURCE_DIR BINARY_DIR)
    set(DXVK_NATIVE_INCLUDE_DIRS
        "${SOURCE_DIR}/include/native/directx"
        "${SOURCE_DIR}/include/native/windows"
    )
    set(DXVK_NATIVE_D3D9_LIB ${BINARY_DIR}/src/d3d9/libdxvk_d3d9.so)
    include_directories("${DXVK_NATIVE_INCLUDE_DIRS}")
    ADD_CUSTOM_TARGET(dependencies ALL DEPENDS dxvk-native)
endif()

### Define library ALIASes for use without CONAN_PKG:: prefix
foreach (conan_target ${CONAN_TARGETS})
    string(REPLACE "CONAN_PKG::" "" unprefixed_target ${conan_target})
	add_library(${unprefixed_target} ALIAS ${conan_target})
endforeach()
###

### Set up build scripts
set(SRC_DIRS "src" CACHE STRING "File locations for public header files" FORCE)
set(PUBLIC_INCLUDE_DIRS "include" CACHE STRING "File locations for source files" FORCE)
set(TESTSUITE_DIRS "testsuite" CACHE STRING "File locations for tests" FORCE)
set(RESOURCE_DIRS "rsrc" CACHE STRING "File locations for tests" FORCE)

set(SOURCE_FILE_EXTENSIONS "*.cpp" CACHE STRING "File extensions for source files" FORCE)
set(HEADER_FILE_EXTENSIONS "*.h" "*.hpp" CACHE STRING "File extensions for header files" FORCE)
###

add_subdirectory(src)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT engine)

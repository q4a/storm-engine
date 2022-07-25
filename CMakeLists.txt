cmake_minimum_required(VERSION 3.18)
project(storm-engine)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
option(STORM_ENABLE_CONAN "Enable conan package manager" ON)
if (STORM_ENABLE_CONAN)
    include(conan)
endif()
include(StormSetup)

option(STORM_ENABLE_CRASH_REPORTS "Enable automatic crash reports" OFF)
option(STORM_ENABLE_STEAM "Enable Steam integration" OFF)

### Set up output paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)

set(STORM_WATERMARK_FILE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/watermark.hpp CACHE FILEPATH "Include file containing build revision, etc." FORCE)

### Set up third-party dependencies
if (STORM_ENABLE_CONAN)
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

    set(LIBSAFEC_LIBRARIES "libsafec")
    set(SDL2_LIBRARIES "sdl")
    set(SPDLOG_LIBRARIES "spdlog")
    set(ZLIB_LIBRARIES "sentry-native")
    set(SENTRY_NATIVE_LIBRARIES "sentry-native")
    set(FMOD_LIBRARIES "fmod")
else()
    set(CMAKE_CXX_STANDARD 20)

    include(FindPkgConfig)
    pkg_search_module(CATCH2 REQUIRED catch2>=2.13.7)
    message("CATCH2 found ${CATCH2_LIBRARIES}")
    pkg_search_module(LIBSAFEC REQUIRED libsafec>=3.6.0)
    message("LIBSAFEC found ${LIBSAFEC_LIBRARIES}")
    pkg_search_module(SDL2 REQUIRED sdl2>=2.0.18)
    message("SDL2 found ${SDL2_LIBRARIES}")
    pkg_search_module(SPDLOG REQUIRED spdlog>=1.9.2)
    message("SPDLOG found ${SPDLOG_LINK_LIBRARIES}")
    pkg_search_module(ZLIB REQUIRED zlib>=1.2.11)
    message("ZLIB found ${ZLIB_LIBRARIES}")
    set(FMOD_LIBRARIES "$<IF:$<CONFIG:Release>,${CMAKE_CURRENT_SOURCE_DIR}/extern/fmod/lib/libfmod.so,${CMAKE_CURRENT_SOURCE_DIR}/extern/fmod/lib/libfmodL.so>")

    include_directories(${LIBSAFEC_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/fmod/include)

    include(FetchContent)
    FetchContent_Declare(fastfloat
        GIT_REPOSITORY    https://github.com/fastfloat/fast_float
        GIT_TAG           v3.4.0
        GIT_SHALLOW       ON
    )
    FetchContent_MakeAvailable(fastfloat)
    include_directories(${fastfloat_SOURCE_DIR}/include)

    include(ExternalProject)
    ExternalProject_Add(sentry
        GIT_REPOSITORY    https://github.com/getsentry/sentry-native
        GIT_TAG           origin/master
        GIT_SUBMODULES_RECURSE ON
        GIT_SHALLOW       ON
        BUILD_ALWAYS      OFF
        CONFIGURE_HANDLED_BY_BUILD ON
        CONFIGURE_COMMAND cmake ../sentry -DCMAKE_BUILD_TYPE=Release
        BUILD_COMMAND     cmake --build . --parallel
        INSTALL_COMMAND   ""
    )
    ExternalProject_Get_property(sentry SOURCE_DIR BINARY_DIR)
    message("sentry-native SOURCE_DIR = ${SOURCE_DIR}")
    message("sentry-native BINARY_DIR = ${BINARY_DIR}")
    set(SENTRY_NATIVE_INCLUDE_DIRS "${SOURCE_DIR}/include")
    set(SENTRY_NATIVE_LIBRARIES ${BINARY_DIR}/libsentry.so)
    include_directories("${SENTRY_NATIVE_INCLUDE_DIRS}")
    add_custom_target(dependencies ALL DEPENDS sentry)
endif()

if (NOT WIN32)
    message("Using DXVK-native for D3D9 API")

    include(ExternalProject)
    ExternalProject_Add(dxvk-native
        GIT_REPOSITORY    https://github.com/Joshua-Ashton/dxvk-native
        GIT_TAG           a2dc99c407340432d4ba5bfa29efa685c27942ea
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
    add_dependencies(dependencies dxvk-native)
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

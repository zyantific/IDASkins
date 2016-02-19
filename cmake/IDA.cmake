#
# The MIT License (MIT)
#
# Copyright (c) 2015 athre0z
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0054 NEW)

option(IDA_ARCH_64 "Build for 64 bit IDA" False)
set(IDA_SDK_PATH "" CACHE PATH "Path to IDA SDK")
set(IDA_INSTALL_DIR "" CACHE PATH "Install path of IDA")

# If variable has not been set, then set it with the environment variable
function(set_var_from_env dst_var src_var env_var_name)
    # Check if variable is already set
    if (DEFINED ${dst_var})
        message(STATUS "variable ${dst_var} was already set to \"${${dst_var}}\"")
        RETURN()
    endif()

    # Check if source variable set
    message(STATUS "Checking ${src_var}, ${${src_var}}")
    if (DEFINED ${src_var})
        message(STATUS "Getting ${dst_var} value from source \"${src_var}\"")
        set(${dst_var} ${${src_var}} PARENT_SCOPE)
        RETURN()
    endif()

    # Check that environment variable was set
    if (DEFINED ENV{${env_var_name}})
        set(${dst_var} $ENV{${env_var_name}} PARENT_SCOPE)
    else()
        message(FATAL_ERROR "ENV variable \"${env_var_name}\" must be set")
    endif()
endfunction()

set_var_from_env(ida_sdk IDA_SDK_PATH IDASDK)

set_var_from_env(ida_dir_env IDA_INSTALL_DIR IDADIR)
file(TO_CMAKE_PATH "${ida_dir_env}" ida_dir)

# Compiler specific switches
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(compiler_specific "-m32 -std=c++0x ")
    set(ida_lib_path_compiler "gcc")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(compiler_specific "/WX /wd4996 /MP /D__VC__")
    set(ida_lib_path_compiler "vc")

    # Hack for MSVC to always use /MD, even when generating debug code
    set(manipulated_vars
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO)
    foreach(cur_var ${manipulated_vars})
        string(REGEX REPLACE "/(LD|(M(T|D)))d?( +|$)" "/MD " new_var ${${cur_var}})
        string(REGEX REPLACE "(/|-)D *_DEBUG" "" new_var ${new_var})
        set(${cur_var} ${new_var} CACHE STRING "" FORCE)
    endforeach()
endif ()

# OS specific switches
if (WIN32)
    set(os_specific "-D_WIN32_WINNT=0x0501 -D__NT__")
    set(ida_lib_path_platform "win")

    if (IDA_ARCH_64)
        set(plugin_extension ".p64")
    else()
        set(plugin_extension ".plw")
    endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(os_specific "-D__MAC__ -D_FORTIFY_SOURCE=0")
    set(ida_lib_path_platform "mac")

    if (IDA_ARCH_64)
        set(plugin_extension ".pmc64")
    else()
        set(plugin_extension ".pmc")
    endif()
elseif (UNIX)
    set(os_specific "-D__LINUX__")
    set(ida_lib_path_platform "linux")

    if (IDA_ARCH_64)
        set(plugin_extension ".plx64")
    else()
        set(plugin_extension ".plx")
    endif()
endif ()

# IDA target architecture specific
if (IDA_ARCH_64)
    set(ida_lib_path_arch "64")
    set(arch_specific "-D__EA64__")
else ()
    set(ida_lib_path_arch "32")
endif ()

set(IDA_LIB_DIR "${ida_sdk}/lib/x86_${ida_lib_path_platform}_${ida_lib_path_compiler}_${ida_lib_path_arch}"
    CACHE PATH "IDA SDK library path" FORCE)
message(STATUS "IDA library path: ${IDA_LIB_DIR}")
if (NOT EXISTS ${IDA_LIB_DIR})
    set(IDA_LIB_DIR, NOTFOUND)
endif ()

set(plugin_extension "${plugin_extension}" CACHE INTERNAL "Plugin file extension" FORCE)
set(other_defs "-DQT_NAMESPACE=QT -DNO_OBSOLETE_FUNCS -D__IDP__ -DQPROJECT_LIBRARY")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compiler_specific} ${os_specific} ${arch_specific} ${other_defs}"
    CACHE STRING "Flags used by the compiler during all build types." FORCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${compiler_specific} ${os_specific} ${arch_specific} ${other_defs}"
    CACHE STRING "Flags used by the compiler during all build types." FORCE)

# Library dependencies and include pathes
if (WIN32)
    find_library(IDA_IDA_LIBRARY NAMES "ida" PATHS ${IDA_LIB_DIR} REQUIRED)
    list(APPEND ida_libraries ${IDA_IDA_LIBRARY})
    find_library(IDA_PRO_LIBRARY NAMES "pro" PATHS ${IDA_LIB_DIR})
    if (IDA_PRO_LIBRARY)
        list(APPEND ida_libraries ${IDA_PRO_LIBRARY})
    endif ()
elseif (UNIX OR APPLE)
    # We hardwire the path here as the lib lacks the "lib" prefix, making
    # find_library ignoring it.
    list(APPEND ida_libraries  "${IDA_LIB_DIR}/pro.a")
endif ()
set(ida_libraries ${ida_libraries} CACHE INTERNAL "IDA libraries" FORCE)
include_directories("${ida_sdk}/include")

if (ida_dir)
    set(PLUGIN_INSTALL_PREFIX "${ida_dir}" CACHE STRING "Customizable install prefix")
    set(CMAKE_INSTALL_PREFIX "${PLUGIN_INSTALL_PREFIX}" CACHE STRING 
        "(see PLUGIN_INSTALL_PREFIX)" FORCE)

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        file(TO_NATIVE_PATH ${ida_dir} IDA_NATIVE_DIR)
    endif ()
endif ()

function (add_ida_plugin plugin_name)
    set(sources ${ARGV})
    if (sources)
        list(REMOVE_AT sources 0)
    endif ()

    # Define target
    string(STRIP "${sources}" sources)
    add_library(${plugin_name} SHARED ${sources})
    set_target_properties(${plugin_name} PROPERTIES
        PREFIX ""
        SUFFIX ${plugin_extension}
        OUTPUT_NAME ${plugin_name})

    target_link_libraries(${plugin_name} ${ida_libraries} ida)

    # Define install rule
    install(TARGETS ${plugin_name} DESTINATION plugins)

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        # When generating for Visual Studio, 
        # generate user file for convenient debugging support.
        configure_file(
            "cmake/template.vcxproj.user" 
            "${plugin_name}.vcxproj.user" 
            @ONLY)
    endif ()
endfunction (add_ida_plugin)

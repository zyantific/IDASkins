#
# The MIT License (MIT)
#
# Copyright (c) 2014 athre0z
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

cmake_minimum_required(VERSION 3.1.0)

project(IDASkins)

option(IDA_ARCH_64 "Build plugin for 64 bit IDA" False)
set(ida_sdk $ENV{IDASDK})

if (NOT CONFIGURED_ONCE)
    # Compiler specific switches
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
            "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(compiler_specific "-m32 -std=c++0x -O2 -Werror")
        set(ida_lib_path_compiler "gcc")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(compiler_specific "/WX /wd4996 /MP /D__VC__")
        set(ida_lib_path_compiler "vc")

        # When generating for Visual Studio, 
        # generate user file for convenient debugging support.
        configure_file("template.vcxproj.user" 
            "${CMAKE_PROJECT_NAME}.vcxproj.user" @ONLY)

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
    elseif (UNIX) # Untested!
        set(os_specific "-D__LINUX__=1")
        set(ida_lib_path_platform "mac")

        if (IDA_ARCH_64)
            message(SEND_ERROR "FIXME: What is the file extension here?")
        else()
            set(plugin_extension ".plx")
        endif()
    elseif (APPLE) # Untested!
        set(os_specific "-D__MAC__=1 -D_FORTIFY_SOURCE=0")
        set(ida_lib_path_platform "linux")

        if (IDA_ARCH_64)
            message(SEND_ERROR "FIXME: What is the file extension here?")
        else()
            set(plugin_extension ".pmc")
        endif()
    endif ()

    # IDA target architecture specific
    if (IDA_ARCH_64)
        set(ida_lib_path_arch "64")
        set(arch_specific "-D__EA64__")
    else ()
        set(ida_lib_path_arch "32")
    endif ()

    # Check if we are inside a GIT repo
    set(git_scan_cur_dir ${CMAKE_CURRENT_LIST_DIR})
    set(git_scan_counter 0)
    while (git_scan_counter LESS 32)
        if (EXISTS "${git_scan_cur_dir}/.git")
            # Yep, we are. Obtain SHA1 of current head
            message(STATUS "git root located at ${git_scan_cur_dir}")
            execute_process(COMMAND git rev-parse HEAD 
                RESULT_VARIABLE git_return_val
                OUTPUT_VARIABLE git_head_sha1
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                OUTPUT_STRIP_TRAILING_WHITESPACE)
            
            if (NOT git_return_val EQUAL 0)
                set(git_head_sha1 "unknown")
            endif ()

            message(STATUS "git revision SHA-1: ${git_head_sha1}")
            set(git_defs "-DGIT_HEAD_SHA1=\\\"${git_head_sha1}\\\"")
            break()
        endif ()
        set(git_scan_cur_dir "${git_scan_cur_dir}/..")
        math(EXPR git_scan_counter "${git_scan_counter}+1")
    endwhile ()

    set(IDA_LIB_DIR "${ida_sdk}/lib/x86_${ida_lib_path_platform}_${ida_lib_path_compiler}_${ida_lib_path_arch}"
        CACHE PATH "IDA SDK library path" FORCE)
    if (NOT EXISTS ${IDA_LIB_DIR})
        set(IDA_LIB_DIR, NOTFOUND)
    endif()

    set(plugin_extension "${plugin_extension}" 
        CACHE INTERNAL "Plugin file extension" FORCE)
    
    set(other_defs "-DQT_NAMESPACE=QT -DNO_OBSOLETE_FUNCS -D__IDP__ -DQPROJECT_LIBRARY")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compiler_specific} ${os_specific} ${arch_specific} ${git_defs} ${other_defs}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${compiler_specific} ${os_specific} ${arch_specific} ${git_defs} ${other_defs}"
        CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif ()

# Locate relevant files
file(GLOB project_headers ./*.hpp ./*.h)
file(GLOB project_sources ./*.cpp ./*.cc)
file(GLOB project_forms ./ui/*.ui)
#file(GLOB project_resources ./resources/*.qrc)

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt4 REQUIRED QtCore QtGui)
QT4_WRAP_UI(project_forms_headers ${project_forms})
#QT4_ADD_RESOURCES(project_resources_rcc ${project_resources})

# Hack Qt to use release libraries even when generating debug binaries
# for compatibility with IDA.
get_cmake_property(all_vars VARIABLES)
foreach(cur_var ${all_vars})
    string(REGEX MATCH "^(QT_.*LIBRARY)$" lib_match ${cur_var})
    if (lib_match)
        set(${lib_match} "${lib_match}_RELEASE")
    endif()
endforeach()

# Other dependencies
find_library(ida_libraries NAMES ida PATHS ${IDA_LIB_DIR} REQUIRED)
include_directories("${ida_sdk}/include")

# Build target
add_library(${CMAKE_PROJECT_NAME} SHARED
#    ${project_headers}
    ${project_sources}
    ${project_forms_headers})
#    ${project_resources_rcc})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    SUFFIX "${plugin_extension}"
    OUTPUT_NAME ${CMAKE_PROJECT_NAME})
target_link_libraries(${CMAKE_PROJECT_NAME} Qt4::QtCore Qt4::QtGui)
target_link_libraries(${CMAKE_PROJECT_NAME} ${ida_libraries})

# Define install rules
file(TO_CMAKE_PATH "$ENV{IDADIR}" ida_dir)
if (ida_dir)
    set(CMAKE_INSTALL_PREFIX "${ida_dir}")
    if (WIN32)
        file(TO_NATIVE_PATH "${ida_dir}" IDA_NATIVE_DIR)
    endif ()
endif ()
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION plugins)

set(CONFIGURED_ONCE TRUE CACHE INTERNAL "CMake has configured at least once.")
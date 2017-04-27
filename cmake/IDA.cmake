#
# The MIT License (MIT)
#
# Copyright (c) 2017 Joel Höner <athre0z@zyantific.com>
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

cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0054 NEW)

# =============================================================================================== #
# Overridable options                                                                             #
# =============================================================================================== #

set(IDA_ARCH_64     False CACHE BOOL "Build for 64 bit IDA"                       )
set(IDA_SDK_PATH    ""    CACHE PATH "Path to IDA SDK"                            )
set(IDA_INSTALL_DIR ""    CACHE PATH "Install path of IDA"                        )
set(IDA_VERSION     690   CACHE INT  "IDA Version to build for (e.g. 6.9 is 690).")

# =============================================================================================== #
# General preparation                                                                             #
# =============================================================================================== #

# Compiler specific switches
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(compiler_specific "-m32 -std=c++0x ")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(compiler_specific "/WX /wd4996 /MP /D__VC__")

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

    if (IDA_ARCH_64)
        set(plugin_extension ".p64")
    else()
        set(plugin_extension ".plw")
    endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(os_specific "-D__MAC__ -D_FORTIFY_SOURCE=0")

    if (IDA_ARCH_64)
        set(plugin_extension ".pmc64")
    else()
        set(plugin_extension ".pmc")
    endif()
elseif (UNIX)
    set(os_specific "-D__LINUX__")

    if (IDA_ARCH_64)
        set(plugin_extension ".plx64")
    else()
        set(plugin_extension ".plx")
    endif()
endif ()

if (IDA_ARCH_64)
    set(ida_lib_path_arch "64")
    set(arch_specific "-D__EA64__")
else ()
    set(ida_lib_path_arch "32")
endif ()

set(plugin_extension "${plugin_extension}" CACHE INTERNAL "Plugin file extension" FORCE)
set(other_defs "-DQT_NAMESPACE=QT -DNO_OBSOLETE_FUNCS -D__IDP__ -DQPROJECT_LIBRARY")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compiler_specific} ${os_specific} ${arch_specific} ${other_defs}"
    CACHE STRING "Flags used by the compiler during all build types." FORCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${compiler_specific} ${os_specific} ${arch_specific} ${other_defs}"
    CACHE STRING "Flags used by the compiler during all build types." FORCE)

# Library dependencies and include pathes
if (WIN32)
    # On Windows, we use HR's lib files shipped with the SDK.
    set(IDA_LIB_DIR "${IDA_SDK}/lib/x86_win_vc_${ida_lib_path_arch}"
        CACHE PATH "IDA SDK library path" FORCE)

    message(STATUS "IDA library path: ${IDA_LIB_DIR}")

    if (NOT EXISTS ${IDA_LIB_DIR})
        set(IDA_LIB_DIR NOTFOUND)
    endif ()

    find_library(IDA_IDA_LIBRARY NAMES "ida" PATHS ${IDA_LIB_DIR} REQUIRED)
    list(APPEND ida_libraries ${IDA_IDA_LIBRARY})
    find_library(IDA_PRO_LIBRARY NAMES "pro" PATHS ${IDA_LIB_DIR})
    if (IDA_PRO_LIBRARY)
        list(APPEND ida_libraries ${IDA_PRO_LIBRARY})
    endif ()
elseif (UNIX)
    # On unixoid platforms, we link against IDA directly.
    if (IDA_ARCH_64)
        find_library(IDA_IDA_LIBRARY NAMES "ida64" PATHS ${IDA_INSTALL_DIR} REQUIRED)
    else ()
        find_library(IDA_IDA_LIBRARY NAMES "ida" PATHS ${IDA_INSTALL_DIR} REQUIRED)
    endif ()
    list(APPEND ida_libraries ${IDA_IDA_LIBRARY})
endif ()

set(ida_libraries ${ida_libraries} CACHE INTERNAL "IDA libraries" FORCE)
include_directories("${IDA_SDK}/include")

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND NOT IDA_INSTALL_DIR STREQUAL "")
    # Normalize path.
    file(TO_CMAKE_PATH ${IDA_INSTALL_DIR} ida_dir)
    file(TO_NATIVE_PATH ${ida_dir} IDA_NATIVE_DIR)
endif ()

# =============================================================================================== #
# Qt support                                                                                      #
# =============================================================================================== #

if (IDA_ENABLE_QT_SUPPORT)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    # IDA 6.9 and above use Qt5, older versions Qt4.
    if (IDA_VERSION LESS 690)
        set(ida_qt_major 4)
    else ()
        set(ida_qt_major 5)
    endif ()

    # On macOS, we can look up the path of each Qt library inside the IDA installation.
    # It might be possible to get this to work also on linux by parameterizing the file suffix.
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        foreach(qtlib Gui;Core;Widgets)
            file(GLOB_RECURSE qtlibpaths "${IDA_INSTALL_DIR}/../Frameworks/Qt${qtlib}")
            # Typically we will find exactly 2 paths to the libfile on macOS because of 
            # how the framework versioning works. Either one is fine, so pick the first.
            foreach(p ${qtlibpaths})
                set(IDA_Qt${qtlib}_LIBRARY ${p} CACHE FILEPATH "Path to IDA's Qt${qtlib}")
                break()
            endforeach()
        endforeach()
    endif()

    # Locate Qt.
    if (ida_qt_major EQUAL 4)
        find_package(Qt4 REQUIRED QtCore QtGui)
    else ()
        find_package(Qt5Widgets REQUIRED)
    endif ()

    # Hack Qt to use release libraries even when generating debug binaries
    # for compatibility with IDA.
    get_cmake_property(all_vars VARIABLES)
    foreach(cur_var ${all_vars})
        string(REGEX MATCH "^(QT_.*LIBRARY)$" lib_match ${cur_var})
        if (lib_match)
            set(${lib_match} "${lib_match}_RELEASE")
        endif()
    endforeach()
endif ()

# =============================================================================================== #
# Functions for adding IDA plugin targets                                                         #
# =============================================================================================== #

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

    target_link_libraries(${plugin_name} ${ida_libraries})

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


# =============================================================================================== #
# Functions for adding IDA plugin targets with Qt support                                         #
# =============================================================================================== #


function (add_ida_qt_plugin plugin_name)
    set(sources ${ARGV})
    if (sources)
        list(REMOVE_AT sources 0)
    endif ()

    # Divide between UI files and regular C/C++ sources. 
    foreach (cur_file ${sources})
        if (${cur_file} MATCHES ".*\\.ui")
            list(APPEND ui_sources "${cur_file}")
        else ()
            list(APPEND non_ui_sources ${cur_file})
        endif ()
    endforeach ()

    # Compile UI files.
    if (ida_qt_major EQUAL 4)
        QT4_WRAP_UI(form_headers ${ui_sources})
    else ()
        QT5_WRAP_UI(form_headers ${ui_sources})
    endif ()

    # Add plugin.
    add_ida_plugin(${plugin_name} ${non_ui_sources} ${form_headers})

    # Link against Qt.
    if (ida_qt_major EQUAL 4)
        foreach (qtlib Core;Gui)
            target_link_libraries(${CMAKE_PROJECT_NAME} "Qt4::Qt${qtlib}")
            # On macs, we need to link to the frameworks in the IDA application folder
            if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
                set_target_properties(
                    "Qt4::Qt${qtlib}" 
                    PROPERTIES 
                    IMPORTED_LOCATION_RELEASE "${IDA_Qt${qtlib}_LIBRARY}")
            endif ()
        endforeach()
    else ()
        foreach (qtlib Core;Widgets;Gui)
            target_link_libraries(${CMAKE_PROJECT_NAME} "Qt5::${qtlib}")
            if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
                set_target_properties(
                    "Qt5::${qtlib}"
                    PROPERTIES 
                    IMPORTED_LOCATION_RELEASE "${IDA_Qt${qtlib}_LIBRARY}")
            endif ()
        endforeach()
    endif ()
endfunction ()

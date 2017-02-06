#
# The MIT License (MIT)
#
# Copyright (c) 2016 athre0z
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

project(IDASkins)
include("cmake/IDA.cmake")
set(IDA_VERSION 690 CACHE INT "IDA Version to build for (e.g. 6.9 is 690).")

set(headers
    "src/Config.hpp"
    "src/Core.hpp"
    "src/IdaFontConfig.hpp"
    "src/Settings.hpp"
    "src/ThemeManifest.hpp"
    "src/ThemeSelector.hpp"
    "src/Utils.hpp"
    "src/ObjectInspector.hpp")
set(sources
    "src/Core.cpp"
    "src/IdaFontConfig.cpp"
    "src/idaskins.cpp"
    "src/Settings.cpp"
    "src/ThemeManifest.cpp"
    "src/ThemeSelector.cpp"
    "src/Utils.cpp"
    "src/ObjectInspector.cpp")
set(forms
    "ui/ThemeSelector.ui"
    "ui/ObjectInspector.ui")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# IDA 6.9 and above use Qt5, older versions Qt4.
if (IDA_VERSION LESS 690)
    find_package(Qt4 REQUIRED QtCore QtGui)
    QT4_WRAP_UI(form_headers ${forms})
else ()
    find_package(Qt5Widgets REQUIRED)
    QT5_WRAP_UI(form_headers ${forms})
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

# Add IDA plugin target
include_directories("src")
add_ida_plugin(${CMAKE_PROJECT_NAME} ${headers} ${sources} ${form_headers})

if (IDA_VERSION LESS 690)
    target_link_libraries(${CMAKE_PROJECT_NAME} Qt4::QtCore Qt4::QtGui)
else ()
    target_link_libraries(${CMAKE_PROJECT_NAME} Qt5::Widgets)
    # On macs, we need to link to the frameworks in the IDA application folder
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        foreach (qtlib Widgets;Gui;Core)
            set_target_properties(Qt5::${qtlib} PROPERTIES IMPORTED_LOCATION_RELEASE "${IDA_Qt${qtlib}_LIBRARY}")
        endforeach()
    endif ()
endif ()

install(DIRECTORY skin/ DESTINATION skin)

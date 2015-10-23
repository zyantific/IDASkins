/**
 * This file is part of IDASkins IDA PRO plugin (zyantific.com).
 * 
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Joel Höner (athre0z)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef IDAFONTCONFIG_HPP
#define IDAFONTCONFIG_HPP

#include <QSettings>
#include <pro.h>

// ============================================================================================= //
// [IdaFontConfig]                                                                               //
// ============================================================================================= //

class IdaFontConfig : private QSettings
{
public:
    enum FontType
    {
        FONT_DISASSEMBLY,
        FONT_HEXVIEW,
        FONT_DEBUG_REGISTERS,
        FONT_TEXT_INPUT,
        FONT_OUTPUT_WINDOW
    };
public:
    /**
     * @brief   Default constructor.
     */
    explicit IdaFontConfig(FontType type);
    /**
     * @brief   Destructor.
     */
    virtual ~IdaFontConfig();
public:
    QString family();
    quint32 size  ();
    bool    bold  ();
    bool    italic();
    //QString style ();
protected:
    template<typename T>
    T getSetting(const QString &key);
    static const char *typeToSettingsKey(FontType type);
protected:
    FontType m_type;
};

// ============================================================================================= //
// Implementation of inline functions [IdaFontConfig]                                            //
// ============================================================================================= //

template <typename T> inline
T IdaFontConfig::getSetting(const QString &key)
{
    beginGroup("Font");
#   if IDA_SDK_VERSION >= 640
        beginGroup(typeToSettingsKey(m_type));
#   endif

    T tmp = value(key).value<T>();

#   if IDA_SDK_VERSION >= 640
        endGroup();
#   endif
    endGroup();
    return tmp;
}

// ============================================================================================= //

#endif

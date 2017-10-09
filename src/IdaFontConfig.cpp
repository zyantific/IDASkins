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

#include "IdaFontConfig.hpp"

#include <registry.hpp>
#include <QFontInfo>
#include <qfontdatabase.h>

// ============================================================================================= //
// [IdaFontConfig]                                                                               //
// ============================================================================================= //

IdaFontConfig::IdaFontConfig(FontType type)
    : m_subkey(typeToSettingsKey(type))
{

}

QString IdaFontConfig::family() const
{
    qstring result;
    if (reg_read_string(&result, "Name", m_subkey))
        return QString(result.c_str());

    // On Windows, HR doesn't use the system default font.
#ifdef _WIN32
    return "Consolas";
#else
    return QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
#endif
}

int IdaFontConfig::size() const
{
    return reg_read_int("Size", 10, m_subkey);
}

bool IdaFontConfig::bold() const
{
    return reg_read_bool("Bold", false, m_subkey);
}

bool IdaFontConfig::italic() const
{
    return reg_read_bool("Italic", false, m_subkey);
}

char const* IdaFontConfig::typeToSettingsKey(FontType type)
{
    switch (type)
    {
        case FONT_DISASSEMBLY: 
            return "Font\\Disassembly";
        case FONT_HEXVIEW:
            return "Font\\Hex view";
        case FONT_DEBUG_REGISTERS:
            return "Font\\Debug registers";
        case FONT_TEXT_INPUT:
            return "Font\\Text input";
        case FONT_OUTPUT_WINDOW:
            return "Font\\Output window";
    }

    Q_ASSERT(false);
    return nullptr;
}

// ============================================================================================= //

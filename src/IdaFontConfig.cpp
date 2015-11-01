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

#include <QFont>
#include <QFontInfo>

// ============================================================================================= //
// [IdaFontConfig]                                                                               //
// ============================================================================================= //

IdaFontConfig::IdaFontConfig(FontType type)
    : QSettings("Hex-Rays", "IDA")
    , m_type(type)
{

}

QString IdaFontConfig::family()
{
    auto x = getSetting("Name");
    if (x.isValid())
        return x.value<QString>();

#ifdef _WIN32
    // On Windows, HR doesn't use the system's default monospace-font.
    return "Fixedsys";
#else
    // It seems like there is no clean way of obtaining the name of the platform's default 
    // monospace font in Qt4. This issue was addressed in Qt 5.2.
    // TODO: for IDA >= 6.9, use Qt5 approach utilizing `QFontDatabase`.
    QFont font("238ru<MGduz(asuidfz8AS)F0zEmguz234t");
    font.setStyleHint(QFont::TypeWriter);
    QFontInfo finfo(font);
    return finfo.family();
#endif
}

quint32 IdaFontConfig::size()
{
    auto x = getSetting("Size");
    return x.isValid() ? x.value<quint32>() : 10;
}

bool IdaFontConfig::bold()
{
    auto x = getSetting("Bold");
    return x.isValid() ? x.value<bool>() : false;
}

bool IdaFontConfig::italic()
{
    auto x = getSetting("Italic");
    return x.isValid() ? x.value<bool>() : false;
}

//QString IdaFontConfig::style()
//{
//
//}

char const* IdaFontConfig::typeToSettingsKey(FontType type)
{
    switch (type)
    {
        case FONT_DISASSEMBLY: 
            return "Disassembly";
        case FONT_HEXVIEW:
            return "Hex view";
        case FONT_DEBUG_REGISTERS:
            return "Debug registers";
        case FONT_TEXT_INPUT:
            return "Text input";
        case FONT_OUTPUT_WINDOW:
            return "Output window";
    }

    Q_ASSERT(false);
    return nullptr;
}

IdaFontConfig::~IdaFontConfig()
{

}

// ============================================================================================= //

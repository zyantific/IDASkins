/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 athre0z
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "IdaFontConfig.hpp"

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
    return getSetting<QString>("Name");
}

quint32 IdaFontConfig::size()
{
    return getSetting<quint32>("Size");
}

bool IdaFontConfig::bold()
{
    return getSetting<bool>("Bold");
}

bool IdaFontConfig::italic()
{
    return getSetting<bool>("Italic");
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

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

#include <QtGui>
#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <diskio.hpp>

// ============================================================================

/**
 * @brief   Applies the Qt stylesheet.
 * @return  true if it succeeds, false if it fails.
 */
bool apply_stylesheet(void)
{
	QString idaDir = QDir(idadir(nullptr)).absolutePath();
	QFile stylesheet(idaDir + "/skin/stylesheet.css");
	if (!stylesheet.open(QFile::ReadOnly))
	{
		msg("[IDASkins] Unable to load stylesheet file.\n");
		return false;
	}

	QString data = stylesheet.readAll();
	data.replace("<IDADIR>", idaDir);
	data.replace("<SKINDIR>", idaDir + "/skin");
	qApp->setStyleSheet(data);
	msg("[IDASkins] Skin file successfully applied! "
		"(Reload using CTRL+SHIFT+S)\n");
	return true;
}

//-----------------------------------------------------------------------------

int idaapi init(void)
{
	if (!is_idaq()) return PLUGIN_SKIP;
	msg("IDASkins 1.0.0 by athre0z/Ende! loaded!\n");
	apply_stylesheet();
	return PLUGIN_KEEP;
}

//-----------------------------------------------------------------------------

void idaapi term(void)
{
	
}

//-----------------------------------------------------------------------------

void idaapi run(int /*arg*/)
{
	apply_stylesheet();
}

//-----------------------------------------------------------------------------

plugin_t PLUGIN =
{
	IDP_INTERFACE_VERSION,
	0,
	init,
	term,
	run,
	"Advanced IDA skinning",
	"Plugin providing advanced skinning facilities using Qt stylesheets.",
	"IDASkins: Reload stylesheet",
	"Ctrl-Shift-S"
};

// ============================================================================
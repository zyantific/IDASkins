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

#ifndef OBJECTINSPECTOR_HPP
#define OBJECTINSPECTOR_HPP

#include "ui_ObjectInspector.h"

#include <QShortcut>

// ============================================================================================= //
// [ObjectInspector]                                                                             //
// ============================================================================================= //

class ObjectInspector : public QWidget
{
    Q_OBJECT
    Ui::ObjectInspector m_widgets;
    QWidget* m_curSelectedWidget;
    QShortcut m_updateKey;
public:
    explicit ObjectInspector(QWidget *parent = nullptr);
    ~ObjectInspector() {}
protected slots:
    void updateInspection();
    void selectParent();
    void onSelectedWidgetDestroyed(QObject*);
protected:
    void updateSelectedWidget(QWidget *newWidget);
};

// ============================================================================================= //

#endif

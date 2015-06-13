/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 athre0z
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

#include "ObjectInspector.hpp"

#include <ida.hpp>
#include <kernwin.hpp>
#include <QDebug>
#include <QMouseEvent>

// ========================================================================= //
// [ObjectInspector]                                                         //
// ========================================================================= //

ObjectInspector::ObjectInspector(QWidget *parent)
    : QWidget(parent)
    , m_curSelectedWidget(nullptr)
    , m_updateKey(QKeySequence(Qt::Key_F7), this)
{
    m_updateKey.setContext(Qt::ApplicationShortcut);

    m_widgets.setupUi(this);
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    m_widgets.teInspectionResults->setFont(font);

    connect(m_widgets.btnSelectParent, SIGNAL(released()), SLOT(selectParent()));
    connect(&m_updateKey, SIGNAL(activated()), SLOT(updateInspection()));
}

void ObjectInspector::updateInspection()
{
    auto widget = qApp->widgetAt(QCursor::pos());
    updateSelectedWidget(widget);
}

void ObjectInspector::selectParent()
{
    if (m_curSelectedWidget)
    {
        auto parent = m_curSelectedWidget->parent();
        if (parent && parent->inherits("QWidget"))
        {
            updateSelectedWidget(static_cast<QWidget*>(parent));
        }
    }
}

void ObjectInspector::onSelectedWidgetDestroyed(QObject*)
{
    m_curSelectedWidget = nullptr;
}

void ObjectInspector::updateSelectedWidget(QWidget *newWidget)
{
    if (m_curSelectedWidget)
    {
        disconnect(m_curSelectedWidget, SIGNAL(destroyed(QObject*)), 
            this, SLOT(onSelectedWidgetDestroyed(QObject*)));
    }

    m_curSelectedWidget = newWidget;

    if (newWidget)
    {
        m_widgets.btnSelectParent->setEnabled(newWidget->parent() != nullptr);
        m_widgets.teInspectionResults->setText(
            QString("Type: ") + newWidget->metaObject()->className()
            + "\nName: " + (newWidget->objectName().isEmpty() ? "<none>" : newWidget->objectName())
            + "\nNumber of children: " + QString::number(newWidget->children().length())
        );

        connect(newWidget, SIGNAL(destroyed(QObject*)), 
            SLOT(onSelectedWidgetDestroyed(QObject*)));
    }
    else
    {
        m_widgets.teInspectionResults->setText("<no object under cursor>");
    }
}

// ========================================================================= //

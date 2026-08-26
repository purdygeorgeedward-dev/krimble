/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "SvgTextQuickActionBar.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QAction>
#include <klocalizedstring.h>

SvgTextQuickActionBar::SvgTextQuickActionBar(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setAutoFillBackground(true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(2);

    QAction *cutAction = new QAction(i18n("Cut"), this);
    QAction *copyAction = new QAction(i18n("Copy"), this);
    m_pasteAction = new QAction(i18n("Paste"), this);
    QAction *selectAllAction = new QAction(i18n("Select All"), this);

    connect(cutAction, &QAction::triggered, this, &SvgTextQuickActionBar::sigCut);
    connect(copyAction, &QAction::triggered, this, &SvgTextQuickActionBar::sigCopy);
    connect(m_pasteAction, &QAction::triggered, this, &SvgTextQuickActionBar::sigPaste);
    connect(selectAllAction, &QAction::triggered, this, &SvgTextQuickActionBar::sigSelectAll);

    const QList<QAction *> actions = {cutAction, copyAction, m_pasteAction, selectAllAction};
    Q_FOREACH (QAction *action, actions) {
        QToolButton *button = new QToolButton(this);
        button->setDefaultAction(action);
        button->setToolButtonStyle(Qt::ToolButtonTextOnly);
        layout->addWidget(button);
    }

    setLayout(layout);
    adjustSize();
    hide();
}

void SvgTextQuickActionBar::setPasteEnabled(bool enabled)
{
    m_pasteAction->setEnabled(enabled);
}

/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef SVGTEXTQUICKACTIONBAR_H
#define SVGTEXTQUICKACTIONBAR_H

#include <QWidget>

/**
 * Krimble: a small floating toolbar (Cut/Copy/Paste/Select All) shown near
 * an active text selection, the same pattern every mobile OS text field
 * uses instead of requiring a menu-bar trip for basic clipboard actions.
 * Purely a dumb button strip -- SvgTextTool owns showing/hiding/positioning
 * it and connects its signals to the actual cursor operations.
 */
class SvgTextQuickActionBar : public QWidget
{
    Q_OBJECT
public:
    explicit SvgTextQuickActionBar(QWidget *parent = nullptr);
    ~SvgTextQuickActionBar() override = default;

    /// Enable/disable the Paste button based on clipboard content availability.
    void setPasteEnabled(bool enabled);

Q_SIGNALS:
    void sigCut();
    void sigCopy();
    void sigPaste();
    void sigSelectAll();

private:
    QAction *m_pasteAction;
};

#endif // SVGTEXTQUICKACTIONBAR_H

/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef SVGADJUSTSELECTIONHANDLESTRATEGY_H
#define SVGADJUSTSELECTIONHANDLESTRATEGY_H

#include <KoInteractionStrategy.h>
#include <QPointF>

class SvgTextCursor;

/**
 * Krimble: dragging one of the two mobile-style selection-endpoint handles
 * (see SvgTextCursor::selectionHandlePos/hitTestSelectionHandle) to adjust
 * an existing selection after the fact, moving only the grabbed end and
 * leaving the other end fixed -- unlike SvgSelectTextStrategy, which always
 * starts a brand new selection from a fresh tap.
 */
class SvgAdjustSelectionHandleStrategy : public KoInteractionStrategy
{
public:
    SvgAdjustSelectionHandleStrategy(KoToolBase *tool, SvgTextCursor *cursor, bool adjustingStartHandle);
    ~SvgAdjustSelectionHandleStrategy() override = default;

    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    KUndo2Command *createCommand() override;
    void cancelInteraction() override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;

private:
    SvgTextCursor *m_cursor;
    bool m_adjustingStartHandle;
};

#endif // SVGADJUSTSELECTIONHANDLESTRATEGY_H

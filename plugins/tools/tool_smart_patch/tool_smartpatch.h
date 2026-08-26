/*
 *  SPDX-FileCopyrightText: 2017 Eugene Ingerman
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef TOOL_SMARTPATCH_H_
#define TOOL_SMARTPATCH_H_

#include <QObject>
#include <QVariant>
#include <KisActionPlugin.h>

class ToolSmartPatch : public KisActionPlugin
{
    Q_OBJECT
public:
    ToolSmartPatch(QObject *parent, const QVariantList &);
    ~ToolSmartPatch() override;

private Q_SLOTS:
    /**
     * Krimble: Content-Aware Fill. Registered here rather than as a
     * KisFilter subclass because KisFilter::processImpl() never receives
     * the real KisSelectionSP -- only the final composite-back step does,
     * via the non-virtual, non-overridable KisFilter::process() wrapper --
     * so a KisFilter-based version can't see the actual (possibly
     * irregular) selection shape to inform the inpaint algorithm's own
     * fill/source decisions, only get the correct final clipped output.
     * This action gets the real selection directly from the view manager
     * instead.
     */
    void slotContentAwareFill();
};

#endif // TOOL_SMARTPATCH_H_

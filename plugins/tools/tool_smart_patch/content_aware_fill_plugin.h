/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CONTENT_AWARE_FILL_PLUGIN_H_
#define CONTENT_AWARE_FILL_PLUGIN_H_

#include <QObject>
#include <QVariant>
#include <KisActionPlugin.h>

// Krimble: registered separately from ToolSmartPatch (see
// tool_smartpatch.cpp) because that plugin is loaded via the "Krita/Tool"
// mechanism (KoToolRegistry::init(), no per-view parent, called once at
// startup), while KisActionPlugin's own constructor requires a real
// per-view KisViewManager as its parent. Those two loading paths can't be
// combined in one class. This one is instead registered as
// "Krita/ViewPlugin" (see contentawarefill.json), loaded the same way
// KisMainWindow.cpp loads every other per-view plugin, with a genuine
// KisViewManager as parent.
class ContentAwareFillPlugin : public KisActionPlugin
{
    Q_OBJECT
public:
    ContentAwareFillPlugin(QObject *parent, const QVariantList &);
    ~ContentAwareFillPlugin() override;

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

#endif // CONTENT_AWARE_FILL_PLUGIN_H_

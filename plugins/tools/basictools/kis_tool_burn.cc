/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_burn.h"
#include "KrimbleEmbeddedPreset.h"

#include <KoCanvasBase.h>
#include <KoCompositeOpRegistry.h>

#include <KisResourceModel.h>
#include <KisResourceTypes.h>
#include <brushengine/kis_paintop_preset.h>
#include <brushengine/kis_paintop_settings.h>

#include <kis_canvas2.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>

KisToolBurn::KisToolBurn(KoCanvasBase *canvas)
    : KisToolBrush(canvas)
{
    setObjectName("tool_burn");
}

KisToolBurn::~KisToolBurn()
{
}

void KisToolBurn::activate(const QSet<KoShape*> &shapes)
{
    KisToolBrush::activate(shapes);

    // Krimble: clone the standard default brush preset and override its
    // CompositeOp to "burn" (COMPOSITE_BURN) -- see KisToolDodge for the
    // full rationale on why a clone is required.
    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    // KRIMBLE 2026-10-02: the lookup below never found anything (the embedded presets are
    // not in the resource database), so the tool painted with the current brush. Load the
    // built-in default brush ("defaultPreset") directly instead; it is a fresh copy on every
    // call, so the shared default brush is not changed. Old code kept for reference:
    // KisResourceModel model(ResourceType::PaintOpPresets);
    // const QVector<KoResourceSP> matches = model.resourcesForName("defaultPreset");
    // if (matches.isEmpty()) {
    //     return;
    // }
    // KisPaintOpPresetSP basePreset = matches.first().dynamicCast<KisPaintOpPreset>();
    KisPaintOpPresetSP basePreset = krimbleLoadEmbeddedPreset(QStringLiteral("paintbrush"));
    if (!basePreset) {
        return;
    }

    KisPaintOpPresetSP burnPreset = basePreset->clone().dynamicCast<KisPaintOpPreset>();
    if (!burnPreset || !burnPreset->settings()) {
        return;
    }

    burnPreset->settings()->setProperty("CompositeOp", COMPOSITE_BURN);
    // Krimble: see KisToolDodge.cc for why BUILDUP mode (not the base
    // preset's default WASH mode) is required for continuous darkening
    // while dragging.
    burnPreset->settings()->setProperty("PaintOpAction", 1);
    canvas2->viewManager()->canvasResourceProvider()->setPaintOpPreset(burnPreset);
}

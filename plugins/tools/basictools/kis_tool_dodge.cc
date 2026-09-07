/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_dodge.h"

#include <KoCanvasBase.h>
#include <KoCompositeOpRegistry.h>

#include <KisResourceModel.h>
#include <KisResourceTypes.h>
#include <brushengine/kis_paintop_preset.h>
#include <brushengine/kis_paintop_settings.h>

#include <kis_canvas2.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>

KisToolDodge::KisToolDodge(KoCanvasBase *canvas)
    : KisToolBrush(canvas)
{
    setObjectName("tool_dodge");
}

KisToolDodge::~KisToolDodge()
{
}

void KisToolDodge::activate(const QSet<KoShape*> &shapes)
{
    KisToolBrush::activate(shapes);

    // Krimble: clone the standard default brush preset and override its
    // CompositeOp to "dodge" (COMPOSITE_DODGE) -- see header comment for
    // why a clone is required rather than modifying the shared preset.
    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    KisResourceModel model(ResourceType::PaintOpPresets);
    const QVector<KoResourceSP> matches = model.resourcesForName("defaultPreset");
    if (matches.isEmpty()) {
        return;
    }

    KisPaintOpPresetSP basePreset = matches.first().dynamicCast<KisPaintOpPreset>();
    if (!basePreset) {
        return;
    }

    KisPaintOpPresetSP dodgePreset = basePreset->clone().dynamicCast<KisPaintOpPreset>();
    if (!dodgePreset || !dodgePreset->settings()) {
        return;
    }

    dodgePreset->settings()->setProperty("CompositeOp", COMPOSITE_DODGE);
    // Krimble: base preset defaults to WASH mode (PaintOpAction=2), which
    // applies opacity once per continuous stroke, not while dragging back
    // and forth. Real Dodge accumulates continuously as you paint over the
    // same area within one stroke -- BUILDUP mode (PaintOpAction=1) is
    // required for that.
    dodgePreset->settings()->setProperty("PaintOpAction", 1);
    canvas2->viewManager()->canvasResourceProvider()->setPaintOpPreset(dodgePreset);
}

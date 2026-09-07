/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_soften.h"

#include <KoCanvasBase.h>

#include <KisResourceModel.h>
#include <KisResourceTypes.h>
#include <brushengine/kis_paintop_preset.h>

#include <kis_canvas2.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>

KisToolSoften::KisToolSoften(KoCanvasBase *canvas)
    : KisToolBrush(canvas)
{
    setObjectName("tool_soften");
}

KisToolSoften::~KisToolSoften()
{
}

void KisToolSoften::activate(const QSet<KoShape*> &shapes)
{
    KisToolBrush::activate(shapes);

    // Krimble: auto-load the bundled "DFP" preset (Krita's existing default
    // filter-brush preset, filterop engine, already configured with
    // Filter/id = "gaussian blur") so this tool softens immediately on
    // selection -- no manual preset/filter picking required. Falls back to
    // whatever preset was already active if not found, same as KisToolSmudge.
    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    KisResourceModel model(ResourceType::PaintOpPresets);
    const QVector<KoResourceSP> matches = model.resourcesForName("DFP");
    if (matches.isEmpty()) {
        return;
    }

    KisPaintOpPresetSP softenPreset = matches.first().dynamicCast<KisPaintOpPreset>();
    if (softenPreset) {
        canvas2->viewManager()->canvasResourceProvider()->setPaintOpPreset(softenPreset);
    }
}

/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tool_smudge.h"

#include <KoCanvasBase.h>

#include <KisResourceModel.h>
#include <KisResourceTypes.h>
#include <brushengine/kis_paintop_preset.h>

#include <kis_canvas2.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>

KisToolSmudge::KisToolSmudge(KoCanvasBase *canvas)
    : KisToolBrush(canvas)
{
    setObjectName("tool_smudge");
}

KisToolSmudge::~KisToolSmudge()
{
}

void KisToolSmudge::activate(const QSet<KoShape*> &shapes)
{
    KisToolBrush::activate(shapes);

    // Krimble: auto-load the bundled "smudge" preset (Krita's existing
    // colorsmudge paintop engine) so this tool smudges immediately on
    // selection, matching Photoshop's dedicated Smudge tool -- no manual
    // preset picking required. If the preset isn't found for any reason
    // (renamed/removed resource), the tool still falls back to whatever
    // preset was already active, same as any other brush-family tool.
    KisCanvas2 *canvas2 = dynamic_cast<KisCanvas2*>(canvas());
    if (!canvas2 || !canvas2->viewManager()) {
        return;
    }

    KisResourceModel model(ResourceType::PaintOpPresets);
    const QVector<KoResourceSP> matches = model.resourcesForName("smudge");
    if (matches.isEmpty()) {
        return;
    }

    KisPaintOpPresetSP smudgePreset = matches.first().dynamicCast<KisPaintOpPreset>();
    if (smudgePreset) {
        canvas2->viewManager()->canvasResourceProvider()->setPaintOpPreset(smudgePreset);
    }
}

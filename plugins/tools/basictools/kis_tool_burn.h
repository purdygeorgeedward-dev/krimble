/*
 *  SPDX-FileCopyrightText: 2026 Krimble
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_TOOL_BURN_H_
#define KIS_TOOL_BURN_H_

#include "kis_tool_brush.h"

#include <KoIcon.h>
#include <kis_icon.h>

class KoCanvasBase;

// Krimble: dedicated toolbox entry for Photoshop's Burn tool. Same
// mechanism as KisToolDodge -- clones the standard default brush preset
// and overrides CompositeOp to COMPOSITE_BURN, an already-supported
// composite op with no prior dedicated toolbox tool. See KisToolDodge's
// header comment for the full rationale.
class KisToolBurn : public KisToolBrush
{
    Q_OBJECT

public:
    explicit KisToolBurn(KoCanvasBase *canvas);
    ~KisToolBurn() override;

    void activate(const QSet<KoShape*> &shapes) override;
};

class KisToolBurnFactory : public KisToolPaintFactoryBase
{
public:
    KisToolBurnFactory()
        : KisToolPaintFactoryBase("KritaShape/KisToolBurn")
    {
        setToolTip(i18n("Burn Tool"));
        setSection(ToolBoxSection::PSOrder);
        setIconName(koIconNameCStr("krita_tool_burn"));
        setShortcut(QKeySequence(Qt::Key_K)); // Krimble: Dodge took O (matches PS); Burn shares PS's flyout with Dodge rather than having its own key, so this is an arbitrary free key since Krimble gives it a separate toolbox slot instead of a flyout.
        setPriority(24); // Krimble: directly after Dodge (23).
        setToolBoxPriority(124);
        setActivationShapeId(KRITA_TOOL_ACTIVATION_ID);
    }

    ~KisToolBurnFactory() override {}

    KoToolBase *createTool(KoCanvasBase *canvas) override
    {
        return new KisToolBurn(canvas);
    }
};

#endif // KIS_TOOL_BURN_H_

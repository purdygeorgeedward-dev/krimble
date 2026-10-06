/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KISBRUSHCOLORWELLS_H
#define KISBRUSHCOLORWELLS_H

#include <QObject>

class KisViewManager;

/**
 * KRIMBLE 2026-10-05: the foreground / background color wells (the same two-squares widget as at the bottom of the
 * toolbox) as a toolbar item named "brush_color_wells", so the "BrushOptions" toolbar in krita5.xmlgui can show them
 * (George: "I think it might be a good idea to have color wells on the brush toolbar").
 */
class KisBrushColorWells : public QObject
{
    Q_OBJECT
public:
    explicit KisBrushColorWells(KisViewManager *viewManager);
};

#endif // KISBRUSHCOLORWELLS_H

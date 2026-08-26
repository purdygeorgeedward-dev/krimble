/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISWIDEDOCKSEPARATORSTYLE_H
#define KISWIDEDOCKSEPARATORSTYLE_H

#include "kritawidgetutils_export.h"
#include <QProxyStyle>

/**
 * Krimble: a proxy style that doubles the pixel metric governing the width
 * of dock widget (panel) separators. On a touchscreen, the default width
 * most desktop styles use (a handful of pixels, sized for a mouse cursor) is
 * too small to reliably grab with a finger to resize a docked panel. Qt ties
 * the visible separator width and its resize hit-test/grab zone to the same
 * single pixel metric, so there's no way to widen only the grab zone without
 * also widening what's drawn on screen -- this doubles both together.
 */
class KRITAWIDGETUTILS_EXPORT KisWideDockSeparatorStyle : public QProxyStyle
{
public:
    KisWideDockSeparatorStyle(QStyle *baseStyle);
    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
};

#endif // KISWIDEDOCKSEPARATORSTYLE_H

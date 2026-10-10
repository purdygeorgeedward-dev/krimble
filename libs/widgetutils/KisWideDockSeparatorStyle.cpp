/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "KisWideDockSeparatorStyle.h"

#include <QStyleFactory>

KisWideDockSeparatorStyle::KisWideDockSeparatorStyle(QStyle *baseStyle)
    : QProxyStyle(QStyleFactory::create(baseStyle->objectName()))
{
}

int KisWideDockSeparatorStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    if (metric == QStyle::PM_DockWidgetSeparatorExtent) {
        // Krimble 2026-10-10: on Android at least 20 px (the same touch size as the dialog edges), so a finger can grab it.
        // return QProxyStyle::pixelMetric(metric, option, widget) * 2;
#ifdef Q_OS_ANDROID
        return qMax(QProxyStyle::pixelMetric(metric, option, widget) * 2, 20);
#else
        return QProxyStyle::pixelMetric(metric, option, widget) * 2;
#endif
    }

    return QProxyStyle::pixelMetric(metric, option, widget);
}

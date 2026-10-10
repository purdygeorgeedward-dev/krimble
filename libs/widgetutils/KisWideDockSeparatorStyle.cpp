/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "KisWideDockSeparatorStyle.h"

#include <QStyleFactory>
#include <QPainter>
#include <QStyleOption>

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

void KisWideDockSeparatorStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
#ifdef Q_OS_ANDROID
    if (element == QStyle::PE_IndicatorDockWidgetResizeHandle) {
        // Krimble 2026-10-10: the wide strip is only a touch target. It is drawn as a black line with a highlight line right
        // beside it (below it, or to its right), centered in the strip, so it stands out without a thick border.
        // (The first version drew three faint dots, which were not visible.)
        painter->save();
        painter->setPen(Qt::NoPen);
        const QRect r = option->rect;
        const bool tall = r.height() >= r.width();
        const int lineWidth = 2;
        const QColor blackLine(0, 0, 0);
        const QColor highlightLine(255, 255, 255, 110);
        if (tall) {
            const int x = r.center().x() - lineWidth;
            painter->fillRect(QRect(x, r.top(), lineWidth, r.height()), blackLine);
            painter->fillRect(QRect(x + lineWidth, r.top(), lineWidth, r.height()), highlightLine);
        } else {
            const int y = r.center().y() - lineWidth;
            painter->fillRect(QRect(r.left(), y, r.width(), lineWidth), blackLine);
            painter->fillRect(QRect(r.left(), y + lineWidth, r.width(), lineWidth), highlightLine);
        }
        painter->restore();
        return;
    }
#endif
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

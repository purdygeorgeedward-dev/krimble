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
        // Krimble 2026-10-10: the wide strip is only a touch target. Draw three small dots in its middle (the grip), and no
        // line or band, so it does not look like a thick border around the toolbox and panels.
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        QColor dotColor = option->palette.color(QPalette::WindowText);
        dotColor.setAlpha(110);
        painter->setBrush(dotColor);
        const QRect r = option->rect;
        const bool tall = r.height() >= r.width();
        const QPointF c = r.center();
        for (int i = -1; i <= 1; ++i) {
            const QPointF p = tall ? QPointF(c.x(), c.y() + i * 14) : QPointF(c.x() + i * 14, c.y());
            painter->drawEllipse(p, 3.0, 3.0);
        }
        painter->restore();
        return;
    }
#endif
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

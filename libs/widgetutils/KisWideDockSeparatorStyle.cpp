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
        // Krimble 2026-10-10 (2nd): the wide strip is only a touch target. Draw three small dots in its middle (the grip), and no
        // line or band, so it does not look like a thick border around the toolbox and panels.
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        QColor dotColor = option->palette.color(QPalette::WindowText);
        dotColor.setAlpha(150);
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

        // Krimble 2026-10-10 (2nd): the black-line-and-highlight version (18th entry), drawn on the dock strips by mistake.
        // Kept, not deleted. It is not reached.
#if 0
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
#endif
    }
#endif
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void KisWideDockSeparatorStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
#ifdef Q_OS_ANDROID
    if (element == QStyle::CE_MenuItem) {
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator && menuItem->text.isEmpty()) {
                // Krimble 2026-10-10: the divider between menu items: a black line with a highlight line below it.
                const QRect r = menuItem->rect;
                const int y = r.center().y();
                painter->save();
                painter->setPen(Qt::NoPen);
                painter->fillRect(QRect(r.left() + 6, y - 1, r.width() - 12, 2), QColor(0, 0, 0));
                painter->fillRect(QRect(r.left() + 6, y + 1, r.width() - 12, 2), QColor(255, 255, 255, 110));
                painter->restore();
                return;
            }
            // Krimble 2026-10-10: draw the item without its shortcut text (no keyboard on a touch screen).
            QStyleOptionMenuItem plain(*menuItem);
            const int tab = plain.text.indexOf(QLatin1Char('\t'));
            if (tab >= 0) plain.text = plain.text.left(tab);
            plain.tabWidth = 0;
            QProxyStyle::drawControl(element, &plain, painter, widget);
            return;
        }
    }
#endif
    QProxyStyle::drawControl(element, option, painter, widget);
}

QSize KisWideDockSeparatorStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
#ifdef Q_OS_ANDROID
    if (type == QStyle::CT_MenuItem) {
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator && menuItem->text.isEmpty()) {
                QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
                s.setHeight(qMax(s.height(), 8));
                return s;
            }
            // Krimble 2026-10-10: no keyboard on a touch screen, so no shortcut text and no column reserved for it.
            QStyleOptionMenuItem plain(*menuItem);
            const int tab = plain.text.indexOf(QLatin1Char('\t'));
            if (tab >= 0) plain.text = plain.text.left(tab);
            plain.tabWidth = 0;
            return QProxyStyle::sizeFromContents(type, &plain, size, widget);
        }
    }
#endif
    return QProxyStyle::sizeFromContents(type, option, size, widget);
}

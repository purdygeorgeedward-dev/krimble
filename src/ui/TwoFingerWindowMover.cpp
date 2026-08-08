#include "TwoFingerWindowMover.h"

#include <QTouchEvent>
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <QRect>

static QPointF centroidOfTwo(const QTouchEvent::TouchPoint &a, const QTouchEvent::TouchPoint &b) {
    return (a.screenPos() + b.screenPos()) * 0.5;
}

TwoFingerWindowMover::TwoFingerWindowMover(QObject *parent)
    : QObject(parent)
    , m_active(false)
{}

bool TwoFingerWindowMover::eventFilter(QObject *watched, QEvent *ev) {
    if (ev->type() != QEvent::TouchBegin &&
        ev->type() != QEvent::TouchUpdate &&
        ev->type() != QEvent::TouchEnd) {
        return QObject::eventFilter(watched, ev);
    }

    QTouchEvent *te = static_cast<QTouchEvent*>(ev);
    const auto points = te->touchPoints();

    if (points.count() >= 2) {
        const auto &p0 = points.at(0);
        const auto &p1 = points.at(1);
        QPointF centroid = centroidOfTwo(p0, p1);

        if (te->type() == QEvent::TouchBegin) {
            QWidget *widgetUnder = QApplication::widgetAt(centroid.toPoint());
            if (!widgetUnder) return QObject::eventFilter(watched, ev);
            QWidget *top = widgetUnder->window();
            if (!top || !(top->windowFlags() & Qt::Window)) return QObject::eventFilter(watched, ev);

            top->setAttribute(Qt::WA_AcceptTouchEvents, true);
            m_lastCentroid = centroid;
            m_active = true;
            m_targetWidget = top;
            ev->accept();
            return true;
        } else if (te->type() == QEvent::TouchUpdate && m_active && m_targetWidget) {
            QPointF delta = centroid - m_lastCentroid;
            if (delta.manhattanLength() < 0.5) { return true; }

            QWidget *w = m_targetWidget.data();
            if (!w) { m_active = false; return false; }

            if (w->isMaximized()) w->showNormal();
            QPoint newTopLeft = (w->frameGeometry().topLeft() + delta).toPoint();

            QScreen *screen = QGuiApplication::screenAt(centroid.toPoint());
            if (!screen) screen = QGuiApplication::primaryScreen();
            QRect avail = screen->availableGeometry();

            int newX = qBound(avail.left(), newTopLeft.x(), avail.right() - w->width());
            int newY = qBound(avail.top(), newTopLeft.y(), avail.bottom() - w->height());

            w->move(newX, newY);

            m_lastCentroid = centroid;
            ev->accept();
            return true;
        } else if (te->type() == QEvent::TouchEnd) {
            m_active = false;
            m_targetWidget.clear();
            return true;
        }
    } else {
        m_active = false;
        m_targetWidget.clear();
    }

    return QObject::eventFilter(watched, ev);
}

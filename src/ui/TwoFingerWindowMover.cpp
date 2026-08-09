#include "TwoFingerWindowMover.h"

#include <QTouchEvent>
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <QRect>

#include "canvas/kis_canvas_widget_base.h"

static QPointF centroidOfTwo(const QTouchEvent::TouchPoint &a, const QTouchEvent::TouchPoint &b) {
    return (a.screenPos() + b.screenPos()) * 0.5;
}

// Walk up from the touched widget to see if it lives inside a Krita
// canvas. If so, this is a pinch/pan/rotate gesture on the image and
// should be left alone — not hijacked into a window move.
static bool isInsideCanvas(QWidget *widget) {
    QWidget *w = widget;
    while (w) {
        if (dynamic_cast<KisCanvasWidgetBase*>(w)) {
            return true;
        }
        w = w->parentWidget();
    }
    return false;
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
        const auto &p0 = points

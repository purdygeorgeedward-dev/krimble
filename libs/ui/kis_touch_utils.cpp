#include "kis_touch_utils.h"
#include <QString>
#include <QByteArray>

qreal KisTouch::touchScale()
{
    bool ok = false;
    const QString env = QString::fromLocal8Bit(qgetenv("KRITA_TOUCH_SCALE"));
    const qreal v = env.toDouble(&ok);
    return (ok && v > 0.0) ? v : 1.0;
}

bool KisTouch::isTouchUiEnabled()
{
    return KisTouch::touchScale() > 1.01;
}

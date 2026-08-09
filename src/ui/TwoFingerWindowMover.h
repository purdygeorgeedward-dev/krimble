#ifndef TWOFINGERWINDOWMOVER_H
#define TWOFINGERWINDOWMOVER_H

#include <QObject>
#include <QPointer>
#include <QWidget>
#include <QPointF>

class TwoFingerWindowMover : public QObject {
    Q_OBJECT
public:
    explicit TwoFingerWindowMover(QObject *parent = nullptr);
    bool eventFilter(QObject *watched, QEvent *ev) override;

private:
    QPointF m_lastCentroid;
    bool m_active {false};
    QPointer<QWidget> m_targetWidget;
};

#endif // TWOFINGERWINDOWMOVER_H

/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KISTYPEOPTIONSBAR_H
#define KISTYPEOPTIONSBAR_H

#include <QObject>
#include <QPointer>
#include <QList>

class KisViewManager;
class KoSvgTextPropertiesInterface;
class KisFontComboBoxes;
class QDoubleSpinBox;
class QToolButton;
class QButtonGroup;
class QWidgetAction;
class QWidget;

/**
 * KRIMBLE 2026-10-04: the "Type Options" toolbar, a horizontal bar of the most used text controls (font and
 * style, size, alignment) that is shown under the menu bar while the Type tool is active, like the options bar
 * of the industry-standard editors. The controls are widget actions in the normal action collection, so the
 * toolbar "TypeOptionsBar" in krita5.xmlgui (and "Customize Toolbar") can place and rearrange them. They
 * read and write through the same text properties interface (KoSvgTextPropertiesInterface) as the Text
 * Properties panel, so the two always agree.
 *
 * First version: Font and Style, Size, Align left / center / right. (Orientation, anti-aliasing, color, panel
 * button and cancel / confirm are not made yet.)
 */
class KisTypeOptionsBar : public QObject
{
    Q_OBJECT
public:
    explicit KisTypeOptionsBar(KisViewManager *viewManager);

private Q_SLOTS:
    void slotInterfaceChanged(KoSvgTextPropertiesInterface *interface);
    void slotSelectionChanged();
    void slotToolChanged();
    void slotFontChanged();
    void slotSizeChanged(double pointSize);
    void slotAlignClicked(int id);

private:
    QWidgetAction *registerAction(const QString &name, const QString &text, QWidget *widget);
    double pointsPerPixel() const;
    void updateVisibility();
    void applyToCharacters(const class KoSvgTextProperties &properties);

    QPointer<KisViewManager> m_view;
    QPointer<KoSvgTextPropertiesInterface> m_interface;
    KisFontComboBoxes *m_font {nullptr};
    QDoubleSpinBox *m_size {nullptr};
    QButtonGroup *m_alignGroup {nullptr};
    QList<QWidgetAction*> m_actions;
    bool m_updating {false};
};

#endif // KISTYPEOPTIONSBAR_H

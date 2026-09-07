/*
 * SPDX-FileCopyrightText: 2005-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2022 Alvin Wong <alvin@alvinhc.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoToolBoxDocker_p.h"
#include "KoToolBox_p.h"
#include "KoToolBoxScrollArea_p.h"

#include <QLabel>
#include <QFontMetrics>
#include <QFrame>
#include <QAction>
#include <QMenu>
#include <QActionGroup>
#include <QVBoxLayout>
#include <QWidget>

#include <klocalizedstring.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

#include <KoDualColorButton.h>
#include <KisViewManager.h>
#include <kis_canvas_resource_provider.h>
#include <kis_display_color_converter.h>
#include <kis_canvas2.h>
#include <kis_image.h>

KoToolBoxDocker::KoToolBoxDocker(KoToolBox *toolBox)
    : QDockWidget(i18n("Toolbox"))
    , m_toolBox(toolBox)
    , m_scrollArea(new KoToolBoxScrollArea(toolBox, this))
{
    // Krimble: wrap the tool-button scroll area in a container so a
    // Photoshop-style foreground/background color swap widget can be added
    // below it, matching Photoshop's toolbox layout. Kept alongside its
    // existing home in the classic toolbar (kis_control_frame.cpp) -- both
    // locations retained per project decision, not a relocation.
    QWidget *toolBoxContainer = new QWidget(this);
    m_containerLayout = new QVBoxLayout(toolBoxContainer);
    m_containerLayout->setContentsMargins(0, 0, 0, 0);
    m_containerLayout->setSpacing(4);
    m_containerLayout->addWidget(m_scrollArea, 1);
    setWidget(toolBoxContainer);

    QLabel *w = new QLabel(" ", this);
    w->setFrameShape(QFrame::StyledPanel);
    w->setFrameShadow(QFrame::Raised);
    w->setFrameStyle(QFrame::Panel | QFrame::Raised);
    QFont font = qApp->font();
    // Font size may be in pixels (on Android) or points (everywhere else.)
    qreal ratio = 0.9;
    if (font.pixelSize() == -1) {
        font.setPointSizeF(font.pointSizeF() * ratio);
    } else {
        font.setPixelSize(qRound(font.pixelSize() * ratio));
    }
    int titleSize = QFontMetrics(font).height();
    w->setMinimumSize(titleSize, titleSize);
    setTitleBarWidget(w);

    KConfigGroup cfg =  KSharedConfig::openConfig()->group("KoToolBox");
    const int layoutDirUnchecked = cfg.readEntry<int>("layoutDir", Qt::LayoutDirectionAuto);
    switch (layoutDirUnchecked) {
    case Qt::LayoutDirectionAuto:
    case Qt::LeftToRight:
    case Qt::RightToLeft:
        m_layoutDir = static_cast<Qt::LayoutDirection>(layoutDirUnchecked);
        break;
    default:
        m_layoutDir = Qt::LayoutDirectionAuto;
        break;
    }
    updateLayoutDir();

    const int orientUnchecked = cfg.readEntry<int>("orientation", Auto);
    switch (orientUnchecked) {
    case Horizontal:
    case Vertical:
    case Auto:
        m_orientation = static_cast<Orientation>(orientUnchecked);
        break;
    default:
        m_orientation = Auto;
        break;
    }
    if (m_orientation != Auto) {
        setToolBoxOrientation(static_cast<Qt::Orientation>(m_orientation));
    }

    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(updateToolBoxOrientation(Qt::DockWidgetArea)));
    connect(this, SIGNAL(topLevelChanged(bool)),
            this, SLOT(updateFloating(bool)));
}

void KoToolBoxDocker::setCanvas(KoCanvasBase *canvas)
{
    Q_UNUSED(canvas);
}

void KoToolBoxDocker::unsetCanvas()
{
}

void KoToolBoxDocker::setViewManager(KisViewManager *viewManager)
{
    m_toolBox->setViewManager(viewManager);
    m_viewManager = viewManager;

    // Krimble: construct the color swap widget once a real KisViewManager
    // (and its canvasResourceProvider) is available -- not possible any
    // earlier, since the docker only gets a real one here.
    if (!m_dualColorButton && viewManager) {
        const KoColorDisplayRendererInterface *displayRenderer =
            KisDisplayColorConverter::dumbConverterInstance()->displayRendererInterface();
        m_dualColorButton = new KoDualColorButton(viewManager->canvasResourceProvider(), displayRenderer,
                                                    viewManager->mainWindowAsQWidget(), viewManager->mainWindowAsQWidget());
        m_dualColorButton->setFixedSize(28, 28);
        m_containerLayout->addWidget(m_dualColorButton, 0, Qt::AlignHCenter);

        connect(m_dualColorButton, SIGNAL(foregroundColorChanged(KoColor)), viewManager->canvasResourceProvider(), SLOT(slotSetFGColor(KoColor)));
        connect(m_dualColorButton, SIGNAL(backgroundColorChanged(KoColor)), viewManager->canvasResourceProvider(), SLOT(slotSetBGColor(KoColor)));
        connect(viewManager->canvasResourceProvider(), SIGNAL(sigBGColorChanged(KoColor)), m_dualColorButton, SLOT(setBackgroundColor(KoColor)));
        connect(viewManager->canvasResourceProvider(), SIGNAL(sigFGColorChanged(KoColor)), m_dualColorButton, SLOT(setForegroundColor(KoColor)));

        connect(viewManager, &KisViewManager::viewChanged, this, &KoToolBoxDocker::slotUpdateDisplayRenderer);
        slotUpdateDisplayRenderer();
    }
}

void KoToolBoxDocker::slotUpdateDisplayRenderer()
{
    if (!m_dualColorButton || !m_viewManager) return;
    if (m_viewManager->canvasBase()) {
        m_dualColorButton->setDisplayRenderer(m_viewManager->canvasBase()->displayColorConverter()->displayRendererInterface());
        m_dualColorButton->updateColorSpace();
        m_viewManager->canvasBase()->image()->disconnect(m_dualColorButton);
        connect(m_viewManager->canvasBase()->image(), SIGNAL(sigColorSpaceChanged(const KoColorSpace*)), m_dualColorButton, SLOT(updateColorSpace()), Qt::UniqueConnection);
    } else if (m_viewManager->viewCount() == 0) {
        m_dualColorButton->setDisplayRenderer();
    }
}

void KoToolBoxDocker::resizeEvent(QResizeEvent *event)
{
    QDockWidget::resizeEvent(event);
    if (m_orientation == Auto) {
        setToolBoxOrientation(width() > height() ? Qt::Horizontal : Qt::Vertical);
    }
}

void KoToolBoxDocker::updateToolBoxOrientation(Qt::DockWidgetArea area)
{
    m_dockArea = area;
    updateLayoutDir();
    if (m_orientation == Auto) {
        setToolBoxOrientation(width() > height() ? Qt::Horizontal : Qt::Vertical);
    }
}

void KoToolBoxDocker::updateLayoutDir()
{
    if (m_layoutDir == Qt::LayoutDirectionAuto) {
        if (m_dockArea == Qt::RightDockWidgetArea) {
            m_scrollArea->setLayoutDirection(Qt::RightToLeft);
        } else if (m_dockArea == Qt::LeftDockWidgetArea) {
            m_scrollArea->setLayoutDirection(Qt::LeftToRight);
        } else {
            m_scrollArea->unsetLayoutDirection();
        }
    } else {
        m_scrollArea->setLayoutDirection(m_layoutDir);
    }
}

void KoToolBoxDocker::changeLayoutDir(Qt::LayoutDirection dir)
{
    KConfigGroup cfg = KSharedConfig::openConfig()->group("KoToolBox");
    cfg.writeEntry<int>("layoutDir", dir);
    m_layoutDir = dir;
    updateLayoutDir();
}

void KoToolBoxDocker::changeOrientation(const Orientation orientation)
{
    if (m_orientation == orientation) {
        return;
    }
    KConfigGroup cfg = KSharedConfig::openConfig()->group("KoToolBox");
    cfg.writeEntry<int>("orientation", orientation);
    m_orientation = orientation;
    if (m_orientation == Auto) {
        setToolBoxOrientation(width() > height() ? Qt::Horizontal : Qt::Vertical);
    } else {
        setToolBoxOrientation(static_cast<Qt::Orientation>(m_orientation));
    }
}

void KoToolBoxDocker::changeCompact(const bool state)
{
    KConfigGroup cfg = KSharedConfig::openConfig()->group("KoToolBox");
    cfg.writeEntry<bool>("compact", state);
    m_scrollArea->setCompact(state);
}

void KoToolBoxDocker::setToolBoxOrientation(Qt::Orientation orientation)
{
    if (m_scrollArea->orientation() == orientation) {
        return;
    }
    if (orientation == Qt::Horizontal) {
        setFeatures(features() | QDockWidget::DockWidgetVerticalTitleBar);
        m_scrollArea->setOrientation(Qt::Horizontal);
    } else {
        setFeatures(features() & ~QDockWidget::DockWidgetVerticalTitleBar);
        m_scrollArea->setOrientation(Qt::Vertical);
    }
}

void KoToolBoxDocker::updateFloating(bool v)
{
    m_toolBox->setFloating(v);
}

void KoToolBoxDocker::contextMenuEvent(QContextMenuEvent *event)
{
    if (!m_contextMenu) {
        m_contextMenu = new QMenu(this);

        QAction *compact = m_contextMenu->addAction(i18n("Compact"));
        compact->setCheckable(true);
        compact->setChecked(m_toolBox->isCompact());
        connect(compact, &QAction::triggered, this, &KoToolBoxDocker::changeCompact);

        m_contextMenu->addSection(i18n("Icon Size"));
        m_toolBox->setupIconSizeMenu(m_contextMenu);

        m_contextMenu->addSection(i18nc("Toolbox layout", "Layout"));
        QActionGroup *layoutActionGroup = new QActionGroup(m_contextMenu);

        QAction *layoutAuto = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox layout direction", "&Automatic"));
        layoutAuto->setActionGroup(layoutActionGroup);
        layoutAuto->setCheckable(true);
        connect(layoutAuto, &QAction::triggered, this, [this]() {
            changeLayoutDir(Qt::LayoutDirectionAuto);
        });

        QAction *layoutLtr = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox layout direction", "&Left-to-right"));
        layoutLtr->setActionGroup(layoutActionGroup);
        layoutLtr->setCheckable(true);
        connect(layoutLtr, &QAction::triggered, this, [this]() {
            changeLayoutDir(Qt::LeftToRight);
        });

        QAction *layoutRtl = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox layout direction", "&Right-to-left"));
        layoutRtl->setActionGroup(layoutActionGroup);
        layoutRtl->setCheckable(true);
        connect(layoutRtl, &QAction::triggered, this, [this]() {
            changeLayoutDir(Qt::RightToLeft);
        });

        switch (m_layoutDir) {
        case Qt::LayoutDirectionAuto:
            layoutAuto->setChecked(true);
            break;
        case Qt::LeftToRight:
            layoutLtr->setChecked(true);
            break;
        case Qt::RightToLeft:
            layoutRtl->setChecked(true);
            break;
        }

        m_contextMenu->addSection(i18nc("Toolbox orientation", "Orientation"));
        QActionGroup *orientActionGroup = new QActionGroup(m_contextMenu);

        QAction *orientAuto = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox orientation", "&Automatic"));
        orientAuto->setActionGroup(orientActionGroup);
        orientAuto->setCheckable(true);
        connect(orientAuto, &QAction::triggered, this, [this]() {
            changeOrientation(Auto);
        });

        QAction *orientHorizontal = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox orientation", "&Horizontal"));
        orientHorizontal->setActionGroup(orientActionGroup);
        orientHorizontal->setCheckable(true);
        connect(orientHorizontal, &QAction::triggered, this, [this]() {
            changeOrientation(Horizontal);
        });

        QAction *orientVertical = m_contextMenu->addAction(i18nc("@item:inmenu Toolbox orientation", "&Vertical"));
        orientVertical->setActionGroup(orientActionGroup);
        orientVertical->setCheckable(true);
        connect(orientVertical, &QAction::triggered, this, [this]() {
            changeOrientation(Vertical);
        });

        switch (m_orientation) {
        case Horizontal:
            orientHorizontal->setChecked(true);
            break;
        case Vertical:
            orientVertical->setChecked(true);
            break;
        case Auto:
            orientAuto->setChecked(true);
            break;
        }
    }

    m_contextMenu->exec(event->globalPos());
}

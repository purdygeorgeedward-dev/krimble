/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2026 Krimble contributors
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_vibrance_filter.h"

#include <filter/kis_filter_category_ids.h>
#include <filter/kis_filter_configuration.h>
#include <KoColorSpace.h>
#include <kis_paint_device.h>
#include <kis_processing_information.h>
#include <KisSequentialIteratorProgress.h>
#include <KisGlobalResourcesInterface.h>

// Krimble: Vibrance implementation. No equivalent existed in Krita; the
// underlying algorithm is not Adobe's own (that formula is proprietary
// and undocumented -- confirmed via web research, no public source
// reproduces it exactly). This instead uses the well-known open-source
// "vibrance" shader algorithm originally written by CeeJay.dk, MIT
// licensed, distributed for over a decade via SweetFX/ReShade/GShade
// across thousands of games. It is a *different*, simpler
// implementation of the same general idea (boost muted colors more
// than already-saturated ones) -- not a byte-for-byte match to any
// commercial product's internal math.
//
// Algorithm, per pixel:
//   1. luma = 0.212656*R + 0.715158*G + 0.072186*B  (Rec. 709 weights)
//   2. saturation = max(R,G,B) - min(R,G,B)          (simple 0-1 proxy)
//   3. each channel is blended toward/away from luma by a factor of
//      1 + vibrance * (1 - saturation), so the effect is strongest on
//      already-muted pixels and fades out as saturation approaches 1.
// Implemented directly on RGB16 (via KoColorSpace::toRgbA16/fromRgbA16,
// the same pair of colorspace-agnostic conversion functions the
// existing Match Color filter uses for Lab -- see
// fastcolortransfer.cpp's use of fromLabA16), rather than a templated
// per-bit-depth KoColorTransformation registered in the color engine
// (the pattern kis_hsv_adjustment.cpp uses) -- simpler, and sufficient
// since vibrance doesn't need to run in a specific working colorspace
// the way Match Color's Lab statistics do.
//
// Pixel channel order confirmed as [Blue, Green, Red, Alpha] for the
// RGB16 space toRgbA16 converts through -- see KoBgrColorSpaceTraits.h
// (red_pos=2, green_pos=1, blue_pos=0).

KisVibranceFilter::KisVibranceFilter() : KisFilter(id(), FiltersCategoryAdjustId, i18n("&Vibrance..."))
{
    setColorSpaceIndependence(FULLY_INDEPENDENT);
    setSupportsPainting(true);
    setSupportsThreading(true);
    setSupportsAdjustmentLayers(true);
}

KisConfigWidget * KisVibranceFilter::createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP dev, bool) const
{
    Q_UNUSED(dev);
    return new KisVibranceConfigWidget(parent);
}

KisFilterConfigurationSP KisVibranceFilter::defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const
{
    KisFilterConfigurationSP config = factoryConfiguration(resourcesInterface);
    config->setProperty("vibrance", 0);
    return config;
}

void KisVibranceFilter::processImpl(KisPaintDeviceSP device,
                                    const QRect& applyRect,
                                    const KisFilterConfigurationSP config,
                                    KoUpdater* progressUpdater) const
{
    Q_ASSERT(device != 0);

    const double vibrance = config ? config->getInt("vibrance", 0) * 0.01 : 0.0;
    if (vibrance == 0.0) return;

    const double vibranceSign = vibrance < 0.0 ? -1.0 : 1.0;
    const KoColorSpace *cs = device->colorSpace();

    quint16 rgba[4];

    KisSequentialIteratorProgress it(device, applyRect, progressUpdater);
    while (it.nextPixel()) {
        cs->toRgbA16(it.oldRawData(), reinterpret_cast<quint8*>(rgba), 1);

        const double b = rgba[0] / 65535.0;
        const double g = rgba[1] / 65535.0;
        const double r = rgba[2] / 65535.0;

        const double luma = 0.212656 * r + 0.715158 * g + 0.072186 * b;

        const double maxChannel = qMax(r, qMax(g, b));
        const double minChannel = qMin(r, qMin(g, b));
        const double saturation = maxChannel - minChannel;

        const double factor = 1.0 + vibrance * (1.0 - vibranceSign * saturation);

        double newR = luma + (r - luma) * factor;
        double newG = luma + (g - luma) * factor;
        double newB = luma + (b - luma) * factor;

        newR = qBound(0.0, newR, 1.0);
        newG = qBound(0.0, newG, 1.0);
        newB = qBound(0.0, newB, 1.0);

        rgba[0] = quint16(newB * 65535.0 + 0.5);
        rgba[1] = quint16(newG * 65535.0 + 0.5);
        rgba[2] = quint16(newR * 65535.0 + 0.5);
        // rgba[3] (alpha) left unchanged

        cs->fromRgbA16(reinterpret_cast<quint8*>(rgba), it.rawData(), 1);
    }
}

KisVibranceConfigWidget::KisVibranceConfigWidget(QWidget* parent) : KisConfigWidget(parent)
{
    m_page = new Ui_WdgVibrance();
    m_page->setupUi(this);

    m_page->vibranceSlider->setMinimum(-100);
    m_page->vibranceSlider->setMaximum(100);
    m_page->vibranceSpinbox->setMinimum(-100);
    m_page->vibranceSpinbox->setMaximum(100);

    connect(m_page->vibranceSlider, SIGNAL(valueChanged(int)), m_page->vibranceSpinbox, SLOT(setValue(int)));
    connect(m_page->vibranceSpinbox, SIGNAL(valueChanged(int)), m_page->vibranceSlider, SLOT(setValue(int)));

    connect(m_page->vibranceSlider, SIGNAL(valueChanged(int)), SIGNAL(sigConfigurationItemChanged()));

    connect(m_page->pushResetButton, SIGNAL(clicked()), SLOT(slotReset()));
}

KisVibranceConfigWidget::~KisVibranceConfigWidget()
{
    delete m_page;
}

KisPropertiesConfigurationSP KisVibranceConfigWidget::configuration() const
{
    KisFilterConfigurationSP config = new KisFilterConfiguration(KisVibranceFilter::id().id(), 1, KisGlobalResourcesInterface::instance());
    config->setProperty("vibrance", m_page->vibranceSlider->value());
    return config;
}

void KisVibranceConfigWidget::setConfiguration(const KisPropertiesConfigurationSP config)
{
    m_page->vibranceSlider->setValue(config->getInt("vibrance", 0));
}

void KisVibranceConfigWidget::slotReset()
{
    m_page->vibranceSlider->setValue(0);
}

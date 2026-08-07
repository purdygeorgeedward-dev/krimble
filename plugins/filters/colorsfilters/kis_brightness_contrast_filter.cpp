/*
 *  SPDX-FileCopyrightText: 2026 Krita Mobile contributors
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kis_brightness_contrast_filter.h"
#include <filter/kis_filter_category_ids.h>
#include "filter/kis_color_transformation_configuration.h"
#include "kis_selection.h"
#include "kis_paint_device.h"
#include "kis_processing_information.h"
#include <KisGlobalResourcesInterface.h>

KisBrightnessContrastFilter::KisBrightnessContrastFilter()
        : KisColorTransformationFilter(id(), FiltersCategoryAdjustId, i18n("&Brightness/Contrast..."))
{
    setSupportsPainting(true);
}

KisConfigWidget * KisBrightnessContrastFilter::createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP dev, bool) const
{
    Q_UNUSED(dev);
    return new KisBrightnessContrastConfigWidget(parent);
}

KoColorTransformation * KisBrightnessContrastFilter::createTransformation(const KoColorSpace* cs, const KisFilterConfigurationSP config) const
{
    // NOTE for future development:
    // This applies the brightness/contrast curve directly on the image's
    // native color space (matching Photoshop's own Brightness/Contrast
    // dialog, which is believed to operate in RGB as well - unverified).
    //
    // Krita's Levels filter (KisLevelsFilter) declares
    // setColorSpaceIndependence(TO_LAB16), which looks like it signals an
    // intent to run lightness adjustments through Lab space instead -
    // adjusting only the L channel avoids the hue/saturation drift that can
    // occur when the same nonlinear curve is applied independently to R, G,
    // and B (a real, established technique, similar to how Darktable
    // handles tonal adjustments in Lab).
    //
    // As of this writing, nothing in the codebase actually reads
    // colorSpaceIndependence() to perform that conversion - it's set but
    // never consumed. So even Levels' "lightness mode" runs directly on the
    // native color space today, not through Lab. TO_LAB16 may be a
    // half-finished feature from earlier Krita development that never got
    // wired up.
    //
    // A genuine Lab-based version of this filter (convert to Lab, curve the
    // L channel only, convert back) would likely give better results with
    // less color shift on strong adjustments, but is a separate, larger
    // task from this RGB-native implementation.

    double brightness = 0.0;
    double contrast = 0.0;
    if (config) {
        brightness = config->getInt("brightness", 0) * 0.01;
        contrast = config->getInt("contrast", 0) * 0.01;
    }

    const double contrastFactor = 1.0 + contrast;

    quint16 transfer[256];
    for (int i = 0; i < 256; i++) {
        double value = i / 255.0;
        value = (value - 0.5) * contrastFactor + 0.5 + brightness;
        value = qBound(0.0, value, 1.0);
        transfer[i] = quint16(value * 0xFFFF + 0.5);
    }

    return cs->createBrightnessContrastAdjustment(transfer);
}

KisFilterConfigurationSP KisBrightnessContrastFilter::defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const
{
    KisFilterConfigurationSP config = factoryConfiguration(resourcesInterface);
    config->setProperty("brightness", 0);
    config->setProperty("contrast", 0);
    return config;
}

KisBrightnessContrastConfigWidget::KisBrightnessContrastConfigWidget(QWidget* parent) : KisConfigWidget(parent)
{
    m_page = new Ui_WdgBrightnessContrast();
    m_page->setupUi(this);

    m_page->brightnessSlider->setMinimum(-100);
    m_page->brightnessSlider->setMaximum(100);
    m_page->brightnessSpinbox->setMinimum(-100);
    m_page->brightnessSpinbox->setMaximum(100);

    m_page->contrastSlider->setMinimum(-100);
    m_page->contrastSlider->setMaximum(100);
    m_page->contrastSpinbox->setMinimum(-100);
    m_page->contrastSpinbox->setMaximum(100);

    connect(m_page->brightnessSlider, SIGNAL(valueChanged(int)), m_page->brightnessSpinbox, SLOT(setValue(int)));
    connect(m_page->brightnessSpinbox, SIGNAL(valueChanged(int)), m_page->brightnessSlider, SLOT(setValue(int)));
    connect(m_page->contrastSlider, SIGNAL(valueChanged(int)), m_page->contrastSpinbox, SLOT(setValue(int)));
    connect(m_page->contrastSpinbox, SIGNAL(valueChanged(int)), m_page->contrastSlider, SLOT(setValue(int)));

    connect(m_page->brightnessSlider, SIGNAL(valueChanged(int)), SIGNAL(sigConfigurationItemChanged()));
    connect(m_page->contrastSlider, SIGNAL(valueChanged(int)), SIGNAL(sigConfigurationItemChanged()));

    connect(m_page->pushResetButton, SIGNAL(clicked()), SLOT(slotReset()));
}

KisBrightnessContrastConfigWidget::~KisBrightnessContrastConfigWidget()
{
    delete m_page;
}

KisPropertiesConfigurationSP KisBrightnessContrastConfigWidget::configuration() const
{
    KisColorTransformationConfigurationSP c = new KisColorTransformationConfiguration(KisBrightnessContrastFilter::id().id(), 0, KisGlobalResourcesInterface::instance());
    c->setProperty("brightness", m_page->brightnessSlider->value());
    c->setProperty("contrast", m_page->contrastSlider->value());
    return c;
}

void KisBrightnessContrastConfigWidget::setConfiguration(const KisPropertiesConfigurationSP config)
{
    m_page->brightnessSlider->setValue(config->getInt("brightness", 0));
    m_page->contrastSlider->setValue(config->getInt("contrast", 0));
}

void KisBrightnessContrastConfigWidget::slotReset()
{
    m_page->brightnessSlider->setValue(0);
    m_page->contrastSlider->setValue(0);
}
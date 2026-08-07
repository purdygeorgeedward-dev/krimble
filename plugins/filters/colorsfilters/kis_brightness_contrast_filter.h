/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2026 Krita Mobile contributors
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KIS_BRIGHTNESS_CONTRAST_FILTER_H
#define KIS_BRIGHTNESS_CONTRAST_FILTER_H

#include <filter/kis_color_transformation_filter.h>
#include <kis_config_widget.h>

#include "ui_wdg_brightness_contrast.h"

class QWidget;
class KoColorSpace;
class KoColorTransformation;

class KisBrightnessContrastFilter : public KisColorTransformationFilter
{
public:
    KisBrightnessContrastFilter();

    KisConfigWidget *createConfigurationWidget(QWidget *parent,
                                               const KisPaintDeviceSP dev,
                                               bool useForMasks) const override;

    KoColorTransformation *createTransformation(
        const KoColorSpace *cs,
        const KisFilterConfigurationSP config) const override;

    KisFilterConfigurationSP
    defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const override;

    static inline KoID id()
    {
        return KoID("brightnesscontrast", i18n("Brightness/Contrast"));
    }
};

class KisBrightnessContrastConfigWidget : public KisConfigWidget
{
    Q_OBJECT

public:
    explicit KisBrightnessContrastConfigWidget(QWidget *parent);
    ~KisBrightnessContrastConfigWidget() override;

    KisPropertiesConfigurationSP configuration() const override;
    void setConfiguration(const KisPropertiesConfigurationSP config) override;

public Q_SLOTS:
    void slotReset();

private:
    Ui_WdgBrightnessContrast *m_page;
};

#endif
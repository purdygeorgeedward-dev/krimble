/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2026 Krimble contributors
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_VIBRANCE_FILTER_H
#define KIS_VIBRANCE_FILTER_H

#include <filter/kis_filter.h>
#include <kis_config_widget.h>

#include "ui_wdg_vibrance.h"

class KisVibranceFilter : public KisFilter
{
public:
    KisVibranceFilter();

    void processImpl(KisPaintDeviceSP device,
                     const QRect& applyRect,
                     const KisFilterConfigurationSP config,
                     KoUpdater* progressUpdater) const override;

    static inline KoID id() {
        return KoID("vibrance", i18n("&Vibrance..."));
    }

    KisConfigWidget * createConfigurationWidget(QWidget* parent, const KisPaintDeviceSP dev, bool useForMasks) const override;
    KisFilterConfigurationSP defaultConfiguration(KisResourcesInterfaceSP resourcesInterface) const override;
};

class KisVibranceConfigWidget : public KisConfigWidget
{
    Q_OBJECT

public:
    explicit KisVibranceConfigWidget(QWidget *parent);
    ~KisVibranceConfigWidget() override;

    KisPropertiesConfigurationSP configuration() const override;
    void setConfiguration(const KisPropertiesConfigurationSP config) override;

public Q_SLOTS:
    void slotReset();

private:
    Ui_WdgVibrance *m_page;
};

#endif

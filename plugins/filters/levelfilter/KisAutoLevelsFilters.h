/*
 * KRIMBLE 2026-10-02: one-click Auto Tone and Auto Color filters.
 *
 * Both run the existing auto levels engine (KisAutoLevels, the same code
 * behind the "auto levels" buttons in the Levels dialog) with fixed settings
 * and then apply the result through the existing Levels transformation.
 * No new algorithm is introduced.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_AUTO_LEVELS_FILTERS_H
#define KIS_AUTO_LEVELS_FILTERS_H

#include <KoID.h>
#include <klocalizedstring.h>
#include <filter/kis_filter.h>

#include <KisAutoLevels.h>

/**
 * Shared implementation. Works on RGB images only (same limit as the
 * "auto levels for all channels" button in the Levels dialog); other color
 * models are left unchanged.
 */
class KisAutoLevelsFilterBase : public KisFilter
{
public:
    KisAutoLevelsFilterBase(const KoID &id,
                            const QString &menuEntry,
                            KisAutoLevels::MidtonesAdjustmentMethod midtonesMethod,
                            qreal midtonesAmount);

    void processImpl(KisPaintDeviceSP device,
                     const QRect &applyRect,
                     const KisFilterConfigurationSP config,
                     KoUpdater *progressUpdater) const override;

private:
    KisAutoLevels::MidtonesAdjustmentMethod m_midtonesMethod;
    qreal m_midtonesAmount; // 0.0 - 1.0
};

/**
 * Auto Tone: per-channel contrast stretch (black and white points found
 * separately for each channel, 0.1% clipping at each end). Midtones are not
 * touched.
 */
class KisAutoToneFilter : public KisAutoLevelsFilterBase
{
public:
    KisAutoToneFilter();

    static inline KoID id()
    {
        return KoID("autotone", i18n("Auto Tone"));
    }
};

/**
 * Auto Color: same per-channel contrast stretch as Auto Tone, plus midtone
 * neutralization: each channel's mean is moved to 50% gray (full strength),
 * which removes color casts.
 */
class KisAutoColorFilter : public KisAutoLevelsFilterBase
{
public:
    KisAutoColorFilter();

    static inline KoID id()
    {
        return KoID("autocolor", i18n("Auto Color"));
    }
};

#endif

/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_FILTER_CONTENT_AWARE_FILL_H
#define KIS_FILTER_CONTENT_AWARE_FILL_H

#include "filter/kis_filter.h"

/**
 * Krimble: Content-Aware Fill, wired directly to the existing PatchMatch
 * inpainting algorithm (kis_inpaint.cpp / patchImage()) that already backs
 * the Healing Brush tool -- no new fill algorithm here, just a selection-
 * based entry point for it that doesn't require painting a brush mask.
 *
 * First-pass implementation, intentionally simple: fills the whole apply
 * rect (the active selection's bounding rect, expanded for context via
 * neededRect()) rather than deriving a precise per-pixel mask from the
 * selection's actual shape -- final compositing is still correctly clipped
 * to the real selection shape by the generic filter-application framework,
 * so the visible result is correct even though the algorithm's internal
 * search/fill boundary is a plain rectangle. Good enough to be useful; not
 * pretending to be a polished reimplementation of Adobe's feature.
 */
class KisFilterContentAwareFill : public KisFilter
{
public:
    KisFilterContentAwareFill();

    void processImpl(KisPaintDeviceSP device,
                     const QRect &applyRect,
                     const KisFilterConfigurationSP config,
                     KoUpdater *progressUpdater) const override;

    static inline KoID id() {
        return KoID("content_aware_fill", i18n("Content-Aware Fill"));
    }

    QRect neededRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const override;
    QRect changedRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const override;
};

#endif // KIS_FILTER_CONTENT_AWARE_FILL_H

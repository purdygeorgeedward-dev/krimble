/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "kis_filter_content_aware_fill.h"

#include <kis_paint_device.h>
#include <KoColorSpaceRegistry.h>
#include <KoColor.h>
#include <filter/kis_filter_category_ids.h>
#include <filter/kis_filter_configuration.h>

// Declared in kis_inpaint.cpp, same as kis_tool_smart_patch.cpp already
// forward-declares it -- shared build target (kritatoolSmartPatch), no
// cross-plugin linkage involved.
QRect patchImage(KisPaintDeviceSP imageDev, KisPaintDeviceSP maskDev, int radius, int accuracy, KisSelectionSP selection);

KisFilterContentAwareFill::KisFilterContentAwareFill()
    : KisFilter(id(), FiltersCategoryOtherId, i18n("&Content-Aware Fill..."))
{
    setSupportsPainting(false);
    setSupportsAdjustmentLayers(false);
    setSupportsThreading(false);
    setSupportsLevelOfDetail(false);
    setColorSpaceIndependence(FULLY_INDEPENDENT);
}

void KisFilterContentAwareFill::processImpl(KisPaintDeviceSP device,
                                             const QRect &applyRect,
                                             const KisFilterConfigurationSP config,
                                             KoUpdater *progressUpdater) const
{
    Q_UNUSED(config);
    Q_UNUSED(progressUpdater);

    if (applyRect.isEmpty() || !device) return;

    // Sloppy but working first pass: a plain fully-opaque rectangular mask
    // over applyRect, rather than a mask precisely shaped like the actual
    // selection. patchImage() searches for source material anywhere in
    // `device` outside the mask and fills everywhere the mask indicates.
    // The real selection's shape still wins in the end -- the generic
    // filter-application framework only composites this filter's output
    // back within the actual selected pixels, same as any other filter.
    KisPaintDeviceSP maskDev = new KisPaintDevice(KoColorSpaceRegistry::instance()->alpha8());
    maskDev->fill(applyRect, KoColor(Qt::white, maskDev->colorSpace()));

    const int patchRadius = 4; // matches the Healing Brush tool's own default
    const int accuracy = 50;   // matches the Healing Brush tool's own default

    patchImage(device, maskDev, patchRadius, accuracy, nullptr);
}

QRect KisFilterContentAwareFill::neededRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const
{
    Q_UNUSED(config);
    Q_UNUSED(lod);
    // Inpainting needs to search for source material well beyond the area
    // being filled. A fixed generous margin rather than the true image
    // bounds -- sloppy, but works for typical selection/canvas sizes.
    const int margin = 512;
    return rect.adjusted(-margin, -margin, margin, margin);
}

QRect KisFilterContentAwareFill::changedRect(const QRect &rect, const KisFilterConfigurationSP config, int lod) const
{
    Q_UNUSED(config);
    Q_UNUSED(lod);
    return rect;
}

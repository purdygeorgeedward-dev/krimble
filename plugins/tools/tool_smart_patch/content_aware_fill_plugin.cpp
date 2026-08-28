/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "content_aware_fill_plugin.h"

#include <kpluginfactory.h>

#include <klocalizedstring.h>
#include <kundo2magicstring.h>
#include <KisViewManager.h>
#include <kis_image.h>
#include <kis_node.h>
#include <kis_selection.h>
#include <kis_pixel_selection.h>
#include <kis_action.h>
#include <kis_processing_applicator.h>
#include <commands_new/kis_transaction_based_command.h>
#include <kis_transaction.h>
#include <KisCursorOverrideLock.h>

// Declared in kis_inpaint.cpp -- compiled into this plugin module
// separately from kritatoolSmartPatch's own copy (see CMakeLists.txt);
// each is an independent shared library, so this is a normal, safe
// duplication, not an ODR violation.
QRect patchImage(KisPaintDeviceSP imageDev, KisPaintDeviceSP maskDev, int radius, int accuracy, KisSelectionSP selection);

namespace {

// Krimble: same tiny transaction-wrapping command shape as
// KisToolSmartPatch::InpaintCommand in kis_tool_smart_patch.cpp -- kept as
// its own local copy here rather than reused across the tool/action
// boundary, since the tool's version is a private nested class.
class ContentAwareFillCommand : public KisTransactionBasedCommand
{
public:
    ContentAwareFillCommand(KisPaintDeviceSP maskDev, KisPaintDeviceSP imageDev, int accuracy, int patchRadius, KisSelectionSP selection)
        : m_maskDev(maskDev), m_imageDev(imageDev), m_accuracy(accuracy), m_patchRadius(patchRadius), m_selection(selection)
    {
    }

    KUndo2Command *paint() override
    {
        KisTransaction transaction(m_imageDev);
        patchImage(m_imageDev, m_maskDev, m_patchRadius, m_accuracy, m_selection);
        return transaction.endAndTake();
    }

private:
    KisPaintDeviceSP m_maskDev, m_imageDev;
    int m_accuracy, m_patchRadius;
    KisSelectionSP m_selection;
};

}

K_PLUGIN_FACTORY_WITH_JSON(ContentAwareFillPluginFactory, "contentawarefill.json", registerPlugin<ContentAwareFillPlugin>();)

ContentAwareFillPlugin::ContentAwareFillPlugin(QObject *parent, const QVariantList &)
    : KisActionPlugin(parent)
{
    KisAction *action = createAction("content_aware_fill");
    connect(action, SIGNAL(triggered()), this, SLOT(slotContentAwareFill()));
}

ContentAwareFillPlugin::~ContentAwareFillPlugin()
{
}

void ContentAwareFillPlugin::slotContentAwareFill()
{
    KisImageSP image = viewManager()->image();
    if (!image) return;

    KisNodeSP node = viewManager()->activeNode();
    if (!node || !node->paintDevice()) return;

    KisSelectionSP selection = viewManager()->selection();
    if (!selection) return; // nothing to fill without an active selection

    KisCursorOverrideLock cursorLock(Qt::WaitCursor);

    if (!viewManager()->blockUntilOperationsFinished(image)) return;

    // Krimble: unlike the sloppy KisFilter-based first pass, this passes
    // the selection's own real pixel mask straight through -- correct for
    // irregular/feathered selections, since patchImage() gets to see the
    // true selected shape while deciding what to fill vs. what's valid
    // source material, not just a rectangular bounding-box approximation.
    KisPaintDeviceSP maskDev = selection->pixelSelection();

    const int patchRadius = 4; // matches the Healing Brush tool's own default
    const int accuracy = 50;   // matches the Healing Brush tool's own default

    KisProcessingApplicator applicator(image, node, KisProcessingApplicator::NONE,
                                        KisImageSignalVector(), kundo2_i18n("Content-Aware Fill"));

    applicator.applyCommand(new ContentAwareFillCommand(maskDev, node->paintDevice(), accuracy, patchRadius, selection),
                             KisStrokeJobData::BARRIER, KisStrokeJobData::EXCLUSIVE);

    applicator.end();
    image->waitForDone();
}

#include "content_aware_fill_plugin.moc"

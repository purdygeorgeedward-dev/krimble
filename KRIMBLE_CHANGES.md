# Krimble Changes

This file tracks fixes and changes made in this fork (Krimble) on top of
upstream Krita, specifically for the Android build. It is not exhaustive
history — it's a running log of notable bugs found and fixed, so future
work (by anyone, including an AI assistant with no memory of past
sessions) can see what's already been addressed and why, without having
to rediscover it from scratch.

## Standing rules for this fork

- **Never delete menu/action XML entries.** Comment them out (`<!-- -->`)
  instead. A deletion buried in an unrelated commit is silent and
  unrecoverable without git archaeology (see the Close All incident
  below); a comment is visible in the file itself and easy to restore.
- **Photoshop feature parity — same features, same menu locations — is
  the primary UI goal** for this fork.

## 2026-09-02 — File/Edit/View toolbar items silently deleted, restored

**File:** `krita/krita5.xmlgui`

`file_close`, `file_close_all`, `file_quit` (File toolbar/menu) and
`toggle-selection-overlay-mode`, `show-global-selection-mask`,
`view_show_canvas_only`, `fullscreen` (View toolbar) were all deleted in
commit `718e83878` ("Update krita5.xmlgui", 2026-08-24, George Edward
Purdy). That commit bundled these deletions together with unrelated
cleanup across three different toolbar blocks in one generic-message
commit, with no indication any of it was intentional — it has the
signature of accidental collateral damage from a broader toolbar-trim
pass, not a deliberate decision to drop Close All specifically.

All of the above still had valid, registered actions (nothing else
referenced them, but they weren't gone from the codebase) and have been
restored, with comments explaining why, per the no-delete rule above.

## Missing Photoshop-parity features (not just menu entries — the underlying feature doesn't exist)

Found while restoring the toolbar deletions above: two groups of actions
were referenced in the pre-`718e83878` menu but no longer exist
*anywhere* in the codebase — not in any `.action` file, not registered
via `createAction()` in any `.cpp`. These aren't menu-restoration jobs;
the feature itself needs to be built:

- **File > Print / Print One Copy** (`file_print`, `file_print_preview`)
  — Photoshop has both under File. No print implementation currently
  exists in this codebase at all.
- **Edit > Find and Replace** (`edit_find`, `edit_find_next`,
  `edit_find_prev`, `edit_replace`) — Photoshop has Find and Replace Text
  under Edit. No find/replace implementation currently exists in this
  codebase at all (this would apply to text layers/the text tool,
  Krita's closest equivalent to Photoshop's text-focused find/replace).

Left commented out in `krita5.xmlgui` at their original menu locations so
the intended placement isn't lost, pending a full menu-by-menu Photoshop
parity audit (comparing every menu against
`PHOTOSHOP_27_MENUS.TXT`/`Adobephotoshopshortcutkeyspdf.pdf` in the
project files) to catalog anything else in the same situation.



**Commit:** `b1eb52e`
**File:** `plugins/color/lcms2engine/LcmsColorSpace.h`

`createBrightnessContrastAdjustment()` built its LittleCMS device link
tagged as `cmsSigLabData` and chained it between two copies of the
working ICC profile (`RGB -> "Lab" -> RGB`, via
`cmsCreateMultiprofileTransform`). Tagging the link as Lab makes
LittleCMS perform a *real* colorimetric conversion into and out of Lab,
which is subject to the profile's rendering intent and gamut mapping.
That round-trip could keep true black (0,0,0) and true white (255,255,255)
from mapping back to themselves exactly, which is what caused the
Brightness/Contrast filter to not reach full black/white even at extreme
slider values.

The filter's own curve math (in
`plugins/filters/colorsfilters/kis_brightness_contrast_filter.cpp`)
already clamps to a full 0.0–1.0 range and was never the problem — the
bug was purely in how the resulting curve got applied at the LittleCMS
level.

**Fix:** build the device link using the color space's own signature
(`this->colorSpaceSignature()`) and apply it with a single-profile
`cmsCreateTransform`, exactly the same pattern already used correctly a
few lines down in `createPerChannelAdjustment()` in the same file. No
Lab conversion, no rendering-intent-dependent round trip — the curve is
applied directly to the working color space's own channels, so its own
clamping is what determines the output range.

## 2026-09-02 — Play Store icon for "Next"/debug flavor still showed stock Krita paintbrush

**Commit:** `a1b59e8`

An earlier fix (`a4a5b53`) corrected the adaptive-icon foreground and all
mipmap-density launcher icons to the Krimble gear/K mark, but missed
`packaging/android/apk/ic_launcher_next-playstore.png` — the Play Store
listing image for the debug/"next" build flavor, which was still the
original Krita paintbrush + git-branch badge artwork. Replaced with the
same Krimble gear/K image used for the main flavor's Play Store icon.

## 2026-09-02 — Debug/"Next" flavor icon made visually distinct

**Commit:** `dae8061`

The debug ("Next") build flavor's launcher icon was identical to the
release flavor's icon (same gear/K artwork, no visual distinction),
making it hard to tell debug and release installs apart on-device.
Generated a distinct variant: the same Krimble gear/K mark desaturated
to a grayscale/steel tone, with an orange circular "N" badge overlaid in
the bottom-right corner. Applied across all mipmap densities
(`mdpi` through `xxxhdpi`), both square and round variants, and the
debug flavor's Play Store PNG.

## 2026-09-03 — Added real "Merge Visible" action (Photoshop parity, Ctrl+Shift+E)

**Commit:** `e1a7e5a`
**Files:** `libs/ui/kis_layer_manager.h`, `libs/ui/kis_layer_manager.cc`,
`krita/krita.action`, `krita/krita5.xmlgui`

Photoshop's Layer menu has a dedicated "Merge Visible" command
(Ctrl+Shift+E) between Merge Down and Flatten Image. Krita has always
had the equivalent capability, but only as a two-step manual sequence
(per the Krita manual): Layer ▸ Select ▸ Visible Layers, then Layer ▸
Merge with Layer Below. No single menu item triggered both steps.

Added a new `merge_visible_layers` action to `KisLayerManager` that
does exactly that sequence and nothing else — it calls
`KisNodeManager::selectVisibleNodes()` then the existing
`mergeLayer()` slot, which already handles merging a multi-node
selection via `KisImage::mergeMultipleLayers()`. No new merge logic
was written.

**Bonus fix:** while placing the new action, found that the Layer
menu's existing `flatten_layer` entry was mislabeled "Merge Visible"
in `krita5.xmlgui`, even though `flatten_layer` only flattens the
single active layer — it has nothing to do with visible-layer merging.
Relabeled that entry to its correct text, "Flatten Layer", and inserted
the new `merge_visible_layers` action in the correct Photoshop-parity
position (between Merge Down and Flatten Image).

**Shortcut:** Photoshop uses Ctrl+Shift+E for Merge Visible and has no
default shortcut for Flatten Image. Krita had Ctrl+Shift+E assigned to
`flatten_image`. Moved the shortcut from `flatten_image` to the new
`merge_visible_layers` action to match Photoshop's defaults.

## 2026-09-03 — Exposed transform submodes as separate Photoshop-parity menu items

**Commit:** `c7630d4`
**File:** `krita/krita5.xmlgui`

Photoshop splits transformation into distinct menu commands: Edit ▸
Free Transform, Edit ▸ Transform ▸ Perspective/Warp/etc., Edit ▸ Puppet
Warp, and Filter ▸ Liquify. Krita has a single unified Transform tool
that switches between these modes on the fly from its own tool options
— functionally equivalent, but it meant none of these had a dedicated,
discoverable menu entry, and switching modes inside one tool felt
inconsistent with how the rest of the app is organized.

Investigated the transform tool's implementation
(`plugins/tools/tool_transform2/`) and found Krita already ships
separate, fully-functional actions for each submode —
`KisToolTransformFree`, `KisToolTransformPerspective`,
`KisToolTransformWarp`, `KisToolTransformCage`,
`KisToolTransformLiquify`, `KisToolTransformMesh` — each wired to its
own `activateSubtoolXxx()` slot in `kis_tool_transform.cc`. They were
just never placed anywhere except the toolbox flyout and the
transform tool's own right-click context menu, so nobody could find
them from the menu bar.

Added menu entries for these existing actions, no new tool code:

- **Edit ▸ Puppet Warp** → `KisToolTransformCage` (Krita's cage
  transform is the closest existing equivalent to Photoshop's
  mesh-pin-based Puppet Warp)
- **Edit ▸ Free Transform** → `KisToolTransformFree`
- **Edit ▸ Transform ▸ Perspective** → `KisToolTransformPerspective`
- **Edit ▸ Transform ▸ Warp** → `KisToolTransformWarp`
- **Filter ▸ Liquify...** → `KisToolTransformLiquify`

All five inserted at their corresponding Photoshop menu positions.
Photoshop's Transform submenu also has Scale/Rotate/Skew/Distort/
Flip/Rotate 180°, but those are modifier-key interactions within
Krita's Free Transform mode rather than separate tool activations, so
they don't get their own menu entries — Free Transform already covers
that functionality once opened.

## 2026-09-03 — Filled out Edit > Transform to Photoshop's full list, matched exact wording

**Commits:** `4a49e0d`, `55de2a2` (`a7d3e31` fixed an XML comment syntax
mistake introduced by `55de2a2`)
**File:** `krita/krita5.xmlgui`

Added the remaining Photoshop Edit ▸ Transform items — Scale, Rotate,
Skew, Rotate 180°, Rotate 90° Clockwise, Rotate 90° Counter Clockwise,
Flip Horizontal, Flip Vertical — joining Perspective and Warp already
there. All ten map to existing, distinct, already-functional Krita
actions (`layersize`, `rotatelayer`, `shearlayer`, `rotateLayer180`,
`rotateLayerCW90`, `rotateLayerCCW90`, `mirrorNodeX`, `mirrorNodeY`,
plus the two `KisToolTransform` submode actions from the previous
entry) — no new code, just menu placement.

Verified none of the 8 underlying one-shot actions are restricted to a
specific layer type: no `setExcludedNodeTypes()` calls, and their
`activationFlags` cover layers, shape layers, transparency masks, and
both selection types. Also traced each one's actual C++ implementation
and confirmed they behave like Photoshop's Edit ▸ Transform: if a pixel
selection is active, the operation is constrained to
`selection->selectedExactRect()`; otherwise it falls back to the whole
active layer's bounds (`KisImage::rotateImpl`, `KisNodeManager::
mirrorNodes`, `ImageSize::slotLayerSize`, `ShearImage::slotShearLayer`
all confirmed).

"Distort" is deliberately left out: Krita has no separate constrained
distort-only mode, only unconstrained corner-drag inside Free
Transform, so a menu item here would just be a duplicate label for the
same interaction as Free Transform, not real distinct functionality.

Text matches Photoshop's exact wording (no ellipsis on Scale/Rotate/
Skew) even though those three open a numeric dialog in Krita rather
than Photoshop's live on-canvas drag — same command, different
interaction model, not worth a misleading label difference.

## 2026-09-03 — Reordered Layer menu so Merge Down/Merge Visible/Flatten Image are adjacent

**Commit:** `c41569f`
**File:** `krita/krita5.xmlgui`

Confirmed against real Photoshop Layer menu screenshots: Merge Down,
Merge Visible, and Flatten Image sit as an uninterrupted triplet with
nothing between them, and Photoshop has no "Flatten Layer" or "Merge
Shape Layers" concept at all.

Krimble had three Krita-specific extras (`merge_selected_layers`,
`flatten_layer`, `merge_all_shape_layers`) interspersed inside that
triplet, breaking the exact adjacency. Moved all three to their own
group directly after the Photoshop-parity triplet instead, so the
triplet itself matches Photoshop exactly and the Krita-only extras are
clearly set apart as bonus functionality.

## 2026-09-03 — Contrast slider couldn't reach pure black/white at maximum

**Commit:** `19062f3`
**File:** `plugins/filters/colorsfilters/kis_brightness_contrast_filter.cpp`

Bug report: pushing Contrast to maximum on a grayscale image should
produce a pure black-and-white result, and didn't.

This file is Krimble/"Krita Mobile"-authored, not inherited from
upstream Krita. Its contrast curve used `contrastFactor = 1.0 +
contrast`, a plain linear scale that caps out at a slope of 2.0 even
at contrast=+100. That only clips pixels already below 25% gray or
above 75% gray to full black/white — the middle 50% of the tonal range
could never reach pure black/white no matter how far the slider was
pushed. Verified numerically: at max contrast, a pixel at 45% gray
only reached 40% output, not 0%.

Replaced the positive-contrast side with Photoshop's legacy contrast
formula (`factor = 1 / (1 - contrast)`), whose slope diverges toward
infinity as contrast approaches its maximum — that divergence is what
actually produces the posterize-to-black/white look. Handled the true
maximum (contrast = +100) as an explicit hard threshold at the 50%
midpoint rather than relying on floating-point infinity, guaranteeing
exact 0.0/1.0 output. Negative contrast (flattening toward gray) is
unchanged. Confirmed the fix numerically against the old formula
before committing.

## 2026-09-03 — UI-scale-on-startup dialog silently missing (bug items 3/6)

**Commit:** `913eb00`
**Files:** `krita/main.cc`, `libs/ui/dialogs/kis_dlg_preferences.cc`

Bug report: a dialog with a percentage slider for setting the
interface (UI) scale used to appear on first launch in a previous
Krimble build, then disappeared entirely in a later build with no
error or explanation. Both that startup dialog and the Settings >
Interface Scale menu item were confirmed missing.

Traced the full chain: `KisApplication::start()` calls
`KisAndroidDonations::showDonationDialog(true)`, through JNI to
`MainActivity.showDonationDialogInternal()`; on dismiss that fires
`JNIWrappers.onSplashDialogDismissed()`, which comes back into C++ as
`KisAndroidScaling::slotSplashDialogDismissed()`, calling
`maybeShowDialog(true)`. That entire path was intact and unconditional
— not the actual problem.

Root cause was one level deeper: `KisAndroidScaling::isSupported()`
(which gates the Settings menu item's creation in `KisMainWindow.cpp`)
and `maybeShowDialog()`'s early-return both depend on
`isHighDpiScalingEnabled()`, which only ends up true if the
"EnableHiDPI" `kritadisplayrc` key is true. That key defaulted to
`false` in `main.cc`, so on any install without a pre-existing config,
the entire interface-scaling subsystem (menu item + startup dialog)
never got a valid primary screen to work with and silently never
appeared — no crash, no log, just absent.

Changed the default to `true` in both `main.cc` (actual startup
behavior) and `kis_dlg_preferences.cc` (so the Preferences checkbox
reflects the same default, rather than showing unchecked while the
feature is actually on). `androidScalingAskOnStartup` already defaulted
to true and wasn't part of the problem.

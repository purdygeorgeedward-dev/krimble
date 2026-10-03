# Krimble Changes

This file tracks fixes and changes made in this fork (Krimble) on top of
upstream Krita, specifically for the Android build. It is not exhaustive
history — it's a running log of notable bugs found and fixed, so future
work (by anyone, including an AI assistant with no memory of past
sessions) can see what's already been addressed and why, without having
to rediscover it from scratch.

## Standing rules for this fork

- **Design priority is mobile-first, then industry-standard parity
  second.** Mobile UI/UX constraints and conventions take precedence
  over matching desktop industry-standard tools exactly, where the two
  conflict (see e.g. the transform-mode-as-separate-tool decision,
  which departs from the industry-standard single-tool-with-modifier-
  keys model because modifier-key combinations don't translate to
  touch input).
- **Never write the name of the proprietary industry-standard tool
  this fork targets parity with, anywhere public-facing** — code
  comments, this file, commit messages, or any other note. Use the
  phrase "industry standard" instead. This has been stated more than
  once because it did not reliably get preserved as a written rule
  before; writing it here is the fix for that.
- **Never delete menu/action XML entries.** Comment them out (`<!-- -->`)
  instead. A deletion buried in an unrelated commit is silent and
  unrecoverable without git archaeology (see the Close All incident
  below); a comment is visible in the file itself and easy to restore.
- **Industry-standard feature parity — same features, same menu
  locations — is the primary UI goal** for this fork, subordinate to
  mobile-first design per above.
- **Verify actual behavior before cataloging an industry-standard
  feature as missing or existing.** Some features exist via a different
  mechanism or multi-step sequence in Krita (e.g. Merge Visible =
  Select Visible Layers + Merge with Layer Below) rather than a single
  equivalently-named action.
- **Ask before making codebase changes.** Investigating is fine
  unprompted, and so is writing/updating documentation. Only actual
  code/config edits, commits, and pushes need explicit go-ahead first.
- **Document every change twice**: inline code comments explaining the
  change, AND an entry in this file. Every time, not just sometimes.

## Open bug list

Reported as a numbered list; numbers are kept stable across sessions so
they can be referenced directly. Descriptions below are the *corrected*
versions after clarification — several were initially misread, and the
wrong initial readings are noted so they aren't repeated.

1. ~~Krita icon instead of Krimble icon~~ — **Fixed**, commit
   `a4a5b53`. Adaptive-icon foreground was the stock Krita paintbrush;
   replaced with Krimble's own icon.
2. ~~Brush presets open on load~~ — **Likely fixed**, commit `a73387e`,
   pending your on-device confirmation. See the 2026-09-04 changelog
   entry above.
3. **UI-scale-on-startup dialog missing.** A dialog with a percentage
   slider for setting the *interface* (UI) scale — not canvas zoom —
   used to appear on first launch in a previous Krimble build, then
   disappeared with no error or explanation. **Fixed** — see the
   2026-09-03 "UI-scale-on-startup dialog silently missing" entry
   below. (Initially misread as being about a canvas zoom-fit prompt;
   it is not.)
4. **Brightness/Contrast doesn't reach full black/white.** Specifically:
   pushing *Contrast* to maximum on a grayscale image should threshold
   it to pure black-and-white, and didn't. **Fixed** — see the
   2026-09-03 "Contrast slider couldn't reach pure black/white at
   maximum" entry below. Note this is a *different* bug from the
   earlier LcmsColorSpace.h Lab-round-trip fix (`b1eb52e`) — that one
   fixed a color-space clipping issue; this one fixed the contrast
   curve's math not being steep enough at max value. Both were real,
   separate bugs in the same feature.
5. **Resize handles hard to grab, including on the Transform tool.**
   A previous attempt was made to enlarge the hit-target size of
   resize handles (windows and Transform tool) and had no effect.
   Not yet investigated — the prior attempt itself needs to be found
   (git history search for handle/resize/grab-related commits turned
   up nothing in the visible history) and understood before trying
   again.
6. **No way to set UI zoom scale.** Same root cause as #3 above — this
   is not about canvas zoom. **Fixed** together with #3.
7. ~~Window snap is broken~~ — **Likely fixed**, commit `c50f03a`,
   pending your on-device confirmation. See the 2026-09-04 changelog
   entry above. (Initially misread as a "docked layout doesn't persist
   across sessions" issue; it is not — it's about the snapping
   behavior itself failing.)
8. ~~Transform tool does nothing~~ — **Fixed**, commit `05d1fc1`.
   `activateSubtool()` was ignoring the requested mode entirely; each
   Edit/Filter Transform menu item now correctly activates the mode
   it's labeled with. Also removed Free Transform's ability to
   silently blend into Perspective or Skew, per later direction that
   each mode should be a fully separate tool.
9. **Massive rendering delay during brush painting.** Investigated —
   ruled out the most likely causes: OpenGL is enabled by default
   (`useOpenGL()` defaults to "auto", not disabled), and there are
   zero Krimble-authored changes anywhere in the core paint/brush/image
   engine (`libs/image`, `libs/brush`, `plugins/paintops`). Unlike other
   fixes this session, this doesn't appear to be a Krimble-introduced
   regression sitting in a findable spot — likely needs on-device
   profiling/reproduction rather than further static code reading.
10. **Type tool does nothing.** Investigated — no confident root cause
    found. Checked `nodeEditable()` (generic node-lock check, not
    text-specific) and `SvgCreateTextStrategy.cpp` (no early-return
    guards, no Krimble markers). Substantial mobile-specific rework
    already exists in this tool (draggable selection handles,
    triple-tap-to-select-paragraph, floating action bar), which makes
    a simple obvious bug less likely. Needs on-device reproduction to
    diagnose further — ties to the in-progress "Type tool rework part
    1" commit mentioned previously, not reachable in this shallow
    clone's history to verify further.
11. **Cartoon mascot in support screen.** User is addressing this one
    themselves — not part of Claude's task list.
12. ~~Tool docker panels render as unmanageable narrow column shapes~~
    — **Two separate contributing bugs found and fixed**, commits
    `c50f03a` and `af46ff2`. (Described as "vertical stripes"
    initially, corrected to "unmanageable column shapes/proportions".)
    `c50f03a` (2026-09-04) fixed a global docking-mechanism bug in
    `KisMainWindow.cpp` — a self-contradicting `setAllowedAreas()` call
    was breaking dock/snap layout for every panel. `af46ff2`
    (2026-09-10) separately fixed 4 dockers (Advanced Color Selector,
    History, Histogram, SVG Symbol Collection) that all declared
    `defaultDockPosition() = DockRight`, causing them to pile into the
    same narrow column on fresh installs. Both plausibly contributed to
    the same visible symptom; pending your on-device confirmation that
    panels now render correctly with both fixes applied.
13. ~~File dialogs open at roughly half the size they should be by
    default~~ — **Fixed**, commit `4016d2b`. See the 2026-09-04
    changelog entry above.
14. ~~Dialog windows don't remember their size/position after being
    manually resized~~ — **Fixed**, commit `c27416b`. See the
    2026-09-04 changelog entry above.
15. ~~Convert to Profile is not available from the Edit menu~~ —
    **Fixed**, commit `623f3c5`. Placed the existing
    `imagecolorspaceconversion` action in Edit as well.
16. ~~Keyboard shortcut customization is not available from the Edit
    menu~~ — **Fixed**, commit `623f3c5`. Placed the existing
    `options_configure_keybinding` action, which had no menu placement
    anywhere before this.
17. ~~Toolbar customization is not available from the Edit menu~~ —
    **Fixed**, commit `623f3c5`. Placed the existing
    `options_configure_toolbars` action in Edit as well (it was
    already in Settings).
18. **Assign Profile (reinterpret pixels under a different color
    profile without converting them) does not exist anywhere in the
    app.** Not yet investigated whether Krita's color-management code
    has an underlying "assign without convert" operation to wrap.
19. ~~Purge (manually free cached memory...) does not exist as a menu
    command~~ — **Not actually missing.** `purge_unused_image_data`
    already exists and is already placed in the Image menu (matches
    what's visible in the reference Krimble screenshots). No work
    needed; this was a cataloging error, not a real gap.
20. ~~Step Forward / Step Backward... do not exist~~ — **Not actually
    missing.** These exist in the industry standard only because its
    plain Undo (Ctrl+Z) is a toggle (press again to redo the same
    thing back), a legacy quirk Krita doesn't have. Confirmed in
    KisMainWindow::undo() — it's an unconditional, repeatable stack
    walk, not a toggle. Krita's existing Undo/Redo already provide
    exactly this functionality. Building separate actions would just
    duplicate Undo/Redo.

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

## Missing industry-standard-parity features (not just menu entries — the underlying feature doesn't exist)

Found while restoring the toolbar deletions above: two groups of actions
were referenced in the pre-`718e83878` menu but no longer exist
*anywhere* in the codebase — not in any `.action` file, not registered
via `createAction()` in any `.cpp`. These aren't menu-restoration jobs;
the feature itself needs to be built:

- **File > Print / Print One Copy** (`file_print`, `file_print_preview`)
  — the industry standard has both under File. No print implementation currently
  exists in this codebase at all.
- **Edit > Find and Replace** (`edit_find`, `edit_find_next`,
  `edit_find_prev`, `edit_replace`) — the industry standard has Find and Replace Text
  under Edit. No find/replace implementation currently exists in this
  codebase at all (this would apply to text layers/the text tool,
  Krita's closest equivalent to the industry standard's text-focused find/replace).

Left commented out in `krita5.xmlgui` at their original menu locations so
the intended placement isn't lost, pending a full menu-by-menu industry-standard
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

## 2026-09-03 — Added real "Merge Visible" action (industry-standard parity, Ctrl+Shift+E)

**Commit:** `e1a7e5a`
**Files:** `libs/ui/kis_layer_manager.h`, `libs/ui/kis_layer_manager.cc`,
`krita/krita.action`, `krita/krita5.xmlgui`

The industry standard's Layer menu has a dedicated "Merge Visible" command
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
the new `merge_visible_layers` action in the correct industry-standard-parity
position (between Merge Down and Flatten Image).

**Shortcut:** the industry standard uses Ctrl+Shift+E for Merge Visible and has no
default shortcut for Flatten Image. Krita had Ctrl+Shift+E assigned to
`flatten_image`. Moved the shortcut from `flatten_image` to the new
`merge_visible_layers` action to match the industry standard's defaults.

## 2026-09-03 — Exposed transform submodes as separate industry-standard-parity menu items

**Commit:** `c7630d4`
**File:** `krita/krita5.xmlgui`

The industry standard splits transformation into distinct menu commands: Edit ▸
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
  transform is the closest existing equivalent to the industry standard's
  mesh-pin-based Puppet Warp)
- **Edit ▸ Free Transform** → `KisToolTransformFree`
- **Edit ▸ Transform ▸ Perspective** → `KisToolTransformPerspective`
- **Edit ▸ Transform ▸ Warp** → `KisToolTransformWarp`
- **Filter ▸ Liquify...** → `KisToolTransformLiquify`

All five inserted at their corresponding industry-standard menu positions.
The industry standard's Transform submenu also has Scale/Rotate/Skew/Distort/
Flip/Rotate 180°, but those are modifier-key interactions within
Krita's Free Transform mode rather than separate tool activations, so
they don't get their own menu entries — Free Transform already covers
that functionality once opened.

## 2026-09-03 — Filled out Edit > Transform to the industry standard's full list, matched exact wording

**Commits:** `4a49e0d`, `55de2a2` (`a7d3e31` fixed an XML comment syntax
mistake introduced by `55de2a2`)
**File:** `krita/krita5.xmlgui`

Added the remaining industry-standard Edit ▸ Transform items — Scale, Rotate,
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
and confirmed they behave like the industry standard's Edit ▸ Transform: if a pixel
selection is active, the operation is constrained to
`selection->selectedExactRect()`; otherwise it falls back to the whole
active layer's bounds (`KisImage::rotateImpl`, `KisNodeManager::
mirrorNodes`, `ImageSize::slotLayerSize`, `ShearImage::slotShearLayer`
all confirmed).

"Distort" is deliberately left out: Krita has no separate constrained
distort-only mode, only unconstrained corner-drag inside Free
Transform, so a menu item here would just be a duplicate label for the
same interaction as Free Transform, not real distinct functionality.

Text matches the industry standard's exact wording (no ellipsis on Scale/Rotate/
Skew) even though those three open a numeric dialog in Krita rather
than the industry standard's live on-canvas drag — same command, different
interaction model, not worth a misleading label difference.

## 2026-09-03 — Reordered Layer menu so Merge Down/Merge Visible/Flatten Image are adjacent

**Commit:** `c41569f`
**File:** `krita/krita5.xmlgui`

Confirmed against real industry-standard Layer menu screenshots: Merge Down,
Merge Visible, and Flatten Image sit as an uninterrupted triplet with
nothing between them, and the industry standard has no "Flatten Layer" or "Merge
Shape Layers" concept at all.

Krimble had three Krita-specific extras (`merge_selected_layers`,
`flatten_layer`, `merge_all_shape_layers`) interspersed inside that
triplet, breaking the exact adjacency. Moved all three to their own
group directly after the industry-standard-parity triplet instead, so the
triplet itself matches the industry standard exactly and the Krita-only extras are
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

Replaced the positive-contrast side with the industry standard's legacy contrast
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

## 2026-09-03 — Assigned industry-standard-parity shortcuts to Transform submodes

**Commit:** `27404fb`
**File:** `plugins/tools/tool_transform2/KisToolTransform.action`

Cross-referenced the industry-standard shortcuts reference: Ctrl+T is
always Free Transform there, and Liquify has its own dedicated
shortcut (Ctrl+Shift+X), but Perspective, Warp, and Puppet Warp have
no dedicated shortcut at all — reached only via menu (or a
modifier-drag inside Free Transform, which this fork deliberately
doesn't replicate). Mesh has no equivalent in the reference at all.

Moved Ctrl+T from the generic `KisToolTransform` action (activates the
Transform tool in whatever mode it last used) to
`KisToolTransformFree` specifically, so it matches the reference's
"Ctrl+T always means Free Transform" behavior. Assigned Ctrl+Shift+X
to `KisToolTransformLiquify`. Left Perspective, Warp, Cage, and Mesh
with no shortcut, matching the reference. Checked for conflicts with
existing shortcuts before assigning; none found.

## 2026-09-03 — Added Convert to Profile, Configure Shortcuts, Customize Toolbar to Edit menu

**Commit:** `623f3c5`
**File:** `krita/krita5.xmlgui`

Bug items 15–17. All three reuse existing, already-registered,
already-functional actions — no new code. `imagecolorspaceconversion`
already worked in the Image and Layer menus; `options_configure_toolbars`
already worked in Settings. `options_configure_keybinding` had no menu
placement anywhere before this. Placed together directly before
Preferences, matching the industry standard's grouping of these
app-config items at the end of the Edit menu.

## 2026-09-04 — Implemented Vibrance filter (real algorithm, not a wrapper)

**Commit:** `375a647`
**New files:** `plugins/filters/colorsfilters/kis_vibrance_filter.h/.cpp`, `wdg_vibrance.ui`
**Modified:** `colorsfilters.cpp`, `CMakeLists.txt` (registration only)

No equivalent existed in Krita. The industry standard's own Vibrance
formula is proprietary and undocumented (confirmed via research). Uses
the well-known open-source "vibrance" algorithm by CeeJay.dk instead —
MIT licensed, distributed via SweetFX/ReShade/GShade for over a decade.
A different, simpler implementation of the same idea, not a
byte-for-byte match to any commercial product.

Per pixel: luma via Rec. 709 weights, saturation as
`max(R,G,B) - min(R,G,B)`, blend each channel toward/away from luma by
`1 + vibrance*(1-saturation)` — strongest effect on muted pixels,
fading out as saturation approaches 1. Verified numerically before
implementation.

Implemented as a plain `KisFilter` with direct pixel iteration
(`KisSequentialIteratorProgress`) and `KoColorSpace::toRgbA16`/
`fromRgbA16` — the same conversion functions the existing Match Color
filter uses for Lab — rather than the heavier templated
`KoColorTransformation` pattern HSV Adjustment uses. No changes needed
to `plugins/color/colorspaceextensions/`, `krita5.xmlgui`, or any
`.action` file — self-registers into the existing Adjustments category.

## 2026-09-04 — Fixed docker snap/rendering (setAllowedAreas self-contradiction)

**Commit:** `c50f03a`
**File:** `libs/ui/KisMainWindow.cpp`

Bug items 7 and 12. Every dock panel had
`setAllowedAreas(Qt::NoDockWidgetArea)` called on it, then
`addDockWidget(side, dockWidget)` immediately placed that same widget
into `side` — a direct self-contradiction fed to Qt's dock-layout
engine (the widget was just told it's allowed in zero areas, then
force-placed into one anyway).

Strong candidate for both: window snap being broken (a dragged panel
declaring zero allowed areas can't be dropped back into any dock zone),
and docker panels rendering as unmanageable narrow columns (plausible
symptom of Qt's internal dock-layout width allocation misbehaving for
a widget that structurally shouldn't be docked anywhere, yet is).

The original intent (per the replaced comment) was to stop a dragged
panel from auto-snapping back into a dock zone — reasonable goal,
wrong API, broke docking far more broadly than intended. Commented
out rather than deleted. `QDockWidget` defaults to
`Qt::AllDockWidgetAreas` when `setAllowedAreas()` is never called,
restoring normal dock/undock/snap behavior.

## 2026-09-04 — Replaced the actual About KDE dialog artwork

**Commit:** `addd1f4` (uploaded directly, not via this session's usual commit flow)
**File:** `libs/widgetutils/xmlgui/aboutkde.png` (150 × 250)

Earlier this session, Claude mistakenly identified and replaced
`krita/pics/Breeze-light/light_kde.svg` /
`krita/pics/Breeze-dark/dark_kde.svg` — the small icon shown next to
"About KDE" in the Help menu — believing that was the graphic in
question. It was not. The actual content shown inside the About KDE
dialog itself comes from a completely separate file,
`libs/widgetutils/xmlgui/aboutkde.png`, sitting alongside
`kaboutkdedialog_p.cpp`/`.h`. That file already contained genuine
Konqi (KDE's real dragon mascot) artwork before any of this session's
changes — confirmed directly by viewing it.

User corrected both mistakes directly: reverted `light_kde.svg` /
`dark_kde.svg` back to the original stock vector icon (undoing
Claude's incorrect edit), and replaced `aboutkde.png` with new dragon
artwork. Both changes are the current, correct state.

## 2026-09-04 — Simplified Default workspace to ToolBox, Tool Options, Layers only

**Commit:** `a73387e`
**File:** `krita/data/workspaces/Default.kws`

Bug item 2 (brush presets opening unexpectedly on load). This file's
docker layout is a binary Qt `QMainWindow::saveState()` blob, not
human-readable per-docker flags — confirmed by decoding it, finding
all ~90 known docker object names (`ToolBox`, `sharedtooldocker`/Tool
Options, `KisLayerBox`/Layers, `PresetDocker`/brush presets, and
dozens more) embedded as UTF-16BE strings.

Replaced the `<state>` CDATA block only (nothing else in the file
touched) with a freshly-generated blob built via Qt's own `saveState()`
API: every known docker object name recreated, only ToolBox (left)
and Tool Options + Layers (right, stacked vertically) set visible,
everything else explicitly hidden.

Verified via round-trip test before writing: restored the generated
blob into a completely separate, freshly-built `QMainWindow` with the
same dockers and confirmed exactly `{ToolBox, sharedtooldocker,
KisLayerBox}` come back visible and nothing else — `restoreState()`
reported success. An earlier attempt at this same generation failed
silently (`restoreState()` returned false, zero dockers visible) until
`.show()` was called on the `QMainWindow` before
`saveState()`/`restoreState()`.

Per project direction: workspaces are meant to be customized by the
user afterward, so the default should be minimal — not because Krimble
avoids brush-related features.

## 2026-09-04 — Fixed file dialog default size and size persistence

**Commits:** `4016d2b` (item 13), `c27416b` (item 14)
**File:** `libs/widgetutils/KoFileDialog.cpp`

**Item 13:** the `QInputDialog` used on Android to pick a file format
before the native save picker opens had no explicit size at all, so it
fell back to Qt's tiny default (built for a one-line text prompt, not
a scrollable format list on a touchscreen). Sized to 85%/60% of the
available screen instead of a fixed pixel value, so it's reasonable
across different device sizes. There was already a comment
acknowledging the list was hard to scroll on touch, partially
addressed (`UseListViewForComboBoxItems`), but the overall dialog size
was never fixed.

**Item 14:** `KoFileDialog` (Save As, Open, Import, Export) never
remembered a size the user manually resized it to — every reopen reset
to the default. Reused the exact same `KConfigGroup` ("File Dialogs")
+ `dialogName` pattern already used by `getUsedDir()`/`saveUsedDir()`
for remembering the last-used directory. Geometry restored once in
`createFileDialog()` before the dialog is shown; saved via the
`finished()` signal so it's captured regardless of which `exec()` call
site is used, and regardless of accept/cancel.

## 2026-09-06 — Fixed crop tool false-undo on short strokes

**Files:** `plugins/tools/tool_crop/kis_tool_crop.h`,
`plugins/tools/tool_crop/kis_tool_crop.cc`

Any short stroke or incidental tap with the crop tool active could
silently undo the user's last operation and replace the crop
selection with an old one. Root cause: `endPrimaryAction()`'s
`haveValidRect` check ("was a real crop rectangle drawn, or just a
tap?") reused `m_handleSize` as its threshold. `m_handleSize` was
previously bumped from 13 to 44 (see item 4 above) to make resize
handles reliably grabbable on touch -- correct for that purpose, but
it silently raised the "is there a rect" threshold to 44px too. Any
stroke shorter than that in either dimension then fell through to
`tryContinueLastCropAction()`, which -- if the last undo-stack command
was a previous crop -- calls `undoLastCommand()` to reopen it. On
touch, that fallback fired on essentially any incidental short touch.

Fix: added a separate `m_minimumCropSize` (13, the original mouse-era
value) used only for the `haveValidRect` check, decoupled from
`m_handleSize`'s touch-target sizing. Handles stay easy to grab; short
strokes no longer trigger an unintended undo.

## 2026-09-06 — Shifted default theme from neutral gray to blue-gray

**File:** `krita/data/themes/KritaDark.colors`

Replaced the neutral gray UI base (`71,71,71` on Window/Complementary,
and matching values on Button/Tooltip/View/WM) with a blue-gray
sampled directly from a reference color (#373952 / RGB 55,57,82,
"Lavender Blue") using a color-picker tool. Every background value in
the theme was shifted by the same delta (dR -16, dG -14, dB +11) from
its old neutral-gray value, so the existing lightness relationships
between sections (Button vs. Window vs. Tooltip, etc.) are preserved
-- only the hue changes. Selection/accent blue (83,114,142) and all
foreground/text colors are unchanged.

Also fixed a pre-existing mismatch: `KisApplication.cpp` and
`KisMainWindow.cpp` default to theme name "Krimble dark", but this
file's [General] Name/ColorScheme were still "Krita dark", so the
lookup likely never matched on a fresh install. Renamed both to
"Krimble dark" to match.

## 2026-09-06 — Padded crop handle hit-test to reduce near-miss resets

**Files:** `plugins/tools/tool_crop/kis_tool_crop.h`,
`plugins/tools/tool_crop/kis_tool_crop.cc`

A tap that missed a crop handle's 44px hit box even slightly -- hard
to avoid on a small touchscreen -- fell through to
`mouseOnHandleType == None`, which `beginPrimaryAction()` treats as
"discard the existing crop selection and start a brand new one at
this point." Any near-miss looked like the tool randomly restarting.

Fix: added `m_handleHitPadding` (16px), applied only inside
`mouseOnHandle()`'s hit-test via `.adjusted(-pad,-pad,pad,pad)` on
each handle rect -- giving each handle an effective ~76px grab zone
while leaving the drawn/visible handle size at 44px, matching the same
decoupled-threshold pattern as `m_minimumCropSize` above.

## 2026-09-06 — Temporarily disabled Type tool (bug #9, random self-activation)

**File:** `plugins/tools/svgtexttool/Plugin.cpp`

Type tool was randomly self-activating in the toolbox, disrupting
other work in progress. Root cause not yet confirmed -- ruled out
KoToolManager::attachCanvas()'s lowest-priority auto-select (that path
only scans ToolBoxSection::Main, and the Type tool is registered under
ToolBoxSection::PSOrder alongside Brush, so it isn't a candidate
there). Some other auto-switch path is the likely culprit.

Commented out the one line in Plugin.cpp that registers
SvgTextToolFactory with KoToolRegistry. The plugin still builds and
loads; the tool simply never enters the registry, so it can't appear
in the toolbox or be activated by anything. Fully reversible --
uncomment that line once the actual root cause is found and fixed.

## 2026-09-06 — Added dedicated Smudge tool to the toolbox

**Files:**
- `krita/pics/tools/SVG/16/light_krita_tool_smudge.svg` (new)
- `krita/pics/tools/SVG/16/dark_krita_tool_smudge.svg` (new)
- `krita/pics/tools/SVG/16/tools-svg-16-icons.qrc`
- `plugins/tools/basictools/kis_tool_smudge.h` (new)
- `plugins/tools/basictools/kis_tool_smudge.cc` (new)
- `plugins/tools/basictools/default_tools.cc`
- `plugins/tools/basictools/CMakeLists.txt`

Krita's colorsmudge paintop engine (plugins/paintops/colorsmudge/)
already existed and is more capable than Photoshop's Smudge tool, but
was only reachable by manually finding a smudge preset in the brush
picker while using the generic Freehand tool -- no dedicated toolbox
entry.

Added KisToolSmudge (subclasses KisToolBrush, reusing its option
widget) and KisToolSmudgeFactory, registered in default_tools.cc
alongside Brush. On activate(), it looks up the bundled "smudge"
preset via KisResourceModel(ResourceType::PaintOpPresets) and applies
it through the canvas's KisCanvasResourceProvider, so the tool
smudges immediately with no manual preset selection. Own icon (16px
SVG, dark/light), own toolbox slot (priority 21, directly after Brush
at 20), own shortcut (R, previously unused).

## 2026-09-06 — Added FG/BG color swap widget to bottom of toolbox

**Files:** `libs/ui/toolbox/KoToolBoxDocker_p.h`,
`libs/ui/toolbox/KoToolBoxDocker.cpp`

Photoshop's toolbox has the foreground/background color swatches (with
swap and reset-to-black/white controls) built into the bottom of the
tool column itself. Krimble already has this widget (KoDualColorButton,
swap/reset built in) but only in the classic toolbar
(kis_control_frame.cpp).

Wrapped the toolbox's existing tool-button scroll area in a container
widget (QVBoxLayout) so a second KoDualColorButton instance could be
appended below it, matching Photoshop's layout. Construction/wiring
deferred to setViewManager() since that's the first point a real
KisViewManager (and canvasResourceProvider) is available -- mirrors
the classic toolbar's connection pattern exactly (FG/BG signals both
directions, display renderer updates via a new slotUpdateDisplayRenderer,
color-space-change reconnection).

Kept in both locations (classic toolbar + toolbox) per project
decision -- this is an addition, not a relocation.

## 2026-09-06 — Wired up Android holiday splash screen (Dec 1-26)

**Files:** `libs/ui/kis_splash_screen.cpp`, `krita/data/splash/splash-android.qrc`,
`krita/data/splash/logo_splash_holidays.png`

Krita's holiday splash feature was a real, maintained feature 2015-2018,
then explicitly disabled upstream in 2021 ("Dummy out the holidays splash
for now") and never re-added -- confirmed via upstream commit history.
It was never implemented for Android at all, even when active on desktop
-- `kis_splash_screen.cpp`'s Android branch went straight to `hd.jpg`
unconditionally, no date check ever existed there.

Replaced the placeholder `logo_splash_holidays.png` with new artwork,
registered it in `splash-android.qrc` (was previously missing from the
Android build entirely -- only in the desktop-only `splash.qrc`), and
added a date check to the Android branch of `getImageSource()`: active
Dec 1 through Dec 26 inclusive, switching to `:/splash/holiday.png`.
Desktop's holiday splash remains disabled (upstream's `#if 0` block,
untouched) -- this only covers Android, Krimble's actual target.

## 2026-09-06 — Added Screen Mode toggle to bottom of toolbox

**File:** `libs/ui/toolbox/KoToolBoxDocker.cpp`

Third of Photoshop's three bottom-of-toolbox items (FG/BG swatches,
Quick Mask, Screen Mode) -- this one didn't need a new feature, just a
toolbox entry point. Reused the existing `view_show_canvas_only` action
(already checkable, already bound to Tab) via a QToolButton with
setDefaultAction(), added below the color swap widget in the same
container layout.

Icon overridden locally to `view-fullscreen` (the action's own icon,
`document-new`, is a placeholder) -- kritamenu.action itself untouched.

Quick Mask remains the one genuinely unbuilt item of the three (logged
separately in KRIMBLE_ROADMAP.md).

## 2026-09-07 — Added dedicated Soften tool to the toolbox

**Files:**
- `krita/pics/tools/SVG/16/light_krita_tool_soften.svg` (new)
- `krita/pics/tools/SVG/16/dark_krita_tool_soften.svg` (new)
- `krita/pics/tools/SVG/16/tools-svg-16-icons.qrc`
- `plugins/tools/basictools/kis_tool_soften.h` (new)
- `plugins/tools/basictools/kis_tool_soften.cc` (new)
- `plugins/tools/basictools/default_tools.cc`
- `plugins/tools/basictools/CMakeLists.txt`

Second of the two roadmap tools (KRIMBLE_ROADMAP.md), same pattern as
Smudge: Krita's filterop paintop engine (plugins/paintops/filterop/)
already lets you paint with any filter applied per-stroke, and already
ships a Gaussian-Blur-configured preset (plugins/paintops/defaultpresets/filter.kpp,
internal resource name "DFP") -- but with no dedicated toolbox entry,
matching Photoshop's Blur tool.

Added KisToolSoften (subclasses KisToolBrush) and KisToolSoftenFactory,
registered in default_tools.cc directly after Smudge. On activate(),
looks up the "DFP" preset via KisResourceModel(ResourceType::PaintOpPresets)
and applies it through KisCanvasResourceProvider -- identical mechanism
to KisToolSmudge, just a different preset name. Own icon (water-droplet
shape, Photoshop's Blur tool convention), toolbox priority 22 (directly
after Smudge at 21), shortcut U (previously unused).

## 2026-09-07 — Added touch-friendly edge/corner resize to all KoDialog dialogs

**Files:** `libs/widgets/KoDialog.h`, `libs/widgets/KoDialog_p.h`,
`libs/widgets/KoDialog.cpp`

Krita's dialogs had no custom resize handling at all -- no QSizeGrip,
no margin constant anywhere -- relying entirely on whatever the
platform provides natively for frameless windows, which isn't
something reliably touch-friendly (or possibly not present at all) on
Android, unlike desktop window managers.

Added a generous (20px) invisible resize margin along every edge of
every KoDialog-derived dialog, implemented via mousePressEvent/
mouseMoveEvent/mouseReleaseEvent overrides on the shared KoDialog base
class -- every dialog in the app gets this for free, no per-dialog
changes needed. Supports all 8 drag directions (4 edges + 4 corners),
respects the dialog's existing minimumSize()/maximumSize(), and adds
desktop cursor-hover feedback (harmless no-op on touch input) via
setMouseTracking(true).

Same decoupled-margin-from-visible-size pattern used for the crop
tool's handle hit-test padding and m_minimumCropSize earlier this
session.

## 2026-09-07 — Added dedicated Dodge and Burn tools to the toolbox

**Files:**
- `krita/pics/tools/SVG/16/{light,dark}_krita_tool_{dodge,burn}.svg` (4 new)
- `krita/pics/tools/SVG/16/tools-svg-16-icons.qrc`
- `plugins/tools/basictools/kis_tool_{dodge,burn}.{h,cc}` (4 new)
- `plugins/tools/basictools/default_tools.cc`
- `plugins/tools/basictools/CMakeLists.txt`

A third Smudge/Soften-pattern win, found after initially (incorrectly)
concluding Dodge/Burn had no existing engine to reuse. Correction: Krita's
core compositing engine already has fully-working "dodge" and "burn"
composite ops (COMPOSITE_DODGE/COMPOSITE_BURN in KoCompositeOpRegistry.h)
usable by any brush -- no bundled preset needed, unlike Smudge/Soften.

Different mechanism from Smudge/Soften since there's no preset to look
up by name: on activate(), both tools clone the standard default brush
preset ("defaultPreset", plugins/paintops/defaultpresets/paintbrush.kpp)
via KisPaintOpPreset::clone() and override CompositeOp via
KisPaintOpSettings::setProperty() before applying. Cloning is required --
modifying the shared cached preset in place would corrupt the default
brush everywhere else it's used.

Dodge: icon (hollow circle, PS convention), shortcut O (matches PS's
actual Dodge shortcut), priority 23. Burn: icon (filled circle), shortcut
K (arbitrary -- PS groups Burn with Dodge in one flyout key, Krimble
gives it a separate toolbox slot instead), priority 24.

## 2026-09-07 — Fixed Dodge/Burn to build up continuously while dragging

**Files:** `plugins/tools/basictools/kis_tool_dodge.cc`,
`plugins/tools/basictools/kis_tool_burn.cc`

The cloned base preset defaults to WASH mode (PaintOpAction=2), which
applies a stroke's opacity once, flattened at release -- not while
dragging back and forth over the same spot. Real Dodge/Burn darken or
lighten continuously the longer you paint over an area within a single
stroke, which requires BUILDUP mode (PaintOpAction=1).

Both tools now set PaintOpAction=1 alongside their CompositeOp override.

## 2026-09-10 — Fixed cramped/unusable docker panels on mobile (bug item 12)

**Files:** `plugins/dockers/advancedcolorselector/colorselectorng.cpp`,
`plugins/dockers/historydocker/History.cpp`,
`plugins/dockers/histogram/histogramdocker.cpp`,
`plugins/dockers/svgcollectiondocker/SvgSymbolCollectionDocker.h`

Four secondary dockers (Advanced Color Selector, History, Histogram, SVG
Symbol Collection) all declared `defaultDockPosition() = DockRight`,
meaning on any fresh install or Reset Configuration they all fought to
auto-dock into the same narrow right-side column simultaneously --
producing the unusable vertical-stripe panels reported in bug item 12.

Changed all four to `DockMinimized`: they still exist and can be opened
manually, but no longer auto-expand into the right column on load.
Complements the earlier Default workspace simplification (`a73387e`),
which controls saved-layout state -- this fixes the class-level fallback
used when no saved layout applies.

## 2026-09-16 — Fixed build break in KoToolBoxDocker (missing
kactioncollection.h)

**Files:** `libs/ui/toolbox/KoToolBoxDocker.cpp`

The "Canvas Only" toolbox button added for the Photoshop-parity toolbox
layout calls `viewManager->actionCollection()->action(...)`, but
`KisKActionCollection` is only forward-declared by the headers this
file already includes (e.g. `KoToolManager.h`) -- never given a full
definition. Compiler rejected the `->action(...)` call with "member
access into incomplete type 'KisKActionCollection'".

Added `#include <kactioncollection.h>`, matching the pattern every
other file calling `actionCollection()->action(...)` already follows
(`KisMainWindow.cpp`, `KisViewManager.cpp`, `kis_node_manager.cpp`).

## 2026-09-15 — Fixed build break in KoFileDialog geometry-persistence
connect() (bug item 14 follow-up)

**Files:** `libs/widgetutils/KoFileDialog.cpp`

The `connect(d->fileDialog, &QDialog::finished, ...)` call added for bug
item 14 (dialog geometry persistence) passed `d->fileDialog` -- a
`QScopedPointer<KisPreviewFileDialog>` -- directly as the sender, which
doesn't convert to the `QObject*` `connect()` needs. Compiler rejected
every overload with "no known conversion from
'QScopedPointer<KisPreviewFileDialog>' to ... 'const QDialog *'".

Changed to `d->fileDialog.get()`, matching the `.get()` pattern already
used for the other `connect()` calls in `createFileDialog()`.

**Follow-up (same commit-day):** the line above the `connect()`,
`QPointer<QFileDialog> dialogPtr = d->fileDialog;`, had the identical
bug and was missed on the first pass -- same `QScopedPointer` ->
`QObject*`-family conversion failure, this time against `QPointer`'s
constructor. Changed to `d->fileDialog.get()` as well.

## 2026-09-13 — Renamed Palettize to Indexed Color, moved to Image >
Mode, added Grayscale and Black and White palettes

**Files:**
- `plugins/filters/palettize/palettize.cpp`
- `plugins/filters/palettize/palettize.action`
- `libs/ui/kis_filter_manager.cc`
- `krita/krita5.xmlgui`
- `krita/data/palettes/grayscale.gpl` (new)
- `krita/data/palettes/black-and-white.gpl` (new)
- `krita/data/palettes/CMakeLists.txt`

Renamed the "Palettize" filter to "Indexed Color" (i18n string in the
filter constructor, plus matching strings in the .action file so the
Keyboard Shortcuts dialog stays consistent) and moved it from Filter >
Adjust to Image > Mode, matching industry-standard menu placement.
Filters are auto-inserted into their category's Filter-menu submenu in
code (`KisFilterManager::insertFilter`), not xmlgui, so this required a
one-filter exception there: the action is still created (so xmlgui can
place it in Image > Mode and enable/disable state still tracks via
`filters2Action`), only the line that adds it to the Filter-menu
`KActionMenu` is skipped for `palettize` specifically.

Not a true colorspace/document-mode change -- Krita has no indexed
colorspace at all (confirmed: nothing registers one). This filter
repaints pixel colors to match a chosen saved palette while the image
stays full RGB/Lab; it does not reduce the document to paletted storage.
Relevant for anyone chasing actual 8-bit indexed export later: of the
formats checked, GIF is the only one that's genuinely paletted by
format spec; BMP export (`plugins/impex/qimageio/kis_qimageio_export.cpp`)
always calls `convertToQImage()` which produces full ARGB regardless of
source, so indexed-color output never survives to a BMP file; PCX has
no import/export plugin at all.

Also added two new bundled palette resources (`.gpl`, same GIMP Palette
format as the existing `web.gpl`/`ps.gpl`) so the Indexed Color picker's
built-in options get closer to industry-standard parity: `grayscale.gpl`
(256 entries, R=G=B=n for n=0..255) and `black-and-white.gpl` (2 entries:
pure black, pure white). Web (`web.gpl`, 216 colors) and a classic
industry-standard default swatch set (`ps.gpl`, 131 colors) already
existed and needed no changes. Deliberately skipped the legacy System
(Mac OS) / System (Windows) 256-color palettes from that dialog -- low
value today. No code changes needed to surface the two new files in the
picker: it pulls from `ResourceType::Palettes`, the same global resource
pool as everything else, driven purely by the install list in
`krita/data/palettes/CMakeLists.txt`.

## 2026-09-13 — Padded Transform tool resize-handle grab radius, enlarged
aspect-lock button (bug #5)

**Files:**
- `plugins/tools/tool_transform2/kis_transform_utils.h`
- `plugins/tools/tool_transform2/kis_transform_utils.cpp`
- `libs/widgets/KoAspectButton.cpp`

Two related touch-target fixes reported together as "resize handles hard
to grab" (bug item 5).

**Transform tool.** Free Transform's corner/edge scale handles,
Perspective's handles, and Mesh transform's control-point/node/segment
hit-testing all route through one function,
`KisTransformUtils::effectiveHandleGrabRadius()`, which converted the
raw 8px `handleRadius` constant straight into a hit-test radius with
zero padding -- visual size and touch tolerance were the same number.
(There's already a separate, larger `handleVisualRadius` (12px) used
for drawing in Free Transform/Perspective; mesh transform draws using
half of the *grab* constant instead, so the raw `handleRadius` value
itself was left untouched to avoid changing mesh's drawn handle size.)
Added `handleGrabPadding` (30px), applied only inside
`effectiveHandleGrabRadius()`, bringing the effective grab radius to
38px / 76px diameter -- matching the touch-target size already
established for the crop tool's handle padding fix. Rotation handles
use a separate, untouched function (`effectiveRotationHandleGrabRadius`)
-- out of scope, since the report was specifically about resize.

**Aspect-ratio lock button.** `KoAspectButton` (shared by the Image
Size, Canvas Size, and Layer Size dialogs' lock-proportions toggle) had
a hardcoded `setFixedSize(19, 34)` / `setIconSize(QSize(9, 24))` with no
DPI or UI-scale awareness at all -- became especially hard to hit with
the UI scaled down. Doubled both to 38x68 / 18x48, same proportions
(kept narrow-tall since it spans two grid rows as a bracket between the
Width/Height fields, not squared off).

## 2026-09-14 — Krimble identity in About dialog, working Help menu links

**Files:**
- `krita/main.cc`
- `libs/widgetutils/xmlgui/khelpmenu.cpp`
- `krita/kritamenu.action`
- `libs/ui/KisMainWindow.h`
- `libs/ui/KisMainWindow.cpp`
- `krita/krita5.xmlgui`

The `KAboutData` block in `main.cc` was still upstream Krita's identity
verbatim -- description ("Krita is the full-featured digital art
studio"), copyright, and homepage (krita.org) all unchanged since the
fork started. Updated description to Krimble's, copyright now credits
Krita upstream plus Purdy Design's modifications, homepage points at
krimble.org. Internal app-id string (`"krita"`) and `organizationDomain`
left untouched -- both affect on-disk config paths and weren't part of
what was asked.

**Report Bug was invisible, and would have opened the wrong place.**
`setBugAddress()` had never been called, so `KisKHelpMenu` never
instantiated the Report Bug action at all -- it wasn't just missing from
the menu, the QAction object itself didn't exist. Added
`setBugAddress()`. Investigating the actual `reportBug()` slot turned up
a second, more important problem: the function is a
`#ifdef KRITA_STABLE`/`#else` split, and this build has `KRITA_STABLE`
undefined (`KRITA_ALPHA` is set in the top-level `CMakeLists.txt`, which
suppresses it) -- meaning the live branch was never the simple URL-open,
it was `KisKBugReport`, a full dialog that submits to
`https://bugs.kde.org/enter_bug.cgi`. Report Bug would have sent users
to file issues against upstream KDE Krita, not this fork. Collapsed the
function to unconditionally open the forum's Bug Reports tag
(`https://forum.krimble.org/t/bug-reports`); old code commented out
(`#if 0`), not deleted, per standing rule.

**Three new Help menu items**, none of which had any prior framework
hook (unlike Report Bug, which at least had partial KHelpMenu
scaffolding): Feature Request, Krimble Website, Krimble Forum. Each is a
plain `KisAction` wired straight to `QDesktopServices::openUrl()` with a
fixed URL -- new `.action` declarations in `kritamenu.action` (Help
category), new slots declared in `KisMainWindow.h`, created/connected in
`KisMainWindow::createActions()`, defined next to `showAboutApplication()`.

Help menu is now: Handbook, separator, Report Bug / Feature Request /
Krimble Website / Krimble Forum, separator, About Krimble.

Fixed build error in `KoToolBoxDocker.cpp`: `KisKActionCollection` was
only forward-declared (via `KoToolManager.h`), but the file calls
`viewManager->actionCollection()->action(...)`, which needs the complete
type. Added `#include <kactioncollection.h>`.

Rebranded the Android package identity from `org.krita` to `org.krimble`:
manifest package + activity name (root + debug/next flavors), `build.gradle`
namespace, moved 13 Java files from `src/org/krita/android/` to
`src/org/krimble/android/` with updated package declarations, 7 JNI native
symbols in `KisAndroidScaling.cpp`/`KisAndroidDonations.cpp` renamed to match
(`Java_org_krita_android_*` -> `Java_org_krimble_android_*`), 8 `import
org.krita.R;` statements, one notification channel ID string, and the
ProGuard keep-rule. This is what installs on the device and what would show
in a Play Store listing. Left unchanged: `android.app.lib_name` meta-data
(still "krita" — tied to the CMake target/`.so` name, a separate,
not-yet-decided rename), MIME type strings (`x-krita*`, for `.kra` file
compatibility).

## 2026-09-19 — Fixed 6 remaining stale `org/krita/android` JNI class-path strings

The earlier full package rebrand (`org.krita` → `org.krimble`, Java file
moves, native symbol renames) missed a different mechanism: reflection-
style JNI calls that pass the class path as a **string literal**, not a
compiled symbol. `KisAndroidUtils.cpp` (x2), `KisLongPressEventFilter.cpp`,
`KisKineticScroller.cpp`, and `KisAndroidMediaEncoderRunnable.cpp` (x2)
were still calling `QJniObject::callStaticMethod("org/krita/android/...",
...)` against class paths that no longer exist post-rebrand — these would
throw `ClassNotFoundException` at runtime. `KisLongPressEventFilter` and
`KisKineticScroller` run during normal window/widget init, not some rare
path. Commits `612b8e2` (first batch found, in `KisAndroidDonations.cpp`,
turned out to already be part of the full rebrand) and `21952b8` (the
actual 6 remaining refs above).

## 2026-09-19 — Replaced launcher icon (K + paw mark), all densities + debug/next variant

Adaptive-icon foreground (`ic_launcher.webp` / `ic_launcher_round.webp`)
replaced at all 5 mipmap densities (mdpi–xxxhdpi) with the new K+paw
logo. Source: a transparent PNG with correct adaptive-icon safe-zone
padding (a same-design JPG variant was rejected — baked-in white
background, no transparency, tighter crop). Commit `888a590`. The
`ic_launcher_next`/`_round` (nightly/debug flavor) variant was still
carrying the old Krita mark — updated separately in commit `4bf581c` for
consistency.

## 2026-09-20 — Added release signing config

`assembleRelease` had no `signingConfigs` block at all — would produce an
unsigned build, not installable anywhere. Added one to `build.gradle`:
keystore path resolved from `$HOME/krimble-release.jks` (must exist on
whichever machine actually runs the build — the release keystore itself
is generated once via `keytool` and lives outside the repo, never
committed), passwords read from `KRIMBLE_KEYSTORE_PASSWORD` /
`KRIMBLE_KEY_PASSWORD` env vars set in that machine's shell profile.
Added `*.jks`/`*.keystore` to `.gitignore` so the keystore file can never
land in the repo by accident. Commit `4c7c698`.

Caught one bug in the fix itself before it shipped: the first draft used
a local Groovy variable named `keyPassword`, which collides with the
Gradle DSL setter of the same name — `keyPassword keyPassword` would
have tried to call the string value as a method and broken the build.
Renamed the local var to `keyPass` before committing.

## 2026-09-25 — Two build fixes pushed from server; build pipeline corrected

Two source fixes had been sitting uncommitted on the build server. Pushed:

- `CMakeLists.txt`: added `find_package(Threads REQUIRED)` just before the
  WebP lookup in the optional-dependencies section.
- `plugins/filters/colorsfilters/kis_vibrance_filter.cpp`: added
  `#include <KoUpdater.h>` — the type was only forward-declared, so the
  progress-update calls failed to compile.

Both carry inline comments.

Build pipeline findings (full procedure now in `BUILD_ANDROID.md`):

- The build server is **ARM64** (Oracle Ampere). NDK 27.3's host tools are
  x86_64 and run under emulation there. The emulated `ld.lld` segfaults,
  which surfaced as a misleading CMake error: "Host compiler must support
  64-bit std::atomic!". Fix: the NDK's `lld` was swapped for Ubuntu's
  native `lld-18` (server-side only, not a repo change).
- Hand-patching `build.gradle` / hand-running cmake was abandoned. The
  build now follows Krita's own CI recipe in
  `build-tools/ci-scripts/android.yml`.
- `androiddeployqt` silently drops all Qt QML modules when `_install` is
  inside the source tree. Earlier hand-rolled builds had exactly that
  layout — likely cause of the earlier crash-on-launch APK. Builds now go
  to `~/kwd`, outside the source tree.

## 2026-09-26 — First clean native build via the official CI pipeline

Native build (step 2 in `BUILD_ANDROID.md`) completed clean on commit
`bf2f7b3`: started ~00:56, finished ~08:30 server time. All Krimble libs
(`libkrita_arm64-v8a.so` etc.) and the `qml` folder landed in
`~/kwd/krita/_install`. First build on the server using Krita's own CI
recipe and the native ARM linker.

Also corrected `BUILD_ANDROID.md`: the lld-swap undo command no longer
works on this server (the swap was run twice and overwrote the Intel
backup). Old undo line commented out; re-run guard added.

APK packaging (step 3) not yet run.

## 2026-09-26 — Fixed the weeks-long "no launcher icon" bug

Root cause found: `ic_launcher_foreground.xml` (and the `_next` variant),
the foreground layer of the adaptive launcher icon, had
`android:src="@mipmap/ic_launcher"` — but on Android 8+ that name
resolves to the adaptive-icon XML itself (`mipmap-anydpi-v26/ic_launcher.xml`),
which requires this very file to render its foreground. A self-reference.
The icon renderer fails silently on the loop, leaving a blank icon.
Confirmed by installing a built release APK on a Samsung Galaxy A26
(Android 16) and finding no icon on the install prompt or home screen.

Fix: added `res/drawable-nodpi/ic_launcher_fg.webp` and
`ic_launcher_next_fg.webp` (copies of the existing xxxhdpi artwork,
given their own resource names), and pointed both foreground XML files
at those instead. No more self-reference.

Files:
- `packaging/android/apk/res/drawable-nodpi/ic_launcher_fg.webp` (new)
- `packaging/android/apk/res/drawable-nodpi/ic_launcher_next_fg.webp` (new)
- `packaging/android/apk/res/drawable/ic_launcher_foreground.xml`
- `packaging/android/apk/res/drawable/ic_launcher_next_foreground.xml`

Not yet re-verified with a fresh build/install — next step.

## 2026-09-30 — Launcher icon: rainbow background removed, K enlarged

Two problems seen on a Samsung Galaxy A26 after installing the
beta2 release APK built from `5b84bcd`: the icon showed the K+paw
over the Krita rainbow gradient instead of white, and the K was
small.

Background: all four adaptive-icon files pointed at
`@drawable/ic_launcher_background` (or `ic_launcher_next_background`),
which resolves to the drawable XML that holds the Krita rainbow
gradient. A white `<color>` with the same name already existed in
`res/values/` but was never used. Each file now points at
`@color/...` (white). The old `<background>` line is kept as a
comment. The rainbow drawable files are untouched.

K size: `ic_launcher_fg.webp` and `ic_launcher_next_fg.webp` are
192x192 px images. The foreground XMLs used `gravity="center"`, which
places a bitmap at its own pixel size without scaling, so the K
covered only about 26% of the icon width on a high-density screen.
Both foreground XMLs now wrap the bitmap in a 12dp `<inset>` with
`gravity="fill"`, which scales it up to about 45% of the icon width.
The old `<bitmap>` element is kept as a comment.

Note: the 192px source art will look slightly soft when scaled up.
A larger K+paw source image would fix that.

Files (all under `packaging/android/apk/res/`):
- `mipmap-anydpi-v26/ic_launcher.xml`
- `mipmap-anydpi-v26/ic_launcher_round.xml`
- `mipmap-anydpi-v26/ic_launcher_next.xml`
- `mipmap-anydpi-v26/ic_launcher_next_round.xml`
- `drawable/ic_launcher_foreground.xml`
- `drawable/ic_launcher_next_foreground.xml`

Not yet verified with a fresh build and install.

## 2026-10-01 — Renamed "Advanced Color Selector" docker to "Color Selector"

**File:** `plugins/dockers/advancedcolorselector/kis_color_selector_ng_dock.cpp`

The docker title (also used as its tab label) changed from "Advanced
Color Selector" to "Color Selector". The old `setWindowTitle` line is
kept as a comment. The Wide Gamut Color Selector docker keeps its name.

Not changed: the same name still appears in the dropdown in
`kis_color_selector_settings.cpp` (line 43). Not yet verified with a
fresh build and install.

## 2026-10-01 — Hue/Saturation naming; filter dialog width cap

**Hue/Saturation rename.** The existing HSV filter (already under
Image > Adjustments, shortcut Ctrl+U) is now named to match the
industry-standard name. Files:
- `plugins/filters/colorsfilters/kis_hsv_adjustment_filter.cpp` -- menu
  text "&HSV Adjustment..." -> "&Hue/Saturation..." (old line commented).
- `plugins/filters/colorsfilters/kis_hsv_adjustment_filter.h` -- display
  name "HSV/HSL Adjustment" -> "Hue/Saturation" (old line commented).
  The internal id `hsvadjustment` is unchanged.
Decision: Hue and Saturation sliders are NOT added to Brightness/Contrast,
since this filter already provides them.

**Filter dialog width cap.** File: `libs/ui/dialogs/kis_dlg_filter.cpp`.
After the saved geometry is restored, if the dialog is wider than half
the screen's available width it is resized to exactly half. Applies to
every filter dialog. Height unchanged. Cause of the original width not
confirmed (saved geometry vs. content size hint). Not yet verified with a
fresh build and install; the dialog's later `adjustSize()` call may
override it.

**Not done:** Brightness/Contrast dialog title not drawn (cause
unknown); color selector docker half-size (no change made).

## 2026-10-01 — Filter dialog bottom row split into two rows (real cause of width)

**File:** `libs/ui/forms/wdgfilterdialog.ui`

Brightness/Contrast and HSV (Hue/Saturation) dialogs were wider than the
screen on Android. Measured from a screenshot: the bottom row held 6
widgets (gallery toggle, Preview, Multiframe, Create Filter Mask, Cancel,
OK) and needed about 1370px minimum, so no sizing code could shrink the
dialog below it. This applies to every filter dialog.

The row is now two rows: gallery toggle, Preview and Multiframe on the
first; Create Filter Mask, Cancel and OK on the second. Widget names and
connections are unchanged. The original row is kept as an XML comment.
The half-screen width cap added earlier today in `kis_dlg_filter.cpp`
stays; it could not take effect while this row set the minimum width.

Remaining known width source: the preset row (preset dropdown, Use last
preset, Edit Presets, XML) in `wdgfilterselector.ui`, about 650px
minimum. Not changed. Not yet verified with a fresh build and install.

## 2026-10-01 — Color Selector height cap; Android filter dialog title label; settings dropdown rename

**Color Selector docker height.** File:
`plugins/dockers/advancedcolorselector/kis_color_selector_ng_dock.cpp`.
The docker content is now capped at 42% of the screen's shorter side
(about 454px on a 1080px-wide portrait screen). Before, it took about
910px in portrait (measured from a screenshot), so this is roughly half.
Landscape is unaffected. Width is not capped separately: the selector
scales with its height. The 42% figure is an estimate and may need
tuning after a test on the device. Not yet built or installed.

**Filter dialog title label (Android only).** File:
`libs/ui/dialogs/kis_dlg_filter.cpp`. The dialog title ("Filter:
Brightness/Contrast...") is set in code but Android does not draw it,
leaving a blank white strip. Added a bold, centered label at the top of
the dialog that shows the filter name, wrapped in `#ifdef Q_OS_ANDROID`
so desktop builds are unchanged. It updates through `setDialogTitle()`.
Not yet built or installed. This code was not compiled before pushing.

**Settings dropdown.** File:
`plugins/dockers/advancedcolorselector/kis_color_selector_settings.cpp`.
The color docker settings dropdown entry "Advanced Color Selector" is
now "Color Selector" (old line commented). Index-based, so nothing else
depends on the text.

## 2026-10-01 — Version string alpha1 -> beta2 (splash and About)

**File:** `CMakeLists.txt` (both `KRITA_VERSION_STRING` lines, Qt5 and Qt6
branches). The splash and About dialog read this string and still said
"1.0.0-alpha1"; only the Gradle `versionName` (APK filename) had been
bumped to beta2. Old lines kept as comments.

Deliberately NOT changed: `KRITA_ALPHA 1` stays set. Switching it to
`KRITA_BETA` would change `BRANDING` from "Next" to "Beta" and swap the
icon and splash asset sets already customised for the "Next" variant.

Needs a CMake reconfigure on the next build (`make` does this
automatically). The git hash on the splash also updates then. Not yet
built or verified.

## 2026-10-01 — Krimble marked as Beta (CMake flag, README badge)

**Files:** `CMakeLists.txt`, `README.md`.
- `CMakeLists.txt`: `KRITA_ALPHA` is now `KRITA_BETA` (old line commented).
  To stop that from switching `BRANDING` from "Next" to "Beta" (which would
  swap the customised icon and splash assets), `BRANDING` is set to "Next"
  when not given on the command line. The version string was already bumped
  to `1.0.0-beta2` earlier today.
- `README.md`: Status badge text `1.0.0--alpha1` -> `1.0.0--beta2`.
  History line 180 ("Version reset to alpha1") left as-is; it is history.

Not yet built or verified. The next `make` re-runs CMake automatically.

## 2026-10-01 — Default toolbox icon size 32 -> 16 (16 x 16)

**File:** `libs/ui/toolbox/KoToolBox.cpp`

On Android the default toolbox icon size was a fixed 32. It is now 16
(16 x 16), matching the "16x16" entry already in the toolbox's icon size
menu. Old line commented out. Only the default changes: an icon size
already saved on a device (`KoToolBox/iconSize` in the config) still wins,
so existing installs keep their saved size until it is reset to
"Default" from the toolbox context menu. Fresh installs get 16.
Not yet built or verified.

## 2026-10-02 — Photoshop-style Image > Adjustments menu; Auto Tone, Auto Color, Photo Filter

Per project direction (reversal of the earlier "move the whole Adjust
submenu" approach): the auto-generated Adjust submenu is back in the
Filter menu, and Image > Adjustments is now a hand-built menu.

**`krita/krita5.xmlgui` (no comments added to the file, per direction)**
- Filter menu: `adjust_filters` (auto-generated "Adjust" submenu) placed
  first among the filter categories, after Liquify.
- Image menu: the single `adjust_filters` entry replaced by an
  "Adjustments" submenu listing individual filters in this order:
  Brightness/Contrast, Levels, Curves (label for `perchannel`) | Vibrance,
  Hue/Saturation, Color Balance, Photo Filter | Invert, Posterize,
  Threshold, Gradient Map | Desaturate, Match Color.
  Directly below it: Auto Tone, Auto Contrast, Auto Color.
- Not in the Adjustments menu (still available in Filter > Adjust and the
  Filter Gallery): Dodge, Burn, Slope/Offset/Power, Cross-channel curves,
  Normalize, Index Colors.
- Actions are referenced as `krita_filter_<id>`; an action name that does
  not exist is ignored by the menu loader.

**New filters (no new algorithms)**
- `plugins/filters/levelfilter/KisAutoLevelsFilters.{h,cpp}` (registered in
  `KisLevelsFilterPlugin.cpp`, added to that plugin's `CMakeLists.txt`):
  - Auto Tone (`autotone`): per-channel contrast stretch using the
    existing auto levels engine (`KisAutoLevels::adjustPerChannelContrast`),
    0.1% clipping each end, midtones untouched.
  - Auto Color (`autocolor`): the same, plus midtone neutralization: each
    channel's mean is moved to 50% gray at full strength.
  - Both apply the result through the existing Levels transformation.
    RGB images only (same limit as the "auto levels for all channels" button
    in the Levels dialog); other color models are left unchanged.
  - Clipping, offset and target values are the defaults of the Levels
    auto dialog; they are our choices and are not claimed to match any other
    program numerically.
- `plugins/filters/colors/KisFilterPhotoFilter.{h,cpp}` (registered in
  `colors.cpp`, added to that plugin's `CMakeLists.txt`): Photo Filter
  (`photofilter`) with Color, Density and Preserve Luminosity. It reuses the
  Fast Color Overlay blend: Color blend mode when Preserve Luminosity is on,
  Multiply when off. Defaults: warm orange (236,138,0), density 25%, preserve
  luminosity on. Blend modes are approximations of a photo filter, not a
  copy of any other program's math.

**What was and was not verified**
- Verified: both new `.cpp` files and both plugin registration files pass a
  compiler syntax-only check against the real Krita headers (generated
  config/export/moc headers stubbed). The auto levels math was reproduced on a
  synthetic low-contrast color-cast image: Auto Tone stretches every channel to
  the full range; Auto Color additionally brings the channel means to about
  50%.
- NOT verified: a real build, linking, running the filters in the app, the
  menu layout on a device, and the `Curves...` label override. The first
  server build after this commit may still surface errors.

**Not done (need new algorithms):** Exposure, Black & White, Channel Mixer,
Equalize, Shadows/Highlights, Replace Color, Selective Color, HDR Toning,
Color Lookup.

### Resulting Image menu (2026-10-02)

```
Image
  Mode                       >
  Adjustments                >
      Brightness/Contrast...
      Levels...                      Ctrl+L
      Curves...                      Ctrl+M
      ---------------------------
      Vibrance...
      Hue/Saturation...              Ctrl+U
      Color Balance...               Ctrl+B
      Photo Filter...                (new)
      ---------------------------
      Invert                         Ctrl+I
      Posterize...
      Threshold...
      Gradient Map...
      ---------------------------
      Desaturate...                  Ctrl+Shift+U
      Match Color...
  ---------------------------
  Auto Tone                          (new)
  Auto Contrast
  Auto Color                         (new)
  ---------------------------
  Image Size...                      (unchanged below this line)
```

Shortcuts are the ones already assigned to each filter in code; no
shortcut was added or changed. Auto Tone, Auto Contrast, Auto Color and
Photo Filter have none.

## 2026-10-02 — Default workspace repaired: only ToolBox, Tool Options and Layers on startup

**File:** `krita/data/workspaces/Default.kws`

**Cause found.** Commit `a73387e` (2026-09-05, "Simplify Default workspace
to ToolBox, Tool Options, Layers only") saved the panel layout (`<state>`,
a base64 string) with every letter converted to upper case: 0 lower-case
characters in 7,752, against about 1,500 lower-case in every earlier version.
Base64 is case-sensitive, so the string decoded to garbage, Qt refused to
restore it, and the app silently fell back to its built-in panel defaults.
That is why Color Selector and Brush Presets kept appearing at startup and the
Sept 4 simplification never took effect on a device.

**Fix.** The `<state>` is replaced with a valid one that contains exactly three
visible panels: ToolBox on the left; Tool Options (`sharedtooldocker`) above
Layers (`KisLayerBox`) on the right. The other 90 dockers in the file are
present and hidden, so they can still be opened from Settings > Dockers.
The rest of the file (settings, thumbnail) is the last valid version from
`62a77b2` (2026-08-24). The broken state from `a73387e` is not kept in the
file; recover it from that commit if ever needed.

**How the new state was made and checked.** A small Qt program built a main
window with a dock widget for every name found in the Aug 24 state, set the
layout above, and called `saveState()`. The result starts with the correct
Qt magic bytes, restores successfully into a fresh window, and in that window
exactly ToolBox, Tool Options and Layers are visible (90 of 93 hidden).

**Not verified:** on a real device. Installs that already have a copy of the
old workspace file in their app data may keep using it until the app data is
cleared; not tested. Dock widths are Qt defaults for a 1080 px-wide screen and
may need tuning.

## 2026-10-02 — Theme fallback and Themes menu check (first-launch gray theme)

**File:** `libs/ui/thememanager.cpp` (two small changes, old lines commented out)

Symptom: on a fresh install the first launch showed the plain gray default
palette instead of "Krimble dark"; the second launch was correct.

Two leftovers from renaming the theme were found in `ThemeManager`:
1. `currentThemeName()` fell back to `"Krita dark"` when no name was known.
   No theme file has that name any more, so the palette lookup found nothing and
   the app kept Qt's gray default. Now falls back to `"Krimble dark"`.
2. `populateThemeMenu()` decided which Themes-menu entry to check by calling
   `currentThemeName()` on a menu group that had just been created with nothing
   checked. That call returned the fallback name, so no entry was checked, and
   any later palette refresh used the fallback. It now compares against the theme
   name that was requested (default `"Krimble dark"`).

Confidence: these are real bugs and the fixes are correct, but I have NOT
confirmed they are the whole cause of the first-launch gray theme; I could not
reproduce it. Not compiled (the syntax-check setup could not resolve
`kstandardshortcut.h` for this file; the change only uses QString/QStringLiteral
already used in the file). Test: clear app data, install, first launch.

## 2026-10-02 — File > New dialog narrowed on Android

**Files:** `libs/ui/KisOpenPane.cpp`, `libs/ui/widgets/kis_custom_image_widget.cc`
(both changes wrapped in `#ifdef Q_OS_ANDROID`; desktop builds unchanged)

Symptom: the New Image dialog was wider than a portrait screen (its left list
was cut off) and, in landscape, the Width / Height / Resolution fields, their
unit dropdowns and the Predefined dropdown stretched across a lot of empty
space.

- `KisOpenPane.cpp`: the left list (Custom Document, templates...) gets a
  maximum width of 24% of the screen width. Longer names are elided. On a
  landscape screen the cap is larger than the list's natural width, so
  landscape is unchanged.
- `kis_custom_image_widget.cc`: maximum widths (in the app's scaled pixels) of
  90 for the width, height and resolution fields, 95 for their unit dropdowns,
  and 130 for the Predefined dropdown. They were about 123 each before. Only
  maximums were added, nothing can become smaller than it needs to be.

Both files pass a compiler syntax-only check against the real Krita and KDE
headers (generated headers stubbed). Not verified: a real build, and how the
dialog looks on a device. The numbers are estimates taken from a screenshot and
may need tuning.

Also re-checked with the same syntax-only compile: `libs/ui/thememanager.cpp`
(theme fix from earlier today) now passes; it could not be checked before.

## 2026-10-02 — Settings > Panels > Detach Panel

**File:** `libs/ui/KisMainWindow.cpp` (after the panel list is built; no
`krita5.xmlgui` change, the item is added to the code-generated Panels menu)

New submenu at the bottom of Settings > Panels, "Detach Panel". It lists the
panels that are currently shown and docked (hidden panels and panels that are
already floating are not listed). Choosing one makes it a free-floating window.
The list is rebuilt each time the submenu opens. If no panel is docked it shows a
disabled "No docked panels" entry.

A locked panel (lock icon in its title bar) cannot float, so choosing it
unlocks it first, then detaches it.

**Why Tool Options was greyed out in Settings > Panels:** that is Krita's
built-in panel lock, not a bug. While a panel's lock is on, its entry in the
Panels list is disabled and its close/float buttons are hidden. Tapping the lock
icon in the panel's title bar turns the lock off and re-enables the entry.
Nothing was changed for this; the lock still works as before.

**Verified:** the menu logic was run in a small standalone Qt program using
stand-ins for the two title-bar classes (copying their real lock behaviour): only
visible docked panels are listed, a locked panel is unlocked and floated, it
disappears from the list afterwards, and the empty case is handled.
**Not verified:** `KisMainWindow.cpp` as a whole could not be syntax-checked here
(it needs the external `lager` library headers, which are not available), the
real build, and floating panels on a real Android device.

## 2026-10-02 — Floating panels on touch: stay detached, easier to grab, Attach Panel

**File:** `libs/ui/KisMainWindow.cpp` (the first part is Android only,
`#ifdef Q_OS_ANDROID`)

Feedback from device testing: floating panels re-docked too eagerly when
dropped, and were hard to grab with touch.

- **Stay detached.** When a panel floats it is now allowed in no dock area
  (`setAllowedAreas(Qt::NoDockWidgetArea)`), so dropping it over a dock area
  does not snap it back. It is docked again through its float button or the new
  Settings > Panels > Attach Panel item. Its allowed areas are restored when it is
  docked. The change is made only from the `topLevelChanged` signal, i.e. after
  the panel was already added to a dock area. (The 2026-09-04 attempt set
  the allowed areas before `addDockWidget()` and broke docking; this does not.)
- **Easier to grab.** While floating, the panel's title bar has a minimum
  height of 40 (app-scaled pixels; docked panels are unchanged).
- **Attach Panel.** New submenu under Settings > Panels, below Detach Panel. Lists
  floating panels; choosing one docks it back into its previous area.

**Verified:** in a standalone Qt program (offscreen): the allowed-areas change
and the taller title bar happen on floating, `setFloating(false)` still docks a
panel back into its previous area even while its allowed areas are empty, and
the allowed areas and title height are restored afterwards.
**Not verified:** on a device. In particular, whether Qt on Android honours the
empty allowed areas while a floating panel is being dragged (this is how Qt
decides whether a drop docks the panel), and whether 40 is the right height.
The offscreen test cannot simulate dragging. `KisMainWindow.cpp` still cannot be
syntax-checked here (missing external `lager` headers).

## 2026-10-02 — Floating panels: title bar overlap fixed, default size, resize handle

**File:** `libs/ui/KisMainWindow.cpp` (Android only, `#ifdef Q_OS_ANDROID`).
Replaces part of the 2026-10-02 "stay detached" change after device testing.

**Bug found on device:** floating panels showed their title text and buttons on
top of the panel's content (Layers header over the blend mode box, Color
Selector title over the hue bar). Cause: I made the title bar taller by setting
a minimum height. The dock layout only reserves the title bar's size hint, so the
taller title bar was drawn over the content below it. Reproduced in a standalone
Qt test: title bar 80 px tall, content starting at 34 px.

**Fix:** the extra title height is now added as padding in the title bar's own
layout (14 on top and bottom while floating; removed when docked again), which
the dock layout does reserve. Same test: title bar and content no longer overlap.

**New:**
- Default size. A panel that starts floating is capped to 60% x 75% of the
  screen's shorter side (a panel already smaller stays as it is).
- Resize handle. A visible 36 x 36 corner handle (three diagonal lines) in the
  lower right of each floating panel; hidden when the panel is docked. It uses
  Qt's size grip, so the panel is resized by dragging the handle.

**Verified (standalone Qt test, offscreen):** no overlap; the default size is
applied; the handle sits in the lower right corner and resizes the panel when
dragged (650x800 dragged by -100 px wide gave 550 wide; the height result was
limited by the test screen); docking again removes the padding and hides the
handle. **Not verified:** on a device, touch dragging of the handle, and the size
and padding numbers (14, 36, 60%, 75%) are first guesses.

## 2026-10-02 — File > New still too wide: Profile dropdown minimum width found

**Files:** `libs/ui/widgets/kis_color_space_selector.cc` (Android only),
`libs/ui/KisOpenPane.cpp`

Device test of the earlier narrowing (ff26aa9) showed the fields were narrower
but the dialog was still wider than a portrait screen (left list cut off).

**Cause found:** `libs/ui/forms/wdgcolorspaceselector.ui` gives the Profile
dropdown (`cmbProfile`) a hard minimum width of 300. On the test phone the app
scales by about 2.5, so that is about 750 screen pixels, matching the width of the
dropdown in the screenshot (745 px). It set the minimum width of the whole Custom
Document page on its own.

**Fix:** on Android only, the dropdown's minimum width is set to 100 in the
selector's constructor. The dropdown is a squeezed combo box that elides long
profile names, so nothing is lost. The same selector is used by other dialogs
(for example image properties and color conversion), which also get narrower on
Android. The left list cap in `KisOpenPane.cpp` was lowered from 24% to 22% of the
screen width.

**Estimate, not measured:** with a 2.5 scale, a portrait screen is about 432 wide
in app units. The left list (22%) is about 95; the page should now need roughly
290, so the total is about 385. The 290 is an estimate from the other rows, not a
measurement. **Verified:** both files pass a compiler syntax-only check. **Not
verified:** the real dialog on a device.

## 2026-10-02 — Detach Panel and Attach Panel moved under "Panels" in the Settings menu

**Files:** `libs/ui/KisMainWindow.cpp`, `krita/krita5.xmlgui` (George approved
this edit to the file; no comments were added to it)

Device feedback: the Settings > Panels list already has far too many entries, so
Detach Panel and Attach Panel should not be at the bottom of it.

- `KisMainWindow.cpp`: both are now `KActionMenu` actions registered in the
  action collection as `settings_detach_panel_menu` and
  `settings_attach_panel_menu`, instead of submenus added to the Panels list.
  Their contents and behaviour are unchanged. The old lines that added them to
  the Panels list are commented out.
- `krita5.xmlgui`: two `<Action>` entries added in the Settings menu, directly
  after the `settings_dockers_menu` ("Panels") entry.

**Caveat:** `KisMainWindow` loads a locally customized `krita5.xmlgui` from the
app's data folder if one exists, and that file replaces the built-in one. A phone
with such a file will not show the two new entries until that local copy is
removed or updated.

**Verified:** `krita5.xmlgui` is well-formed XML. **Not verified:** the menu on a
device; `KisMainWindow.cpp` cannot be syntax-checked here (missing external
`lager` headers).

## 2026-10-02 — Default workspace applied once on first launch (Brush Presets / Color Selector at startup)

**File:** `libs/ui/KisApplication.cpp` (right after the existing `--workspace`
handling in `KisApplication::start()`)

Device test with cleared app data showed Brush Presets and Color Selector still
visible, even after `Default.kws` was repaired (9f951f5). Reading the startup
code showed why: Krita applies a workspace only when asked (command line option,
canvas-only mode, Reset All Settings). On a normal first launch no workspace is
applied, so the panels come from each panel's built-in default, and the repaired
`Default.kws` was never used.

**Fix:** on the first launch of an installation (no `DefaultWorkspaceApplied`
entry in the `Krimble` config group), the workspace named "Default" is applied
once and the entry is written. Later launches keep whatever layout the user
left, so panels the user opens or moves are not reset. Skipped when a workspace
was given on the command line.

**Limits:** it needs the repaired `Default.kws` to be the "Default" workspace in
the app's resource database, which is true on a fresh install (clear app data).
An install over an older one keeps the old workspace and is not changed by this.
Settings > Reset All Settings clears the flag along with the other settings, so
the layout is applied again after a reset.

**Verified:** no compile error in the added lines (the syntax-only check of the
whole file reports unrelated errors from stubbed generated headers). **Not
verified:** on a device.

## 2026-10-02 — Detach Panel / Attach Panel: summary, README entry, toolbox keeps its size

This entry summarizes the feature built across the 2026-10-02 entries above
(86f1474, cad9232, 9a6c1b0, c71726a) and records two further changes.

**What the feature is**
- Settings > **Detach Panel** (below "Panels"): lists the panels that are shown
  and docked; choosing one makes it a free-floating window. A locked panel is
  unlocked first.
- Settings > **Attach Panel**: lists the floating panels; choosing one docks it
  back into its previous area.
- A panel that floats stays where it is dropped (it is allowed in no dock area
  while floating), has extra title bar padding for touch, is capped to a default
  size (60% x 75% of the screen's shorter side) and has a resize handle in its
  lower right corner. Android only, except the two menu items.
- Code: `libs/ui/KisMainWindow.cpp`; menu placement: `krita/krita5.xmlgui`.

**README:** the "Panels" row of the "What Krimble changes" table now mentions
Detach Panel, Attach Panel, stay-where-dropped and the resize handle.

**Toolbox keeps its size when detached** (`libs/ui/KisMainWindow.cpp`, Android
only). Device feedback: the toolbox did not keep its dimensions when detached. The
size every panel has while docked is now remembered, and the toolbox is given its
docked size back when it floats instead of being capped to the default size.
The cause of the size loss is not confirmed: in a standalone Qt test (including a
scroll-area based panel like the toolbox) a panel keeps its size when floated, so
this change restores the size explicitly rather than fixing a known cause.
**Verified:** the size tracking and restore in a standalone Qt test. **Not
verified:** on a device with the real toolbox; whether the toolbox then keeps its
vertical layout.

## 2026-10-02 — Six panel plugins no longer built (George's edit, commit 563cc24)

**File:** `plugins/dockers/CMakeLists.txt` (the `add_subdirectory` lines were
commented out, not deleted)

To make the app leaner, these panel plugins are no longer compiled or packaged:
- Artistic Color Selector (`artisticcolorselector`)
- Animation panels: Animation Timeline, Animation Curves, Onion Skins (`animation`)
- Brush Preset History (`presethistory`)
- Log Viewer (`logdocker`)
- Storyboard (`storyboarddocker`)
- Wide Gamut Color Selector (`widegamutcolorselector`)

They no longer appear in Settings > Panels, which also shortens that list.

**Checked (read-only):** nothing else in the build refers to these plugin targets
(a search of all CMake files, scripts and CI files found no references outside the
six directories), so the build should still configure and link.

**Important for the server build:** commenting a plugin out does not delete the
copy already installed. The old plugin libraries (`kritaartisticcolorselector`,
`kritaanimationdocker`, `kritapresethistory`, `kritalogdocker`,
`kritastoryboarddocker`, `kritawgcolorselector`, each `_arm64-v8a.so`) stay in
`_install/lib`, and packaging would still put them in the APK, so the panels would
keep showing. They need to be deleted from `_install/lib` before packaging.

## 2026-10-02 — Splash images are JPG; the old PNGs are no longer packaged

**Files:** `krita/data/splash/splash-android.qrc`, `krita/data/splash/splash.qrc`,
`libs/ui/kis_splash_screen.cpp`; new files `krita/data/splash/logo_splash.jpg` and
`krita/data/splash/logo_splash_holidays.jpg` (added by George, commit 2c76225)

The new splash images have no transparency, so they are saved as JPG: 379 KB and
387 KB (1536 x 1024, RGB, no alpha), against about 2.1 MB and 2.4 MB for the PNGs.

- Both resource lists now embed the two JPGs. The old PNG lines are kept as XML
  comments in each file. Android uses `splash-android.qrc` (aliases `hd.jpg` and
  `holiday.jpg`); the desktop list `splash.qrc` uses `0.jpg` and `1.jpg`.
- `kis_splash_screen.cpp` loads the new alias names. The old lines are commented out.
- The PNG files stay in the repository (George wants to keep them) but nothing
  references them any more, so they are not built into the app. A search of the
  code, resource and build files found no other reference.
- The December 1-26 holiday splash is now landscape (it was portrait); the splash
  code scales any image to a fixed height and keeps its aspect ratio.
- The splash window still draws its own gray background, visible only in the bar
  under the image that holds "Loading resources...".

**Verified:** both resource files are well-formed XML; the splash code passes a
compiler syntax-only check; Qt reads both JPGs (1536 x 1024, no alpha channel).
**Not verified:** a real build, and that the Android build includes Qt's JPEG
image plugin (upstream Krita's Android build used a JPEG splash, which suggests
it does). Check after the build that the splash shows.

## 2026-10-02 — Brush Presets still at startup (real cause), panel renamed "Brushes", Attach docks where the panel is

**Files:** `libs/ui/KisMainWindow.cpp`,
`plugins/dockers/presetdocker/presetdocker_dock.cpp`

**1. Brush Presets and Color Selector still showed after the first new file, even with the first-launch workspace (d250bcc).**
Cause: while the welcome page is shown the panels are hidden and the layout from
before hiding is kept in `dockerStateBeforeHiding`; it is put back when a document
opens. The first-launch workspace was applied while the welcome page was up and
left that saved layout untouched, so the first new file brought back the old
layout. Reproduced in a standalone Qt test: without the fix the old panels return
after the first new file, with it only ToolBox, Tool Options and Layers do.
Fix: `KisMainWindow::restoreWorkspace()` now, when no document is open, calls
`toggleDockersVisibility(false, true)` right after restoring the workspace, which
captures the workspace layout and hides the panels again as the welcome page does.

**2. Panel renamed "Brush Presets" -> "Brushes"** (`presetdocker_dock.cpp`; old
line commented out). It also changes the name in Settings > Panels.

**3. Settings > Attach Panel now docks where the panel was put**, instead of back
into its old place. The panel goes to the nearer side of the window (left or right,
by the position of the panel's centre) and, on that side, between the panels above
and below it. While detached it still does not snap anywhere. The old behaviour is
kept in a comment. Panels are not put above or below the canvas.
Verified in a standalone Qt test with five drop positions: left/right side chosen
by position; top, middle or bottom of the side order correctly among three docked
panels; the panel is docked afterwards and its allowed areas are restored.
Docking via the panel's own float button still returns it to its old place.

**Not verified:** a real build and a device. `KisMainWindow.cpp` cannot be
syntax-checked here (missing external `lager` headers); the new code was tested as
a standalone copy.

## 2026-10-02 — Resize handle no longer covers the Layers Delete button; smaller color picker

**Files:** `libs/ui/KisMainWindow.cpp` (Android only),
`libs/widgets/KisDlgInternalColorSelector.cpp` (Android only)

**Resize handle over the Delete button.** On a floating Layers panel the corner
handle sat on top of the Delete (trash) button. Instead of moving the button, the
handle now has its own strip: while a panel floats, a 36-high strip is reserved
under its content (via the panel's content margins) and the handle sits in that
strip. When the panel is docked again the strip is removed. This applies to every
floating panel, so no panel loses anything under the handle. Verified in a
standalone Qt test: the content shrinks by exactly the strip height while floating
and the margin returns to 0 when docked. The Delete button was not moved.

**Color picker too big.** The dialog opened by tapping a color well (for example
the foreground/background colors in the toolbox) is designed at 505 x 490. On
Android it now opens at 80% of that, never more than 90% of the screen; the layout
still enforces any minimum it needs. The 80% is a first guess from "a little too big".

**Verified:** `KisDlgInternalColorSelector.cpp` passes a compiler syntax-only
check; the strip logic was tested as a standalone copy. **Not verified:** a real
build or a device; `KisMainWindow.cpp` cannot be syntax-checked here.

## 2026-10-02 — Four panels renamed to match industry-standard names

**Files (title lines only, old lines kept as comments):**
`plugins/dockers/advancedcolorselector/kis_color_selector_ng_dock.cpp`,
`plugins/dockers/compositiondocker/compositiondocker_dock.cpp`,
`plugins/dockers/snapshotdocker/SnapshotDocker.cpp`,
`libs/ui/toolbox/KoToolBoxDocker.cpp`

| Was | Now |
|---|---|
| Color Selector | **Color** |
| Compositions | **Layer Comps** |
| Snapshot Panel | **Snapshots** |
| Toolbox | **Tools** |

Together with "Brush Presets" -> "Brushes" (765acdb). The names also change in
Settings > Panels. Saved panel layouts refer to panels by an internal name, not by
title, so they are not affected, and no code compares against the old titles
(a search found none).

**Kept on purpose (George's decision):** "Swatches (Palette)", because it is more
explanatory. **Not changed:** Tool Options, Text Properties, Grid and Guides.
The Color Selector entry in the color settings dropdown
(`kis_color_selector_settings.cpp`) still says "Color Selector".

**Verified:** all four files pass a compiler syntax-only check. **Not verified:**
a real build or a device.

## 2026-10-02 — Resize handle moved the wrong corner on the toolbox

**File:** `libs/ui/KisMainWindow.cpp` (Android only)

Device feedback: the handle in the lower right corner of the floating toolbox
moved the lower left corner.

**Cause (reproduced in a standalone Qt test):** the handle was a `QSizeGrip`. Qt
decides which corner a size grip controls from where it sits in the window: if
its left edge is in the left half of the window it acts as a lower left grip. The
toolbox is narrow, so the 36 pixel wide handle was in the left half. Test: on a
panel 70 wide, dragging the handle right moved the left edge from 302 to 310
and shrank the panel; on a 300 wide panel it behaved correctly.

**Fix:** the handle is now a plain widget that resizes its panel from the lower
right: the left and top edges never move; dragging by (+60, +80) adds 60 to the
width and 80 to the height. Same test, both widths: left edge fixed, right edge
and bottom edge move as dragged. Look, size (36 x 36) and position are unchanged.
The old `QSizeGrip` include is commented out.

**Verified:** standalone Qt test (wide and narrow panel). **Not verified:** a
device; `KisMainWindow.cpp` cannot be syntax-checked here.

## 2026-10-02 — Edit > Preferences window far wider than the screen

**File:** `libs/ui/dialogs/kis_dlg_preferences.cc` (Android only)

Device feedback: Preferences opened as a very wide window.

**Cause found:** the form of the General page (`libs/ui/forms/wdggeneralsettings.ui`)
gives its top widget a hard minimum width of 552. At the phone's app scale (about 2.5)
that is about 1380 screen pixels, and a window cannot be narrower than its widest
page. The page already scrolls inside (it has a scroll area), so it does not need that
minimum.

**Fix:** (1) the General page's minimum width is set to 0. (2) After the dialog is
built it is resized to no more than 95% of the screen width and 90% of the screen
height (the smaller of that and its natural size).

**Checked:** other pages' forms have only smaller hard minimum widths (the tablet
pressure curve 200, brush preview 320 and scratch pad 250 on a brush settings form
that may not be part of this dialog). **Not verified:** that every page now fits;
another page may still force it wider. **Not verified:** a build or a device. The file
could not be syntax-checked here (it includes headers from the external `lager`
library); the added lines are plain Qt calls. If Preferences is still too wide, a
screenshot of the page that is too wide will show which form to fix next.

## 2026-10-02 — Type tool re-enabled (George's decision); cause of bug #9 still unknown

**File:** `plugins/tools/svgtexttool/Plugin.cpp`

Re-enabled the line that registers the Type tool (`SvgTextToolFactory`) with
`KoToolRegistry`, which had been commented out since 2026-09-06 (bug #9). The
old comment and the old commented line are kept above the new line. With the tool
registered again, the Type tool appears in the toolbox and all 18 items in the
Type menu work again; they had nothing to act on while the tool was off.

**Bug #9 is NOT fixed.** The Type tool was randomly activating itself, and the
reason was never found. This session's read-only search of the automatic tool
switching code found no cause: `KoToolManager::preferredToolForSelection`
(used by `KisShapeController::setInitialShapeForCanvas` and
`DefaultTool::explicitUserStrokeEndRequest`) picks the lowest-priority-number
tool whose shape list matches the selected shapes, and the Type tool matches only
text shapes (plus `flake/always`, which no shape uses); the "no active tool"
fallback only looks at the Main section, and the Type tool is in `PSOrder`. The
Type tool's own priority (1) and section (`PSOrder`) are Krimble settings (upstream
Krita uses priority 5 and the Vector section); I did not change them.

If it activates by itself again, please note what was being done (which tool was
active, what was tapped, whether a text layer existed) so it can be traced.
**Not verified:** a build or a device; the file could not be syntax-checked here (the
Qt QML headers are not installed in the sandbox). The change is one uncommented line.

## 2026-10-02 — Resize handles missing on every panel (bug from cfa2343) fixed

**File:** `libs/ui/KisMainWindow.cpp` (Android only)

Device feedback on the 19:05 build: the sizing handles on all floating panels
were missing.

**Cause (my bug, from cfa2343):** when the handle stopped being a `QSizeGrip`
subclass and became a plain `QWidget`, the code that checks "does this panel
already have a handle?" (`findChild<KisFloatingDockSizeGrip*>()`) stopped
working. The class has no `Q_OBJECT`, so Qt's type check matches any widget.
It returned the first child of the panel (in a standalone test: the panel's own
title button), the code concluded a handle existed, and no handle was ever
created. It also moved and showed that other widget in the lower right corner.
Reproduced in a standalone Qt test.

**Fix:** the handle sets an object name (`krimbleResizeHandle`) and is looked up
by that name among the panel's direct children. Same test: no handle -> nothing
found; after creating one -> found.

**Verified:** the lookup in a standalone Qt test. **Not verified:** a build or a
device; `KisMainWindow.cpp` cannot be syntax-checked here. Lesson: a standalone
test covered the handle's resizing but not this lookup; the next test should
create the handle through the same code path as the app.

## 2026-10-02 — A panel's own float button now docks it where it was put

**File:** `libs/ui/KisMainWindow.cpp` (the tracking is Android only)

Device feedback: detached panels "fly away" instead of snapping to the nearest
zone. Settings > Attach Panel already docked a panel where it was put (765acdb),
but the float button in the panel's own title bar still used Qt's default, which
sends the panel back to its old place. That is the likely path that was used.

**Fix:** the app remembers the centre of a floating panel as it is moved. When a
panel is docked again by any route other than Attach Panel, it is docked with the
same placement (nearer side, left or right; between the panels above and below).
Attach Panel sets a flag while it runs so it is not handled twice; the remembered
position is cleared after use. While floating, panels still do not snap anywhere.

**Verified (standalone Qt test, same logic):** a panel that started at the end of
the right column and was put near the top docks first; put in the middle docks
between B and C; put near the left docks on the left; the menu path gives the same
result as before. **Not verified:** a build or a device; `KisMainWindow.cpp` cannot
be syntax-checked here. Dragging a floating panel onto a dock area still does
nothing by design.

## 2026-10-02 — In-app window and tab icon: orange K + paw instead of the rainbow K

**Files:** `krita/pics/branding/Next/sc-apps-krita.svgz` (replaced);
`krita/pics/branding/Next/sc-apps-krita-rainbow-original.svgz` (the old rainbow icon,
kept, not referenced by anything)

Device feedback: the document tab and window icon was still the old rainbow K.
That icon is `krita-branding`, loaded in `KisApplication.cpp` (line 243) and used
for the document tabs (`KisMainWindow.cpp`). It comes from `branding.qrc`, which maps
`krita-branding.svgz` to `sc-apps-krita.svgz`. The Android launcher icon had been
replaced earlier (Sept 19) but this in-app icon had not.

The new `sc-apps-krita.svgz` is an SVG that embeds the orange K + paw art (taken from
`packaging/android/apk/res/drawable-nodpi/ic_launcher_fg.webp`, cropped to the art with
a margin, scaled to 512 x 512, transparent background). Rendered with Qt at 16, 32,
64 and 256 pixels it is clear at every size. The file is 101 KB, against 281 KB for the
old one.

**Limits:** the source art is only 192 pixels across, so very large uses (above about
256 pixels) will be slightly soft; a vector or larger version of the art would fix
that. The desktop PNG icons in the same folder (`16-apps-krita.png` ... `1024-apps-krita.png`)
and the Play Store icon `packaging/android/apk/ic_launcher-playstore.png` (still the
old rainbow gear K at 512 x 512) were NOT changed; the PNGs are not used by the Android
app, but the Play Store icon will need replacing before any Play Store upload.
**Not verified:** a build or a device.

## 2026-10-02 — Smudge, Soften, Dodge, Burn painted plain black (cause found); stale gear-K icons removed

### 1. Smudge / Soften / Dodge / Burn tools

**Files:** `plugins/tools/basictools/kis_tool_{smudge,soften,dodge,burn}.cc`, new
`plugins/tools/basictools/KrimbleEmbeddedPreset.h`

Device feedback: all four tools just paint black, like the freehand brush with a
different icon.

**Cause:** each tool looked its brush preset up by name in the resource database
(`KisResourceModel::resourcesForName`: "smudge", "DFP", "defaultPreset"). Those presets
are built into the app (`plugins/paintops/defaultpresets`, embedded as
`:/presets/<engine>.kpp`); they are not database resources, so the lookup found nothing
and the tools quietly kept the current brush. (The property names the Dodge/Burn
tools set, `CompositeOp` and `PaintOpAction`, do exist in the engine, so those were fine.)

**Fix:** a helper (`krimbleLoadEmbeddedPreset`) loads the embedded preset directly, the
same way `KisPaintopBox::defaultPreset()` does. Smudge uses the Color Smudge engine
(`colorsmudge`), Soften the filter brush ("DFP": gaussian blur, `filter`), Dodge and Burn
the default brush (`paintbrush`) with dodge/burn blending and build-up as before. The old
lookup code is kept as comments in each file.
**Verified:** all four files pass a compiler syntax-only check; the preset names and engine
ids were read from the embedded `.kpp` files. **Not verified:** a build or the tools
actually smudging, softening, dodging or burning on a device.

### 2. Stale icons

At George's instruction the old metallic gear-K icons (with the rainbow ring or the
orange "N" badge) were removed or replaced:
- Replaced with the orange K + paw: `krita/pics/branding/Next/{16,22,24,32,48,64,128,256,512,1024}-apps-krita.png`,
  `krita/pics/branding/Next/krita.ico`, and the Play Store icons
  `packaging/android/apk/ic_launcher-playstore.png` and `ic_launcher_next-playstore.png`
  (white square, K + paw centred). Large sizes are re-sharpened from the 192 pixel launcher art;
  a bigger original would still be better.
- Deleted: the unused branding sets `krita/pics/branding/default`, `Plus` and `Beta`; the
  Next variant's macOS-only `krita.icon` assets; `krita/pics/branding/krita.ico`;
  `krita/pics/branding/generate_icons.sh` (it generated the old icons); and the rainbow
  copy `sc-apps-krita-rainbow-original.svgz` added earlier today.
The build only uses `krita/pics/branding/Next` (BRANDING is forced to Next); a search of the
CMake, script, CI and resource files found no other reference to the deleted files.
**Not changed (still the old art):** the Windows installer assets in `packaging/windows/msix/pkg/Assets`
(45 files) and the macOS icon in `packaging/macos`; neither is used by the Android build.

## 2026-10-03 — Support notice that opens at start now goes to Buy Me a Coffee, not Krita

**Files:** `packaging/android/apk/src/org/krimble/android/DonationProductView.java`,
`packaging/android/apk/res/values/strings.xml`

George pointed out that the support notice shown when the app starts sent people to
Krita instead of his Buy Me a Coffee campaign.

**Found:** the start-up "Support Krimble" dialog (Java, Android) shows two fallback
buttons when Google Play billing is not available. "Join the Fund" opened
`https://fund.krita.org/` and "Donate" opened `https://krita.org/en/donations`; the
Donate text also described a PayPal donation on Krita's page.

**Changed:** the fallback list is now one product, "Donate", that opens
`https://www.buymeacoffee.com/GeorgeEdwardPurdy` (the same link the Start page and the
splash already used). Its description now reads "Help Krimble's continued development
with a donation through Buy Me a Coffee." The old two-button list and the old wording are
kept as comments. Strings file is well-formed XML.

**Already correct (checked):** the Start page "Support Krimble" link and the splash
screen link both already point to Buy Me a Coffee; the README does too.

**Still open, not changed:**
- The Google Play billing products (Supporter Subscription, one-time Supporter Pack,
  supporter badge) and the Settings menu entries "Manage Supporter Benefits..." and
  "Manage Subscriptions..." are Krita's Google Play purchase flow. Their product ids belong
  to Krita's Play Console account, so they cannot work for Krimble's own Play account. Decide
  whether to hide them for now.
- The one-time pack is still titled "Krimble 1.0.0-alpha1 Supporter Pack" and promises
  resource bundles (brushes, "Digital Atelier") that Krimble does not sell.
**Not verified:** a build or a device.

## 2026-10-03 — Toolbox resize bar and panel edge could not be grabbed: system back gesture excluded at the screen edges (theory, not confirmed)

**File:** `packaging/android/apk/src/org/krimble/android/MainActivity.java` (new method
`installEdgeGestureExclusion`, called at the end of `onCreate`; Android 10 and up)

Device feedback: the bar that resizes the toolbox, the frame edge of the panels, and
dialog edges "can no longer be grabbed", even with the phone's magnifier on.

**Theory:** Android's back gesture takes any touch that starts at the left or right screen
edge (about 30 to 40 dp, depending on the phone's gesture sensitivity setting). Since
the toolbox became a single narrow column (16 px icons, 2026-10-01), its resize bar sits
inside the left edge zone, and the panels' outer edge is in the right edge zone, so the
system takes those touches. The wider two-column toolbox kept the bar further in. Nothing
in the app's own event code was changed in a way that could explain the loss of both.

**Change:** a 64 dp wide strip at the middle of each side is excluded from system
gestures (`setSystemGestureExclusionRects`, updated whenever the window is laid out). The
system allows at most 200 dp of height per edge, so only the middle 200 dp of each edge
can be grabbed; the rest of the edge still triggers the back gesture. Java syntax checked
with a parser only.

**How to test the theory without a build:** Settings > Display > Navigation bar > switch to
"Buttons" (3-button navigation). If the bars can be grabbed then, the theory is right.
**Not verified:** a build or a device. If it is wrong, the cause is somewhere else.

## 2026-10-03 — Stale Windows and macOS icon art deleted (George's instruction)

**Deleted:** all 51 files in `packaging/windows/msix/pkg/Assets` (Windows Store tile and file
icons: Square44/71/150/310, Wide310x150, StoreLogo, fileicon, in all scales) and
`packaging/macos/KritaIcon.icns` (the macOS disk image volume icon). All were the old Krita art.
Nothing in the Android build uses them (the CMake, script and CI files searched). The Windows
and macOS packaging scripts still mention these paths (`build_msix.py` and
`macos-apptodmg.py` copy them), so building Windows or macOS packages will need new icons
first. **Not verified:** a build.

## 2026-10-03 — Build stamp: the version shown by the app now changes with every build

**Files:** new `libs/version/KrimbleBuildStamp.cmake`; `libs/version/CMakeLists.txt`;
`libs/version/KritaVersionWrapper.cpp`

George asked why the version always reads "1.0.0 beta 2" when a new build number is made
every build. Reason: the version text is a fixed string in `CMakeLists.txt`
(`KRITA_VERSION_STRING`), and the git hash shown next to it is only refreshed when CMake
reconfigures, which does not happen on most rebuilds (that is why the splash kept showing the
hash `3a607b4` and `38d3800` on newer builds).

**Change:** a small CMake script runs on every build (target `krimble_build_stamp`, part of
the normal build) and writes `krimble_build_stamp.h` with the build time as `YYMMDD-HHMM` in
Mountain time (the same clock as the APK file names) and the short hash of the checked-out
commit. `KritaVersionWrapper::versionString(true)`, the text shown in the splash screen, logs,
crash and bug reports, now reads for example
**`1.0.0-beta2 build 261003-1905 (git 76c7150)`**.
`versionString(false)`, the plain version used inside saved files and the resource database,
is not changed. If the generated header is missing the old format is used.

**Cost:** each build recompiles one small file and relinks one small library.
**Verified:** the script run here gives the right Mountain time and git hash and leaves the git
part out when there is no checkout; the version text was compiled and run with stand-in headers.
**Not verified:** a real build, and how the longer text fits on the splash screen (about 42
characters instead of 26; it is drawn right-aligned over the image). The CMake file change
makes the next build reconfigure CMake once.

### Update 2026-10-03: build number is now a counter (replaces the date-time format above)

George's decision: instead of the build time, add one to a number on every new build.
`KrimbleBuildStamp.cmake` now keeps a counter in `~/krimble-build-number.txt` (home folder of
whoever builds, so it survives a clean build folder). Each build adds one; if the file is
missing it starts at 1. To start from another number, write the number BEFORE the one wanted
into that file (for example `echo 9 > ~/krimble-build-number.txt` makes the next build 10).
The app now shows **`1.0.<counter>-beta (<Month><Day>, git <hash>)`**, for example
`1.0.58-beta (Oct3, git 76c7150)`. The "-beta" comes from the beta flag in `CMakeLists.txt`.
The plain internal version (`1.0.0-beta2`, used in saved files and the resource database)
is unchanged.
**Verified:** three runs of the script gave 1, 2, 3; a start value of 9 gave 10; the text was
compiled and run with stand-in headers. **Not verified:** a real build.
**Note:** the counter goes up on every `make` run, including one that is repeated after a
failed build; re-running only the packaging step does not change it. Suggested APK name:
`Krimble-Beta2-Oct3-1447-b58.apk` (the last part is the counter).

## 2026-10-03 — Dialogs that opened above the screen are now kept on screen (Android)

**File:** `libs/ui/KisApplication.cpp` (class `KrimbleDialogKeeper`, Android only)

Device feedback: new dialogs sometimes pop up partly above the top of the screen, so their
title bar and edges cannot be grabbed.

**Change:** an app-wide event filter watches every top-level dialog. After it is shown, and again
after it is resized, the dialog is made no larger than the usable screen area and moved so its
top-left corner is on screen (checked after the current event, once the dialog has its final
size and place). Dialogs that are already fine are not touched.
**Verified (standalone Qt test, same code, 800 x 600 test screen):** a dialog asked for at y -180
ended at y 2; one at x -120 ended at x 2; one past the bottom-right moved fully inside; a
1500 x 1200 dialog became 800 x 600; a dialog already inside stayed where it was.
**Not verified:** a build or a device; `KisApplication.cpp` could not be syntax-checked as a whole in
the sandbox (the checker lacks some brush headers); the new class compiled and ran on its own.

## 2026-10-03 — Hide / Show Right Panels (menu) and tap-the-bar behaviour (Android)

**Files:** `libs/ui/KisMainWindow.cpp` (helper code marked BEGIN/END KRIMBLE RIGHT PANELS, and
the action registered next to Attach Panel), `krita/krita5.xmlgui` (one new action line, added
with George's permission)

George's idea: collapse the whole right-hand panel column without closing the toolbox; a tap on
the sizing bar hides/unhides the side panels, a drag still resizes.

**Menu:** Settings > "Hide Right Panels" (becomes "Show Right Panels" while they are hidden),
placed under Attach Panel. It hides every docked panel in the right-hand area and remembers which
ones, so exactly those come back. The toolbox (left side) is never touched. After a restart, when
nothing is remembered, "Show Right Panels" shows Tool Options, Layers and Color.
**Tap on the bar (Android only):** a press and release without moving (under 12 px) on the bar
between the canvas and the right panels hides them. A drag is not touched, so it resizes as
before. The event is never consumed.
**Handle:** while the right panels are hidden, a small handle (40 x 180) sits at the right edge of
the window, vertically centred; a tap on it shows the panels again. It disappears as soon as any
right panel is visible (also when one is shown by hand from Settings > Panels).
**Verified (standalone Qt test, the same code, a main window with a toolbox on the left and three
panels on the right):** tap on the bar hid the three right panels and left the toolbox; the handle
appeared; a tap on the handle brought all three back; a 60 px drag on the bar did nothing; a tap
on the canvas did nothing; the menu action toggled and its text changed; showing one panel by hand
hid the handle; with nothing remembered, "Show" brought back the usual three.
**Not verified:** a build or a device; `KisMainWindow.cpp` as a whole cannot be syntax-checked in
the sandbox (the extracted code compiled with the translation call stubbed). Whether Qt's own
bar-drag on a phone still works is the open problem from Oct 2; this change does not touch it.
`krita5.xmlgui` checked as well-formed XML.

### Update 2026-10-03: the side button replaces the tap-on-Qt's-bar idea (George: "long press is drag, a tap is short")

**File:** `libs/ui/KisMainWindow.cpp` (block marked KRIMBLE RIGHT PANELS; Android only)

George wanted the slender dotted button on the side made easier to press: a short tap should
hide/show the side panels, a long press should drag (resize). The first version (`eeb51e1`) only
watched taps on Qt's own thin bar, which was already hard to grab on the phone, so it is replaced.

**Now:** a real button, 52 x 170 px, laid over the bar between the canvas and the right-hand
panels, halfway down the panels. Quick tap (under 16 px of movement): hides all the right-hand
panels (the toolbox is not touched). While they are hidden the same button sits at the right edge
with an arrow; a tap shows them again. Long press (hold 0.35 s) turns it into a drag handle (it
turns brighter); moving the finger sideways then changes the width of the panel column (left =
wider). A quick sideways swipe of more than 16 px starts the drag without waiting. A long press
without moving does nothing. Dragging from the edge button while the panels are hidden shows them
first. The menu entry Settings > Hide/Show Right Panels is unchanged and works on every platform.
**Verified (standalone Qt test, the same code, realistic finger positions):** quick tap hides and
shows; long press + 90 px left grew the column as far as the layout allows (the test window was
too small for the full 90); long press + 120 px right shrank it by exactly 120; a quick 40 px swipe
widened it by exactly 40; long press without movement changed nothing; dragging from the hidden
edge button showed the panels and resized them.
**Not verified:** a build or a device (feel of the 0.35 s hold, the button size, whether it covers
something you need at the left edge of the panels); `KisMainWindow.cpp` cannot be syntax-checked
as a whole in the sandbox.

## 2026-10-03 — Seven tools removed from the top of the toolbox so Move is the first button

**Files:** `plugins/tools/karbonplugins/tools/KarbonToolsPlugin.cpp`,
`plugins/tools/tool_knife/ToolKnife.cpp`, `plugins/tools/tool_dyna/tool_dyna.cpp`,
`plugins/tools/tool_polyline/tool_polyline.cc`, `plugins/tools/basictools/default_tools.cc`,
`libs/ui/toolbox/KoToolBox.cpp`

George: the first tool should be Move; the group above the industry-standard tools should go.
That group was two toolbox sections: "Main" (Select Shapes, Calligraphy, Comic Panel Editing)
and "Shape" (Dynamic Brush, Polyline, Multibrush, Pencil).

**Changed:** the registration line of Calligraphy, Comic Panel Editing, Dynamic Brush, Polyline,
Multibrush and Pencil is commented out (same method as the Type tool in September); they no
longer exist in the app, so they have no toolbox button, tool options or shortcuts. To bring one
back, uncomment its line. **Select Shapes** (`InteractionTool`) is NOT unregistered: the tool
manager picks it as the default tool when a document opens and other code switches to it. Instead
`KoToolBox::addButton` skips it, so it has no button. The toolbox layout already skips an empty
section, so no stray divider line is expected above Move.
**Checked:** other code only mentions these tool ids as text (a list in `kis_node_manager.cpp`
and the recorder's tool list) or in `krita5.xmlgui` toolbar line 1084 (`KritaShape/KisToolMultiBrush`),
which will simply find no action; no menu depends on them.
**Syntax:** the five plugin files pass the compiler check; `KoToolBox.cpp` stops at a build-generated
`moc_` file that does not exist in the sandbox (not caused by this change).
**Not verified:** a build or a device. **Not changed:** the order of the remaining groups (the
industry-standard block now comes first; the Krita-only groups still sit after Zoom).

## 2026-10-03 — Six new toolbox icons that read like the industry-standard tools

**Files (same names, light and dark versions each, 12 files) in `krita/pics/tools/SVG/16/`:**
`krita_tool_move` (Move: pointer arrow plus the four-way cross), `tool_outline_selection`
(Lasso: loop with a rope tail), `krita_draw_path` (Pen: pen nib), `krita_tool_smart_patch`
(Healing Brush: adhesive bandage), `krita_tool_smudge` (Smudge: pointing finger),
`shape_handling` (Direct Selection: hollow arrow).

George: toolbox icons should look like the tool in the industry-standard editor where Krita's did
not. These are original drawings, not copies of that editor's artwork; they use the same
well-known symbols. 16 x 16 grid, round caps, strokes 1.2 to 1.5 so they stay readable at
16 pixels; light theme version #d2d2d2, dark theme version #373737, the same colours as the
neighbouring icons. George approved the look from an on-screen preview (72, 32 and 16 pixels).
The old icons are in git history (the commit before this one). Note that these icon names are
shared: the same file is used wherever the app shows that icon (for example the Lasso icon also
appears for the outline selection action in menus).
**Checked:** all 12 files are well-formed XML and render in the preview. **Not verified:** a build
or a device (how they look on the real toolbox).

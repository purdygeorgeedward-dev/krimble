![Krimble](krimble-logo.png)

Krimble (formerly Krita Mobile) is a mobile-first fork of Krita designed to be a graphics workhorse for Android phones instead of a paint/animation app. The objective is to make a mobile version which is aware of the restrictions of limited screen real estate on mobile devices. Stripping away unnecessary UI components and scaling down others, creating new default behaviors that show awareness of mobile users needs, and making the UI a little more universal are objectives.

I am a lifelong professional artist trained with a wide range of traditional media and digital tools from Deluxe Paint to Photoshop to 3DS Max, Mudbox, ZBrush, et al. and to me trying to "paint" with a phone is about as appealing as writing a novel on a window with a bar of soap. However, I find myself using Krita on my smartphone as a daily driver for graphics tasks, so I am motivated to modify it for my own purposes. Sharing it, of course.

[![Buy Me a Coffee](https://krimble.org/buy-me-a-coffee.png)](https://www.buymeacoffee.com/GeorgeEdwardPurdy)

https://krimble.org

### Krita User Manual
https://docs.krita.org/en/user_manual.html

### Krita Development Notes and Build Instructions

Please follow [the online documentation](https://docs.krita.org/en/untranslatable_pages/building_krita.html).

Other developer guides, notes and wiki:

https://docs.krita.org/en/untranslatable_pages.html

APIdox:

https://api.kde.org/legacy/krita/html/index.html

### Bugs and Wishes

Wishlist:

1. Change default tool on open from paint tool to pan/hand tool. (see below)

2. Smaller splash image on load so it fits mobile screens. (DONE.)

3. Two finger move of any open window. (DONE — see item 32.)

4. Remove Tools menu to make the Settings menu easier to reach. (DONE.)

5. Change Toolbox default sort order. (DONE. Pan tool given lowest `priority`/`toolBoxPriority` in `kis_tool_pan.cpp`. Also found and fixed a separate bug forcing the Brush tool active every time a pixel layer was selected, in `KisNodeManager::slotUiActivatedNode` — that logic is now disabled with a comment rather than forcing any tool.

6. Disable autoload of file recovery on startup because mobile users don't tend to close apps with Quit/Exit. (DONE — already commented out in `KisApplication.cpp`: the `checkAutosaveFiles()` call that pops the recovery dialog on launch is inert. No other call site triggers it. Confirmed, no code change needed.)

7. Move Configure Krita menu and rename Preferences. (DONE. Preferences now lives under the Edit menu in `krita5.xmlgui')

8. Reconfigure menus. (DONE. Select > Shrink renamed to Contract..., Layer > Merge Layer renamed to Merge Down, Filter menu categories remapped from Krita's own taxonomy (Artistic/Colors/Edge Detection/Emboss/Enhance/Map) to (Blur/Distort/Noise/Pixelate/Render/Sharpen/Stylize/Other), two dead menu categories with zero registered filters removed (Decor, Non-Photorealistic), and five color-adjustment filters — Index Colors, Posterize, Gradient Map, Palettize, Normalize — moved out of the Filter menu into Image > Adjustments.)

9. Remove rotation from pinch zoom defaults. (DONE.)

10. Limit transform default to scaling. (DONE. In `kis_free_transform_strategy.cpp`, an unmodified drag near an edge — not on an explicit scale handle — used to fall through to shear based on proximity alone, easy to trigger by accident with imprecise touch input. Rather than removing shear-by-drag outright, it's now gated behind a new "Allow Shear by Dragging" checkbox in the Free Transform tool options, off by default — same persisted-toggle pattern as the existing mesh-transform `chkScaleHandles` checkbox. Also fixed a real uninitialized-variable bug found along the way: `defaultFunction` was left unset when the cursor was outside the shape and Alt wasn't held.)

11. Limit move default to selected layer or floating selection until deliberately changed by user. (DONE. User reported the actual reproduction (select Move tool, drag a selection, drag again and the whole image moves) and it traced to a real bug: `kritadefault.profile` mapped a one-finger long-press to `TertiaryAlternateModeShortcut` → `KisTool::AlternateFourth`, and `KisToolMove::beginAlternateAction()` sent every unhandled alternate action — not just that one — into an unconditional whole-image `MoveGroup` stroke. A second tap lingering even slightly on a touchscreen could register as a hold and silently trigger it. Fixed both the touch binding (removed) and the code (catch-all now a no-op, commented out per standing preference rather than deleted). Krita has no distinct "floating selection" object — paste creates a layer directly, and Move already respects whatever selection mask is active on the target layer, so that half of the original requirement needed no separate handling.)

12. Implement industry standard terms for tools or menu items as needed. (DONE.)

13. Default to Snapping OFF. (DONE.)

14. Add zoom to 200% in menu. (DONE. TESTED. WORKS.)
    
15. Create custom default toolbar in imitation of classic toolbar. (DONE.)
    
16. Add the Adjust-Brightness/Contrast dialog (DONE. TESTED. WORKS.)

17. Widen sizing gadgets on windows and sections to make touchscreen resizing easier. (DONE. Two separate fixes: Crop tool's resize handles (`m_handleSize` in `kis_tool_crop.h`/`.cc`) went from 13px, sized for a mouse cursor, to 44px, matching standard mobile touch-target guidance — a near-miss previously made the tool discard the crop rect and start drawing a new one from scratch instead of grabbing the handle. Separately, dock panel resize borders were doubled app-wide via a new `KisWideDockSeparatorStyle` proxy style overriding `PM_DockWidgetSeparatorExtent`, applied in `KisApplication.cpp` — Qt ties a dock separator's visible width and its resize hit-test zone to the same single pixel metric, so there's no way to widen only the grab zone without also widening what's drawn on screen, unlike the crop tool where those were decoupled.)

18. Rework the text/type tool, which is very hard to use currently. (Substantially reworked. Type menu reconstructed (Orientation, Anti-Alias, Panels > Glyphs), fake/nonexistent items dropped rather than faked. Shared handle-radius/grab-sensitivity touch-target defaults bumped app-wide. Triple-tap-to-select-paragraph added (double-tap-to-select-word already worked via Qt's own touch-to-mouse synthesis). Draggable mobile-style selection-endpoint handles added, matching the two "teardrop" handles every phone keyboard shows. Floating Cut/Copy/Paste/Select All quick-action bar added on selection. Remaining, not yet built: a magnifier/loupe while dragging a cursor or selection handle, so a fingertip doesn't block the view of exactly where the cursor will land -- the one idea from the original five that wasn't attempted.)

19. Create Krimble logo splash. (DONE.)

20. Rewrite larger dialogs windows to fit small screens. (DONE. Surveyed every dialog's actual top-level `QDialog` geometry and found 8 genuinely oversized for a phone screen: `recorder_profile_settings`, `bbdkss` (script starter), `KisDonationManagementDialog`, `kis_dlg_brush_hud_config`, `excepthook`, `KisSupporterBundlesDialog`, `wdgcustombrush`, `wdgclipboardbrush`. Most had no hard size constraint at all — just a large Qt Designer initial-geometry hint, freely resizable in practice — trimmed to ~380px width. One real blocker: `bbdkss.ui`'s top-level dialog was locked `Fixed`/`Fixed` at 607×430, unable to resize at all regardless of screen size; relaxed to `Preferred`/`Preferred`. Checked each dialog's child widgets for minimum-width constraints that would force overflow before changing anything — none found beyond a couple of small 110×110 Fixed preview thumbnails and unconstrained Fixed `QLineEdit`s, both harmless.)

21. Create new icon for Krimble. (DONE.)

22. Trim list of available save formats. (DONE.)

23. Item 1 (default tool) had never actually been applied to code — the hardcoded call in `libs/ui/KisView.cpp` still forced `KritaShape/KisToolBrush` on every new view. Fixed to `PanTool` (hand tool) to match item 1. (DONE.)

24. Item 21/24 (new icon) only replaced `ic_launcher`/`ic_launcher_round` at every density. The app was still falling back to the separate `ic_launcher_next`/`ic_launcher_next_round` mipmaps AND the adaptive-icon foreground/background vector drawables (`ic_launcher_next_foreground.xml`, `ic_launcher_next_background.xml`), which still had the old placeholder Krita mark baked in as vector paths. All four Next-variant assets (mipmap webp x5 densities, plus the two adaptive vector drawables) now mirror the already-fixed regular assets. (DONE.)

25. `KRITA_ALPHA` flag was left set in `CMakeLists.txt`, which drove the "DEV BUILD" welcome-screen label. Commented out — this was the actual cause, confirmed on the build server (Qt5, `BUILD_WITH_QT6` was never enabled, defaults OFF). Originally suspected a Qt6-specific carve-out in `KritaVersionWrapper::isDevelopersBuild()` was involved; it wasn't, since the build has been Qt5 all along, but the carve-out was removed anyway so the check no longer depends on Qt major version. Decision: stick with Qt5 (Qt6 isn't production-ready upstream either). (DONE. Superseded in part by item 33 — `KRITA_ALPHA` was deliberately re-enabled once the build was actually versioned as an alpha, so the DEV BUILD label is back by design, not a regression.)

26. Splash screen still showed "Artwork by: Tyson Tan" — hardcoded in `libs/ui/kis_splash_screen.cpp` regardless of which splash image resource was actually loaded, so replacing the splash graphic alone never removed it. Cleared the credit string since Krimble's splash is an original asset. (DONE.)

27. Aligned every hardcoded keyboard shortcut default to industry standard where a non-colliding key exists. Corrected ~200 shortcuts baked into `.action` XML files (which override the `photoshop_compatible.shortcuts` scheme when that scheme doesn't explicitly list them), synced the scheme file itself, and set the default scheme in `kis_action_registry.cpp` instead of Krita's own `Default`. (DONE.)

30. Reassigned tool-selection shortcuts  where possible: Move (V), Marquee (M/Shift+M), Lasso (L/Shift+L), Magic Wand/Similar (W/Shift+W), Crop (C), Eyedropper (I), Brush (B), Gradient (G), Pen (P), Type (T), Zoom (Z), Hand/Pan (H). Where a Krita-only global shortcut already held the letter needed (brush opacity, brush color lighter, mirror canvas, MyPaint shade selector, instant preview mode, wrap-around mode, common colors), relocated those seven Krita-only actions to modifier-heavy combos instead of settling for an approximate tool key, so the letter is free. Note: the other tool cycles multiple tools under one key via repeated presses (e.g. Shift+M twice); Krimble's toolbox binds one key per tool with no cycling, so this is an exact-key match, not full cycling behavior. (DONE.)

31. Replaced KDE's "docker" with "panel" everywhere it appeared as user-facing text — menu labels, tooltips, dialog strings, translatable UI strings (18 files). Left internal class/widget/object names and the `plugins/dockers/` directory structure untouched since renaming those has no user-visible benefit and risks breaking signal/slot wiring. (DONE.)

32. Two-finger move of any open window/panel (see item 3). (DONE. `KisDockerHud` now handles `QEvent::TouchBegin`/`TouchUpdate`/`TouchEnd`/`TouchCancel` via an `event()` override, claiming the gesture only when exactly two touch points are active — a single-finger tap on its combo box or menu button passes through normally. Delta is computed in screen coordinates, not widget-local, since local coordinates shift under the fingers as the widget itself moves mid-drag. The drag moves `window()`, not the `KisDockerHud` instance itself — it's normally embedded in a parent layout (`KisPopupButtonFrame`, when hosted via `KisPopupButton::setPopupWidget()`) rather than being independently top-level, so moving the instance directly would've been silently overridden by that layout; confirmed `KisPopupButtonFrame` sets `Qt::Dialog`/`Qt::Popup` window flags, both genuine top-level types, so `window()` resolves correctly. No conflict with canvas pan/zoom's own two-finger handling since Qt routes touch events by which widget the gesture started on.)

33. Version renumbered from Krita's inherited `5.4.0-prealpha` to Krimble's own `1.0.0-alpha1` in `CMakeLists.txt` (both the Qt5 and Qt6 branches, major/minor versions reset to 1/0). `KRITA_ALPHA` re-enabled since the build is now honestly alpha — this restores the "DEV BUILD" watermark on the welcome screen (see item 27's note). (DONE.)

34. Full pass replacing user-facing "Krita" references with "Krimble" — menus, dialogs, tooltips, About dialog, Android donation/IAP strings, ~35 files. Found and fixed the biggest miss: the actual Android app label in `AndroidManifest.xml` (home screen icon, app drawer, app switcher) was still "Krita" despite everything else being rebranded; also fixed the "Krita Next" build-flavor label to "Krimble Next". Deliberately preserved: historical `.kra` file-format version-compatibility notes (e.g. "Creamy (Krita 4.2+)") since those name real upstream milestones, not branding; the SVG/KRA XML namespace URI (`http://krita.org/namespaces/svg/krita`) and Qt's `organizationDomain("krita.org")` since those are internal identifiers, not user-facing text, and changing them risks breaking file-format compatibility; code comments citing real upstream KDE commit URLs for bug-fix attribution; and the welcome page's paragraph crediting real Krita's actual contributors/sponsors/development fund (skipped on request — text there still says "Krita"). (DONE.)

35. All real krita.org/docs.krita.org/krita-artists.org links replaced: donation links now point to buymeacoffee.com/GeorgeEdwardPurdy, the Source Code link on the welcome page now points to the real Krimble GitHub repo instead of Krita's, and everything else (manual, community, scripting school, bug-report guide, RSS news feeds) points to krimble.org as a placeholder pending those actually being built. (DONE.)

36. Removed a leftover from an earlier partial edit that invented a nonexistent legal entity. Replaced with "The Krimble Project is committed to preserving Krimble as free software." (DONE.)

37. Full toolbox visual reordering. Krita's toolbox is grouped into sections (Shape/Transform/Fill/View/Select/Main/Navigation) that render in alphabetical order of their internal string keys (`QMap` in `KoToolBox.cpp`), with per-section priority controlling order only within a section — so there was previously no way to get one continuous list across the whole toolbox. Added a new unified `ToolBoxSection::PSOrder` and moved every tool with a real equivalent into it (Move, Marquee, Lasso, Wand, Crop, Eyedropper, Healing Brush, Brush, Gradient, Paint Bucket, Pen, Path Selection, Type, Shape tools, Hand, Zoom), sequentially prioritized. Tools with no equivalent keep their existing sections and now sort after this block.

Also fixed four stale factory-level shortcuts found along the way (a separate mechanism from the `.action` XML system fixed earlier) that still held pre-alignment keys: Move (was T, now V), Eyedropper (was P, now I), Rectangular Marquee (was Ctrl+R, now M), Elliptical Marquee (was J, now Shift+M). (DONE.)


### Krita Project Website

https://www.krita.org

### License

Krita as a whole is licensed under the GNU Public License, Version 3. Individual files may have a different, but compatible license.

### AI

Krita development has a moratorium on use of AI.

I don't. Is it likely to cause problems? Yes.

Move fast and break things.

I'll clean up the mess as I go.








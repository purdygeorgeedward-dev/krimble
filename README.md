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

Apidox:

https://api.kde.org/legacy/krita/html/index.html

### Bugs and Wishes

Wishlist:

1. Change default tool on open from paint tool to hand tool. (DONE.)

2. Smaller splash image on load so it fits mobile screens. (DONE.)

3. Two finger move of any open window. (Implemented, untested.)

4. Remove Tools menu to make the Settings menu easier to reach. (DONE.)

5. Change Toolbox defaults. (DONE. Added new value setToolBoxPriority for control of toolbox placement separate from priority value which controls default tool behavior on file open.)

6. Disable autoload of file recovery on startup because mobile users don't tend to close apps with Quit/Exit. (DONE.)

7. Set HiDPI setting default to off. (DONE.)

8. Reconfigure menus. (DONE.)

9. Remove rotation from pinch zoom defaults. (DONE.)

10. Limit transform default to scaling. (DONE.)

11. Limit move default to selected layer or floating selection until deliberately changed by user. (DONE.)

12. Implement industry standard terms for tools or menu items as needed. (DONE.)

13. Default to Snapping OFF. (DONE.)

14. Default to 12 pt. Helvetica in UI. (must check on Helvetica source)
    
15. Create custom default toolbars in imitation of classic toolbars. (DONE.)
    
16. Add the Adjust-Brightness/Contrast dialog (implemented , untested.)

17. Widen sizing gadgets on windows and sections to make touchscreen resizing easier.(implemented, untested)

18. Rework the text/type tool, which is very hard to use currently. (TBD)

19. Create alternate title and logo splash in case Krita org opposes use of "Krita" for new mobile version or use of AI in rapid dev. (DONE. Not added to codebase yet.)

20. Rewrite larger dialogs windows to fit small screens.

21. Create new icon for mobile version. (DONE.)

22. Create Zoom 200% tool to match Zoom 100%. (DONE.)

23. Trim list of available save formats. (DONE.)

24. New icon. (DONE.)

25. Item 1 (default tool) had never actually been applied to code — the hardcoded call in `libs/ui/KisView.cpp` still forced `KritaShape/KisToolBrush` on every new view. Fixed to `PanTool` (hand tool) to match item 1. (DONE.)

26. Item 21/24 (new icon) only replaced `ic_launcher`/`ic_launcher_round` at every density. The app was still falling back to the separate `ic_launcher_next`/`ic_launcher_next_round` mipmaps AND the adaptive-icon foreground/background vector drawables (`ic_launcher_next_foreground.xml`, `ic_launcher_next_background.xml`), which still had the old placeholder Krita mark baked in as vector paths. All four Next-variant assets (mipmap webp x5 densities, plus the two adaptive vector drawables) now mirror the already-fixed regular assets. (DONE.)

27. `KRITA_ALPHA` flag was left set in `CMakeLists.txt`, which drove the "DEV BUILD" welcome-screen label. Commented out. `KritaVersionWrapper::isDevelopersBuild()` also had a hardcoded Qt6 exception that ignored `KRITA_STABLE` entirely on Qt6 builds — removed so it works regardless of Qt major version. (DONE — confirm actual Qt version used on the build server.)

28. Splash screen still showed "Artwork by: Tyson Tan" — hardcoded in `libs/ui/kis_splash_screen.cpp` regardless of which splash image resource was actually loaded, so replacing the splash graphic alone never removed it. Cleared the credit string since Krimble's splash is an original asset. (DONE.)


### Krita Project Website

https://www.krita.org

### License

Krita as a whole is licensed under the GNU Public License, Version 3. Individual files may have a different, but compatible license.

### AI

Krita development has a moratorium on use of AI.

I don't, and I'm more of an anarchist, so I'll do whatever I think works. Is it likely to cause problems? Yes.

Move fast and break things.








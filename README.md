![Picture](http://purdy.design/logo_splash.jpg)

This (soon to be renamed) is a mobile-first fork of Krita designed to be a graphics workhorse for Android phones instead of a paint/animation app. The objective is to make a mobile version which is aware of the restrictions of limited screen real estate on mobile devices. Stripping away unnecessary UI components and scaling down others, creating new default behaviors that show awareness of mobile users needs, and making the UI a little more universal are objectives.

I am a lifelong professional artist trained with a wide range of traditional media and digital tools from Deluxe Paint to Photoshop to 3DS Max, Mudbox, ZBrush, et al. and to me trying to "paint" with a phone is about as appealing as writing a novel on a window with a bar of soap. However, I find myself using Krita on my smartphone as a daily driver for graphics tasks, so I am motivated to modify it for my purposes. Sharing it, of course.

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

3. Two finger move of any open window? 

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

14. Default to 12 pt. Helvetica in UI.
    
15. Create custom default toolbars in imitation of classic toolbars. (DONE.)
    
16. Add the Adjust-Brightness/Contrast dialog (core implementation done, untested.)

17. Widen sizing gadgets on windows and sections to make touchscreen resizing easier.

18. Rework the text/type tool, which is very hard to use currently.

19. Create alternate title and logo splash in case Krita org opposes use of "Krita" for mobile version or use of AI in rapid dev. (DONE.)

20. Rewrite dialogs windows to fit small screens.

### Krita Project Website

https://www.krita.org

### License

Krita as a whole is licensed under the GNU Public License, Version 3. Individual files may have a different, but compatible license.

### AI

Krita development has a moratorium on use of AI.

I don't, and I'm more of an anarchist, so I'll do whatever I think works.



![Picture](http://purdy.design/logo_splash.jpg)

| CI Name     | Master | Stable | Release |
| ------------------- | ---------------- | ------ | ------- |
| Pipeline | [![pipeline status](https://invent.kde.org/graphics/krita/badges/master/pipeline.svg)](https://invent.kde.org/graphics/krita/-/commits/master) | [![pipeline status](https://invent.kde.org/graphics/krita/badges/krita/5.2/pipeline.svg)](https://invent.kde.org/graphics/krita/-/commits/krita/5.2) | [![Latest Release](https://invent.kde.org/graphics/krita/-/badges/release.svg)](https://invent.kde.org/graphics/krita/-/releases) |

Note: Nightly builds are not covered by this table atm

Krita Mobile is intended to be a mobile fork of Krita designed to be a graphics workhorse for Android phones instead of a paint app. The objective is to make a mobile version which is aware of the restrictions of limited screen real estate on mobile devices. Stripping away unnecessary UI components and scaling down others, creating default behaviors that show awareness of mobile users needs, and making the app a little more universal are objectives.

If you are reading this on GitHub, be aware that this is just a mirror. The core code repository is provided by KDE: https://invent.kde.org/graphics/krita.git


### Repository Status

For branch: `master`

| Freeze type    | Status                                                               |
|----------------|----------------------------------------------------------------------|
| Feature Freeze | features are allowed                                                 |
| String Freeze  | strings are allowed                                                  |


### User Manual
https://docs.krita.org/en/user_manual.html

### Development Notes and Build Instructions

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

6. Disable autoload behaviors.

7. Add note about disabling HiDPI setting.

8. Reconfigure menus. (DONE.)

9. Remove rotation from pinch zoom defaults.

10. Limit transform default to scaling.

11. Limit move default to selected layer or floating selection until changed by user.

12. Implement industry standard terms for tools or menu items as needed. (DONE.)

13. Default to Snapping OFF.

14. Default to 12 pt. Helvetica in UI.
    
15. Create custom default toolbars in imitation of classic toolbars. (DONE.)
    
16. Add the Adjust-Brightness/Contrast dialog (core implementation done, untested.)

17. Widen sizing gadgets on windows and sections to make touchscreen resizing easier.


### Discussion Forum

* https://krita-artists.org/

### IRC channel

Most of the developers hang out here. If you are interested in helping with the project this is a great place to start.

libera.chat, #krita

### Project Website

https://www.krita.org

### Nightly builds

#### Unstable Desktop Build

* https://cdn.kde.org/ci-builds/graphics/krita/master/

#### Stable Desktop Build

* https://cdn.kde.org/ci-builds/graphics/krita/krita-5.2/

#### Developers builds

##### Linux build with debug symbols in Qt and Krita

1) Go to Jobs section of Krita's CI: https://invent.kde.org/graphics/krita/-/jobs
2) Search for the latest `linux-debug-weekly` job
3) Enter the job and click on Artifacts->Browse
4) Download the AppImage

##### Linux build with ASAN in Qt and Krita

1) Go to Jobs section of Krita's CI: https://invent.kde.org/graphics/krita/-/jobs
2) Search for the latest `linux-asan-weekly` job
3) Enter the job and click on Artifacts->Browse
4) Download the AppImage
5) Set up environment variable for ASAN:
    ```bash
        export ASAN_OPTIONS=new_delete_type_mismatch=0:detect_leaks=0
    ```
6) Run the AppImage in the modified environment


### License

Krita as a whole is licensed under the GNU Public License, Version 3. Individual files may have a different, but compatible license.

### AI

Use common sense. Ignore cultural hysteria.

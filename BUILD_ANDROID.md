# Building Krimble for Android

This replaces README.android.md, which documents the old upstream Krita
build (NDK r18b, androidbuild.sh, armeabi-v7a) — none of that applies
here anymore.

## Server
- Oracle Cloud instance `instance-20260819-1011`
- IP 129.151.24.131, user `ubuntu`, Ubuntu 24.04, 4 OCPU / 24GB RAM
- SSH: ssh -i ~/.ssh/ssh-key-2026-08-19.key ubuntu@129.151.24.131

## Directory layout on the server — READ THIS FIRST
- ~/krita — raw source drop, NOT a git repo, NOT connected to GitHub.
  Do not build from here. Do not edit here.
- ~/krimble/app — the actual git checkout of this repo. This is the
  only correct source directory.
- ~/krimble (everything else under it) — dependency/build workspace
  (_install prefix etc.) created by setup-env.py. Not source.

## Toolchain
- Qt5/KF5, NOT Qt6 — BUILD_WITH_QT6 must stay OFF
- Target ABI: arm64-v8a
- Android NDK 27.3.13750724 (already installed on the server)
- Dependencies: prebuilt KDE CI packages fetched into an _install
  prefix via setup-env.py — do NOT introduce kdesrc-build or any
  other toolchain

## Build steps
[UNCONFIRMED — exact commands were never written down anywhere in
this repo or in memory. Needs to be filled in from the server's own
shell history or by running setup-env.py --help. Known pieces:]
1. cd ~/krimble/app
2. git pull
3. Run setup-env.py to fetch/refresh the prebuilt dependency packages
   into the _install prefix — exact flags unconfirmed
4. Run the actual Gradle build — likely `./gradlew assembleRelease`
   from packaging/android/apk, but the exact working directory and
   whether any extra env vars (ANDROID_NDK_HOME, etc.) need to be set
   first is unconfirmed
5. Signed release build requires KRIMBLE_KEYSTORE_PASSWORD and
   KRIMBLE_KEY_PASSWORD set in the server's shell profile, and
   ~/krimble-release.jks present on the server

## Signing
- Keystore: ~/krimble-release.jks (server), alias `krimble`
- Passwords: KRIMBLE_KEYSTORE_PASSWORD / KRIMBLE_KEY_PASSWORD env vars
- Never commit the .jks file or passwords — .gitignore already blocks
  *.jks / *.keystore

## Known open issues
- claude/KRIMBLE-BUILD-CHEATSHEET.md is referenced in KRIMBLE_STATUS.md
  but has never existed in this repo's git history — either lost
  (written locally, never committed) or never actually created
- Last confirmed working state (per KRIMBLE_STATUS.md, 2026-09-06):
  libkrita_arm64-v8a.so compiles and links clean. No confirmed
  packaged APK yet as of that date.

# Building Krimble for Android

This replaces README.android.md, which documents the old upstream Krita
build (NDK r18b, androidbuild.sh, armeabi-v7a) — none of that applies
here anymore.

## Server
- Oracle Cloud instance `instance-20260819-1011`
- IP 129.151.24.131, user `ubuntu`, Ubuntu 24.04, 4 OCPU / 24GB RAM
- SSH: ssh -i ~/.ssh/ssh-key-2026-08-19.key ubuntu@129.151.24.131

## Directory layout on the server — READ THIS FIRST
(Corrected 2026-09-25. Earlier versions of this file said ~/krimble/app
was the checkout — that directory does not exist.)
- ~/krimble — the git checkout of THIS repo. Only correct source dir.
  `git pull` here. setup-env.py also drops workspace files in here
  (env, venv/, cache/, ccache/, krita-deps-management/) — not source,
  untracked, leave them alone.
- ~/kwd — build work dir (KDECI_WORKDIR_PATH). Build output lands in
  ~/kwd/krita/_build and ~/kwd/krita/_install. MUST be outside the
  source tree: androiddeployqt silently drops all Qt QML modules if
  _install sits inside the source root (per upstream android.yml).
- ~/krita — raw source drop, NOT a git repo. Do not build or edit here.

## Server CPU is ARM64 — READ THIS TOO
- `uname -m` = aarch64 (Oracle Ampere). The NDK's host tools are
  x86_64 and run under emulation. The emulated linker (ld.lld)
  segfaults. Symptom: CMake fails with "Host compiler must support
  64-bit std::atomic!" — misleading; the real error is a linker
  segfault in _build/CMakeFiles/CMakeConfigureLog.yaml.
- The NDK's prebuilt/linux-aarch64 folder is an empty stub. Unusable.
- NDK r30 = x86-only host tools. Use 27.3.13750724.
- Fix (one-time, server): replace NDK lld with native Ubuntu lld-18:
  ```
  sudo apt-get install -y lld-18 ccache
  cd ~/Android/sdk/ndk/27.3.13750724/toolchains/llvm/prebuilt/linux-x86_64/bin
  mv lld lld.x86_64.orig
  ln -s /usr/lib/llvm-18/bin/lld lld
  ./ld.lld --version     # expect: Ubuntu LLD 18.x
  ```
  Undo: `rm lld && mv lld.x86_64.orig lld` in the same folder.
  (ld -> ld.lld -> lld symlink chain follows automatically.)

## Toolchain
- Qt5/KF5, NOT Qt6 — BUILD_WITH_QT6 must stay OFF
- Target ABI: arm64-v8a
- Android NDK 27.3.13750724 at ~/Android/sdk/ndk/27.3.13750724
- CMake toolchain file: krita-deps-management/tools/android-toolchain-krita.cmake
  (NOT the ECM Android.cmake, NOT the raw NDK android.toolchain.cmake)
- Dependencies: prebuilt KDE CI packages via setup-env.py — do NOT
  introduce kdesrc-build or any other toolchain

## Build steps
Follows Krita's own CI recipe: build-tools/ci-scripts/android.yml.
Do not hand-patch build.gradle or hand-run cmake/gradle.
All commands run on the CLOUD SERVER.

### Step 1 — fetch prebuilt dependencies (confirmed working 2026-09-25)
```
cd ~/krimble && git pull
python3 krita-deps-management/tools/setup-env.py --full-krita-env --android-abi arm64-v8a -r ~/krimble -o env
```
Then check ~/krimble/env points every NDK variable
(KDECI_ANDROID_NDK_ROOT, ANDROID_NDK_ROOT, ANDROID_NDK_HOME) at
27.3.13750724, not 30.x.

### Step 2 — native build (re-run the whole block in every new SSH session)
```
cd ~/krimble
source ~/krimble/env
export KDECI_CACHE_PATH=/home/ubuntu/krimble/cache
export KDECI_GITLAB_SERVER=https://invent.kde.org/
export KDECI_PACKAGE_PROJECT=teams/ci-artifacts/krita-android-arm64-v8a
export KDECI_WORKDIR_PATH=/home/ubuntu/kwd
export CMAKE_TOOLCHAIN_FILE=/home/ubuntu/krimble/krita-deps-management/tools/android-toolchain-krita.cmake
export KDECI_EXTRA_CMAKE_ARGS="-DHIDE_SAFE_ASSERTS=ON -DBUILD_TESTING=OFF"
export KRITACI_ANDROID_PACKAGE_TYPE=release
export KRITACI_ANDROID_RELEASE_MODE=1
unset VIRTUAL_ENV
nohup python3 -u krita-deps-management/ci-utilities/run-ci-build.py --project krita --branch master --platform Android/arm64-v8a/Qt5/Shared --only-build --skip-publishing > ~/build-krita.log 2>&1 &
tail -f ~/build-krita.log
```
Notes:
- The KDECI_CACHE_PATH / GITLAB_SERVER / PACKAGE_PROJECT values are set
  by setup-env.py in memory only and never written to env — they must
  be exported by hand.
- Do NOT set KDECI_GLOBAL_CONFIG_OVERRIDE_PATH. It points at the deps'
  config and breaks the config merge (TypeError: list indices must be
  integers or slices, not str).
- After a failed configure, `rm -rf ~/kwd/krita/_build` before retrying
  — CMake caches failed checks.
- nohup keeps the build alive if SSH drops. Ctrl+C only stops `tail`.

### Step 3 — package the APK
[UNCONFIRMED as of 2026-09-25 — not yet run.] Same shell/exports as
step 2, then:
```
python3 -u build-tools/ci-scripts/build-android-package.py 2>&1 | tee ~/build-apk.log
```
Upstream CI output location: $KDECI_WORKDIR_PATH/krita/_packaging/*.apk

### Signed release
Requires KRIMBLE_KEYSTORE_PASSWORD and KRIMBLE_KEY_PASSWORD in the
server's shell profile, and ~/krimble-release.jks present on the server.

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

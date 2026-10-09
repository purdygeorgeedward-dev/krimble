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
  <!-- Undo: `rm lld && mv lld.x86_64.orig lld` in the same folder. -->
  Undo: NOT AVAILABLE on the current server (2026-09-26). The swap was
  run twice, so lld.x86_64.orig is also a symlink — the Intel lld
  backup is gone. Reverting means reinstalling NDK 27.3.13750724.
  Guard against re-runs: only run the mv if `lld` is NOT already a
  symlink (`test -L lld || mv lld lld.x86_64.orig`).
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
CONFIRMED WORKING 2026-09-26: started ~00:56, finished clean ~08:30
(server time) on commit bf2f7b3 with native lld. Output libs in
~/kwd/krita/_install/lib, qml present in ~/kwd/krita/_install.
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
- If the Termux screen freezes during `tail -f`, the SSH link dropped.
  Close the session, reconnect, then check:
  `pgrep -f run-ci-build.py || echo "BUILD PROCESS FINISHED"`
- A full build takes ~7.5 hours on this server (x86 compiler under
  emulation). ccache makes rebuilds faster.

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

## Server helper scripts (added 2026-10-06)

The build settings and the long build / package / check commands live in `tools/server/`. Copy them once to the home folder of the server:

    cd ~/krimble && git pull origin master && cp tools/server/kb-*.sh ~/ && chmod +x ~/kb-*.sh

Then the whole routine is four short commands: `~/kb-build.sh` (pull the code and build), `~/kb-status.sh` (how it is going, in plain words), `~/kb-package.sh` (make the APK once the build has finished) and `~/kb-apk.sh` (check the APK, name it, print the copy line for the phone).
`kb-env.sh` holds the settings that used to have to be pasted into every new SSH session. `kb-build.sh` will not start a second build; `kb-package.sh` always leaves exactly one packaging run.

## Release builds, version numbers, size and the Play Store AAB (added 2026-10-09)

Appended to this file. Nothing above was changed. Where this section and an
older section disagree, THIS section is newer.

Sources: George's server (`~/.bash_history`, file listings), the repo, and
two ChatGPT session records recovered 2026-10-09. Every statement is tagged:
**[verified]** = seen on the server or in the repo, **[recorded]** = from the
ChatGPT records only, **[unconfirmed]** = a theory or a gap.

Times: the server clock is UTC. George works in Mountain time (MDT = UTC-6).
The size-reduction work happened on 2026-10-07 in Mountain time, which is
2026-10-08 in UTC. Both dates appear in older notes. [verified: AAB file
dated 2026-10-08 04:28 UTC on the server]

### 1. Step 3 is confirmed (supersedes "UNCONFIRMED" above)

Packaging the APK with `build-android-package.py` was confirmed working
2026-09-26 and has been run many times since. [verified]

### 2. The build counter and version numbers

Two different numbers exist. Do not mix them up.

**The build counter** [verified]
- File: `~/krimble-build-number.txt` on the server (one number).
- `libs/version/CMakeLists.txt` adds a target, `krimble_build_stamp`, that
  runs on every `make`. It runs `libs/version/KrimbleBuildStamp.cmake`.
- That script adds 1 to the counter and writes
  `~/kwd/krita/_build/libs/version/krimble_build_stamp.h` (number, date,
  git hash). `KritaVersionWrapper.cpp` shows these in the app.
- So the counter counts `make` runs. Failed builds and extra `make` runs
  use up numbers too.
- To start from another number, write the number BEFORE the one you want
  into the file.
- If the file is lost, counting restarts at 1.

**The Android version** (`packaging/android/apk/build.gradle`) [verified]
- `versionCode = project.ext.constant*1000000 + versionMajor*10000 +
  versionMinor*100 + versionRelease` (major 5, minor 4; the constant is 5,
  worked out from 5050400, not read from the file). `versionRelease` is the
  last two digits.
- Google Play rejects an upload whose versionCode was already used.
  5050400 was used by an earlier upload, so `versionRelease` was changed
  0 -> 1 (5050401). A fixed value repeats the rejection on the next upload.
- `versionName` was hardcoded `"1.0.0-beta2"` for many builds, so the APK
  never showed a new version. It was changed to `"1.0.29-beta"` on
  2026-10-09 to match the GitHub release 1.0.29b (2026-10-07).
- Status 2026-10-09: the repo has the fixed values above. A change to make
  `versionName` and `versionRelease` follow the build counter was drafted
  and is NOT yet in the repo. [unconfirmed until pushed]
- Not verified: whether `create-apk` also runs the stamp target (which
  would bump the counter during packaging, so the app's number and the
  APK's version could differ by 1).

**APK file names** [verified]
- `kb-apk.sh` names the copy `Krimble-Beta2-<MonDay>-<HHMM>-b<counter>.apk`
  (Mountain time, from the APK file's time). `Beta2` is hardcoded in the
  script. Who chose it is not recorded.

### 3. Why the package was about 509 MB, and the fix

**Cause: native debug symbols.** [recorded, measurements exact]
- Debug symbols are data for debuggers (source lines, symbol names). The
  app does not run them. Removing them does not remove any app code.
- The C++ was already built optimised (`-O3 -DNDEBUG`), so this was not a
  Debug-versus-Release problem. [recorded]
- Largest case, `lib_kritalcmsengine_arm64-v8a.so`:
  219,082,456 bytes -> 23,180,648 bytes (89.4% smaller). [recorded]
- APK: about 509 MB -> 165,553,065 bytes (157.8 MiB). [recorded; matches
  the 158 MB asset on the GitHub release 1.0.29b, verified]

**How it was found** [recorded, commands from `~/.bash_history`: verified]
1. Stale NDK references were replaced (see section 4).
2. A copy of the library was stripped by hand to prove the idea:
   `llvm-strip --strip-debug` using NDK 27's `llvm-strip`
   (about 209 MB -> about 31 MB on the test copy).
3. A temporary CMake hook (`strip-android-libs.cmake`, called from
   `ECMAndroidDeployQt.cmake`) was run to strip during packaging.
4. That hook was removed. Verified 2026-10-09: the file is gone and
   `ECMAndroidDeployQt.cmake` has no "strip" text on the server.
5. The final mechanism is Gradle's own task `stripReleaseDebugSymbols`,
   which runs as part of a release package build. No custom strip step
   remains.

**Why Gradle's stripping started working** [unconfirmed]
- Theory: `ndkVersion` in `build.gradle` was `"22.1.7171670"`, an NDK that
  was not installed where Gradle looks. The repo's own comment says a
  mismatched NDK makes AGP fail to strip native libraries. Changing it to
  `"27.3.13750724"` (server history line 1891) came before the strip tests.
- Nobody has compared the Gradle log for that task before and after.
- If the package ever grows back to about 500 MB, check `ndkVersion` first.

### 4. NDK versions: only 27.3.13750724 is correct

- Correct: `27.3.13750724` at `~/Android/sdk/ndk/27.3.13750724`.
- r22b and r30 are wrong for this server. r30 has x86-only host tools.
- Places that held stale versions, all fixed on the server [verified in
  history]: `~/krimble/env` (r30 -> 27), `~/.bashrc` lines 135-137, and
  `ndkVersion` in the server's `build.gradle` (r22 -> 27).
- Removed 2026-10-09 [verified]: `~/Android/android-ndk-r22b` and its
  `.zip` (about 4.7 GB). `~/Android/sdk/ndk/` now holds only 27.3.13750724.
  The empty folder `~/Android/ndk` remains.
- Repo text that still mentions old NDKs (comments and warnings only, none
  used by a build): a comment in `build.gradle` saying r22b is installed
  (outdated), the commented-out `ndkVersion "22.1.7171670"` line,
  `build-tools/ci-scripts/android.yml` line 18 (upstream comment), and the
  r18b/r30 mentions at the top of this file.

### 5. Building the AAB for Google Play

Play Store needs an `.aab`, not an `.apk`.

Script: `build-tools/ci-scripts/krimble-build-aab.py` [verified: in repo]
- A copy of `build-android-package.py` with `./gradlew assembleRelease`
  changed to `./gradlew bundleRelease`, and the output match changed from
  `*.apk` to `*.aab`. The header comment in the file says the same.
- Do not use `build-android-appbundle.py` for this. It expects artifacts
  from a separate earlier job.
- It deletes the whole `~/kwd/krita/_packaging` folder before writing, so
  an APK made earlier is wiped. Copy it out first.

Run (cloud server), after the native build has finished, with the same
exports as step 2 (`kb-env.sh` has them):
```
export KDECI_ANDROID_ABI=arm64-v8a
export KDECI_WORKDIR_PATH=$HOME/kwd
cd ~/kwd/krita
python3 ~/krimble/build-tools/ci-scripts/krimble-build-aab.py --package-type release
```
Output: `~/kwd/krita/_packaging/krimble-arm64-v8a-<versionName>-release.aab`.
First AAB: 271,313,916 bytes (258.8 MiB), `BUILD SUCCESSFUL`. [verified:
file on the server, dated 2026-10-08 04:28 UTC]

Target API: `targetSdkVersion` is 36 (was 35). Google Play required it. It
is in the repo as of 2026-10-09. `compileSdk` stays 35. [verified]

Play Console result for the first upload (internal testing track)
[recorded]: the size check and the target-API check passed. The upload was
rejected because versionCode 5050400 was already used. Not confirmed: that
a later upload was accepted or that the app was published.

Open question [unconfirmed]: the AAB (258.8 MiB) is larger than the
stripped APK (157.8 MiB). Not known whether stripping was applied to the AAB.

### 6. Server housekeeping notes (2026-10-09)

- `~/Krimble-Beta2-*.apk` (11 files, pre-strip, 486 to 497 MB each) were
  deleted. The stripped 158 MB APK exists only as the GitHub release asset.
- The server's `~/krimble/.kde-ci.yml` is modified (a 168-line dependency
  list replacing the repo's 19 lines; made before 2026-10-01). Do not
  discard it without reading its diff.
- Before `git pull` in `~/krimble`, the server's uncommitted edits to
  `packaging/android/apk/build.gradle` will conflict. Its edits (API 36,
  `versionRelease`, `ndkVersion`) are now in the repo.
- `kb-build.sh` runs `git pull origin master | tail -n 2` and does not stop
  if the pull fails. A failed pull builds the old code without warning.
- The history of every command run on the server is in `~/.bash_history`.
  Long commands: always `setsid nohup ... < /dev/null &`, not `nohup` alone.

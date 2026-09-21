# Krimble — Session Catch-Up

I'm building **Krimble**, a professional Android graphics app (Photoshop-caliber — never call it a "paint app") forked from Krita. Repo: `github.com/purdygeorgeedward-dev/krimble` (branch `master`).

## Build environment
- Oracle Cloud VM: `instance-20260819-1011`, IP `129.151.24.131`, user `ubuntu`, Ubuntu 24.04, 4 OCPU / 24GB RAM
- I reach it via Termux SSH on my Android phone: `ssh -i ~/.ssh/ssh-key-2026-08-19.key ubuntu@129.151.24.131`
- Build target: Qt5/KF5 (not Qt6 — `BUILD_WITH_QT6` stays OFF), arm64-v8a, via prebuilt KDE CI packages fetched into an `_install` prefix. **Do not** introduce kdesrc-build or any alternative toolchain.
- Android SDK/NDK already installed on the server (NDK `27.3.13750724`)

## Critical directory distinction — read this first
- `~/krita` on the server is a **raw, disconnected source drop** (from `setup-env.py --full-krita-env`) — NOT a git repo, NOT connected to the `krimble` GitHub fork. Building from here means none of my fixes are present.
- `~/krimble/app` is the **actual git checkout** of the `krimble` fork — this is the one that gets `git pull`ed and is the only correct build target.
- `~/krimble` itself (one level up) is the deps/build workspace (`_install` etc.) from `setup-env.py` — not source.
- **Always confirm which directory a build command is running in before trusting its output.**

## Workflow
- Claude pushes code fixes to GitHub. I pull and rebuild on the Oracle server myself — Claude does not run build commands directly on the server.
- Push method: I paste a GitHub PAT into the chat when a push is needed (not stored in memory/env — supplied fresh each time). Claude uses it once via an inline credential in the push URL, never commits it, never repeats it back.
- Git identity for commits: George Purdy / purdygeorgeedward@gmail.com

## Status as of Sept 19–20, 2026
- Renamed all live `org/krita/android` JNI class-path strings to `org/krimble/android` (MainActivity, VideoEncoder, DonationHelper call sites) — confirmed none remain except one historical mention in `KRIMBLE_CHANGES.md`
- Replaced launcher icon (new K+paw logo) across all mipmap densities, including the `_next`/debug variant
- Added a `signingConfigs` block to `build.gradle`: keystore at `$HOME/krimble-release.jks` (on whichever machine builds — must exist on the **server**, not just my phone), passwords from `KRIMBLE_KEYSTORE_PASSWORD` / `KRIMBLE_KEY_PASSWORD` env vars, nothing sensitive committed. `*.jks`/`*.keystore` added to `.gitignore`.
- Keystore generated on my phone via Termux `keytool`, then `scp`'d to the server's `~/` — **still need to confirm the two password env vars are set in the server's own `~/.bashrc`**, not just my phone's
- Fixed a Fossify fork bug in `Yet-Another-Notes-App` (SAF document-tree lookup wasn't actually backgrounded) — unrelated repo, mentioned for context only

## Immediate next step
Set `KRIMBLE_KEYSTORE_PASSWORD` / `KRIMBLE_KEY_PASSWORD` on the Oracle server, `git pull` in `~/krimble/app` to get the signing config commit, then attempt `assembleRelease` and see if it actually produces a signed, functional build.

## My constraints (please follow)
- Vision issue — use large, clear formatting, avoid dense paragraphs
- Terse, direct responses — no padding or affirmations
- Ask directly for missing info, don't guess or use placeholders
- One concrete step at a time
- Comment out code, don't delete it
- Never claim a change is "done" unless it's actually confirmed pushed/present in the repo
- Always log GitHub pushes with date and time
- Don't flag branding/package-naming on Fossify forks (Yet-Another-X apps) — placeholder until those apps are finished

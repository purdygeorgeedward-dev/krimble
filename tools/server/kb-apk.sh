#!/bin/bash
# STEP 3: after PACKAGING: FINISHED OK. Checks the APK, gives it its proper name and prints the exact line to copy it to your phone.
A=$(ls -t ~/kwd/krita/_packaging/*.apk 2>/dev/null | head -n 1)
if [ -z "$A" ]; then echo "NO APK FOUND. Run ~/kb-package.sh first."; exit 1; fi
echo "APK MADE: $(date -r "$A")"
grep -E "NUMBER|GIT" ~/kwd/krita/_build/libs/version/krimble_build_stamp.h
# Krimble 2026-10-09: was Krimble-Beta2-<date>-b<counter>.apk. Name now follows the build counter, same as versionName in build.gradle.
# N="Krimble-Beta2-$(TZ=America/Denver date -r "$A" +%b%-d-%H%M)-b$(cat ~/krimble-build-number.txt).apk"
# Krimble 2026-10-09: the name's version now comes from the APK's own file name (set by hand in build.gradle), not the counter.
# N="Krimble-1.0.$(cat ~/krimble-build-number.txt)-beta-$(TZ=America/Denver date -r "$A" +%b%-d-%H%M).apk"
V=$(basename "$A" .apk | sed -e 's/^krimble-arm64-v8a-//' -e 's/-release$//')
N="Krimble-${V}-$(TZ=America/Denver date -r "$A" +%b%-d-%H%M).apk"
cp "$A" ~/"$N"
ls -l ~/"$N" | cut -c25-140
echo "-----"
echo "FILE NAME: $N"
echo "NEXT: type exit, then in Termux on your phone (prompt shows ~ \$) run:"
echo "scp -i ~/.ssh/ssh-key-2026-08-19.key ubuntu@129.151.24.131:$N ~/storage/downloads/"

#!/bin/bash
# STEP 2: after the build has FINISHED OK, makes the APK (about 10 minutes). Safe to run twice: it stops any earlier packaging run and starts exactly one.
source ~/kb-env.sh
if pgrep -f "[m]ake -j4" > /dev/null; then
  echo "THE BUILD IS STILL RUNNING. Wait for it to finish. Run ~/kb-status.sh"
  exit 1
fi
pkill -f '[b]uild-android-package'
pkill -f '[G]radleDaemon'
sleep 3
rm -rf ~/kwd/krita/_build/krita_build_apk
setsid nohup python3 -u build-tools/ci-scripts/build-android-package.py > ~/package-latest.log 2>&1 < /dev/null &
echo "PACKAGING STARTED. Next: wait about 10 minutes, then run ~/kb-status.sh"

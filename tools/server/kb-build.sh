#!/bin/bash
# STEP 1: gets the newest code from GitHub and starts the build (about 30 to 60 minutes). Safe to run twice: it will not start a second build.
source ~/kb-env.sh
if pgrep -f "[m]ake -j4" > /dev/null; then
  echo "A BUILD IS ALREADY RUNNING. Nothing new was started. Run ~/kb-status.sh to see how it is going."
  exit 1
fi
git pull origin master | tail -n 2
echo "CODE VERSION: $(git log -1 --format=%h)"
cd ~/kwd/krita/_build || exit 1
setsid nohup make -j4 install > ~/build-latest.log 2>&1 < /dev/null &
echo "BUILD STARTED. Next: wait, then run ~/kb-status.sh"

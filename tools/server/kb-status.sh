#!/bin/bash
# Tells you how the build and the packaging are going, in plain words. Only reads; changes nothing.
echo "TIME (UTC): $(date)"
echo "-----"
if pgrep -f "[m]ake -j4" > /dev/null; then
  echo "BUILD: STILL RUNNING"
  tail -n 1 ~/build-latest.log | cut -c1-100
elif [ -f ~/build-latest.log ]; then
  if tail -n 3 ~/build-latest.log | grep -q "Installing"; then
    echo "BUILD: FINISHED OK"
  else
    echo "BUILD: STOPPED WITH A PROBLEM. Send me these lines:"
    grep -n -i "\*\*\*\|no such file\|cannot find\|error:" ~/build-latest.log | grep -v "_find_sip\|_find_pyqt5" | cut -c1-200 | head -6
  fi
else
  echo "BUILD: nothing started yet"
fi
echo "-----"
if pgrep -f "[b]uild-android-package" > /dev/null; then
  echo "PACKAGING: STILL RUNNING"
elif [ -f ~/package-latest.log ]; then
  if grep -q "BUILD SUCCESSFUL" ~/package-latest.log; then
    echo "PACKAGING: FINISHED OK"
    ls -l ~/kwd/krita/_packaging/*.apk | cut -c25-130
  else
    echo "PACKAGING: STOPPED WITH A PROBLEM. Send me these lines:"
    tail -n 5 ~/package-latest.log | cut -c1-140
  fi
else
  echo "PACKAGING: nothing started yet"
fi

#!/bin/bash
# Krimble build settings. The other kb-*.sh scripts load this file; you never run it yourself.
cd ~/krimble || exit 1
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

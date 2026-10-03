# KRIMBLE 2026-10-03: writes krimble_build_stamp.h at BUILD time (every make), so the version
# shown by the app changes with every build. It is run by the custom target
# "krimble_build_stamp" in libs/version/CMakeLists.txt:
#   cmake -DOUT=<header to write> -DSRC=<source dir> -DCOUNTER=<counter file> -P KrimbleBuildStamp.cmake
#
# KRIMBLE_BUILD_STAMP: build time as YYMMDD-HHMM in Mountain time (the same clock the APK
#                      file names use).
# KRIMBLE_BUILD_NUMBER: the build counter (1, 2, 3 ...), one more on every build.
# KRIMBLE_BUILD_DATE:  month and day as in the APK file name (e.g. Oct3).
# KRIMBLE_BUILD_GIT:   short git hash of the checkout that was built (left out if git is not
#                      available or the folder is not a git checkout).
#
# The header is only rewritten when its content changes (the counter makes that every build).

set(ENV{TZ} "America/Denver")
string(TIMESTAMP KRIMBLE_STAMP "%y%m%d-%H%M")
# Build number: a counter that goes up by one on every build. It is kept in the file given
# with -DCOUNTER=<file>. If the file is missing, counting starts at 1. To start from another
# number, write the number BEFORE the one you want into that file.
set(KRIMBLE_NUMBER 0)
if(COUNTER AND EXISTS "${COUNTER}")
    file(READ "${COUNTER}" KRIMBLE_COUNTER_TEXT)
    string(STRIP "${KRIMBLE_COUNTER_TEXT}" KRIMBLE_COUNTER_TEXT)
    if(KRIMBLE_COUNTER_TEXT MATCHES "^[0-9]+$")
        set(KRIMBLE_NUMBER ${KRIMBLE_COUNTER_TEXT})
    endif()
endif()
math(EXPR KRIMBLE_NUMBER "${KRIMBLE_NUMBER} + 1")
if(COUNTER)
    file(WRITE "${COUNTER}" "${KRIMBLE_NUMBER}\n")
endif()
# Month and day as in the APK file names: "Oct3".
string(TIMESTAMP KRIMBLE_MONTH "%b")
string(TIMESTAMP KRIMBLE_DAY "%d")
string(REGEX REPLACE "^0" "" KRIMBLE_DAY "${KRIMBLE_DAY}")
set(KRIMBLE_DATE "${KRIMBLE_MONTH}${KRIMBLE_DAY}")

set(KRIMBLE_GIT "")
find_program(KRIMBLE_GIT_EXE git)
if(KRIMBLE_GIT_EXE AND EXISTS "${SRC}/.git")
    execute_process(
        COMMAND ${KRIMBLE_GIT_EXE} rev-parse --short=7 HEAD
        WORKING_DIRECTORY "${SRC}"
        OUTPUT_VARIABLE KRIMBLE_GIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
endif()

set(CONTENT "// Generated at build time by KrimbleBuildStamp.cmake - do not edit.\n#ifndef KRIMBLE_BUILD_STAMP_H\n#define KRIMBLE_BUILD_STAMP_H\n#define KRIMBLE_BUILD_STAMP \"${KRIMBLE_STAMP}\"\n#define KRIMBLE_BUILD_NUMBER \"${KRIMBLE_NUMBER}\"\n#define KRIMBLE_BUILD_DATE \"${KRIMBLE_DATE}\"\n")
if(KRIMBLE_GIT)
    string(APPEND CONTENT "#define KRIMBLE_BUILD_GIT \"${KRIMBLE_GIT}\"\n")
endif()
string(APPEND CONTENT "#endif\n")

set(OLD "")
if(EXISTS "${OUT}")
    file(READ "${OUT}" OLD)
endif()
if(NOT OLD STREQUAL CONTENT)
    file(WRITE "${OUT}" "${CONTENT}")
endif()

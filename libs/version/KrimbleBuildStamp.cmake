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
# KRIMBLE 2026-10-10: the number shown in the app is now the hand-set version number from build.gradle
# (krimbleBuildNumber), the same number the APK's versionName uses. The build counter below is no longer used.
set(KRIMBLE_NUMBER 0)
set(KRIMBLE_GRADLE_FILE "${SRC}/packaging/android/apk/build.gradle")
if(EXISTS "${KRIMBLE_GRADLE_FILE}")
    file(READ "${KRIMBLE_GRADLE_FILE}" KRIMBLE_GRADLE_TEXT)
    if(KRIMBLE_GRADLE_TEXT MATCHES "(^|\n)[ \t]*def krimbleBuildNumber[ \t]*=[ \t]*([0-9]+)")
        set(KRIMBLE_NUMBER ${CMAKE_MATCH_2})
    endif()
endif()
# (old counter, replaced by the above)
# set(KRIMBLE_NUMBER 0)
# if(COUNTER AND EXISTS "${COUNTER}")
#     file(READ "${COUNTER}" KRIMBLE_COUNTER_TEXT)
#     string(STRIP "${KRIMBLE_COUNTER_TEXT}" KRIMBLE_COUNTER_TEXT)
#     if(KRIMBLE_COUNTER_TEXT MATCHES "^[0-9]+$")
#         set(KRIMBLE_NUMBER ${KRIMBLE_COUNTER_TEXT})
#     endif()
# endif()
# math(EXPR KRIMBLE_NUMBER "${KRIMBLE_NUMBER} + 1")
# if(COUNTER)
#     file(WRITE "${COUNTER}" "${KRIMBLE_NUMBER}\n")
# endif()
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

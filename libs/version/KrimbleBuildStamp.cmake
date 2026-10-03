# KRIMBLE 2026-10-03: writes krimble_build_stamp.h at BUILD time (every make), so the version
# shown by the app changes with every build. It is run by the custom target
# "krimble_build_stamp" in libs/version/CMakeLists.txt:
#   cmake -DOUT=<header to write> -DSRC=<source dir> -P KrimbleBuildStamp.cmake
#
# KRIMBLE_BUILD_STAMP: build time as YYMMDD-HHMM in Mountain time (the same clock the APK
#                      file names use).
# KRIMBLE_BUILD_GIT:   short git hash of the checkout that was built (left out if git is not
#                      available or the folder is not a git checkout).
#
# The header is only rewritten when its content changes.

set(ENV{TZ} "America/Denver")
string(TIMESTAMP KRIMBLE_STAMP "%y%m%d-%H%M")

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

set(CONTENT "// Generated at build time by KrimbleBuildStamp.cmake - do not edit.\n#ifndef KRIMBLE_BUILD_STAMP_H\n#define KRIMBLE_BUILD_STAMP_H\n#define KRIMBLE_BUILD_STAMP \"${KRIMBLE_STAMP}\"\n")
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

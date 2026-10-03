/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "KritaVersionWrapper.h"

#include <kritaversion.h>
#include <kritagitversion.h>
// KRIMBLE 2026-10-03: generated at every build, see KrimbleBuildStamp.cmake
#if defined(__has_include)
#  if __has_include(<krimble_build_stamp.h>)
#    include <krimble_build_stamp.h>
#  endif
#endif

QString KritaVersionWrapper::versionString(bool checkGit)
{
    QString kritaVersion = QStringLiteral(KRITA_VERSION_STRING);
    QString version = kritaVersion;

    if (checkGit) {
        // KRIMBLE 2026-10-03: "1.0.0-beta2 build 261003-1905 (git abc1234)". The build stamp is
        // new on every build; its git hash is read at build time, unlike KRITA_GIT_SHA1_STRING,
        // which is only refreshed when CMake reconfigures. versionString(false), the plain
        // version used in saved files and the resource database, is left exactly as it was.
#ifdef KRIMBLE_BUILD_STAMP
        version = QStringLiteral("%1 build %2").arg(kritaVersion, QStringLiteral(KRIMBLE_BUILD_STAMP));
#  ifdef KRIMBLE_BUILD_GIT
        version = QStringLiteral("%1 (git %2)").arg(version, QStringLiteral(KRIMBLE_BUILD_GIT));
#  elif defined(KRITA_GIT_SHA1_STRING)
        version = QStringLiteral("%1 (git %2)").arg(version, QStringLiteral(KRITA_GIT_SHA1_STRING));
#  endif
#else
        // (original behaviour)
#  ifdef KRITA_GIT_SHA1_STRING
        QString gitVersion = QStringLiteral(KRITA_GIT_SHA1_STRING);
        version = QStringLiteral("%1 (git %2)").arg(kritaVersion, gitVersion);
#  endif
#endif
    }
    return version;
}

bool KritaVersionWrapper::isDevelopersBuild()
{
#if defined(KRITA_STABLE)
    return false;
#else
    return true;
#endif
}

/*
 * tool_dyna.cpp -- Part of Krita
 *
 * SPDX-FileCopyrightText: 2009 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tool_dyna.h"

#include <stdlib.h>
#include <vector>

#include <QPoint>

#include <klocalizedstring.h>

#include <kis_debug.h>
#include <kis_paint_device.h>
#include <kpluginfactory.h>

#include <kis_global.h>
#include <kis_types.h>
#include <KoToolRegistry.h>


#include "kis_tool_dyna.h"

K_PLUGIN_FACTORY_WITH_JSON(ToolDynaFactory, "kritatooldyna.json", registerPlugin<ToolDyna>();)


ToolDyna::ToolDyna(QObject *parent, const QVariantList &)
        : QObject(parent)
{
    // KRIMBLE 2026-10-03: removed from the toolbox at George's request so the first button is the Move tool (industry-standard order). Uncomment to bring it back.
    // KoToolRegistry::instance()->add(new KisToolDynaFactory());
}

ToolDyna::~ToolDyna()
{
}

#include "tool_dyna.moc"

/*
 *  SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "taskset_resource.h"

#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QBuffer>

#include <kis_debug.h>
#include <KisPortingUtils.h>

#define TASKSET_VERSION 2

TasksetResource::TasksetResource(const QString& f)
    : KoResource(f)
{
}

TasksetResource::~TasksetResource()
{
}

TasksetResource::TasksetResource(const TasksetResource &rhs)
    : KoResource(rhs),
      m_actions(rhs.m_actions),
      m_filterIds(rhs.m_filterIds),
      m_filterConfigs(rhs.m_filterConfigs),
      m_filterNames(rhs.m_filterNames)
{
}

KoResourceSP TasksetResource::clone() const
{
    return KoResourceSP(new TasksetResource(*this));
}

bool TasksetResource::loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface)
{
    Q_UNUSED(resourcesInterface);

    QDomDocument doc;
    if (!doc.setContent(dev)) {
        return false;
    }
    QDomElement element = doc.documentElement();
    setName(element.attribute("name"));

    const int version = element.attribute("version", "1").toInt();

    m_actions.clear();
    m_filterIds.clear();
    m_filterConfigs.clear();
    m_filterNames.clear();

    QDomNode node = element.firstChild();
    while (!node.isNull()) {
        QDomElement child = node.toElement();
        if (!child.isNull()) {
            if (version >= 2 && child.tagName() == "step") {
                // Krimble: unified per-step element, one of "action" or
                // "filterId"/"filterName" set (with the config XML blob as
                // a child element), the other left blank -- avoids relying
                // on separate parallel tag lists staying in sync by order.
                m_actions.append(child.attribute("action"));
                m_filterIds.append(child.attribute("filterId"));
                m_filterNames.append(child.attribute("filterName"));

                QString configXml;
                QDomElement configElement = child.firstChildElement("config");
                if (!configElement.isNull()) {
                    QDomNode configChild = configElement.firstChild();
                    // The config blob is itself a small XML document (a
                    // KisFilterConfiguration's own toXML() output) stored
                    // as a single CDATA/text child so it round-trips
                    // without needing to merge two different DOMs.
                    if (configChild.isCDATASection()) {
                        configXml = configChild.toCDATASection().data();
                    } else {
                        configXml = configElement.text();
                    }
                }
                m_filterConfigs.append(configXml);
            } else if (child.tagName() == "action") {
                // Krimble: backward compatibility with version-1 files,
                // which only ever stored plain action-trigger steps.
                m_actions.append(child.text());
                m_filterIds.append(QString());
                m_filterConfigs.append(QString());
                m_filterNames.append(QString());
            }
        }
        node = node.nextSibling();
    }
    setValid(true);
    return true;
}

QString TasksetResource::defaultFileExtension() const
{
    return QString(".kts");
}

void TasksetResource::setActionList(const QStringList actions)
{
    m_actions = actions;
}

QStringList TasksetResource::actionList()
{
    return m_actions;
}

void TasksetResource::setFilterList(const QStringList &filterIds, const QStringList &filterConfigs, const QStringList &filterNames)
{
    m_filterIds = filterIds;
    m_filterConfigs = filterConfigs;
    m_filterNames = filterNames;
}

QStringList TasksetResource::filterIdList()
{
    return m_filterIds;
}

QStringList TasksetResource::filterConfigList()
{
    return m_filterConfigs;
}

QStringList TasksetResource::filterNameList()
{
    return m_filterNames;
}

bool TasksetResource::saveToDevice(QIODevice *io) const
{

    QDomDocument doc;
    QDomElement root = doc.createElement("Taskset");
    root.setAttribute("name", name() );
    root.setAttribute("version", TASKSET_VERSION);

    for (int i = 0; i < m_actions.size(); ++i) {
        QDomElement element = doc.createElement("step");
        element.setAttribute("action", m_actions.at(i));
        element.setAttribute("filterId", i < m_filterIds.size() ? m_filterIds.at(i) : QString());
        element.setAttribute("filterName", i < m_filterNames.size() ? m_filterNames.at(i) : QString());

        const QString configXml = i < m_filterConfigs.size() ? m_filterConfigs.at(i) : QString();
        if (!configXml.isEmpty()) {
            QDomElement configElement = doc.createElement("config");
            configElement.appendChild(doc.createCDATASection(configXml));
            element.appendChild(configElement);
        }

        root.appendChild(element);
    }
    doc.appendChild(root);

    QTextStream textStream(io);
    KisPortingUtils::setUtf8OnStream(textStream);
    doc.save(textStream, 4);

    KoResource::saveToDevice(io);

    return true;
}

/*
 *  SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */


#ifndef TASKSET_RESOURCE_H
#define TASKSET_RESOURCE_H

#include <KoResource.h>
#include <QStringList>


class TasksetResource : public KoResource
{

public:
    TasksetResource(const QString& filename);
    ~TasksetResource() override;

    TasksetResource(const TasksetResource &rhs);
    TasksetResource &operator=(const TasksetResource &rhs) = delete;
    KoResourceSP clone() const override;

    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;
    bool saveToDevice(QIODevice* dev) const override;

    QString defaultFileExtension() const override;

    QPair<QString, QString> resourceType() const override
    {
        return QPair<QString, QString>(ResourceType::TaskSets, "");
    }

    void setActionList(const QStringList actions);
    QStringList actionList();

    /**
     * Krimble: parallel, index-aligned lists for filter-application steps
     * (see TasksetStep in tasksetmodel.h) -- a plain action-trigger step
     * has an empty string at the same index in all three of these lists.
     */
    void setFilterList(const QStringList &filterIds, const QStringList &filterConfigs, const QStringList &filterNames);
    QStringList filterIdList();
    QStringList filterConfigList();
    QStringList filterNameList();

private:

    QStringList m_actions;
    QStringList m_filterIds;
    QStringList m_filterConfigs;
    QStringList m_filterNames;
};

typedef QSharedPointer<TasksetResource> TasksetResourceSP;

#endif // TASKSET_RESOURCE_H

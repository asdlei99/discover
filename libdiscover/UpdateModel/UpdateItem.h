/***************************************************************************
 *   Copyright © 2011 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef UPDATEITEM_H
#define UPDATEITEM_H

// Qt includes
#include <QSet>
#include <QString>
#include "discovercommon_export.h"
#include "resources/AbstractBackendUpdater.h"

#include <QIcon>

class AbstractResource;
class DISCOVERCOMMON_EXPORT UpdateItem
{
public:
    explicit UpdateItem(AbstractResource *app);

    ~UpdateItem();

    void setProgress(qreal progress);
    qreal progress() const;

    AbstractBackendUpdater::State state() const { return m_state; }
    void setState(AbstractBackendUpdater::State state) { m_state = state; }

    QString changelog() const;
    void setChangelog(const QString &changelog);

    AbstractResource *app() const;
    QString name() const;
    QString version() const; // Deprecated; use availableVersion() instead
    QString availableVersion() const;
    QString installedVersion() const;
    QVariant icon() const;
    qint64 size() const;
    Qt::CheckState checked() const;

    AbstractResource* resource() const { return m_app; }
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

private:
    AbstractResource * const m_app;

    const QString m_categoryName;
    const QIcon m_categoryIcon;
    qreal m_progress = 0.;
    bool m_visible = true;
    AbstractBackendUpdater::State m_state = AbstractBackendUpdater::None;
    QString m_changelog;
};

#endif // UPDATEITEM_H

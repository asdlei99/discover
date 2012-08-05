/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "ApplicationListView.h"
#include "../BreadcrumbWidget/BreadcrumbWidget.h"

// Qt includes
#include <QtGui/QIcon>
#include <QtGui/QStackedWidget>

// KDE includes
#include <KSeparator>

// Libmuon includes
#include <resources/ResourcesModel.h>

// Own includes
#include "ApplicationViewWidget.h"

ApplicationListView::ApplicationListView(QWidget *parent, const QModelIndex &index)
        : AbstractViewContainer(parent)
{
    m_appViewWidget = new ApplicationViewWidget(this);
    m_appViewWidget->setTitle(index.data(Qt::DisplayRole).toString());
    m_appViewWidget->setIcon(index.data(Qt::DecorationRole).value<QIcon>());
    m_breadcrumbWidget->setRootItem(m_appViewWidget->breadcrumbItem());

    m_viewStack->addWidget(m_appViewWidget);
    m_viewStack->setCurrentWidget(m_appViewWidget);

    ResourcesModel *resourcesModel = ResourcesModel::global();
    connect(resourcesModel, SIGNAL(searchInvalidated()),
            m_breadcrumbWidget, SLOT(startSearch()));
    connect(m_appViewWidget, SIGNAL(registerNewSubView(AbstractViewBase*)),
            this, SLOT(registerNewSubView(AbstractViewBase*)));
    connect(m_appViewWidget, SIGNAL(switchToSubView(AbstractViewBase*)),
            this, SLOT(switchToSubView(AbstractViewBase*)));
}

void ApplicationListView::setStateFilter(AbstractResource::State state)
{
    m_appViewWidget->setStateFilter(state);
}

void ApplicationListView::setOriginFilter(const QString &origin)
{
    m_appViewWidget->setOriginFilter(origin);
}

void ApplicationListView::setShouldShowTechnical(bool show)
{
    m_appViewWidget->setShouldShowTechnical(show);
}

void ApplicationListView::setCanShowTechnical(bool canShow)
{
    m_appViewWidget->setCanShowTechnical(canShow);
}

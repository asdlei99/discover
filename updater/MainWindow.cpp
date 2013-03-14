/***************************************************************************
 *   Copyright © 2011 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *   Copyright © 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#include "MainWindow.h"

// Qt includes
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QVBoxLayout>

// KDE includes
#include <KAction>
#include <KActionCollection>
#include <KDebug>
#include <KMessageBox>
#include <KMessageWidget>
#include <KProcess>
#include <KProtocolManager>
#include <KStandardDirs>
#include <Solid/Device>
#include <Solid/AcAdapter>
#include <KToolBar>

// Own includes
#include <resources/AbstractResourcesBackend.h>
#include <resources/AbstractBackendUpdater.h>
#include <resources/ResourcesModel.h>
#include <resources/ResourcesUpdatesModel.h>
#include "ChangelogWidget.h"
#include "ProgressWidget.h"
#include "config/UpdaterSettingsDialog.h"
#include "UpdaterWidget.h"
#include "KActionMessageWidget.h"

MainWindow::MainWindow()
    : MuonMainWindow()
    , m_settingsDialog(nullptr)
{
    m_updater = new ResourcesUpdatesModel(this);
    connect(m_updater, SIGNAL(progressingChanged()), SLOT(progressingChanged()));

    initGUI();
}

void MainWindow::initGUI()
{
    setWindowTitle(i18nc("@title:window", "Software Updates"));
    ResourcesModel* m = ResourcesModel::global();
    m->integrateMainWindow(this);

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

    m_powerMessage = new KMessageWidget(mainWidget);
    m_powerMessage->setText(i18nc("@info Warning to plug in laptop before updating",
                                  "It is safer to plug in the power adapter before updating."));
    m_powerMessage->setMessageType(KMessageWidget::Warning);
    checkPlugState();

    m_progressWidget = new ProgressWidget(m_updater, mainWidget);
    m_updaterWidget = new UpdaterWidget(mainWidget);
    m_changelogWidget = new ChangelogWidget(this);
    connect(m_updaterWidget, SIGNAL(selectedResourceChanged(AbstractResource*)),
            m_changelogWidget, SLOT(setResource(AbstractResource*)));

    mainLayout->addWidget(m_powerMessage);
    mainLayout->addWidget(m_progressWidget);
    mainLayout->addWidget(m_updaterWidget);
    mainLayout->addWidget(m_changelogWidget);

    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
    setupActions();
    progressingChanged();

    connect(m, SIGNAL(backendsChanged()), SLOT(finishedReloading()));
    connect(m, SIGNAL(allInitialized()), SLOT(initBackend()));
}

void MainWindow::setupActions()
{
    MuonMainWindow::setupActions();

    m_applyAction = actionCollection()->addAction("apply");
    m_applyAction->setIcon(KIcon("dialog-ok-apply"));
    m_applyAction->setText(i18nc("@action Downloads and installs updates", "Install Updates"));
    connect(m_applyAction, SIGNAL(triggered()), m_updater, SLOT(updateAll()));
    m_applyAction->setEnabled(false);

    KStandardAction::preferences(this, SLOT(editSettings()), actionCollection());

    setActionsEnabled(false);

    setupGUI(StandardWindowOption(KXmlGuiWindow::Default & ~KXmlGuiWindow::StatusBar));
}

void MainWindow::initBackend()
{
    m_updaterWidget->setBackend(m_updater);
    setupBackendsActions();

    setActionsEnabled();
}

void MainWindow::setupBackendsActions()
{
    foreach (QAction* action, m_updater->messageActions()) {
        if (action->priority()==QAction::HighPriority) {
            KActionMessageWidget* w = new KActionMessageWidget(action, centralWidget());
            qobject_cast<QBoxLayout*>(centralWidget()->layout())->insertWidget(1, w);
        } else {
            toolBar("mainToolBar")->addAction(action);
        }
    }
}

void MainWindow::progressingChanged()
{
    QApplication::restoreOverrideCursor();
    m_updaterWidget->setCurrentIndex(0);
    m_changelogWidget->setResource(nullptr);
    m_changelogWidget->hide();

    bool active = m_updater->isProgressing();
    m_progressWidget->setVisible(active);
    m_updaterWidget->setVisible(!active);
    setActionsEnabled(!active);
}

void MainWindow::startedReloading()
{
    setCanExit(false);
    setActionsEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_changelogWidget->setResource(0);
}

void MainWindow::finishedReloading()
{
    QApplication::restoreOverrideCursor();
    checkPlugState();
    setActionsEnabled(true);
    setCanExit(true);
}

void MainWindow::setActionsEnabled(bool enabled)
{
    MuonMainWindow::setActionsEnabled(enabled);
    m_applyAction->setEnabled(enabled && m_updater->hasUpdates());
}

void MainWindow::editSettings()
{
    if (!m_settingsDialog) {
        m_settingsDialog = new UpdaterSettingsDialog(this);
        connect(m_settingsDialog, SIGNAL(okClicked()), SLOT(closeSettingsDialog()));
        m_settingsDialog->show();
    } else {
        m_settingsDialog->raise();
    }
}

void MainWindow::closeSettingsDialog()
{
    m_settingsDialog->deleteLater();
    m_settingsDialog = nullptr;
}

void MainWindow::checkPlugState()
{
    const QList<Solid::Device> acAdapters = Solid::Device::listFromType(Solid::DeviceInterface::AcAdapter);

    bool isPlugged = acAdapters.isEmpty();
    for(Solid::Device device_ac : acAdapters) {
        Solid::AcAdapter* acAdapter = device_ac.as<Solid::AcAdapter>();
        isPlugged |= acAdapter->isPlugged();
        connect(acAdapter, SIGNAL(plugStateChanged(bool,QString)),
                this, SLOT(updatePlugState(bool)), Qt::UniqueConnection);
    }

    updatePlugState(isPlugged);
}

void MainWindow::updatePlugState(bool plugged)
{
    m_powerMessage->setVisible(!plugged);
}

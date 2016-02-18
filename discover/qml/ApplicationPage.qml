/*
 *   Copyright (C) 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library/Lesser General Public License
 *   version 2, or (at your option) any later version, as published by the
 *   Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library/Lesser General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import QtQuick.Layouts 1.1
import org.kde.kquickcontrolsaddons 2.0
import org.kde.discover.app 1.0

Item {
    id: appInfo
    property QtObject application: null
    clip: true

    readonly property var icon: application.icon
    readonly property string title: application.name

    ConditionalLoader {
        anchors.fill: parent
        condition: app.isCompact

        componentFalse: Item {
            readonly property real proposedMargin: (width-app.actualWidth)/2

            GridLayout {
                x: proposedMargin
                width: app.actualWidth
                height: parent.height
                columns: 2
                rows: 2

                ApplicationHeader {
                    id: header
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    application: appInfo.application
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.preferredWidth: app.actualWidth/3

                    Item {
                        id: screenshotsPlaceholder
                        Layout.fillWidth: true
                        Layout.preferredHeight: width/1.618

                        ApplicationScreenshots {
                            initialParent: screenshotsPlaceholder
                            fullParent: appInfo
                            application: appInfo.application
                        }
                    }
                    ApplicationDetails {
                        Layout.fillWidth: true
                        application: appInfo.application
                    }
                    Item {
                        Layout.fillHeight: true
                    }
                }
                ScrollView {
                    id: scroll
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: app.actualWidth/2

                    ApplicationDescription {
                        width: scroll.viewport.width-margin/2
                        application: appInfo.application
                        isInstalling: header.isInstalling
                    }
                }
            }
        }
        componentTrue: ScrollView {
            id: scroll
            flickableItem.flickableDirection: Flickable.VerticalFlick

            ColumnLayout {
                width: scroll.viewport.width
                x: desc.margin/2

                ApplicationHeader {
                    id: header
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    application: appInfo.application
                }

                ApplicationDetails {
                    id: details
                    Layout.fillWidth: true
                    application: appInfo.application
                }

                Item {
                    id: screenshotsPlaceholder
                    Layout.fillWidth: true
                    Layout.minimumHeight: details.height

                    ApplicationScreenshots {
                        application: appInfo.application
                        initialParent: screenshotsPlaceholder
                        fullParent: scroll
                    }
                }

                ApplicationDescription {
                    id: desc
                    Layout.fillWidth: true

                    application: appInfo.application
                    z: -1
                }
            }
        }
    }
}

import QtQuick
import QtQuick.Controls
import QtWebEngine

Item {
    Component {
                id: tabComponent
                WebEngineView {
                    id: webEngineView
                    focus: true

                    onLinkHovered: function(hoveredUrl) {
                        if (hoveredUrl === "")
                            hideStatusText.start();
                        else {
                            statusText.text = hoveredUrl;
                            statusBubble.visible = true;
                            hideStatusText.stop();
                        }
                    }

                    states: [
                        State {
                            name: "FullScreen"
                            PropertyChanges {
                                target: tabBar
                                visible: false
                                height: 0
                            }
                            PropertyChanges {
                                target: navigationBar
                                visible: false
                            }
                        }
                    ]
                    settings.localContentCanAccessRemoteUrls: true
                    settings.localContentCanAccessFileUrls: false
                    settings.autoLoadImages: appSettings.autoLoadImages
                    settings.javascriptEnabled: appSettings.javaScriptEnabled
                    settings.errorPageEnabled: appSettings.errorPageEnabled
                    settings.pluginsEnabled: appSettings.pluginsEnabled
                    settings.fullScreenSupportEnabled: appSettings.fullScreenSupportEnabled
                    settings.autoLoadIconsForPage: appSettings.autoLoadIconsForPage
                    settings.touchIconsEnabled: appSettings.touchIconsEnabled
                    settings.webRTCPublicInterfacesOnly: appSettings.webRTCPublicInterfacesOnly
                    settings.pdfViewerEnabled: appSettings.pdfViewerEnabled
                    settings.imageAnimationPolicy: appSettings.imageAnimationPolicy
                    settings.screenCaptureEnabled: true

                    onCertificateError: function(error) {
                        if (!error.isMainFrame) {
                            error.rejectCertificate();
                            return;
                        }

                        error.defer();
                        sslDialog.enqueue(error);
                    }

                    onNewWindowRequested: function(request) {
                        if (!request.userInitiated)
                            console.warn("Blocked a popup window.");
                        else if (request.destination === WebEngineNewWindowRequest.InNewTab) {
                            var tab = tabBar.createTab(currentWebView.profile, true, request.requestedUrl);
                            tab.acceptAsNewWindow(request);
                        } else if (request.destination === WebEngineNewWindowRequest.InNewBackgroundTab) {
                            var backgroundTab = tabBar.createTab(currentWebView.profile, false);
                            backgroundTab.acceptAsNewWindow(request);
                        } else if (request.destination === WebEngineNewWindowRequest.InNewDialog) {
                            var dialog = applicationRoot.createDialog(currentWebView.profile);
                            dialog.currentWebView.acceptAsNewWindow(request);
                        } else {
                            var window = applicationRoot.createWindow(currentWebView.profile);
                            window.currentWebView.acceptAsNewWindow(request);
                        }
                    }

                    onFullScreenRequested: function(request) {
                        if (request.toggleOn) {
                            webEngineView.state = "FullScreen";
                            browserWindow.previousVisibility = browserWindow.visibility;
                            browserWindow.showFullScreen();
                            fullScreenNotification.show();
                        } else {
                            webEngineView.state = "";
                            browserWindow.visibility = browserWindow.previousVisibility;
                            fullScreenNotification.hide();
                        }
                        request.accept();
                    }

                    onRegisterProtocolHandlerRequested: function(request) {
                        console.log("accepting registerProtocolHandler request for "
                                    + request.scheme + " from " + request.origin);
                        request.accept();
                    }

                    onDesktopMediaRequested: function(request) {
                        // select the primary screen
                        request.selectScreen(request.screensModel.index(0, 0));
                    }

                    onRenderProcessTerminated: function(terminationStatus, exitCode) {
                        var status = "";
                        switch (terminationStatus) {
                        case WebEngineView.NormalTerminationStatus:
                            status = "(normal exit)";
                            break;
                        case WebEngineView.AbnormalTerminationStatus:
                            status = "(abnormal exit)";
                            break;
                        case WebEngineView.CrashedTerminationStatus:
                            status = "(crashed)";
                            break;
                        case WebEngineView.KilledTerminationStatus:
                            status = "(killed)";
                            break;
                        }

                        print("Render process exited with code " + exitCode + " " + status);
                        reloadTimer.running = true;
                    }

                    onSelectClientCertificate: function(selection) {
                        selection.certificates[0].select();
                    }

                    onFindTextFinished: function(result) {
                        if (!findBar.visible)
                            findBar.visible = true;

                        findBar.numberOfMatches = result.numberOfMatches;
                        findBar.activeMatch = result.activeMatch;
                    }

                    onLoadingChanged: function(loadRequest) {
                        if (loadRequest.status === WebEngineView.LoadStartedStatus)
                            findBar.reset();
                    }

                    onPermissionRequested: function(permission) {
                        permissionDialog.permission = permission;
                        permissionDialog.visible = true;
                    }
                    onWebAuthUxRequested: function(request) {
                        webAuthDialog.init(request);
                    }

                    Timer {
                        id: reloadTimer
                        interval: 0
                        running: false
                        repeat: false
                        onTriggered: currentWebView.reload()
                    }
                }
            }

}

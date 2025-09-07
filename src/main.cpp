#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLocale>
#include <QTranslator>
#include "vlistmodel.h"
#include "vrunner.h"
#include <QQmlContext>
#include <QDir>
#include "newsmanager.h"
#include <QQuickWindow>
#include <QQuickGraphicsConfiguration>
#include <mimalloc-new-delete.h>

void tmpClean()
{
    QDir tempdir(QDir::cleanPath(QDir::tempPath() + QDir::separator() + "pvm"));
    if (!tempdir.removeRecursively()){
        qDebug() << "TMP dir cleaning failed.";
    }
}


int main(int argc, char *argv[])
{
    mi_option_set(mi_option_purge_delay, 0);
    mi_option_set(mi_option_purge_decommits, 1);
    mi_option_disable(mi_option_eager_commit);
    mi_option_set(mi_option_abandoned_page_purge, 1);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QCoreApplication::setApplicationName("PVMenu");
    QCoreApplication::setOrganizationName("ProjectV");
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);
    QGuiApplication app(argc, argv);
    QObject::connect(&app, &QGuiApplication::aboutToQuit, tmpClean);

    QDir tempdir(QDir::cleanPath(QDir::tempPath() + QDir::separator() + "pvm"));
    if (tempdir.exists()){
        tempdir.removeRecursively();
        if (!tempdir.mkpath(".")){
            qDebug() << "TMP dir creation failed.";
        }
    } else {
        tempdir.mkpath(".");
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ProjectVMenu_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/ui/");


    VListModel channelsModel;
    VListModel appsModel;
    VListModel newsModel;
    NewsManager newsm(newsModel, channelsModel);
    VRunner vrunner(appsModel);
    const QUrl url(QStringLiteral("qrc:/ui/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("_vrunner", &vrunner);
    engine.rootContext()->setContextProperty("newsm", &newsm);
    engine.rootContext()->setContextProperty("channelsModel", &channelsModel);
    engine.rootContext()->setContextProperty("appsModel", &appsModel);
    engine.rootContext()->setContextProperty("newsModel", &newsModel);
    engine.load(url);

    return app.exec();
}

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLocale>
#include <QTranslator>
#include "vlistmodel.h"
#include "vrunner.h"
#include <QQmlContext>
#include <QDir>
#include "newsmanager.h"


void tmpClean()
{
    QDir tempdir(QDir::cleanPath(QDir::tempPath() + QDir::separator() + "pvm"));
    if (!tempdir.removeRecursively()){
        qDebug() << "TMP dir cleaning failed.";
    }
}


void scanApps(VListModel* model) {
    QDir applicationsDir("/usr/share/applications/");
    if (!applicationsDir.exists()) {
        qWarning() << "Directory /usr/share/applications/ does not exist.";
        return;
    }

    QStringList filters;
    filters << "*.desktop";
    QFileInfoList desktopFiles = applicationsDir.entryInfoList(filters, QDir::Files | QDir::Readable);

    for (const QFileInfo& fileInfo : std::as_const(desktopFiles)) {
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Could not open file for reading:" << fileInfo.absoluteFilePath();
            continue;
        }

        QTextStream in(&file);
        QString name;
        QString execute;
        QString tryexecute;
        QString icon;
        QString currentSection;
        bool desktopEntryFound = false;

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();

            if (line.startsWith("[")) {
                currentSection = line;
                if (currentSection == "[Desktop Entry]") {
                    desktopEntryFound = true;
                }
            } else if (desktopEntryFound && currentSection == "[Desktop Entry]") {
                int equalsIndex = line.indexOf('=');
                if (equalsIndex > 0) {
                    QString key = line.left(equalsIndex);
                    QString value = line.mid(equalsIndex + 1);

                    if (key == "Name") {
                        name = value;
                    } else if (key == "Exec") {
                        execute = value;
                    } else if (key == "TryExec") {
                        tryexecute = value;
                    } else if (key == "Icon") {
                        icon = value;
                    }
                }
            }
        }
        file.close();

        if (desktopEntryFound) {
            model->addData(VElement(name, "file://" + icon, tryexecute.isEmpty() ? execute : tryexecute));
        }
    }
}


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QCoreApplication::setApplicationName("PVMenu");
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
    engine.addImportPath("qrc:/");

    VListModel channelsModel;
    VListModel appsModel;
    VListModel newsModel;
    NewsManager newsm(newsModel, channelsModel);
    scanApps(&appsModel);
    VRunner vrunner(appsModel);

    const QUrl url(QStringLiteral("qrc:/Main.qml"));
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

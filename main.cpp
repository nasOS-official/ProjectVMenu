#include "imageparser.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLocale>
#include <QTranslator>
#include <iostream>
#include <ostream>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/parser.h>

#ifdef __FreeBSD__
#include <malloc_np.h>
#else // __FreeBSD__
#include <jemalloc/jemalloc.h>
#endif // !__FreeBSD__

int main(int argc, char *argv[])
{
    auto backgroundThread = true;
    mallctl("background_thread", nullptr, nullptr, &backgroundThread, sizeof(bool));
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);



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
    // xmlInitParser();
    // curl_global_init(CURL_GLOBAL_DEFAULT);

    // std::string rss_url = "https://habr.com/ru/rss/hubs/reverse-engineering/articles/?fl=ru";

    // std::vector<std::vector<std::string>> articles = parseRSS(rss_url);

    // if (articles.empty()) {
    //     std::cout << "No articles parsed or failed to fetch/parse RSS feed." << std::endl;
    // } else {
    //     std::cout << "Parsed " << articles.size() << " articles from RSS feed:" << std::endl;
    //     for (const auto& article : articles) {
    //         std::cout << "--------------------" << std::endl;
    //         if (article.size() > 0) std::cout << "Title: " << article[0] << std::endl;
    //         if (article.size() > 1) std::cout << "Link: " << article[1] << std::endl;
    //         if (article.size() > 2 && !article[2].empty()) std::cout << "Image: " << article[2] << std::endl;
    //         else if (article.size() > 2) std::cout << "Image: (Not found or not applicable)" << std::endl;
    //     }
    //     std::cout << "--------------------" << std::endl;
    // }

    // xmlCleanupParser();
    // curl_global_cleanup();
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
    engine.load(url);

    return app.exec();
}

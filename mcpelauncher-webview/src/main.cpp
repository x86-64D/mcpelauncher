#include <QtCore/QUrl>
#include <QtCore/QCommandLineParser>
#include <QGuiApplication>
#include <QScreen>
#include <QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#if QT_VERSION >= 0x50C00
#include <QWebEngineUrlScheme>
#endif
#include <QWebEngineUrlRequestJob>
#include <QWebEngineProfile>
#include <iostream>
#include "main.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("startUrl", "The initial URL to open");
    parser.addPositionalArgument("endUrl", "The URL which will cause the webview to quit");
    parser.process(app);
    if (parser.positionalArguments().size() != 2) {
        return -1;
    }
    auto startUrl = QUrl::fromUserInput(parser.positionalArguments()[0]);
    auto endUrl = QUrl::fromUserInput(parser.positionalArguments()[1]);

    auto scheme = endUrl.scheme().toStdString();
#if QT_VERSION >= 0x50C00
    QWebEngineUrlScheme::registerScheme(QWebEngineUrlScheme(scheme.c_str()));
#endif
    SchemeHandler handler(endUrl.toString());
    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler(scheme.c_str(), &handler);

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    context->setContextProperty(QStringLiteral("startUrl"), startUrl);
    QRect geometry = QGuiApplication::primaryScreen()->availableGeometry();
    const QSize size = geometry.size() * 4 / 5;
    const QSize offset = (geometry.size() - size) / 2;
    const QPoint pos = geometry.topLeft() + QPoint(offset.width(), offset.height());
    geometry = QRect(pos, size);

    context->setContextProperty(QStringLiteral("initialX"), geometry.x());
    context->setContextProperty(QStringLiteral("initialY"), geometry.y());
    context->setContextProperty(QStringLiteral("initialWidth"), geometry.width());
    context->setContextProperty(QStringLiteral("initialHeight"), geometry.height());

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();

}

void SchemeHandler::requestStarted(QWebEngineUrlRequestJob *request) {
    auto url = request->requestUrl().toString();
    if (url.startsWith(endUrl)) {
        std::cout << url.toStdString() << std::endl;
        qApp->quit();
    }
}

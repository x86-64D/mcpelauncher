#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCommandLineParser>

#include <QTranslator>
#include <QLocale>
#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("Minecraft Linux Launcher");
    QCoreApplication::setOrganizationDomain("mrarm.io");
    QCoreApplication::setApplicationName("Minecraft Linux Launcher Error UI");

    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Minecraft Linux Launcher Error Helper");
    parser.addHelpOption();
    parser.addPositionalArgument("title", QCoreApplication::translate("main", "Title of the error"));
    parser.addPositionalArgument("description", QCoreApplication::translate("main", "Description of the error"));

    parser.process(app);

    const QStringList args = parser.positionalArguments();

    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("mcpelauncher"), QLatin1String("_"), QLatin1String(":/translations"))) {
        app.installTranslator(&translator);
    } else {
        qDebug() << "cannot load translator " << QLocale().name() << " check content of translations.qrc";
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("TITLE", args.at(0));
    engine.rootContext()->setContextProperty("DESCRIPTION", args.at(1));
    engine.load(QUrl(QStringLiteral("qrc:/src/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
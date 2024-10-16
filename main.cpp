#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "WordsProcessor.h"

QObject* createWordProcessor( QQmlEngine* qmlEngine, QJSEngine* jsEngine ) {
    Q_UNUSED( qmlEngine )
    Q_UNUSED( jsEngine )
    return &WordsProcessor::instance();
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterSingletonType< WordsProcessor >( "com.avitvitskii.wordsprocessor", 1, 0, "WordsProcessor", createWordProcessor );

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/WordsHistogram/Main.qml"));
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

#pragma once

#ifndef WORDSPROCESSOR_H
#define WORDSPROCESSOR_H

#include <QObject>
#include <QFutureWatcher>

class WordsProcessor : public QObject
{
    Q_OBJECT
public:
    static WordsProcessor& instance();

    Q_INVOKABLE void loadFile( const QString &strFilePath );

signals:
    void processingCanceled();
    void processingPaused();
    void processingFinished( const QVariantList &oWordsCount );
    void progressChanged( int progress );

public slots:
    void cancelProcessing();
    void pauseProcessing();

private:
    WordsProcessor();
    ~WordsProcessor();
    QFutureWatcher<void> m_watcher;
    bool m_bCancelRequested;
    bool m_bPauseRequested;

    QVariantList processFile( const QString &strFilePath, std::function<void(int)> reportProgress );
};

#endif // WORDSPROCESSOR_H

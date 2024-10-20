#pragma once

#ifndef WORDSPROCESSOR_H
#define WORDSPROCESSOR_H

#include <QObject>
#include <QFutureWatcher>

class WordsProcessor Q_DECL_FINAL:
    public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool isProcessing READ isProcessing WRITE setIsProcessing NOTIFY isProcessingChanged )
public:
    static WordsProcessor& instance();

    Q_INVOKABLE void loadFile( const QString &strFilePath );

    bool isProcessing() const
    {
        return m_bIsProcessing;
    }

signals:
    void processingCanceled();
    void processingPaused();
    void processingFinished( const QVariantList &oWordsCount );
    void isProcessingChanged( bool isProcessing );
    void progressChanged( int progress );

public slots:
    void cancelProcessing();
    void pauseProcessing();
    void controlStopWords( bool isEnabled );

private:
    WordsProcessor():
        m_bCancelRequested( false ),
        m_bPauseRequested( false ),
        m_bIsProcessing( false ),
        m_bIsStopWordsEnabled( false )
    {
    }
    QFutureWatcher< void > m_watcher;
    bool m_bCancelRequested;
    bool m_bPauseRequested;
    bool m_bIsProcessing;
    bool m_bIsStopWordsEnabled;

    QVariantList processFile( const QString &strFilePath, std::function< void( int ) > reportProgress );
    void setIsProcessing( bool isProcessing );
};

#endif // WORDSPROCESSOR_H

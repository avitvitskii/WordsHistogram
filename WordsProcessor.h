#pragma once

#ifndef WORDSPROCESSOR_H
#define WORDSPROCESSOR_H

#include <QObject>

class WordsProcessor : public QObject
{
    Q_OBJECT
public:
    static WordsProcessor& instance();

    Q_INVOKABLE void loadFile( const QString &strFilePath );

signals:
    void processingFinished( const QVariantList &oWordsCount );
    void progressChanged( int progress );

private:
    WordsProcessor();
    ~WordsProcessor();

    QVariantList processFile( const QString &strFilePath, std::function<void(int)> reportProgress );
};

#endif // WORDSPROCESSOR_H

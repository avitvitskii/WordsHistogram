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

private:
    WordsProcessor();
    ~WordsProcessor();

    QVariantList processFile( const QString &strFilePath );
};

#endif // WORDSPROCESSOR_H

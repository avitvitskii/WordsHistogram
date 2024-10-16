#include <QDebug>
#include <QHash>
#include <QFile>
#include <QUrl>
#include <QRegularExpression>

#include "WordsProcessor.h"

WordsProcessor::WordsProcessor()
{
}

WordsProcessor::~WordsProcessor()
{
}

WordsProcessor &WordsProcessor::instance()
{
    static WordsProcessor instance;
    return instance;
}

QVariantMap convertHashToMap( const QHash< QString, int > &hash )
{
    QVariantMap map;
    for ( QHash< QString, int >::const_iterator it = hash.constBegin(); it != hash.constEnd(); ++it )
    {
        map.insert( it.key(), it.value() );
    }
    return map;
}

void WordsProcessor::loadFile( const QString &strFilePath )
{
    QHash< QString, int > oWordsCount = processFile( strFilePath );

    emit processingFinished( convertHashToMap( oWordsCount ) );
}

QHash< QString, int > WordsProcessor::processFile( const QString &strFilePath )
{
    QHash< QString, int > oWordsCount;
    QUrl url( strFilePath );
    QString strFileName = url.toLocalFile();

    QFile file( strFileName );

    if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        QTextStream in( &file );
        QString strFileContent = in.readAll();
        QStringList arrWords = strFileContent.split( QRegularExpression("[^\\p{L}]+" ), Qt::SkipEmptyParts );
        for ( int i = 0; i < arrWords.length(); i++ )
        {
            oWordsCount[ arrWords[i] ] = oWordsCount.contains( arrWords[i] ) ? oWordsCount.value( arrWords[i] ) + 1 : 1;
        }
    }

    return oWordsCount;
}

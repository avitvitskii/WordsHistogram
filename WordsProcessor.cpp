#include <QDebug>
#include <QHash>
#include <QFile>
#include <QUrl>
#include <QRegularExpression>
#include <QtConcurrent>

#include "WordsProcessor.h"

namespace {

const int MAX_WORDS = 15;

}//namespace

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

bool compareByCount(const std::pair<QString, int> &count1, const std::pair<QString, int> &count2)
{
    return count1.second > count2.second;
}

void WordsProcessor::loadFile( const QString &strFilePath )
{
    QtConcurrent::run([this, strFilePath]() {
        QVariantList oWordsCount = processFile( strFilePath, [this](int progress) {
            emit progressChanged( progress );
        });

        emit processingFinished(  oWordsCount );
    });
}

QVariantList WordsProcessor::processFile( const QString &strFilePath, std::function<void(int)> reportProgress )
{
    QVariantList oWordsCount;
    QUrl url( strFilePath );
    QString strFileName = url.toLocalFile();

    QFile file( strFileName );

    if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        QTextStream in( &file );
        QString strFileContent = in.readAll();
        QStringList arrWords = strFileContent.split( QRegularExpression("[^\\p{L}]+" ), Qt::SkipEmptyParts );

        std::map< QString, int > oWordCountsMap;
        int nCurrentProgress = 0;
        for ( int wordIndex = 0; wordIndex < arrWords.size(); ++wordIndex ) {
            oWordCountsMap[arrWords[wordIndex].toLower()]++;

            size_t nProgress = static_cast< size_t >( static_cast< double >(wordIndex + 1) / ( arrWords.size() / 100 ) );
            if ( nProgress != nCurrentProgress ) {
                nCurrentProgress = nProgress;
                reportProgress( nProgress );
            }
        }
        std::vector< std::pair< QString, int > > arrWordsPairs(oWordCountsMap.begin(), oWordCountsMap.end());
        std::sort( arrWordsPairs.begin(), arrWordsPairs.end(), compareByCount );

        int nWordsToProcess = arrWordsPairs.size() < MAX_WORDS ? arrWordsPairs.size() : MAX_WORDS;
        for ( int i = 0; i < nWordsToProcess; i++ ) {
            QVariantMap wordData;
            wordData["word"] = arrWordsPairs[i].first;
            wordData["count"] = arrWordsPairs[i].second;
            oWordsCount.append( wordData );
        }
    }

    return oWordsCount;
}

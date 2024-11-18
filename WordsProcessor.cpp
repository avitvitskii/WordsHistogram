#include <QDebug>
#include <QHash>
#include <QFile>
#include <QUrl>
#include <QRegularExpression>
#include <QtConcurrent>

#include "WordsProcessor.h"

namespace {

const int MAX_WORDS = 15;
const QStringList STOP_WORDS = {
    // Русские стоп-слова
    "и", "в", "не", "он", "на", "я", "с", "что", "как", "по", "это", "к", "так",
    "все", "но", "да", "из", "который", "для", "мне", "было", "быть", "от",
    "поэтому", "если", "также", "или", "время", "где", "другой", "кто", "свой",
    "чтобы", "всегда", "ни", "после", "перед", "только", "тогда", "то", "как",
    "из-за", "моя", "его", "это", "нам", "всех", "будет", "никогда", "сейчас",
    "вместе", "потому", "между", "даже", "такой", "можно", "теперь", "но",
    "тем", "все-таки", "просто", "такой", "здесь", "далее", "затем", "при",
    "всё", "может", "знать", "через", "хотя", "ли", "чем", "таким", "почему",
    "должен", "всегда", "когда", "такое", "каждый", "где-то", "здесь-то",
    "а", "бы", "ну", "вот", "ты", "мы", "вы", "они", "тебя", "есть", "там",
    "у", "за", "она", "её", "же", "уже", "нет", "меня", "тебе", "ещё", "о",
    "про", "этого", "этом", "какой", "какие", "всего", "чём", "том", "ей",

    // Английские стоп-слова
    "the", "is", "in", "and", "to", "of", "it", "that", "you", "he", "was", "for",
    "on", "are", "with", "as", "by", "this", "an", "be", "at", "have", "or",
    "not", "all", "but", "they", "we", "his", "her", "she", "if", "more", "about",
    "than", "so", "its", "no", "their", "what", "when", "who", "which", "there",
    "out", "up", "do", "only", "just", "now", "some", "my", "your", "would",
    "could", "should", "these", "those", "then", "into", "over", "after",
    "very", "most", "any", "same", "few", "between", "being", "like", "through",
    "back", "another", "because", "before", "two", "way", "make", "me", "him",
    "her", "us", "them", "myself", "himself", "herself", "themselves", "a", "i",
};

}//namespace

WordsProcessor &WordsProcessor::instance()
{
    static WordsProcessor instance;
    return instance;
}

bool compareByCount( const std::pair<QString, int> &count1, const std::pair<QString, int> &count2 )
{
    return count1.second > count2.second;
}

void WordsProcessor::loadFile( const QString &strFilePath )
{
    if ( m_bIsProcessing && !m_bPauseRequested )
    {
        return;
    }

    setIsProcessing(true);
    m_bCancelRequested = false;
    if ( m_bPauseRequested )
    {
        m_bPauseRequested = false;
        return;
    }

    QFuture<void> future = QtConcurrent::run([this, strFilePath]() {
        QVariantList oWordsCount = processFile( strFilePath, [this]( int progress ) {
            emit progressChanged( progress );
        });

        if ( m_bCancelRequested )
        {
            emit processingCanceled();
        }
        else if ( m_bPauseRequested )
        {
            emit processingPaused();
        }
        else
        {
            emit processingFinished( oWordsCount );
        }

        setIsProcessing( false );
    });

    m_watcher.setFuture( future );
}

void WordsProcessor::cancelProcessing()
{
    m_bCancelRequested = true;
    m_bPauseRequested = false;
}

void WordsProcessor::pauseProcessing()
{
    m_bPauseRequested = true;
}

void WordsProcessor::controlStopWords( bool isEnabled )
{
    m_bIsStopWordsEnabled = isEnabled;
}

void WordsProcessor::setIsProcessing(bool isProcessing)
{
    if ( m_bIsProcessing != isProcessing ) {
        m_bIsProcessing = isProcessing;
        emit isProcessingChanged( isProcessing );
    }
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
        for ( int wordIndex = 0; wordIndex < arrWords.size(); ++wordIndex )
        {
            while ( m_bPauseRequested )
            {
                if ( m_bCancelRequested )
                {
                    oWordsCount.clear();
                    return oWordsCount;
                }

                QThread::msleep(100);
            }

            if ( m_bCancelRequested )
            {
                oWordsCount.clear();
                return oWordsCount;
            }

            QString strWord = arrWords[wordIndex].toLower();
            if ( m_bIsStopWordsEnabled && STOP_WORDS.contains( strWord ) )
            {
                continue;
            }

            oWordCountsMap[strWord]++;

            size_t nProgress = static_cast< size_t >( static_cast< double >(wordIndex + 1) / ( arrWords.size() / 100 ) );
            if ( nProgress != nCurrentProgress )
            {
                nCurrentProgress = nProgress;
                reportProgress( nProgress );
            }
        }
        std::vector< std::pair< QString, int > > arrWordsPairs(oWordCountsMap.begin(), oWordCountsMap.end());
        std::sort( arrWordsPairs.begin(), arrWordsPairs.end(), compareByCount );

        int nWordsToProcess = arrWordsPairs.size() < MAX_WORDS ? arrWordsPairs.size() : MAX_WORDS;
        for ( int i = 0; i < nWordsToProcess; i++ )
        {
            QVariantMap wordData;
            wordData["word"] = arrWordsPairs[i].first;
            wordData["count"] = arrWordsPairs[i].second;
            oWordsCount.append( wordData );
        }
    }

    return oWordsCount;
}

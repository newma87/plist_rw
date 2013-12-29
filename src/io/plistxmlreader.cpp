/*
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#include <QtGui>

#include <vector>
#include <string>
#include <map>

#include "io/plistxmlreader.h"
#include "core/frame.h"
#include "core/framecollector.h"

#include "boost/any.hpp"
#include "io/Plist.hpp"

#define tr(str) QObject::tr(str)
#define TITLE tr("Plist Xml Reader")

#define boost_any_cast(type, dict, name)  (boost::any_cast<const type &>(dict.find(name)->second))

using namespace std;

typedef map<string, boost::any> dictionary;

static QList<int> convertString2NumberList(const char* str)
{
    QList<int> num_list;

    QString qStr(str);
    qStr = qStr.replace(QRegExp("[\{\}]"), "");
    QStringList str_list = qStr.split(',');

    foreach (QString s, str_list) {
        num_list.append(s.toInt());
    }

    return num_list;
}

static bool readFrame(const dictionary& dict, const QPixmap& image, Frame* pFrame)
{
    QList<int> num_list = convertString2NumberList(boost::any_cast<const string &>(dict.find("frame")->second).c_str());
    QRect rect;
    rect.setX(num_list[0]);
    rect.setY(num_list[1]);
    rect.setWidth(num_list[2]);
    rect.setHeight(num_list[3]);

    pFrame->rect = rect;
    pFrame->image = image.copy(rect);
    return true;
}

static bool readFrames(const dictionary& dict, const QPixmap& image, FrameCollector* pCollector)
{
    if (!pCollector)
    {
        return false;
    }

    pCollector->setSize(image.size());

    dictionary::const_iterator it = dict.begin();
    for (; it != dict.end(); ++it)
    {
        Frame* pFrame = new Frame();
        pFrame->name = it->first.c_str();
        pFrame->bValid = true;
        if (readFrame(boost::any_cast<const dictionary&>(it->second), image, pFrame))
        {
            pCollector->addFrame(pFrame);
        }
        else
        {
            delete pFrame;
            pFrame = NULL;
        }
    }

    return true;
}

static bool readMetaData(const dictionary& dict, QPixmap& image, const QString file_path)
{
    int format_version = boost_any_cast(int64_t, dict, "format");
    if (format_version != 1)
    {
        QMessageBox::information(qApp->activeWindow(), TITLE, tr("Error file format, plist file format version is not 1!"));
        return false;
    }

    string strSize = boost_any_cast(string, dict, "size");
    QList<int> numList = convertString2NumberList(strSize.c_str());
    QSize imageSize = QSize(numList[0], numList[1]);

    QString imageFile = QString().fromStdString(boost_any_cast(string, dict, "textureFileName"));
    QString entireImagePath = QFileInfo(file_path).path() + "/" + imageFile;

    if (!image.load(entireImagePath, imageFile.split(".", QString::SkipEmptyParts).at(1).toLocal8Bit()))
    {
        QMessageBox::information(qApp->activeWindow(), TITLE, tr("Load image file %1 failed!").arg(entireImagePath));
        return false;
    }

    if (image.size() != imageSize)
    {
        return false;
    }

    return true;
}

PlistXmlReader::PlistXmlReader(FrameCollector* pCollector) :
    m_pCollector(pCollector)
{
}

bool PlistXmlReader::read(QString xmlFile)
{
    dictionary dict;
    QPixmap image;

    Plist::readPlist(xmlFile.toStdString(), dict);
    const dictionary& frameDict = boost_any_cast(dictionary, dict, "frames");
    const dictionary& metadataDict = boost_any_cast(dictionary, dict, "metadata");

    if (!readMetaData(metadataDict, image, xmlFile))
    {
        return false;
    }

    return readFrames(frameDict, image, m_pCollector);
}

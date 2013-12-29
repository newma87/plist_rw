/*
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#include <QtGui>

#include <vector>
#include <map>
#include <string>

#include "io/plistxmlwriter.h"
#include "core/frame.h"
#include "core/framecollector.h"

#include "boost/any.hpp"
#include "io/Plist.hpp"

using namespace std;

#define tr(str) QObject::tr(str)
typedef map<string, boost::any> dictionary;

static string covertQSize2String(const QSize& sz)
{
    return tr("{%1,%2}").arg(sz.width()).arg(sz.height()).toStdString();
}

static string covertQRect2String(const QRect& rt)
{
    return tr("{{%1,%2},{%3,%4}}").arg(rt.x()).arg(rt.y()).arg(rt.width()).arg(rt.height()).toStdString();
}

static void generateFrame(const Frame* pFrame, dictionary& dict)
{
    dictionary subDict;
    subDict["frame"] = covertQRect2String(pFrame->rect);
    subDict["offset"] = string("{0,0}");
    subDict["sourceSize"] = covertQSize2String(pFrame->rect.size());

    dict.insert(make_pair<string, boost::any>(pFrame->name.toStdString(), subDict));
}

static void generateFrames(const FrameCollector* pCollector, dictionary& dict)
{
    FrameCollector::const_iterator it = pCollector->constBegin();
    for (; it != pCollector->constEnd(); ++it)
    {
        generateFrame(*it, dict);
    }
}

static bool generateMetaData(const FrameCollector* pCollector, dictionary& dict, QString file_path)
{
    bool bRet = false;

    QFileInfo fileInfo(file_path);
    QString imageFile = fileInfo.fileName();
    int index = imageFile.lastIndexOf('.');
    if (index != -1)
    {
        imageFile = imageFile.left(index) + ".png";
    }
    else
    {
        imageFile += ".png";
    }
    dict["realTextureFileName"] = imageFile.toStdString();
    dict["textureFileName"] = imageFile.toStdString();
    dict["size"] = covertQSize2String(pCollector->getSize());
    dict["format"] = int(1);

    QImage* pImage = NULL;
    if (pCollector->getImage(&pImage))
    {
        QString imagePath = fileInfo.path() + "/" + imageFile;
        QString backFile = imagePath + ".backup";
        if (QFileInfo(imagePath).exists())
        {
            QFile::rename(imagePath, backFile);
        }

        if (pImage && pImage->save(imagePath, "PNG"))
        {
            bRet = true;
        }

        if (bRet)
        {
            QFile::remove(backFile);
        }
        else
        {
            QFile::rename(backFile, imagePath);
        }
    }

    if (pImage)
    {
        delete pImage;
        pImage = NULL;
    }

    return bRet;
}

static bool generatePlistDictionary(FrameCollector* pCollector, dictionary& dict, QString file_path)
{
    dictionary frames;
    dictionary metadata;
    if (!generateMetaData(pCollector, metadata, file_path))
    {
        return false;
    }
    generateFrames(pCollector, frames);

    dict["frames"] = frames;
    dict["metadata"] = metadata;
    return true;
}

PlistXmlWriter::PlistXmlWriter(FrameCollector* pCollector) :
    m_pCollector(pCollector)
{
}

bool PlistXmlWriter::write(QString xmlFile)
{
    dictionary dict;
    if (!generatePlistDictionary(m_pCollector, dict, xmlFile))
    {
        return false;
    }
    Plist::writePlistXML(xmlFile.toStdString(), dict);

    return true;
}

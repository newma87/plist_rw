/*
 *  plistxmlreader.h
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#ifndef PLISTXMLREADER_H
#define PLISTXMLREADER_H

#include <QString>

class FrameCollector;

class PlistXmlReader
{
public:
    PlistXmlReader(FrameCollector* pCollector);

    bool read(QString xmlFile);
private:
    FrameCollector* m_pCollector;
};

#endif // PLISTXMLREADER_H

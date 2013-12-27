/*
 *  plistxmlwriter.h
 *  plist_rw version 1.0
 *  author by newma
 *  email to newma@live.cn
 */
#ifndef PLISTXMLWRITER_H
#define PLISTXMLWRITER_H

#include <QString>

class FrameCollector;

class PlistXmlWriter
{
public:
    PlistXmlWriter(FrameCollector* pCollector);

    bool write(QString xmlFile);

private:
    FrameCollector* m_pCollector;
};

#endif // PLISTXMLWRITER_H

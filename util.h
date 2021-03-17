#ifndef UTIL_H
#define UTIL_H
#include <QXmlStreamReader>
#include <QString>

QString get_attr(const QXmlStreamReader &reader, const QString &attr_name)
{
    foreach(const QXmlStreamAttribute &attr, reader.attributes())
    {
        if(attr.name().toString()==attr_name)
            return attr.value().toString();
    }
    return "";
}
#endif // UTIL_H


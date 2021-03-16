#ifndef CRC32_H
#define CRC32_H

#include <QtCore>
#include <QString>
#include <QMap>

class Crc32
{

public:
    Crc32();

   static QString calcFileCRC(QString fileName);

   static quint32 calcCRC32(QByteArray data);
};

#endif // CRC32_H

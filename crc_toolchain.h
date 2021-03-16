#ifndef CRC_TOOLCHAIN_H
#define CRC_TOOLCHAIN_H
#include <QByteArray>


class CRC_toolchain
{
public:
    CRC_toolchain();
    static unsigned char CRC8(unsigned char *pcBlock, unsigned int len);

    static unsigned char CRC8_table(unsigned char *pcBlock, unsigned char len);
};






#endif // CRC_TOOLCHAIN_H

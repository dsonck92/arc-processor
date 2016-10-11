/*
 * ARCMemory.h
 *
 *  Created on: Oct 24, 2012
 *      Author: dsonck
 */

#ifndef ARCMEMORY_H_
#define ARCMEMORY_H_

#include <QObject>
#include <QVector>

class ARCMemory
{
    quint32 m_addr;

private:
    virtual void writeHelper(quint32 loc, quint8* data, quint8 size) = 0;
    virtual void readHelper(quint32 loc, quint8* data, quint8 size) const = 0;

public:
    ARCMemory() { m_addr = 0; }
    virtual ~ARCMemory() {}

public:
    virtual void setAddress ( quint32 loc );

    virtual quint8 readByte ( ) const;
    virtual quint16 readWord ( ) const;
    virtual quint32 readDWord ( ) const;

    virtual void writeByte ( quint8 byte );
    virtual void writeWord ( quint16 word );
    virtual void writeDWord ( quint32 dword );

public:
    bool loadBinFile(QString filename);
    void saveBinFile(QString filename);

};

#endif /* ARCMEMORY_H_ */

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

class ARCMemory : public QObject
{
  Q_OBJECT

    quint8 *m_vu32Memory;
    quint32 m_addr;

  public:
    ARCMemory ( );
    virtual ~ARCMemory ( );

  public:
    void setAddress ( quint32 loc );

    quint8 readByte ( ) const;
    quint16 readWord ( ) const;
    quint32 readDWord ( ) const;

    quint32 readDWord ( quint32 loc ) const;

    void writeByte ( quint8 byte );
    void writeWord ( quint16 word );
    void writeDWord ( quint32 dword );

  public:
    quint8 read(quint32 addr) const {return m_vu32Memory[addr];}
    const quint8 *data() const {return m_vu32Memory;}

  public:
    bool loadBinFile(QString filename);
    void saveBinFile(QString filename);

  signals:
    void memoryChange ( quint32 begin , quint32 end );
};

#endif /* ARCMEMORY_H_ */

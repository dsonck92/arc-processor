/*
 * ARCMemory.cpp
 *
 *  Created on: Oct 24, 2012
 *      Author: dsonck
 */

#include "ARCMemory.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>

#include <qdebug.h>

ARCMemory::ARCMemory ( )
{
    // TODO Auto-generated constructor stub
    //m_vu32Memory = new quint8 [0xFFFFFFFF];
    //qMemSet(m_vu32Memory,0,0xFFFFFFFF);
}

ARCMemory::~ARCMemory ( )
{
    // TODO Auto-generated destructor stub
}

void ARCMemory::setAddress(quint32 loc)
{
    m_addr = loc;
}

void ARCMemory::writeByte(quint8 data)
{
//    qMemCopy(m_vu32Memory + m_addr,&data,sizeof(data));

//    emit memoryChange(m_addr,m_addr+1);
}

void ARCMemory::writeWord(quint16 data)
{
//    qMemCopy(m_vu32Memory + m_addr,&data,sizeof(data));

//    emit memoryChange(m_addr,m_addr+2);

}

void ARCMemory::writeDWord(quint32 data)
{
//    qMemCopy(m_vu32Memory + m_addr,&data,sizeof(data));

//    emit memoryChange(m_addr,m_addr+4);
}

quint8 ARCMemory::readByte() const
{
    quint8 data;
//    qMemCopy(&data,m_vu32Memory + m_addr,sizeof(data));

    return data;
}

quint16 ARCMemory::readWord() const
{
    quint16 data;
//    qMemCopy(&data,m_vu32Memory + m_addr,sizeof(data));

    return data;
}

quint32 ARCMemory::readDWord() const
{
    quint32 data;
//    qMemCopy(&data,m_vu32Memory + m_addr,sizeof(data));

    return data;
}

quint32 ARCMemory::readDWord(quint32 loc) const
{
    quint32 data;
//    qMemCopy(&data,m_vu32Memory + loc,sizeof(data));

    return data;
}

bool ARCMemory::loadBinFile(QString filename)
{
    return false;
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    {
        QTextStream str(&file);
        str.readLine(); // ORG
        while(!str.atEnd())
        {
            QStringList code = str.readLine().split(QRegExp("\\s"));

            qDebug() << code;
            quint32 addr = code[0].toUInt(0,16);
            quint32 inst = code[1].toUInt(0,16);

            qDebug() << addr << inst;

            //qMemCopy(m_vu32Memory + addr,&inst,sizeof(inst));

            emit memoryChange(addr,addr+4);
        }
    }

    return true;
}

void ARCMemory::saveBinFile(QString filename)
{
    return;
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    {
        QTextStream str(&file);
        str << "0\n"; // ORG
        for(int I=0;I<0xFFFFFFFF;I+=4)
        {
            quint32 inst;
            //qMemCopy(&inst,m_vu32Memory + I,sizeof(inst));

            if(inst != 0)
            {
                str << QString::number(I,16) << " " << QString::number(inst,16);
                qDebug() << QString::number(I,16) << ":" << QString::number(inst,16);
            }
        }
    }
}

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

void ARCMemory::writeByte(quint8 data)
{
    writeHelper(m_addr, &data, sizeof(data));
}

void ARCMemory::writeWord(quint16 data)
{
    writeHelper(m_addr, (quint8*)(&data), sizeof(data));
}

void ARCMemory::writeDWord(quint32 data)
{
    writeHelper(m_addr, (quint8*)(&data), sizeof(data));
}

quint8 ARCMemory::readByte() const
{
    quint8 data;
    readHelper(m_addr, &data, sizeof(data));

    return data;
}

quint16 ARCMemory::readWord() const
{
    quint16 data;
    readHelper(m_addr, (quint8*)(&data), sizeof(data));

    return data;
}

quint32 ARCMemory::readDWord() const
{
    quint32 data;
    readHelper(m_addr, (quint8*)(&data), sizeof(data));

    return data;
}

void ARCMemory::setAddress(quint32 loc)
{
    m_addr = loc;
}

bool ARCMemory::loadBinFile(QString filename)
{
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

            writeHelper(addr, (quint8*)(&inst), sizeof(inst));
        }
    }

    return true;
}

void ARCMemory::saveBinFile(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    {
        QTextStream str(&file);
        str << "0\n"; // ORG
        for(quint32 I=0;I<0xFFFFFFFF;I+=4)
        {
            quint32 inst;

            readHelper(I, (quint8*)(&inst), sizeof(inst));

            if(inst != 0)
            {
                str << QString::number(I,16) << " " << QString::number(inst,16);
                qDebug() << QString::number(I,16) << ":" << QString::number(inst,16);
            }
        }
    }
}


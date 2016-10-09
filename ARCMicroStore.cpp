/*
 * ARCMicroStore.cpp
 *
 *  Created on: Oct 24, 2012
 *      Author: dsonck
 */

#include "ARCMicroStore.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdebug.h>

ARCMicroStore::ARCMicroStore ( )
    : m_vmpiMicroprogram ( 2048 )

{
  m_access = new QMutex ( );

  m_pmpiMicroprogramCounter = m_vmpiMicroprogram.begin();
}

ARCMicroStore::~ARCMicroStore ( )
{
  delete m_access;
}

void ARCMicroStore::setMPC ( quint16 offset )
{
  offset %= 2048;
  qDebug() << "MPC:"<<offset;
  m_pmpiMicroprogramCounter = m_vmpiMicroprogram.begin ( ) + offset;
  emit newMPC ( offset );
}

void ARCMicroStore::incMPC ( )
{
  m_pmpiMicroprogramCounter++;
  if ( m_pmpiMicroprogramCounter == m_vmpiMicroprogram.end ( ) )
  {
    m_pmpiMicroprogramCounter = m_vmpiMicroprogram.begin ( );

  }
  long int it = m_pmpiMicroprogramCounter - m_vmpiMicroprogram.begin ( );
  emit newMPC ( it );
}

bool ARCMicroStore::isAtStart ( )
{
  return m_pmpiMicroprogramCounter == m_vmpiMicroprogram.begin ( );
}

void ARCMicroStore::loadASMFile ( QString filename )
{

  int addr = 0;
  QFile file ( filename );
  file.open ( QIODevice::ReadOnly );
  {
    QTextStream stream ( &file );
    while ( !stream.atEnd ( ) )
    {
      QString line = stream.readLine ( );
      while ( line.count ( ) > 0 )
      {
        if ( line.startsWith ( "/" ) )
        {
          qDebug ( ) << line.mid ( 1 );
          line = "";
        }
        else if ( line.startsWith ( "ORG" ) )
        {
          line = line.mid ( 3 );

          addr = line.toInt();
          qDebug() << "New addr:"<< addr;
          line = "";
        }
        else if(line.startsWith("0") || line.startsWith("1"))
        {
          m_vmpiMicroprogram[addr] = line.left(41).toULongLong(0,2);
          qDebug() << "New data:"<< QString::number(m_vmpiMicroprogram[addr],2);
          line = line.mid(41);
          addr++;
        }
        else if(line.startsWith(" "))
        {
          line = line.mid(1);
        }
        else
        {
          line = "";
        }
      }
    }
  }
}

void ARCMicroStore::saveASMFile(QString filename )
{
    QFile file ( filename );
    file.open ( QIODevice::WriteOnly );
    {
        QTextStream stream ( &file );
        stream << "/ MicroStore export using ARC Simulator v0.1 (C) 2013 DanielSonckSolutions\n";
        stream << "ORG 0\n";
        for(int I = 0;I < 2048;I ++)
        {
            stream << QString("%1").arg(m_vmpiMicroprogram[I],41,2,QLatin1Char('0')) << " / " << I << "\n";
        }
    }
    file.close();
}

void ARCMicroStore::writeAt(quint16 at, MPI mpi)
{
    if(m_vmpiMicroprogram[at % 2048] != mpi)
    {
        qDebug() << this << "WRITE: " << at << ":" << QString("%1").arg(mpi,41,2);
        m_vmpiMicroprogram[at % 2048] = mpi;
        emit newMPI(at%2048,mpi);
    }
}

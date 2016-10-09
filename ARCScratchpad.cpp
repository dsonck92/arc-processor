/*
 * ARCScratchpad.cpp
 *
 *  Created on: Oct 24, 2012
 *      Author: dsonck
 */

#include "ARCScratchpad.h"

#include <qdebug.h>

ARCScratchpad::ARCScratchpad ( )
:m_vu32Scratchpad ( MAX_REGISTER )
{
  m_access = new QReadWriteLock(QReadWriteLock::Recursive);

  {
    QWriteLocker a(m_access);
  setA ( quint8 ( 0 ) );
  setB ( quint8 ( 0 ) );
  setC ( quint8 ( 0 ) );

  reset();
  }
}

void ARCScratchpad::reset()
{
  int ii=0;
  {
    QWriteLocker a(m_access);
    for(QVector<quint32>::iterator i = m_vu32Scratchpad.begin();i < m_vu32Scratchpad.end();i++,ii++)
    {
      (*i) = 0;
    }
  }
  while(ii -- > 0)
    emit registerChanged(ii);

}

ARCScratchpad::~ARCScratchpad ( )
{
  // TODO Auto-generated destructor stub
}

void ARCScratchpad::setA ( quint8 a )
{
  QWriteLocker acc(m_access);
  m_pu32A = a % MAX_REGISTER;
  qDebug() << this << "SET A MUX:" << m_pu32A;
}

void ARCScratchpad::setB ( quint8 b )
{
  QWriteLocker acc(m_access);
  m_pu32B = b % MAX_REGISTER;
  qDebug() << this << "SET B MUX:" << m_pu32B;
}

void ARCScratchpad::setC ( quint8 c )
{
  QWriteLocker acc(m_access);
  m_pu32C = c % MAX_REGISTER;
  qDebug() << this << "SET C MUX:" << m_pu32C;
}

quint32 ARCScratchpad::readA ( ) const
{
  QReadLocker acc(m_access);
  return m_vu32Scratchpad.at(m_pu32A);
}

quint32 ARCScratchpad::readB ( ) const
{
  QReadLocker acc(m_access);
  return m_vu32Scratchpad.at(m_pu32B);
}

void ARCScratchpad::writeC ( quint32 c )
{
  long int reg = 0;
  {
    QWriteLocker acc(m_access);
    reg = m_pu32C;
    if(reg == 0)
      c = 0;
    m_vu32Scratchpad.replace(m_pu32C,c);
  }
  emit registerChanged(reg);
}


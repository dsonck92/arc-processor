/*
 * ARCMicroStore.h
 *
 *  Created on: Oct 24, 2012
 *      Author: dsonck
 */

#ifndef ARCMICROSTORE_H_
#define ARCMICROSTORE_H_

#include <qobject.h>
#include <qvector.h>
#include <qmutex.h>

typedef quint64 MPI;

class ARCMicroStore : public QObject
{
  Q_OBJECT

    QMutex *m_access;

    QVector < MPI > m_vmpiMicroprogram;

    QVector < MPI >::iterator m_pmpiMicroprogramCounter;

  public:
    ARCMicroStore ( );
    virtual ~ARCMicroStore ( );

    void lock ( )
    {
      m_access->lock ( );
    }
    bool tryLock()
    {
      return m_access->tryLock();
    }
    void unlock ( )
    {
      m_access->unlock ( );
    }

    void setMPC ( quint16 );
    void incMPC ( );

    quint16 getMPC ( );

    MPI instruction ( )
    {
      return *m_pmpiMicroprogramCounter;
    }

    MPI readAt(quint16 at){return m_vmpiMicroprogram[at % 2048];}
    void writeAt(quint16 at,MPI mpi);

  public:
    bool isAtStart ( );

    void loadASMFile(QString file);
    void saveASMFile(QString filename);

  signals:
    void newMPC ( quint16 );
    void newMPI ( quint16 , MPI );
};

#endif /* ARCMICROSTORE_H_ */

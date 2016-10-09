/*
 * ARCScratchpad.h
 *
 *  Created on: Oct 24, 2012
 *      Author: dsonck
 */

#ifndef ARCSCRATCHPAD_H_
#define ARCSCRATCHPAD_H_

#include <qobject.h>
#include <qreadwritelock.h>
#include <qvector.h>

class ARCScratchpad : public QObject
{
    Q_OBJECT

    QReadWriteLock * m_access;

    QVector < quint32 > m_vu32Scratchpad;
    quint8 m_pu32A , m_pu32B, m_pu32C;

public:
    enum
    {
        R0 = 0,
        R1,
        R2,
        R3,
        R4,
        R5,
        R6,
        R7,
        R8,
        R9,
        R10,
        R11,
        R12,
        R13,
        R14,
        R15,
        R16,
        R17,
        R18,
        R19,
        R20,
        R21,
        R22,
        R23,
        R24,
        R25,
        R26,
        R27,
        R28,
        R29,
        R30,
        R31,
        PC,
        TEMP0,
        TEMP1,
        TEMP2,
        TEMP3,
        IR,
        PSR,
        MAX_REGISTER
    };

public:
    ARCScratchpad ( );
    virtual ~ARCScratchpad ( );

    void lockRead ( )
    {
        m_access->lockForRead ( );
    }
    void lockWrite ( )
    {
        m_access->lockForWrite ( );
    }
    void unlock ( )
    {
        m_access->unlock ( );
    }

public:
    quint32 readA ( ) const;
    void setA ( quint8 );

    quint32 readB ( ) const;
    void setB ( quint8 );

    void writeC ( quint32 );
    void setC ( quint8 );

    quint32 readIR ( )
    {
        QReadLocker a ( m_access );
        return m_vu32Scratchpad [ IR ];
    }
    quint32 readPSR ( )
    {
        QReadLocker a ( m_access );
        return m_vu32Scratchpad [ PSR ];
    }
    void writePSR ( quint32 data )
    {
        {
            QWriteLocker a ( m_access );
            m_vu32Scratchpad [ PSR ] = data;
        }
        emit registerChanged(PSR);
    }
    void setCC ( bool nf , bool zf , bool vf , bool cf )
    {
        {
            QWriteLocker a ( m_access );
            // Clear the flags
            m_vu32Scratchpad [ PSR ] &= ~0x00F00000;
            // Set the new flags
            m_vu32Scratchpad [ PSR ] |= nf << 23 | zf << 22 | vf << 21 | cf << 20;
        }
        emit registerChanged(PSR);
    }

    quint32 readRegister ( quint8 reg )
    {
        QReadLocker a ( m_access );
        return m_vu32Scratchpad [ reg ];
    }

    void writeRegister ( quint8 reg, quint32 val )
    {
        {
            QWriteLocker a ( m_access );
            m_vu32Scratchpad[ reg ] = val;
        }
        emit registerChanged(reg);
    }

public slots:
    void reset ( );

signals:
    void registerChanged ( quint8 );
};

#endif /* ARCSCRATCHPAD_H_ */

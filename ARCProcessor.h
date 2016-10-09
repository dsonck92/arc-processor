/*
 * ARCProcessor.h
 *
 *  Created on: Oct 19, 2012
 *      Author: dsonck
 */

#ifndef ARCPROCESSOR_H_
#define ARCPROCESSOR_H_

#include <qthread.h>
#include <qvector.h>

#include <qwaitcondition.h>

#include <QMutex>

class ARCMemory;
class ARCScratchpad;
class ARCMicroStore;

class ARCProcessor : public QThread
{
    Q_OBJECT

    QMutex * m_access;

    QMutex * m_mRun;
    bool m_kill;
    bool m_runTillInstruction;
    quint64 m_steps;

    QWaitCondition * m_wcHalt;

    ARCMicroStore * m_amMicroStore;
    ARCMemory * m_amMemory;
    ARCScratchpad * m_asScratchpad;

    enum ALU{
      ANDCC    = 0b0000,
      ORCC     = 0b0001,
      ORNCC    = 0b0010,
      ADDCC    = 0b0011,
      SRL      = 0b0100,
      AND      = 0b0101,
      OR       = 0b0110,
      ORN      = 0b0111,
      ADD      = 0b1000,
      LSHIFT2  = 0b1001,
      LSHIFT10 = 0b1010,
      SIMM13   = 0b1011,
      SEXT13   = 0b1100,
      INC      = 0b1101,
      INCPC    = 0b1110,
      RSHIFT5  = 0b1111,
    };

    enum JMP
    {
      NEXT     = 0b000,
      NEGATIVE = 0b001,
      ZERO     = 0b010,
      OVERFLOW = 0b011,
      CARRY    = 0b100,
      IREG13   = 0b101,
      ALWAYS   = 0b110,
      DECODE   = 0b111,
    };

    quint8 doALU( ALU alu );

    void run();

public:
    ARCProcessor ( ARCMicroStore * micro, ARCScratchpad * sp, ARCMemory * mem );
    virtual ~ARCProcessor ( );

public:
    void doMicrostep();

    ARCMemory *memory() const { return m_amMemory;}
    ARCScratchpad *scratchpad() const { return m_asScratchpad;}
    ARCMicroStore *microstore() const { return m_amMicroStore;}

public slots:
    void forwardMicrostep();
    void forwardStep();
    void start(){start(-1);}
    void start(qint64 steps);

    void boot();
    void kill();

signals:
    void procStarted();
    void procHalted();
    void procPaused();
};

#endif /* ARCPROCESSOR_H_ */

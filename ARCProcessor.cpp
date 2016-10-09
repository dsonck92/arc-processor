/*
 * ARCProcessor.cpp
 *
 *  Created on: Oct 19, 2012
 *      Author: dsonck
 */

#include "ARCProcessor.h"
#include <QMutexLocker>

#include "ARCMemory.h"
#include "ARCScratchpad.h"
#include "ARCMicroStore.h"

#include <qdebug.h>

ARCProcessor::ARCProcessor ( ARCMicroStore * micro , ARCScratchpad * sp ,
    ARCMemory * mem )
{

  qDebug ( ) << this << "Created";
  m_amMemory = mem;
  m_asScratchpad = sp;
  m_amMicroStore = micro;

  m_runTillInstruction = false;

  m_access = new QMutex ( QMutex::Recursive );
  m_mRun = new QMutex ( );

  m_wcHalt = new QWaitCondition ( );

  m_amMicroStore->lock ( );
  m_amMicroStore->setMPC ( 0 );
  m_amMicroStore->unlock ( );
}

ARCProcessor::~ARCProcessor ( )
{
  kill ( );

  wait ( );

  delete m_access;
  delete m_mRun;

  qDebug ( ) << this << "Destroyed";
  // TODO Auto-generated destructor stub
}

void ARCProcessor::doMicrostep ( )
{
  QMutexLocker a ( m_access );

  qDebug ( ) << this << "BEGIN: doMicrostep()";

  // Read the new instruction
  MPI in = m_amMicroStore->instruction ( );

  qDebug ( ) << this << "IN:" << QString::number ( in , 2 );

  quint64 muxA , muxB , muxC;
  // select the correct registers:
  if ( in & 0x000400000000 ) // MUX A
  {
    qDebug ( ) << this << "A from IR";
    muxA = ( ( m_asScratchpad->readIR ( ) & 0x0007C000 ) >> 14 ); // Select from register
  }
  else
  {
    qDebug ( ) << this << "A from MC";
    muxA = ( ( in & 0x01F800000000 ) >> 35 ); // Select from microcode
  }

  m_asScratchpad->setA ( quint8 ( muxA ) );

  if ( in & 0x08000000 ) // MUX B
  {
    qDebug ( ) << this << "B from IR";
    muxB = ( ( m_asScratchpad->readIR ( ) & 0x0000001F ) ); // Select from register
  }
  else
  {
    qDebug ( ) << this << "B from MC";
    muxB = ( ( in & 0x3F0000000 ) >> 28 ); // Select from microcode
  }

  m_asScratchpad->setB ( quint8 ( muxB ) );

  if ( in & 0x00100000 ) // MUX C
  {
    qDebug ( ) << this << "C from IR";

    muxC = ( ( m_asScratchpad->readIR ( ) & 0x3E000000 ) >> 25 ); // Select from register
  }
  else
  {
    qDebug ( ) << this << "C from MC";
    muxC = ( ( in & 0x07E00000 ) >> 21 ); // Select from microcode
  }

  m_asScratchpad->setC ( quint8 ( muxC ) );

  quint32 psr = m_asScratchpad->readPSR ( ); // Select the previous psr register, delayed action

  qDebug ( ) << this << "PSR:" << QString::number ( psr , 2 );

  qDebug ( ) << this << "A:" << m_asScratchpad->readA ( );
  qDebug ( ) << this << "B:" << m_asScratchpad->readB ( );

  // get the ALU instruction
  ALU alu = (ALU) ( ( in & 0x0003C000 ) >> 14 );

  qDebug ( ) << this << "ALU:" << QString::number ( alu , 2 );

  // Run the ALU
  quint32 res = doALU ( alu );

  qDebug ( ) << this << "ALU:" << res;

  // Set the address
  m_amMemory->setAddress ( m_asScratchpad->readA ( ) );
  if ( in & 0x00040000 )
  {
    // Must write to memory
    m_amMemory->writeDWord ( m_asScratchpad->readB ( ) );
  }
  if ( in & 0x00080000 )
  {
    // Must read from memory
    m_asScratchpad->writeC ( m_amMemory->readDWord ( ) );
  }
  else
  {
    // Must read from ALU
    m_asScratchpad->writeC ( res );
  }

  // Go to the new location
  // Get the condition
  JMP cond = (JMP) ( ( in & 0x3800 ) >> 11 );
  quint64 addr = ( ( in & 0x07FF ) );

  qDebug ( ) << this << "JMP:" << QString::number ( cond , 2 );

  switch ( cond )
  {

    case NEGATIVE:
      qDebug ( ) << this << "On negative";
      if ( psr & ( 1 << 23 ) )
        m_amMicroStore->setMPC ( addr );
      else
        m_amMicroStore->incMPC ( );
      break;
    case ZERO:
      qDebug ( ) << this << "On zero";
      if ( psr & ( 1 << 22 ) )
        m_amMicroStore->setMPC ( addr );
      else
        m_amMicroStore->incMPC ( );
      break;
    case OVERFLOW:
      qDebug ( ) << this << "On overflow";
      if ( psr & ( 1 << 21 ) )
        m_amMicroStore->setMPC ( addr );
      else
        m_amMicroStore->incMPC ( );
      break;
    case CARRY:
      qDebug ( ) << this << "On carry";
      if ( psr & ( 1 << 20 ) )
        m_amMicroStore->setMPC ( addr );
      else
        m_amMicroStore->incMPC ( );
      break;
    case IREG13:
      qDebug ( ) << this << "On IREG13";
      if ( m_asScratchpad->readIR ( ) & ( 1 << 13 ) )
        m_amMicroStore->setMPC ( addr );
      else
        m_amMicroStore->incMPC ( );
      break;
    case ALWAYS:
      qDebug ( ) << this << "On always";
      m_amMicroStore->setMPC ( addr );
      break;
    case DECODE:
      qDebug ( ) << this << "On decode";
      addr = ( m_asScratchpad->readIR ( ) & 0xC1F80000 ) >> 17;
      // Compress the address
      addr |= ( ( addr & 0x6000 ) >> 5 );
      // Set bit 10
      addr |= ( 0x0400 );
      // Mask out unwanted information
      switch ( addr & 0x0300 )
      {
        case 0x0000:
          addr &= 0x07E0;
          break;
        case 0x0100:
          addr &= 0x0700;
          break;
        default:
          addr &= 0x07FC;
      }
      m_amMicroStore->setMPC ( addr );
      break;
    default:
      qDebug ( ) << this << "On next";
      m_amMicroStore->incMPC ( );
  }
}

quint8 ARCProcessor::doALU ( ALU alu )
{
  quint64 result;
  bool setCC = false;
  bool nf , cf , zf , vf;
  switch ( alu & 0xF )
  {
    case ANDCC:
      qDebug ( ) << this << "ANDCC";
      // Set the CC flags afterwards
      setCC = true;
    case AND:
      qDebug ( ) << this << "AND";
      // Perform an bitwise AND
      result = ( m_asScratchpad->readA ( ) ) & ( m_asScratchpad->readB ( ) );
      // Set the relevant flags
      nf = ( ( result & 0x80000000 ) > 0 );
      zf = ( result == 0 );
      cf = vf = false;
      break;
    case ORCC:
      qDebug ( ) << this << "ORCC";
      setCC = true;
    case OR:
      qDebug ( ) << this << "OR";
      // Perform an bitwise OR
      result = ( m_asScratchpad->readA ( ) ) | ( m_asScratchpad->readB ( ) );
      // Set the relevant flags
      nf = ( ( result & 0x80000000 ) > 0 );
      zf = ( result == 0 );
      cf = vf = false;
      break;
    case ORNCC:
      qDebug ( ) << this << "ORNCC";
      setCC = true;
    case ORN:
      qDebug ( ) << this << "ORN";
      // Perform an bitwise OR, inverting B
      result = ( m_asScratchpad->readA ( ) ) | ~ ( m_asScratchpad->readB ( ) );
      // Set the relevant flags
      nf = ( ( result & 0x80000000 ) > 0 );
      zf = ( result == 0 );
      cf = vf = false;
      break;
    case ADDCC:
      qDebug ( ) << this << "ADDCC";
      setCC = true;
    case ADD:
      qDebug ( ) << this << "ADD";
      // Perform an bitwise add, works for two complement
      result = ( m_asScratchpad->readA ( ) ) + ( m_asScratchpad->readB ( ) );
      // Set the relevant flags
      nf = ( ( result & 0x80000000 ) > 0 );
      zf = ( result == 0 );
      // TODO: improve the carry/overflow detection
      {
        // Carry detection, bit 31 has carry
        quint32 tmp = ( ( ( m_asScratchpad->readA ( ) ) & 0xEFFFFFFF )
            + ( ( m_asScratchpad->readB ( ) ) & 0xEFFFFFFF ) ) & 0x80000000;
        cf = ( tmp > 0 );
      }
      {
        // Overflow detection, bit 32 has carry
        quint64 tmp = ( ( m_asScratchpad->readA ( ) )
            + ( m_asScratchpad->readB ( ) ) ) & 0x100000000;
        // Overflow when carry1 XOR carry2
        vf = ( cf || ( tmp > 0 ) ) && ! ( cf && ( tmp > 0 ) );
      }
      break;
    case SRL:
      qDebug ( ) << this << "SRL";
      // Logical shift right
      result = ( m_asScratchpad->readA ( ) ) >> ( m_asScratchpad->readB ( ) );
      break;
    case LSHIFT2:
      qDebug ( ) << this << "LSHIFT2";
      // Left shift by 2
      result = ( m_asScratchpad->readA ( ) ) << 2;
      break;
    case LSHIFT10:
      qDebug ( ) << this << "LSHIFT10";
      // Left shift by 10
      result = ( m_asScratchpad->readA ( ) ) << 10;
      break;
    case SIMM13:
      qDebug ( ) << this << "SIMM13";
      // Extract lower 13 bits
      result = ( m_asScratchpad->readA ( ) ) & 0x00001FFF;
      break;
    case SEXT13:
      qDebug ( ) << this << "SEXT13";
      // Extract lower 13 bits with sign extension
      result = ( m_asScratchpad->readA ( ) ) & 0x00001FFF;
      if ( result & 0x00001000 )
        result |= 0xFFFFE000;
      break;
    case INC:
      qDebug ( ) << this << "INC";
      // Increment by 1
      result = ( m_asScratchpad->readA ( ) ) + 1;
      break;
    case INCPC:
      qDebug ( ) << this << "INCPC";
      // Increment by 4
      result = ( m_asScratchpad->readA ( ) ) + 4;
      break;
    case RSHIFT5:
      qDebug ( ) << this << "RSHIFT5";
      // Arithmetic shift by 5 with sign extension
      result = ( ( m_asScratchpad->readA ( ) ) >> 5 );
      if ( result & 0x0400000 )
        result |= 0xF8000000;
      break;
  }

  // Set CC
  if ( setCC )
  {
    m_asScratchpad->setCC ( nf , zf , vf , cf );
  }

  // Return the result
  return result;
}

void ARCProcessor::forwardMicrostep ( )
{
  QMutexLocker a ( m_access );
  start ( 1 );
}

void ARCProcessor::forwardStep ( )
{
  QMutexLocker a ( m_access );
  m_runTillInstruction = true;
  start ( -1 );
}

void ARCProcessor::start ( qint64 steps )
{
  if ( steps != 0 )
  {
    // Make changing the steps safe
    if ( m_mRun->tryLock ( ) )
    {
      // It doesn't really matter if it's successful
      m_steps = steps;
      m_mRun->unlock ( );
    }
  }
  else
  {
    // Ensure that the step count is set to 0
    m_mRun->lock ( );
    m_steps = steps;
    m_mRun->unlock ( );
  }
  // We give the go signal to the thread
  m_wcHalt->wakeAll ( );
}

void ARCProcessor::run ( )
{
  // We are running
  m_mRun->lock ( );
  while ( !m_kill )
  {

    // We wait for the start signal
    m_wcHalt->wait ( m_mRun );
    // We are running again
    emit procStarted ( );

    m_amMicroStore->lock ( );

    while ( m_steps != 0 && m_asScratchpad->readIR ( ) != 0xFFFFFFFF )
    {
      if ( m_steps > 0 )
        m_steps--;
      doMicrostep ( );

      if ( m_runTillInstruction && !m_amMicroStore->isAtStart ( ) )
      {
        m_runTillInstruction = false;
        m_steps = 0;
      }
      m_mRun->unlock ( );
      // Allow the stop function to alter the step count
      m_mRun->lock ( );
    }

    m_amMicroStore->unlock ( );

    if ( m_asScratchpad->readIR ( ) == 0xFFFFFFFF )
    {
      emit procHalted ( );
    }
    else
    {
      emit procPaused ( );
    }
  }
  m_mRun->unlock ( );
}

void ARCProcessor::boot ( )
{
  if ( !QThread::isRunning ( ) )
  {
    m_kill = false;

    QThread::start ( );
  }
}

void ARCProcessor::kill ( )
{
  m_mRun->lock ( );
  // Make sure that the processor is going to stop
  m_steps = 0;
  // And let the thread die
  m_kill = true;
  m_mRun->unlock ( );

  // We give the go signal to the thread just in case it's waiting
  m_wcHalt->wakeAll ( );
}

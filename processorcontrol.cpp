#include "processorcontrol.h"

#include "ARCScratchpad.h"
#include "ARCMicroStore.h"

ProcessorControl::ProcessorControl ( ARCProcessor * proc, QWidget *parent )
    : QWidget ( parent )
{
  m_iAuto = 0;
  m_apProcessor = proc;
  ui.setupUi ( this );

  connect(m_apProcessor,SIGNAL(procStarted()),SLOT(processorRunning()));
  connect(m_apProcessor,SIGNAL(procHalted()),SLOT(processorHalted()));
  connect(m_apProcessor,SIGNAL(procPaused()),SLOT(processorPaused()));

  connect(m_apProcessor,SIGNAL(started()),SLOT(threadStarted()));
  connect(m_apProcessor,SIGNAL(finished()),SLOT(threadFinished()));

  connect(m_apProcessor->scratchpad(),SIGNAL(registerChanged(quint8)),SLOT(registerChanged(quint8)),Qt::QueuedConnection);

  connect(ui.m_tbForwardPC,SIGNAL(clicked()),m_apProcessor,SLOT(forwardStep()));
  connect(ui.m_tbForwardMC,SIGNAL(clicked()),m_apProcessor,SLOT(forwardMicrostep()));

  connect(m_apProcessor->microstore(),SIGNAL(newMPC(quint16)),SLOT(newMPC(quint16)));

  m_timer = startTimer ( 500 );
}

ProcessorControl::~ProcessorControl ( )
{

}

void ProcessorControl::setAutoForwardMC ( bool mode )
{
  if ( m_iAuto != mode )
  {
    m_iAuto = mode;
    setBoxes ( );
  }
}

void ProcessorControl::setAutoForwardPC ( bool mode )
{
  if ( m_iAuto != 2 * mode )
  {
    m_iAuto = 2 * mode;
    setBoxes ( );
  }
}

void ProcessorControl::timerEvent ( QTimerEvent * )
{
  if ( m_iAuto == 1 )
  {
    ui.m_tbForwardMC->animateClick ( );
  }
  if ( m_iAuto == 2 )
  {
    ui.m_tbForwardPC->animateClick ( );
  }
}

void ProcessorControl::setBoxes ( )
{
  ui.m_chbAutoForwardMC->setChecked ( m_iAuto == 1 );
  ui.m_chbAutoForwardPC->setChecked ( m_iAuto == 2 );
}

void ProcessorControl::processorRunning()
{
  ui.m_tbRun->setChecked(true);
  ui.m_tbPause->setChecked(false);
}

void ProcessorControl::processorHalted()
{
  ui.m_tbRun->setChecked(false);
  ui.m_tbPause->setChecked(true);
}

void ProcessorControl::processorPaused()
{
  ui.m_tbRun->setChecked(false);
  ui.m_tbPause->setChecked(true);
}

void ProcessorControl::threadStarted()
{
  ui.m_tbStop->setChecked(true);
}

void ProcessorControl::threadFinished()
{
  ui.m_tbStop->setChecked(false);
}

void ProcessorControl::startProcessor()
{
  m_apProcessor->start(ui.m_sbMaxSteps->value());
}

void ProcessorControl::stopProcessor()
{
  m_apProcessor->start(0);
}

void ProcessorControl::registerChanged(quint8 reg)
{
  if(reg == ARCScratchpad::PC)
  {
    ui.m_lePC->setText(QString("%1").arg(m_apProcessor->scratchpad()->readRegister(ARCScratchpad::PC),8,16,QLatin1Char('0')));
  }
}

void ProcessorControl::setAutoSpeed(int speed)
{
  killTimer(m_timer);
  m_timer = startTimer(speed);
}

void ProcessorControl::newMPC(quint16 pc)
{
  ui.m_leMC->setText(QString("%1").arg(pc));
}

void ProcessorControl::goToBegin()
{
    m_apProcessor->scratchpad()->writeRegister(ARCScratchpad::PC,0x00000800);
}

void ProcessorControl::goToEnd()
{
    m_apProcessor->scratchpad()->writeRegister(ARCScratchpad::PC,0xFFFFFFFF);
}

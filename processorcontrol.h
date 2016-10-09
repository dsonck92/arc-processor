#ifndef PROCESSORCONTROL_H
#define PROCESSORCONTROL_H

#include <QWidget>
#include "ui_processorcontrol.h"

#include "ARCProcessor.h"

class ProcessorControl : public QWidget
{
  Q_OBJECT

    int m_iAuto;
    int m_timer;

    ARCProcessor * m_apProcessor;

    void setBoxes ( );
  public:
    ProcessorControl ( ARCProcessor *proc , QWidget *parent = 0 );
    ~ProcessorControl ( );

    void timerEvent ( QTimerEvent * );

  public slots:
    void setAutoForwardMC ( bool );
    void setAutoForwardPC ( bool );

    void goToBegin();
    void goToEnd();

  private:
    Ui::ProcessorControlClass ui;

  private slots:
    void processorRunning ( );
    void processorHalted ( );
    void processorPaused ( );

    void threadStarted ( );
    void threadFinished ( );

    void startProcessor();
    void stopProcessor();

    void registerChanged(quint8);

    void setAutoSpeed(int);

    void newMPC(quint16);
};

#endif // PROCESSORCONTROL_H

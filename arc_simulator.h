#ifndef ARC_SIMULATOR_H
#define ARC_SIMULATOR_H

#include <QMainWindow>
#include "ui_arc_simulator.h"

#include "ARCScratchpad.h"
#include "ARCMemory.h"
#include "ARCProcessor.h"
#include "ARCMicroStore.h"

#include <qlabel.h>

class arc_simulator : public QMainWindow
{
    Q_OBJECT
    ARCScratchpad * m_asScratchpad;
    ARCMemory     * m_amMemory;
    ARCProcessor  * m_apProcessor;
    ARCMicroStore * m_amMicroStore;

    QLabel *m_lThreadStatus, *m_lProcessorStatus;

public:
    arc_simulator(QWidget *parent = 0);
    ~arc_simulator();

private:
    Ui::arc_simulatorClass ui;

    private slots:
    void processorRunning();
    void processorHalted();
    void processorPaused();

    void threadStarted();
    void threadFinished();

    void loadProgram();
    void loadMicrostore();
};

#endif // ARC_SIMULATOR_H


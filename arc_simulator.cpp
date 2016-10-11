#include "arc_simulator.h"
#include "programeditor.h"
#include "processorcontrol.h"
#include "registereditor.h"
#include "memoryeditor.h"
#include "microfileeditor.h"
#include "PagedMemory.h"

#include <QDockWidget>

#include <qfiledialog.h>

arc_simulator::arc_simulator(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    statusBar()->addPermanentWidget(m_lThreadStatus = new QLabel());
    statusBar()->addPermanentWidget(m_lProcessorStatus = new QLabel());

    m_asScratchpad = new ARCScratchpad;
    m_amMemory = new PagedMemory;
    m_amMicroStore = new ARCMicroStore;
    m_apProcessor  = new ARCProcessor(m_amMicroStore,m_asScratchpad,m_amMemory);

    QDockWidget *dock;
    addDockWidget(Qt::LeftDockWidgetArea,dock = new QDockWidget());
    dock->setWidget(new RegisterEditor(m_asScratchpad));

    addDockWidget(Qt::TopDockWidgetArea,dock = new QDockWidget());
    dock->setWidget(new ProcessorControl(m_apProcessor));

    ((QTabWidget*)centralWidget())->addTab(new ProgramEditor,"Program Editor");
    ((QTabWidget*)centralWidget())->addTab(new MemoryEditor(m_apProcessor),"Memory Editor");
    ((QTabWidget*)centralWidget())->addTab(new MicrofileEditor(m_amMicroStore),"Microfile Editor");

    connect(m_apProcessor,SIGNAL(procStarted()),SLOT(processorRunning()));
    connect(m_apProcessor,SIGNAL(procHalted()),SLOT(processorHalted()));
    connect(m_apProcessor,SIGNAL(procPaused()),SLOT(processorPaused()));

    connect(m_apProcessor,SIGNAL(started()),SLOT(threadStarted()));
    connect(m_apProcessor,SIGNAL(finished()),SLOT(threadFinished()));

}

arc_simulator::~arc_simulator()
{
    m_apProcessor->kill();

    delete m_apProcessor;
    delete m_amMemory;
    delete m_asScratchpad;
}

void arc_simulator::processorRunning()
{
    m_lProcessorStatus->setText("ARC: Running");
}

void arc_simulator::processorHalted()
{
    m_lProcessorStatus->setText("ARC: Halted");
}

void arc_simulator::processorPaused()
{
    m_lProcessorStatus->setText("ARC: Paused");
}


void arc_simulator::threadStarted()
{
    m_lThreadStatus->setText("SIM: Started");
}

void arc_simulator::threadFinished()
{
    m_lThreadStatus->setText("SIM: Stopped");
}

void arc_simulator::loadProgram()
{
    QString file = QFileDialog::getOpenFileName(0,"Select program .bin file...","~","Binary (*.bin)");

    m_amMemory->loadBinFile(file);

    m_apProcessor->boot();
}

void arc_simulator::loadMicrostore()
{
    QString file = QFileDialog::getOpenFileName(0,"Select microstore .asm file...","~","Assemly (*.asm)");

    m_amMicroStore->loadASMFile(file);
}

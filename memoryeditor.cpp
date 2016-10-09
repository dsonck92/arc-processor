#include "memoryeditor.h"
#include "ARCProcessor.h"

#include "ARCMemory.h"
#include <QFileDialog>


MemoryEditor::MemoryEditor(ARCProcessor *proc, QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    m_mem = proc->memory();

    m_iProgMemIndex = 0;

    connect(ui.m_tbProgU1,SIGNAL(clicked()),SLOT(incProg()));
    connect(ui.m_tbProgD1,SIGNAL(clicked()),SLOT(decProg()));

    QGridLayout *layout = new QGridLayout(ui.m_fProgram);
    layout->setMargin(1);
    layout->setSpacing(1);

    QStringList regs;

    for(int i=0;i<32;i++)
        regs << QString("R%1").arg(i,2,10,QLatin1Char('0'));

    layout->addWidget(new QLabel("Address"),0,0,Qt::AlignCenter);

    layout->addWidget(new QLabel("Break"),0,1,Qt::AlignCenter);
    layout->addWidget(new QLabel("Data"),0,2,Qt::AlignCenter);

    layout->addWidget(new QLabel("Code"),0,3,Qt::AlignCenter);
    layout->setColumnStretch(4,1);

    for(int i=0;i<8;i++)
    {
        layout->addWidget(m_pgProg[i].m_lAddress = new QLabel("00000000"),i+1,0);

        layout->addWidget(m_pgProg[i].m_cbBreakpoint = new QCheckBox(),i+1,1);
        layout->addWidget(m_pgProg[i].m_leData = new QLineEdit(),i+1,2);
        m_pgProg[i].m_leData->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        layout->addWidget(m_pgProg[i].m_lInstruction = new QLabel(),i+1,3);
        m_pgProg[i].m_lInstruction->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
        m_pgProg[i].m_lInstruction->setText("NOP");

    }

    layout = new QGridLayout(ui.m_fMemory);
    layout->setMargin(1);
    layout->setSpacing(1);

    for(int i=0;i<8;i++)
    {
        layout->addWidget(m_mgMem[i].m_lAddress = new QLabel("00000000"),i+1,0);

        layout->addWidget(m_mgMem[i].m_leData[0] = new QLineEdit(),i+1,1);
        layout->addWidget(m_mgMem[i].m_leData[1] = new QLineEdit(),i+1,2);
        layout->addWidget(m_mgMem[i].m_leData[2] = new QLineEdit(),i+1,3);
        layout->addWidget(m_mgMem[i].m_leData[3] = new QLineEdit(),i+1,4);

    }

}

MemoryEditor::~MemoryEditor()
{

}

void MemoryEditor::loadBin()
{
    QString filename = QFileDialog::getOpenFileName(0,"Open memory file...","","Binary (*.bin)");

    if(!filename.isEmpty())
    {
        m_mem->loadBinFile(filename);
    }
}

void MemoryEditor::saveBin()
{
    QString filename = QFileDialog::getSaveFileName(0,"Save memory file...","","Binary (*.bin)");

    if(!filename.isEmpty())
    {
        m_mem->saveBinFile(filename);
    }
}

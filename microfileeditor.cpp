#include "microfileeditor.h"
#include "ARCMicroStore.h"

#include <qgridlayout.h>
#include "qfiledialog.h"

#include "QFrame"
#include <QDebug>

MicrofileEditor::MicrofileEditor ( ARCMicroStore *store , QWidget *parent )
    : QWidget ( parent )
{
    ui.setupUi ( this );

    m_isEditing = m_follow = m_updating = false;

    m_from = 0;

    m_amMicrostore = store;
    QGridLayout *layout = new QGridLayout(ui.m_fEditor);
    layout->setMargin(1);
    layout->setSpacing(1);

    QStringList regs;

    for(int i=0;i<32;i++)
        regs << QString("R%1").arg(i,2,10,QLatin1Char('0'));

    regs << QString("PC");

    for(int i=0;i<4;i++)
        regs << QString("TEMP%1").arg(i);

    regs << QString("IR");

    layout->setRowStretch(0,1);

    layout->addWidget(new QLabel("Address"),1,0,Qt::AlignCenter);

    layout->addWidget(new QLabel("REGISTER"),1,1,Qt::AlignCenter);
    layout->addWidget(new QLabel("A"),1,2,Qt::AlignCenter);

    layout->addWidget(new QLabel("REGISTER"),1,3,Qt::AlignCenter);
    layout->addWidget(new QLabel("B"),1,4,Qt::AlignCenter);

    layout->addWidget(new QLabel("REGISTER"),1,5,Qt::AlignCenter);
    layout->addWidget(new QLabel("C"),1,6,Qt::AlignCenter);

    layout->addWidget(new QLabel("R"),1,7,Qt::AlignCenter);
    layout->addWidget(new QLabel("W"),1,8,Qt::AlignCenter);

    layout->addWidget(new QLabel("ALU"),1,9,Qt::AlignCenter);

    layout->addWidget(new QLabel("JMP"),1,10,Qt::AlignCenter);
    layout->addWidget(new QLabel("TO"),1,11,Qt::AlignCenter);

    QStringList alu;
    alu << "ANDCC"
        << "ORCC"
        << "ORNCC"
        << "ADDCC"
        << "SRL"
        << "AND"
        << "OR"
        << "ORN"
        << "ADD"
        << "LSHIFT2"
        << "LSHIFT10"
        << "SIMM13"
        << "SEXT13"
        << "INC"
        << "INCPC"
        << "RSHIFT5";

    QStringList jmp;
    jmp << "NEXT"
        << "IF N"
        << "IF Z"
        << "IF C"
        << "IF V"
        << "IF IR[13]"
        << "ALWAYS"
        << "DECODE";

    for ( int i = 0 ; i < 16 ; i++ )
    {

        layout->addWidget(m_instGui[i].m_lAddress = new QLabel("00000000"),i+2+(i>6)+(i>7),0);

        layout->addWidget(m_instGui[i].m_cbRegA = new QComboBox(),i+2+(i>6)+(i>7),1);
        m_instGui[i].m_cbRegA->insertItems(0,regs);
        layout->addWidget(m_instGui[i].m_chbMuxA = new QCheckBox(),i+2+(i>6)+(i>7),2);

        layout->addWidget(m_instGui[i].m_cbRegB = new QComboBox(),i+2+(i>6)+(i>7),3);
        m_instGui[i].m_cbRegB->insertItems(0,regs);
        layout->addWidget(m_instGui[i].m_chbMuxB = new QCheckBox(),i+2+(i>6)+(i>7),4);

        layout->addWidget(m_instGui[i].m_cbRegC = new QComboBox(),i+2+(i>6)+(i>7),5);
        m_instGui[i].m_cbRegC->insertItems(0,regs);
        layout->addWidget(m_instGui[i].m_chbMuxC = new QCheckBox(),i+2+(i>6)+(i>7),6);

        layout->addWidget(m_instGui[i].m_chbRD = new QCheckBox(),i+2+(i>6)+(i>7),7);
        layout->addWidget(m_instGui[i].m_chbWR = new QCheckBox(),i+2+(i>6)+(i>7),8);

        layout->addWidget(m_instGui[i].m_cbALU = new QComboBox(),i+2+(i>6)+(i>7),9);
        m_instGui[i].m_cbALU->insertItems(0,alu);

        layout->addWidget(m_instGui[i].m_cbCondition = new QComboBox(),i+2+(i>6)+(i>7),10);
        m_instGui[i].m_cbCondition->insertItems(0,jmp);
        layout->addWidget(m_instGui[i].m_leJump = new QLineEdit(),i+2+(i>6)+(i>7),11);

        connect(m_instGui[i].m_cbRegA,SIGNAL(currentIndexChanged(int)),SLOT(updateControls()));
        connect(m_instGui[i].m_chbMuxA,SIGNAL(clicked()),SLOT(updateControls()));

        connect(m_instGui[i].m_cbRegB,SIGNAL(currentIndexChanged(int)),SLOT(updateControls()));
        connect(m_instGui[i].m_chbMuxB,SIGNAL(clicked()),SLOT(updateControls()));

        connect(m_instGui[i].m_cbRegC,SIGNAL(currentIndexChanged(int)),SLOT(updateControls()));
        connect(m_instGui[i].m_chbMuxC,SIGNAL(clicked()),SLOT(updateControls()));

        connect(m_instGui[i].m_chbRD,SIGNAL(clicked()),SLOT(updateControls()));
        connect(m_instGui[i].m_chbWR,SIGNAL(clicked()),SLOT(updateControls()));

        connect(m_instGui[i].m_cbALU,SIGNAL(currentIndexChanged(int)),SLOT(updateControls()));

        connect(m_instGui[i].m_cbCondition,SIGNAL(currentIndexChanged(int)),SLOT(updateControls()));
        connect(m_instGui[i].m_leJump,SIGNAL(editingFinished()),SLOT(updateControls()));


        updateInstruction(i);
    }

    QFrame *fr = new QFrame();
    fr->setFrameShape(QFrame::HLine);
    layout->addWidget(fr,9,0,1,12);

    fr = new QFrame();
    fr->setFrameShape(QFrame::HLine);
    layout->addWidget(fr,11,0,1,12);

    connect(m_amMicrostore,SIGNAL(newMPC(quint16)),SLOT(newMPC(quint16)));
}

MicrofileEditor::~MicrofileEditor ( )
{

}

void MicrofileEditor::editToggle()
{
    if(!m_isEditing)
    {
        if(m_amMicrostore->tryLock())
        {
            m_isEditing = true;
        }
    }
    else
    {
        m_amMicrostore->unlock();
        m_isEditing = false;
    }
    ui.m_tbEdit->setChecked(m_isEditing);
}

void MicrofileEditor::updateInstruction(int i)
{
    m_updating = true;
    MPI in = m_amMicrostore->readAt((i+m_from)%2048);

    if(!m_follow || i != 7 ) {
        m_instGui[i].m_lAddress->setText(QString("%1").arg((i+m_from)%2048));
    }

    m_instGui[i].m_leJump->setText(QString("%1").arg(in & 0x07FF));

    m_instGui[i].m_cbRegA->setCurrentIndex(int(( in & 0x01F800000000 ) >> 35));
    m_instGui[i].m_chbMuxA->setChecked((in & 0x000400000000) > 0);

    m_instGui[i].m_cbRegB->setCurrentIndex(int(( in & 0x3F0000000 ) >> 28));
    m_instGui[i].m_chbMuxB->setChecked((in & 0x08000000) > 0);

    m_instGui[i].m_cbRegC->setCurrentIndex(int(( in & 0x07E00000 ) >> 21));
    m_instGui[i].m_chbMuxC->setChecked((in & 0x00100000) > 0);

    m_instGui[i].m_chbRD->setChecked((in & 0x00080000) > 0);
    m_instGui[i].m_chbWR->setChecked((in & 0x00040000) > 0);

    m_instGui[i].m_cbALU->setCurrentIndex(int(( in & 0x0003C000 ) >> 14));

    m_instGui[i].m_cbCondition->setCurrentIndex(int( ( in & 0x3800 ) >> 11 ));

    m_updating = false;
}

void MicrofileEditor::updateControl(int i)
{
    if(m_updating)
        return;
    MPI in = 0;

    //                        RR RRRR
    //                        AA AAAA
    //
    in |= (MPI)m_instGui[i].m_cbRegA->currentIndex();
    //                       RRR RRR0
    //                       AAA AAA
    //
    in <<= 1;
    //                       RRR RRRM
    //                       AAA AAAA
    //
    in |= (MPI)m_instGui[i].m_chbMuxA->isChecked();

    //               R RRRR RM00 0000
    //               A AAAA AA
    //
    in <<= 6;
    //               R RRRR RMRR RRRR
    //               A AAAA AABB BBBB
    //
    in |= (MPI)m_instGui[i].m_cbRegB->currentIndex();
    //              RR RRRR MRRR RRR0
    //              AA AAAA ABBB BBB
    //
    in <<= 1;
    //              RR RRRR MRRR RRRM
    //              AA AAAA ABBB BBBB
    //
    in |= (MPI)m_instGui[i].m_chbMuxB->isChecked();

    //        RRR RRRM RRRR RRM0 0000
    //        AAA AAAA BBBB BBB0 0000
    //
    in <<= 6;
    //        RRR RRRM RRRR RRMR RRRR
    //        AAA AAAA BBBB BBBC CCCC
    //
    in |= (MPI)m_instGui[i].m_cbRegC->currentIndex();
    //       RRRR RRMR RRRR RMRR RRR0
    //       AAAA AAAB BBBB BBCC CCC
    //
    in <<= 1;
    //       RRRR RRMR RRRR RMRR RRRM
    //       AAAA AAAB BBBB BBCC CCCC
    //
    in |= (MPI)m_instGui[i].m_chbMuxC->isChecked();

    //     R RRRR RMRR RRRR MRRR RRM0
    //     A AAAA AABB BBBB BCCC CCC
    //
    in <<= 1;
    //     R RRRR RMRR RRRR MRRR RRMR
    //     A AAAA AABB BBBB BCCC CCCD
    //
    in |= (MPI)m_instGui[i].m_chbRD->isChecked();
    //    RR RRRR MRRR RRRM RRRR RMR0
    //    AA AAAA ABBB BBBB CCCC CCD
    //
    in <<= 1;
    //    RR RRRR MRRR RRRM RRRR RMRW
    //    AA AAAA ABBB BBBB CCCC CCDR
    //
    in |= (MPI)m_instGui[i].m_chbWR->isChecked();

    // RR RRRR MRRR RRRM RRRR RMRW 0000
    // AA AAAA ABBB BBBB CCCC CCDR 0000
    //
    in <<= 4;
    // RR RRRR MRRR RRRM RRRR RMRW AAAA
    // AA AAAA ABBB BBBB CCCC CCDR LLLL
    //
    in |= (MPI)m_instGui[i].m_cbALU->currentIndex();

    in <<= 3;
    in |= m_instGui[i].m_cbCondition->currentIndex();

    in <<= 11;
    in |= m_instGui[i].m_leJump->text().toInt();

    qDebug() << "Editor: " << QString("%1").arg((i+m_from)%2048,4) << ":" << QString("%1").arg(in,41,2);
    m_amMicrostore->writeAt((i+m_from)%2048,in);
}

void MicrofileEditor::moveFrom(quint16 offset)
{
    m_from = (m_from+offset) % 2048;

    if(m_amMicrostore->tryLock())
    {
        for(int i=0;i<16;i++)
        {
            updateInstruction(i);
        }
        ui.m_sbOffset->setValue(m_from);
        m_amMicrostore->unlock();
    }
}

void MicrofileEditor::setFrom(int offset)
{
    m_from = offset % 2048;

    if(m_amMicrostore->tryLock())
    {
        for(int i=0;i<16;i++)
        {
            updateInstruction(i);
        }
        ui.m_sbOffset->setValue(m_from);
        m_amMicrostore->unlock();
    }
}

void MicrofileEditor::load()
{
    QString filename = QFileDialog::getOpenFileName(0,"Open microstore file...","","Microfile (*.asm)");

    if(!filename.isEmpty())
    {
        m_amMicrostore->loadASMFile(filename);
    }

    moveFrom(0);
}

void MicrofileEditor::save()
{
    QString filename = QFileDialog::getSaveFileName(0,"Save microstore file...","","Microfile (*.asm)");

    if(!filename.isEmpty())
    {
        m_amMicrostore->saveASMFile(filename);
    }
}

void MicrofileEditor::followToggle()
{
    m_follow = !m_follow;

    ui.m_tbFollow->setChecked(m_follow);
    if(m_follow)
    {
        m_instGui[7].m_lAddress->setPixmap(QPixmap(":/images/current.png"));
    }
    else
    {
        m_instGui[7].m_lAddress->setPixmap(QPixmap());
    }
}

void MicrofileEditor::newMPC(quint16 mpc)
{
    if(m_follow) {
        if(mpc < 7) {
            mpc += 2048;
        }
        m_from = (mpc - 7) % 2048;

        for(int i=0;i<16;i++)
        {
            updateInstruction(i);
        }
        ui.m_sbOffset->setValue(m_from);
    }
}

void MicrofileEditor::updateControls()
{
    for(int i=0;i<16;i++)
    {
        updateControl(i);
    }
}

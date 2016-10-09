#include "registereditor.h"

#include "qgridlayout.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qmath.h>

#include "ARCScratchpad.h"

RegisterEditor::RegisterEditor(ARCScratchpad *sp,QWidget *parent)
    : QWidget(parent)
{
  m_asScratchpad = sp;

  ui.setupUi(this);

  QGridLayout * layout = new QGridLayout(ui.m_fRegisters);

  for(int y = 0;y < 10;y ++)
  {
    for(int x = 0;x < 4;x ++)
    {
      if(y*4+x < 38)
      {
      QLabel * lbl = m_lRegisters[y*4+x] = new QLabel(QString("R%1").arg(y*4+x));
      layout->addWidget(lbl,y,x*2);
      QLineEdit * le = m_leRegisters[y*4+x] = new QLineEdit();
      layout->addWidget(le,y,x*2+1);
      }
    }
  }

  connect(ui.m_pbReset,SIGNAL(clicked()),m_asScratchpad,SLOT(reset()));
  connect(m_asScratchpad,SIGNAL(registerChanged(quint8)),this,SLOT(registerChanged(quint8)));

  ui.m_bgMode->setId(ui.m_rbBin,BIN);
  ui.m_bgMode->setId(ui.m_rbOct,OCT);
  ui.m_bgMode->setId(ui.m_rbDec,DEC);
  ui.m_bgMode->setId(ui.m_rbHex,HEX);

  ui.m_rbBin->setChecked(true);
}

RegisterEditor::~RegisterEditor()
{

}

void RegisterEditor::registerChanged(quint8 reg)
{
  if(reg < 38)
  m_leRegisters[reg]->setText(QString("%1").arg(m_asScratchpad->readRegister(reg),32/(qLn(ui.m_bgMode->checkedId())/qLn(2)),ui.m_bgMode->checkedId(),ui.m_bgMode->checkedId()==10?QLatin1Char(' '):QLatin1Char('0')));
}

void RegisterEditor::resetRegisters()
{
    m_asScratchpad->reset();
}

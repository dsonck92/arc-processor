#ifndef MEMORYEDITOR_H
#define MEMORYEDITOR_H

#include <QtGui/QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include "ui_memoryeditor.h"
#include <qabstractitemmodel.h>

class ARCProcessor;
class ARCMemory;

struct PRGGUI
{
    QLabel *m_lAddress;
    QCheckBox *m_cbBreakpoint;
    QLineEdit *m_leData;
    QLabel *m_lInstruction;
};

struct MEMGUI
{
    QLabel *m_lAddress;
    QLineEdit *m_leData[4];
};

class MemoryEditor : public QWidget
{
  Q_OBJECT

  enum {
    BIN = 2,
    OCT = 8,
    DEC = 10,
    HEX = 16,
  };

  int m_iProgMemIndex;

  PRGGUI m_pgProg[8];
  MEMGUI m_mgMem[8];

  ARCMemory *m_mem;

  public:
    MemoryEditor ( ARCProcessor *proc , QWidget *parent = 0 );
    ~MemoryEditor ( );

  public slots:
    void incProg ( )
    {
      m_iProgMemIndex += 1 ;
    }
    void incProg2 ( )
    {
      m_iProgMemIndex += 2 ;
    }
    void incProg4 ( )
    {
      m_iProgMemIndex += 4 ;
    }
    void decProg ( )
    {
      m_iProgMemIndex -= 1 ;
    }
    void decProg2 ( )
    {
      m_iProgMemIndex -= 2 ;
    }
    void decProg4 ( )
    {
      m_iProgMemIndex -= 4 ;
    }

private slots:
    void loadBin();
    void saveBin();

  private:
    Ui::MemoryEditorClass ui;
};

#endif // MEMORYEDITOR_H

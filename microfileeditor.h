#ifndef MICROFILEEDITOR_H
#define MICROFILEEDITOR_H

#include <QWidget>
#include "ui_microfileeditor.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>

class ARCMicroStore;

struct MPIGUI
{
    QLabel *m_lAddress;
    QComboBox *m_cbRegA , *m_cbRegB , *m_cbRegC , *m_cbALU , *m_cbCondition;
    QLineEdit *m_leJump;
    QCheckBox *m_chbMuxA , *m_chbMuxB , *m_chbMuxC , *m_chbRD , *m_chbWR;
};

class MicrofileEditor : public QWidget
{
  Q_OBJECT
    ARCMicroStore *m_amMicrostore;

    MPIGUI m_instGui [ 16 ];

    quint16 m_from;

    bool m_isEditing,m_follow,m_updating;

    void updateInstruction ( int i );
    void updateControl( int i );

  public:
    MicrofileEditor ( ARCMicroStore *store , QWidget *parent = 0 );
    ~MicrofileEditor ( );

  private:
    Ui::MicrofileEditorClass ui;

  public slots:
  void moveFrom(quint16 offset);
  void setFrom(int offset);
  void incProg()
  {
    moveFrom(1);
  }
  void incProg2 ( )
    {
    moveFrom ( 8 );
    }
    void incProg4 ( )
    {
      moveFrom ( 16 );
    }
    void decProg ( )
    {
      moveFrom ( -1 );
    }
    void decProg2 ( )
    {
      moveFrom ( -8 );
    }
    void decProg4 ( )
    {
      moveFrom ( -16 );
    }

  private slots:
    void editToggle ( );
    void updateControls();
    void followToggle();
    void newMPC(quint16);
    void load();
    void save();
};

#endif // MICROFILEEDITOR_H

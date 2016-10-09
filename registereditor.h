#ifndef REGISTEREDITOR_H
#define REGISTEREDITOR_H

#include <QtGui/QWidget>

#include "ui_registereditor.h"

class QLabel;
class ARCScratchpad;

class RegisterEditor : public QWidget
{
    Q_OBJECT

    enum {
      BIN = 2,
      OCT = 8,
      DEC = 10,
      HEX = 16,
    };

    QLabel *m_lRegisters[38];
    QLineEdit *m_leRegisters[38];

    ARCScratchpad * m_asScratchpad;

public:
    RegisterEditor(ARCScratchpad * scratchpad,QWidget *parent = 0);
    ~RegisterEditor();

private:
    Ui::RegisterEditorClass ui;

    private slots:
    void registerChanged(quint8);
    void resetRegisters();
};

#endif // REGISTEREDITOR_H

#ifndef PROGRAMEDITOR_H
#define PROGRAMEDITOR_H

#include <QWidget>
#include "ui_programeditor.h"

class ProgramEditor : public QWidget
{
    Q_OBJECT

public:
    ProgramEditor(QWidget *parent = 0);
    ~ProgramEditor();

private:
    Ui::ProgramEditorClass ui;
};

#endif // PROGRAMEDITOR_H

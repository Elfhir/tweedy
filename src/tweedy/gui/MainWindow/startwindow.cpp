#include "startwindow.h"
#include "ui_startwindow.h"

#include <iostream>

StartWindow::StartWindow(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::StartWindow)
{
    _ui->setupUi(this);

    setWindowTitle(tr("TWEEDY - logiciel de stop motion"));

}


QToolButton* StartWindow::getNewProjectButton()
{
    return _ui->newProjectButton;
}

QToolButton* StartWindow::getOpenProjectButton()
{
    return _ui->openProjectButton;
}

StartWindow::~StartWindow()
{
    delete _ui;
}

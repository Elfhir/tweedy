#ifndef TIMESHEET_H
#define TIMESHEET_H

#include <QtGui/QDockWidget>
#include <QtGui/QStandardItemModel>
#include <QtCore/QTimer>
#include <string>
#include "ui_TimeSheet.h"

namespace Ui {
    class TimeSheet;
}

class TimeSheet : public QDockWidget
{
    Q_OBJECT

public:
    explicit TimeSheet(QDockWidget *parent = 0);
    ~TimeSheet();
       
private Q_SLOTS:
    void on_playButton_clicked();
    void on_pauseButton_clicked();
    void on_zeroButton_clicked();
    void on_nextButton_clicked();
    void on_previousButton_clicked();

private:
    Ui::TimeSheet *ui;
    int currentTime;
    QTimer * timer;
};

#endif // TIMESHEET_H


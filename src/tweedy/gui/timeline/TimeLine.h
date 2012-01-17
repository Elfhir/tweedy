/* 
 * File:   TimeLine.h
 * Author: julien
 *
 * Created on 19 décembre 2011, 17:51
 */

#ifndef _TIMELINE_H
#define	_TIMELINE_H

#include <stdlib.h>
#include <list>
#include<cstdio>

#include <QtCore/QTimer>

#include <QtGui/QDockWidget>
#include <QtGui/QApplication>
#include <QtGui/QIcon>

#include "../../core/Clip.hpp"

#include "ui_TimeLine.h"

typedef std::list<Clip> listC;
typedef std::list<Clip>::iterator It;

namespace Ui {
    class TimeLine;
}

class TimeLine : public QDockWidget {
    Q_OBJECT
public:
    TimeLine(QDockWidget* parent=0);
    ~TimeLine();
    
    unsigned int time(){ return _time; }
    Ui::TimeLine* ui(){ return _ui; }
    listC clips(){ return _clips; }
    
    void drawMiniatures();
    
            
private Q_SLOTS:
    void increaseTime();
    void writeTime(unsigned int newValue);
    void getCurrentTime(int row,int column);
    void on_playButton_clicked();
    void on_pauseButton_clicked();
    void on_zeroButton_clicked();
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_addTimeButton_clicked();
    void on_plusButton_clicked();
    void on_minusButton_clicked();
       
Q_SIGNALS:
    void valueChanged(unsigned int newValue);
    void displayChanged(unsigned int newValue, listC clips);
    
private:
    QTableWidget* _table;
    unsigned int _time;
    QTimer* _timer;
    Ui::TimeLine* _ui;
    listC _clips;
    QIcon _defautIcon;
    
};




#endif	/* _TIMELINE_H */


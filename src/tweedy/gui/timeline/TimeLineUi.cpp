#include "TimeLineUi.h"
#include <tweedy/core/Projet.hpp>
#include <tweedy/core/action/ActClipSetTimeRange.hpp>
#include <tweedy/core/action/ActAddBlankBeforeClip.hpp>
#include <tweedy/core/action/ActAddBlankAfterClip.hpp>


//_________________________________ constructor ________________________________


TimeLineUi::TimeLineUi(QDockWidget* parent): 
    QDockWidget(parent),
    _time(0),
    _timer(new QTimer(this)),
    _ui(new Ui::TimeLineUi),
    _defautIcon( QIcon("img/none.jpg") )
 {
    _timeline = &(Projet::getInstance()->getTimeline());
    _ui->setupUi(this);
    _ui->table->setIconSize(QSize(75, 75));
   
    
    createActions();
    linkButtonsWithActions();
    
    
    updateTable();
                   
    connect(this, SIGNAL( timeChanged(int) ), this, SLOT(writeTime(int)) );
    connect( _timer, SIGNAL(timeout()), this, SLOT(increaseTime()) );
    connect( this->_ui->table , SIGNAL( cellClicked(int,int) ), this, SLOT( getCurrentTime(int,int)));
        
    Q_EMIT timeChanged(_time);
        
}



void TimeLineUi::createActions(){

    _playAction = new QAction("Play",this);
    _playAction->setShortcut(QKeySequence("Space"));
    _playAction->setStatusTip("Lancer le montage");
    connect(_playAction, SIGNAL(triggered()), this, SLOT(handle_playAction_triggered()));

    _pauseAction = new QAction("Pause",this);
    //_pauseAction->setShortcut(QKeySequence("Space"));
    _pauseAction->setStatusTip("Mettre en pause");
    connect(_pauseAction, SIGNAL(triggered()), this, SLOT(handle_pauseAction_triggered()));

    _nextAction = new QAction("Suivant", this);
    _nextAction->setShortcut(QKeySequence("Alt+Right"));
    _nextAction->setStatusTip("Clip suivant");
    connect(_nextAction, SIGNAL(triggered()), this, SLOT(handle_nextAction_triggered()));

    _prevAction = new QAction("Precedent", this);
    _prevAction->setShortcut(QKeySequence("Alt+Left"));
    _prevAction->setStatusTip("Clip precedent");
    connect(_prevAction, SIGNAL(triggered()), this, SLOT(handle_prevAction_triggered()));
    
    _zeroAction = new QAction("Zero",this);
    _zeroAction->setShortcut(QKeySequence("0"));
    _zeroAction->setStatusTip("Remise a zero");
    connect(_zeroAction, SIGNAL(triggered()), this, SLOT(handle_zeroAction_triggered()));

    _plusAction = new QAction("+",this);
    //_plusAction->setShortcut(QKeySequence("Space"));
    _plusAction->setStatusTip("Augmenter la duree du clip");
    connect(_plusAction, SIGNAL(triggered()), this, SLOT(handle_plusAction_triggered()));

    _minusAction = new QAction("-", this);
    //_minusAction->setShortcut(QKeySequence("Alt+Right"));
    _minusAction->setStatusTip("Diminuer la duree du clip");
    connect(_minusAction, SIGNAL(triggered()), this, SLOT(handle_minusAction_triggered()));

    _blankBeforeAction = new QAction("Avant", this);
    //_blankBeforeAction->setShortcut(QKeySequence("Alt+Left"));
    _blankBeforeAction->setStatusTip("Frame vide avant le clip");
    connect(_blankBeforeAction, SIGNAL(triggered()), this, SLOT(handle_blankBeforeAction_triggered()));
    
    _blankAfterAction = new QAction("Apres", this);
    //_blankAfterAction->setShortcut(QKeySequence("Alt+Left"));
    _blankAfterAction->setStatusTip("Frame vide après le clip");
    connect(_blankAfterAction, SIGNAL(triggered()), this, SLOT(handle_blankAfterAction_triggered()));
    
    _deleteAction = new QAction("Supprimer", this);
    _deleteAction->setShortcut(QKeySequence("Del"));
    _deleteAction->setStatusTip("Supprimer le clip");
    connect(_deleteAction, SIGNAL(triggered()), this, SLOT(handle_deleteAction_triggered()));
}


void TimeLineUi::linkButtonsWithActions()
{
    _ui->playButton->setDefaultAction(_playAction);
    _ui->pauseButton->setDefaultAction(_pauseAction);
    _ui->nextButton->setDefaultAction(_nextAction);
    _ui->prevButton->setDefaultAction(_prevAction);
    
    _ui->zeroButton->setDefaultAction(_zeroAction);
    _ui->plusButton->setDefaultAction(_plusAction);
    _ui->minusButton->setDefaultAction(_minusAction);
    _ui->blankBeforeButton->setDefaultAction(_blankBeforeAction);
    _ui->blankAfterButton->setDefaultAction(_blankAfterAction);
    
    _ui->deleteButton->setDefaultAction(_deleteAction);
    
    
   /* _ui->playButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    _ui->pauseButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    _ui->nextButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    _ui->prevButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);*/
    
}




//_____________________ Update the table with list of clips ____________________


void TimeLineUi::updateTable()
{
    
    //clear timeline
    _ui->table->clearContents();
    while ( _ui->table->columnCount() > 1 )
        _ui->table->removeColumn(0);
     
    //Timeline timeline = Projet::getInstance()->getTimeline();
    //fill the whole table with blanks
    for (unsigned int i=0; i<_timeline->maxTime(); ++i)
    {
        std::cout << _timeline->maxTime() << std::endl;
        _ui->table->insertColumn(i);
        
        std::string header = ( boost::format("%d") %i ).str();
        _ui->table->setHorizontalHeaderItem(i, new QTableWidgetItem(QString::fromStdString(header) ) );
        
        QTableWidgetItem *newItem = new QTableWidgetItem(_defautIcon,"");
        _ui->table->setItem(0, i, newItem);
    }
    
    //fill with icons
    Timeline::OMapClip orderedClips = _timeline->getOrderedClips();
    BOOST_FOREACH( const Timeline::OMapClip::value_type& s, orderedClips )
    {
        for (unsigned int j=s.first; j<(*s.second)->timeOut(); ++j)
        {
            QIcon icon( QString::fromStdString((*s.second)->imgPath().string()) );
            QTableWidgetItem *newItem = new QTableWidgetItem(icon,"");
            _ui->table->setItem(0, j, newItem);
        }
    }
    
    //icon for real time
    QIcon icon( QString::fromStdString("img/realTime.jpg") );
    QTableWidgetItem *newItem = new QTableWidgetItem(icon,"");
    _ui->table->setItem(0, _timeline->maxTime(), newItem);
    
    _ui->table->setCurrentCell(0,_time);
    
    std::cout<< "update timeline" << std::endl;
   
    
}






//____________________ Let the view display the good picture ___________________


void TimeLineUi::emitDisplayChanged()
{
    std::string  filename = "img/none.jpg";
    
    if (_time == _timeline->maxTime())
        //afficher le temps reel
        filename = "img/realTime.jpg";
    else
        bool isClip = _timeline->findCurrentClip(filename,_time);
    
    Q_EMIT displayChanged(filename);
}



//_______________ Write time in label and select the good cell _________________

void TimeLineUi::writeTime(int newValue)
{
    _ui->table->setCurrentCell(0,newValue);
    
    if (newValue == _timeline->maxTime())
        newValue = -1;
    
    _ui->time->setNum(newValue);
    
    emitDisplayChanged();
}


//___________ Increase current time or stop timer if last frame ________________

void TimeLineUi::increaseTime()
{
    if (_time>-1 && _time < _timeline->maxTime())
        ++ _time;
    if (_time == _timeline->maxTime())
        _time = 0; 
        
    Q_EMIT timeChanged(_time);
}



//________________ update current time when a cell is selected _________________

void TimeLineUi::getCurrentTime(int row,int column)
{
    _time = column;
       
    Q_EMIT timeChanged(_time);
    
}



//_______________________________ buttons slots ________________________________


void TimeLineUi::handle_playAction_triggered()
{
   _timer->start(1000);
}


void TimeLineUi::handle_pauseAction_triggered()
{
   _timer->stop(); 
        
}

void TimeLineUi::handle_zeroAction_triggered()
{
   _time = 0;
   Q_EMIT timeChanged(_time);
   
}

void TimeLineUi::handle_nextAction_triggered()
{
    Timeline::OMapClip orderedClips = _timeline->getOrderedClips();
    bool lastClip = true;
    BOOST_FOREACH( const Timeline::OMapClip::value_type& s, orderedClips )
    {
        if (s.first > _time)
        {
            lastClip = false;
            _time = s.first;
            break;
        }
        if ((*s.second)->timeOut() > _time && (*s.second)->timeOut() < _timeline->maxTime())
        {
            lastClip = false;
            _time = (*s.second)->timeOut();
            break;
        }
    }
    
    if (!lastClip)
        Q_EMIT timeChanged(_time);
        
}   

void TimeLineUi::handle_prevAction_triggered()
{
    Timeline::OMapClip orderedClips = _timeline->getOrderedClips();
    int currentTime = _time;
    bool firstClip = true;
    BOOST_FOREACH( const Timeline::OMapClip::value_type& s, orderedClips )
    {
        if (s.first < currentTime)
        {    
            firstClip = false;
            _time = s.first;
            if ( (*s.second)->timeOut() < currentTime)
                _time = (*s.second)->timeOut();
        }
        else
            break;
    }
    
    if (!firstClip)
        Q_EMIT timeChanged(_time);
    
}


void TimeLineUi::handle_plusAction_triggered()
{ 
    
   int currentCell = _ui->table->currentColumn();
   if ( currentCell > -1 && currentCell < _timeline->maxTime() )
   {
       
        // création d'une action ActClipSetTimeRange
       IAction * action = new ActClipSetTimeRange(_time,"Add time action ",_ui->spinDuration->value());
       
       updateTable();
       
       delete action;
   }
   
   
}


void TimeLineUi::handle_minusAction_triggered()
{ 
   int currentCell = _ui->table->currentColumn();
   if ( currentCell > -1 && currentCell < _timeline->maxTime() )
   {
       
        // création d'une action ActClipSetTimeRange
       IAction * action = new ActClipSetTimeRange(_time,"Remove time action ",-_ui->spinDuration->value());
       
       updateTable();
       
       delete action;
       emitDisplayChanged();
       
   }
}


void TimeLineUi::handle_blankBeforeAction_triggered()
{
   int currentCell = _ui->table->currentColumn();
   if ( currentCell > -1 && currentCell < _timeline->maxTime() )
   {
       
      
        // création d'une action ActClipSetTimeRange
       IAction * action = new ActAddBlankBeforeClip(_time,"Add blank before ",_ui->spinDuration->value());
       
       updateTable();
       
       delete action;
     
       emitDisplayChanged();
              
   }


}


void TimeLineUi::handle_blankAfterAction_triggered()
{
   int currentCell = _ui->table->currentColumn();
   if ( currentCell > -1 && currentCell < _timeline->maxTime() )
   {
      
        // création d'une action ActClipSetTimeRange
       IAction * action = new ActAddBlankAfterClip(_time,"Add blank after ",_ui->spinDuration->value());
       
       updateTable();
       
       delete action;
        
   }
    
}

void TimeLineUi::handle_deleteAction_triggered()
{
  
   int currentCell = _ui->table->currentColumn();
   if ( currentCell > -1 && currentCell < _timeline->maxTime() )
   {
       std::string filename;
       bool isClip = _timeline->findCurrentClip(filename,_time);
       
       if (isClip)
           _timeline->deleteClip(filename);
       else
           _timeline->deleteBlank(_time);
              
       updateTable();
                     
       emitDisplayChanged();
    }

}

//_________________________________ destructor _________________________________


TimeLineUi::~TimeLineUi() 
{
    //delete _timeline;
    delete _ui;
    delete _timer;
    delete _deleteAction;
    delete _blankAfterAction;
    delete _blankBeforeAction;
    delete _minusAction;
    delete _plusAction;
    delete _zeroAction;
    delete _prevAction;
    delete _nextAction;
    delete _pauseAction;
    delete _playAction;
}

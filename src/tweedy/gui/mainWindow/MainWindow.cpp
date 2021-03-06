#include "MainWindow.hpp"

#include <tweedy/core/Projet.hpp>
#include <tweedy/core/action/ActCapturePicture.hpp>
#include <tweedy/core/command/GroupeUndoRedoCmd.hpp>
#include <tweedy/core/command/clip/CmdClipSetTimeRange.hpp>

#include <QtGui/QWidget>
#include <QtGui/QDockWidget>
#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QAction>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QLineEdit>
#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtGui/QImage>
#include <QtCore/QSettings>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

MainWindow::MainWindow()
{
        setWindowTitle( tr( "TWEEDY - Stop Motion software" ) );

	createActions();
	createStartWindow();
	createMenuBar();
	createToolBar();
	createWidgets();
	createStatusBar();

        _isPlaying = false;
        _timer = new QTimer( this );
        _fps = 8;
        _time = 0;
        

        connect( this, SIGNAL( timeChanged( int ) ), this->_viewerImg, SLOT( displayChanged( int ) ) );
        connect( &( this->_timelineGraphic->getTimelineDataWrapper() ), SIGNAL( timeChanged( int ) ), this->_viewerImg, SLOT( displayChanged( int ) ) );
        connect( &(this->_timelineGraphic->getTimelineDataWrapper()), SIGNAL( displayChanged( int, int ) ), _chutier, SLOT( changedPixmap( int, int ) ) );
        connect( _timer, SIGNAL( timeout() ), this, SLOT( increaseTime() ) );
        connect( &(this->_timelineGraphic->getTimelineDataWrapper()), SIGNAL( timeChanged( int )), this, SLOT( changeTimeViewer( int ) ) );
        connect( this, SIGNAL(timeChanged(int)), &(this->_timelineGraphic->getTimelineDataWrapper()), SLOT(time(int)) );

	this->adjustSize();

        Q_EMIT timeChanged( _time );
        _timelineGraphic->getTimelineDataWrapper()._currentTime = _time;
        
        QSettings settings("IMAC","Tweedy");     
}

MainWindow::~MainWindow()
{

}

void MainWindow::createActions()
{
	_newProjectAction = new QAction( QIcon( "img/icones/new1.png" ), "New project", this );
	_newProjectAction->setShortcut( QKeySequence( "Ctrl+N" ) );
	_newProjectAction->setStatusTip( "Create a new project" );
	connect( _newProjectAction, SIGNAL( triggered() ), this, SLOT( on_newProjectAction_triggered() ) );

	_openProjectAction = new QAction( QIcon( "img/icones/open.png" ), "Open project", this );
	_openProjectAction->setShortcut( QKeySequence( "Ctrl+O" ) );
	_openProjectAction->setStatusTip( "Open a project" );
	connect( _openProjectAction, SIGNAL( triggered() ), this, SLOT( on_openProjectAction_triggered() ) );

	_saveProjectAction = new QAction( QIcon( "img/icones/save1.png" ), "Save", this );
	_saveProjectAction->setShortcut( QKeySequence( "Ctrl+S" ) );
	_saveProjectAction->setStatusTip( "Save your project" );
        connect( _saveProjectAction, SIGNAL( triggered() ), this, SLOT( on_saveProjectAction_triggered() ) );

	_saveAsProjectAction = new QAction( "Save As", this );
	//_saveProjectAction->setShortcut( QKeySequence( "Ctrl+S" ) );
	_saveProjectAction->setStatusTip( "Save your project" );
	connect( _saveAsProjectAction, SIGNAL( triggered() ), this, SLOT( on_saveAsProjectAction_triggered() ) );

	_quitAction = new QAction( QIcon( "img/icones/quit.png" ), "&Quitter", this );
	_quitAction->setShortcut( QKeySequence( "Ctrl+Q" ) );
	_quitAction->setStatusTip( "Quit Tweedy" );
	connect( _quitAction, SIGNAL( triggered() ), qApp, SLOT( quit() ) );

	_undoAction = new QAction( QIcon( "img/icones/undo.png" ), "Undo", this );
	_undoAction->setShortcut( QKeySequence( "Ctrl+Z" ) );
	connect( _undoAction, SIGNAL( triggered() ), this, SLOT( on_undoButton_clicked() ) );

	_redoAction = new QAction( QIcon( "img/icones/redo.png" ), "Redo", this );
	_redoAction->setShortcut( QKeySequence( "Shift+Ctrl+Z" ) );
	connect( _redoAction, SIGNAL( triggered() ), this, SLOT( on_redoButton_clicked() ) );

	_aboutAction = new QAction( "About Tweedy", this );
	connect( _aboutAction, SIGNAL( triggered() ), this, SLOT( on_aboutAction_triggered() ) );

	_aboutQtAction = new QAction( "About Qt", this );
	connect( _aboutQtAction, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );

	_configAction = new QAction( "Camera configuration", this );
	connect( _configAction, SIGNAL( triggered() ), this, SLOT( on_configAction_triggered() ) );

	_captureAction = new QAction( QIcon( "img/icones/capture.png" ), "Capture", this );
	_captureAction->setShortcut( QKeySequence( "Retour" ) );
	connect( _captureAction, SIGNAL( triggered() ), this, SLOT( on_captureAction_triggered() ) );
        
        _exportAction = new QAction( "Export", this );
        _exportAction->setStatusTip( "Export your timeline" );
        connect( _exportAction, SIGNAL( triggered() ), this, SLOT( on_exportAction_triggered() ) );

        _playPauseAction = new QAction( QIcon( "img/icones/play.png" ), "", this );
        _playPauseAction->setShortcut( QKeySequence( "Space" ) );
        _playPauseAction->setStatusTip( "Play" );
        connect( _playPauseAction, SIGNAL( triggered() ), this, SLOT( handle_playPauseAction_triggered() ) );

        _nextAction = new QAction( QIcon( "img/icones/next.png" ), "Next", this );
        _nextAction->setShortcut( QKeySequence( "Alt+Right" ) );
        _nextAction->setStatusTip( "Next clip" );
        connect( _nextAction, SIGNAL( triggered() ), this, SLOT( handle_nextAction_triggered() ) );

        _prevAction = new QAction( QIcon( "img/icones/previous.png" ), "Previous", this );
        _prevAction->setShortcut( QKeySequence( "Alt+Left" ) );
        _prevAction->setStatusTip( "Previous clip" );
        connect( _prevAction, SIGNAL( triggered() ), this, SLOT( handle_prevAction_triggered() ) );

        _zeroAction = new QAction( QIcon( "img/icones/retour0.png" ), "Reset", this );
        _zeroAction->setShortcut( QKeySequence( "0" ) );
        _zeroAction->setStatusTip( "Reset" );
        connect( _zeroAction, SIGNAL( triggered() ), this, SLOT( handle_zeroAction_triggered() ) );

        _initialPlaceWidgets = new QAction( "Restore display", this );
        _initialPlaceWidgets->setStatusTip( "Restore widget's location" );
        connect( _initialPlaceWidgets, SIGNAL( triggered() ), this, SLOT( on_initialPlaceWidgets_triggered() ) );

}

void MainWindow::createStartWindow()
{
	//creation of the start window
        _startWindowDialog = new StartWindow();
	_startWindowDialog->setWindowFlags( Qt::WindowStaysOnTopHint );
	_startWindowDialog->setModal( false );
	_startWindowDialog->showNormal();

        //put the window on the screen center
	const QRect screen = QApplication::desktop()->screenGeometry();
	_startWindowDialog->move( screen.center() - _startWindowDialog->rect().center() );

	_startWindowDialog->getNewProjectButton()->setDefaultAction( _newProjectAction );
	_startWindowDialog->getNewProjectButton()->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	_startWindowDialog->getNewProjectButton()->setIconSize( QSize( 40, 40 ) );
	_startWindowDialog->getNewProjectButton()->setMinimumWidth( 150 );

	_startWindowDialog->getOpenProjectButton()->setDefaultAction( _openProjectAction );
	_startWindowDialog->getOpenProjectButton()->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	_startWindowDialog->getOpenProjectButton()->setIconSize( QSize( 40, 40 ) );
	_startWindowDialog->getOpenProjectButton()->setMinimumWidth( 150 );

	_startWindowDialog->setModal( true );
	_startWindowDialog->activateWindow();
        
        QSettings settings("IMAC","Tweedy");
        settings.beginGroup("Saves");
        QStringList keys = settings.allKeys();
        QStringListIterator it(keys);
        while(it.hasNext())
        {
            QListWidgetItem* item = new QListWidgetItem(it.next());
            _startWindowDialog->getListRecentsProjects()->addItem(item);
            
        }
        settings.endGroup();
        
        this->setEnabled( false );
        
        connect( _startWindowDialog->getListRecentsProjects(), SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT (on_recentProject_selected(QListWidgetItem*)));
	connect( _startWindowDialog, SIGNAL( rejected() ), this, SLOT( on_close_window() ) );
        
}

void MainWindow::on_recentProject_selected(QListWidgetItem* item)
{
    
    QString projectName = item->text();
    
    QSettings settings("IMAC","Tweedy");
    settings.beginGroup("Saves");
    QString saveFile = settings.value(projectName).toString();
    
    on_loadProjectAction_triggered(saveFile.toStdString().c_str());
    
    settings.endGroup();
    
    _startWindowDialog->hide();
    this->setEnabled( true );
    
}

void MainWindow::on_close_window()
{
	this->setEnabled( true );
}

void MainWindow::createMenuBar()
{
	Projet& projectInstance = project();

	_fileMenu = menuBar()->addMenu( tr( "&File" ) );
	_fileMenu->addAction( _newProjectAction );
	_fileMenu->addAction( _openProjectAction );
	menuBar()->addSeparator();
	_fileMenu->addAction( _saveProjectAction );
	_fileMenu->addAction( _saveAsProjectAction );
        _fileMenu->addAction( _exportAction );
	_fileMenu->addAction( _quitAction );

	_editMenu = menuBar()->addMenu( tr( "&Edit" ) );
	_editMenu->addAction( _undoAction );
	_editMenu->addAction( _redoAction );

	_viewMenu = menuBar()->addMenu( tr( "View" ) );
        _viewMenu->addAction( _initialPlaceWidgets );

	_paramsMenu = menuBar()->addMenu( tr( "Configuration" ) );
	_paramsMenu->addAction( _configAction );

	_helpMenu = menuBar()->addMenu( tr( "Help" ) );
	_helpMenu->addAction( _aboutAction );
	_helpMenu->addAction( _aboutQtAction );
}

void MainWindow::createToolBar()
{
	_fileToolBar = addToolBar( "File" );
	_fileToolBar->addAction( _newProjectAction );
	_fileToolBar->addAction( _openProjectAction );
	_fileToolBar->addAction( _saveProjectAction );
	_fileToolBar->addAction( _quitAction );

	_editToolBar = addToolBar( "Edit" );
	_editToolBar->addAction( _undoAction );
	_editToolBar->addAction( _redoAction );

	_fileToolBar->setIconSize( QSize( 25, 25 ) );
}

void MainWindow::createWidgets()
{
	{
		// Dock Chutier
                _chutierDock = new QDockWidget( "Media List", this );
		{
                        _chutier = new Chutier( _chutierDock );
                        _chutierDock->setWidget( _chutier );
                        addDockWidget( Qt::TopDockWidgetArea, _chutierDock );
                        _viewMenu->addAction( _chutierDock->toggleViewAction() );
                        _viewMenu->addAction( _chutier->_viewerChutierDock->toggleViewAction() );
		}

	}

	{
                // Dock Viewer
                createWidgetViewer();
                saveWidgets();

                // Dock UndoWidget
                QDockWidget * undoDock = new QDockWidget( "Command List", this );
                {
                        _undoView = new UndoView( Projet::getInstance().getCommandManager(), undoDock );
                        _undoWidget = new UndoWidget( _undoView );
                        undoDock->setWidget( _undoWidget );
                        addDockWidget( Qt::TopDockWidgetArea, undoDock );
                        _viewMenu->addAction( undoDock->toggleViewAction() );
                }

                tabifyDockWidget( _chutierDock, undoDock );
                undoDock->setHidden( true );
                undoDock->setFloating( true );

                // Dock Timeline QML
                _graphicTimelineDock = new QDockWidget( "Graphic Timeline", this );
                _timelineGraphic = new TimelineGraphic( this );
                _graphicTimelineDock->setWidget( _timelineGraphic );
                addDockWidget( Qt::BottomDockWidgetArea, _graphicTimelineDock );

                _viewMenu->addAction( _graphicTimelineDock->toggleViewAction() );
	}
}

void MainWindow::createWidgetViewer()
{
        _contentViewerDock = new QDockWidget( "Viewer", this );
        _viewerImg = new ViewerTweedy( _contentViewerDock );
        _contentViewerDock->setWidget( _viewerImg );
        addDockWidget( Qt::TopDockWidgetArea, _contentViewerDock );

        _viewMenu->addAction( _contentViewerDock->toggleViewAction() );

	_viewerImg->getCaptureButton()->setDefaultAction( _captureAction );
	_viewerImg->getCaptureButton()->setIconSize( QSize( 60, 60 ) );

	/// @todo ne pas appeler la timeline depuis le viewer !
	/// Mais emettre des signaux dans chacun et repasser par la MainWindow,
	/// pour changer le temps.
        //connect viewer buttons with timeline actions
        _viewerImg->getNextButton()->setDefaultAction( getNextAction() );
        _viewerImg->getNextButton()->setIconSize( QSize( 26, 26 ) );
        _viewerImg->getPlayPauseButton()->setDefaultAction( getPlayPauseAction() );
        _viewerImg->getPlayPauseButton()->setIconSize( QSize( 26, 26 ) );
        _viewerImg->getPreviousButton()->setDefaultAction( getPreviousAction() );
        _viewerImg->getPreviousButton()->setIconSize( QSize( 26, 26 ) );
        _viewerImg->getRetour0Button()->setDefaultAction( getRetour0Action() );
        _viewerImg->getRetour0Button()->setIconSize( QSize( 26, 26 ) );
	//timer
	//connection slider
	_viewerImg->getTempsSlider()->setTickPosition( QSlider::TicksAbove );
        //signal : valueChanged() : Emitted when the slider's value has changed.
        connect( _viewerImg->getTempsSlider(), SIGNAL( valueChanged( int ) ), this, SLOT( changeTimeViewer( int ) ) );
        _viewerImg->getTempsSlider()->setMaximum( getTimeline().getMaxTime() );

        connect(this,SIGNAL(timeChanged(int)), this, SLOT(changeTimeViewer(int)));
        connect( _viewerImg->getComboFPS(), SIGNAL( currentIndexChanged(QString) ), this, SLOT( changeFps( QString ) ));
}

void MainWindow::on_captureAction_triggered()
{
	Projet& projectInstance = project();

	int isConnected = projectInstance.tryToConnectCamera();
	if( isConnected == 0 )
	{
		QMessageBox::about( this, tr( "Warning" ), tr( "No camera connected to the computer" ) );
	}
	else
	{
		//take HD picture
		Projet& project = Projet::getInstance();
                project.gPhotoInstance().setFolderToSavePictures( project.getProjectFolder() );
		//take the picture's filename
		boost::filesystem::path filenameHD = project.gPhotoInstance().captureToFile();

		//make a LD picture
		QImage img( QString::fromStdString( filenameHD.string() ) );
                QImage petiteImg = img.scaled( QSize( 600, 350 ) );
                //QImage petiteImg = img.scaled( QSize( 150, 100 ) );
                
                std::string filenameLD = filenameHD.string();
                filenameLD.insert( filenameLD.size() - 4, "_LD" );
                filenameLD.erase(filenameLD.begin()+37, filenameLD.begin()+40);

		petiteImg.save( QString::fromStdString( filenameLD ) );

		ActCapturePicture action;
		action( filenameLD );

		//update chutier
		ListWidget & listWidgetCapture = _chutier->getListWidgetCapture();
		QListWidgetItem *item = new QListWidgetItem( QIcon( QString::fromStdString( filenameLD ) ), QString::fromStdString( filenameLD ), &listWidgetCapture );

		listWidgetCapture.addItem( item );

		_chutier->getTabWidget().setCurrentWidget( &listWidgetCapture );
		/*add to chutier core*/
                boost::filesystem::path nameOfFileToImport( filenameLD );
                ChutierMediaExt& chutierPictures = project.getChutierPictures();
                chutierPictures.importMediaToChutier( nameOfFileToImport);
	}
}

void MainWindow::on_newProjectAction_triggered()
{
	_newProjectDialog = new newProjectWindow( this );
	_newProjectDialog->show();
	_startWindowDialog->hide();

	connect( _newProjectDialog, SIGNAL( rejected() ), this, SLOT( on_close_window() ) );
	connect( _newProjectDialog, SIGNAL( accepted() ), this, SLOT( on_acceptedNewProjectWindow() ) );
	connect( _newProjectDialog->getSearchFolderProjectButton(), SIGNAL( clicked() ), this, SLOT( on_searchFolderProjectButton_clicked() ) );
}

void MainWindow::on_searchFolderProjectButton_clicked()
{
	QFileDialog * fileDialog = new QFileDialog();
	QString fileName = fileDialog->getExistingDirectory( this, tr( "Choose folder for project" ), QString( boost::filesystem::initial_path().string().c_str() ) );

	_newProjectDialog->getFolderProjectLineEdit()->setText( fileName );

	//set the name folder for the project files
	Projet& projectInstance = Projet::getInstance();
	boost::filesystem::path pathFolder( fileName.toStdString() );
	projectInstance.setProjectFolder( pathFolder );

	/*Create corresponding folders*/
	pathFolder /= "projet";
        boost::filesystem::create_directory( pathFolder );
	boost::filesystem::path pathFolderPictures = pathFolder / "pictures";
	boost::filesystem::create_directory( pathFolderPictures );
        boost::filesystem::path pathFolderPicturesHD = pathFolderPictures / "HD";
        boost::filesystem::create_directory( pathFolderPicturesHD );
        projectInstance.gPhotoInstance().setFolderToSavePictures( projectInstance.getProjectFolder() );
}

void MainWindow::on_acceptedNewProjectWindow()
{
	this->setEnabled( true );
}

void MainWindow::on_openProjectAction_triggered()
{
	_startWindowDialog->hide();
	QFileDialog * fileDialog = new QFileDialog();
	fileDialog->setAcceptMode( QFileDialog::AcceptOpen );

	QString fileName = fileDialog->getOpenFileName( this, tr( "Open a project" ), QString( boost::filesystem::initial_path().string().c_str() ), "*.tweedy" );

        this->setEnabled( true );


	//plus qu a recuperer le fileName pour ouvrir le projet sauvegarde
        if (!fileName.isNull())

            on_loadProjectAction_triggered(fileName.toStdString().c_str());
}

void MainWindow::on_saveAsProjectAction_triggered()
{
	QFileDialog * fileDialog = new QFileDialog();
	fileDialog->setAcceptMode( QFileDialog::AcceptSave );
	QString fileName = fileDialog->getSaveFileName( this, tr( "Save as a project" ), QString( boost::filesystem::initial_path().string().c_str() ));
        
        std::vector<std::string> strs;
        const std::string st = fileName.toStdString();
        boost::split(strs, st, boost::is_any_of("/"));
        std::string nameFile = strs.back();
        std::string nameFolder = "";
        
        for(std::vector<std::string>::iterator it = strs.begin() ; it < strs.end()-1 ; ++it)
            nameFolder +=  "/" + (*it) ;
                
        project().setProjectFolder(nameFolder);
        project().setProjectFile(nameFile);
        on_saveProjectAction_triggered();
}

void MainWindow::on_undoButton_clicked()
{
	CommandManager& cmdMng = ( Projet::getInstance() ).getCommandManager();
	if( cmdMng.canUndo() )
	{
		cmdMng.undo();
	}
}

void MainWindow::on_redoButton_clicked()
{
	CommandManager& cmdMng = ( Projet::getInstance() ).getCommandManager();
	if( cmdMng.canRedo() )
	{
            cmdMng.redo();
	}
}

void MainWindow::on_aboutAction_triggered()
{
	_aboutWindow = new AboutTweedy( this );
        _aboutWindow->exec();
}

void MainWindow::on_configAction_triggered()
{
	QDockWidget * configCameraDock = new QDockWidget( "Camera Configuration", this );
	{
            _configCamera = new ConfigCamera( configCameraDock );
            configCameraDock->setWidget( ( _configCamera ) );
            addDockWidget( Qt::TopDockWidgetArea, configCameraDock );
	}
	configCameraDock->setFloating( true );
}

//Create the status bar
void MainWindow::createStatusBar()
{
	_statusBar = statusBar();
	_statusBar->showMessage( "Ready" );
}

//save the project
void MainWindow::on_saveProjectAction_triggered()
{
	//make an archive
        //std::string filename = project().getProjectFolder().string() + "/" + project().getProjectFile().string() + ".txt";
	std::string filename = project().getProjectFolder().string() 
                + "/" + project().getProjectFile().string() + ".tweedy";

        std::ofstream ofs( filename.c_str() );
	boost::archive::text_oarchive oa( ofs );
	oa << project();
        ofs.close();
        
        //
        QSettings  settings("IMAC","Tweedy");
        settings.beginGroup("Saves");
        settings.setValue(QString(project().getProjectFile().string().c_str()),filename.c_str());
        settings.endGroup();
}

//load the project
void MainWindow::on_loadProjectAction_triggered(const char* filename)
{
	// open the archive
	std::ifstream ifs( filename );
	boost::archive::text_iarchive ia( ifs );
	ia >> project();
        _chutier->updateChutier();
        Projet& project = Projet::getInstance();
        project.gPhotoInstance().setFolderToSavePictures( project.getProjectFolder() );
        std::cout<<project.getProjectFolder()<<std::endl;
}

void MainWindow::on_exportAction_triggered()
{
    /* place the real time at the end of the timeline */
    project().getTimeline().moveElement(project().getTimeline().getIdRealTime(), project().getTimeline().getMaxTime()-1);
    
    _exportWidget = new ExportWidget( );
    _exportWidget->show();

}

std::string MainWindow::generateTimeData(int value, int choosenFps, int absoluteFps)
{
    double cAbsoluteFps = static_cast<double>(absoluteFps);
    
    //convert clip's width in base 24
    int nbframe = value * (absoluteFps/choosenFps);
    int hour = nbframe/std::pow(cAbsoluteFps,3);
    int min = (nbframe % static_cast<int>(std::pow(cAbsoluteFps,3)))/std::pow(cAbsoluteFps,2);
    int sec = (nbframe % static_cast<int>(std::pow(cAbsoluteFps,2)))/absoluteFps;
    nbframe = nbframe % absoluteFps ;
    
    std::string shour = hour<10 ? "0"+boost::lexical_cast<std::string>(hour) 
            : boost::lexical_cast<std::string>(hour);

    std::string smin = min<10 ? "0"+boost::lexical_cast<std::string>(min) 
            : boost::lexical_cast<std::string>(min);

    std::string ssec = sec<10 ? "0"+boost::lexical_cast<std::string>(sec) 
            : boost::lexical_cast<std::string>(sec);

    std::string sframe = nbframe<10 ? "0"+boost::lexical_cast<std::string>(nbframe) 
            : boost::lexical_cast<std::string>(nbframe);

    return shour + ":" + smin + ":" + ssec + ":" + sframe ;
}

/**
 * @brief buttons slots
 */
void MainWindow::handle_playPauseAction_triggered()
{
        if( !_isPlaying )
        {
             _timer->start( 1000.0 / _fps );
             _isPlaying = true;
             _playPauseAction->setIcon( QIcon( "img/icones/pause.png" ) );
             _playPauseAction->setStatusTip( "Mettre en pause" );
        }
        else
        {
             _timer->stop();
             _isPlaying = false;
             _playPauseAction->setIcon( QIcon( "img/icones/play.png" ) );
             _playPauseAction->setStatusTip( "Lancer le montage" );
        }
}

void MainWindow::handle_zeroAction_triggered()
{
        _time = 0;
        Q_EMIT timeChanged( _time );
}

void MainWindow::handle_nextAction_triggered()
{
        Timeline::OMapClip orderedClips = getTimeline().getOrderedClips();
        bool lastClip = true;

        BOOST_FOREACH( const Timeline::OMapClip::value_type& s, orderedClips )
        {
                if( s.first > _time )
                {
                        lastClip = false;
                        _time = s.first;
                        break;
                }
                if( ( *s.second )->timeOut() > _time && ( *s.second )->timeOut() < getTimeline().getMaxTime() )
                {
                        lastClip = false;
                        _time = ( *s.second )->timeOut();
                        break;
                }
        }

        if( !lastClip )
                Q_EMIT timeChanged( _time );
}

void MainWindow::handle_prevAction_triggered()
{
        Timeline::OMapClip orderedClips = getTimeline().getOrderedClips();
        int currentTime = _time;
        bool firstClip = true;

        BOOST_FOREACH( const Timeline::OMapClip::value_type& s, orderedClips )
        {
                if( s.first < currentTime )
                {
                        firstClip = false;
                        _time = s.first;
                        if( ( *s.second )->timeOut() < currentTime )
                                _time = ( *s.second )->timeOut();
                }
                else
                        break;
        }

        if( !firstClip )
                Q_EMIT timeChanged( _time );
}

/**
 * @brief Increase current time or stop timer if last frame
 */
void MainWindow::increaseTime()
{

        if( _time>-1 && _time < getTimeline().getMaxTime() )
                ++ _time;
        if( _time == getTimeline().getMaxTime() )
                _time = 0;

        Q_EMIT timeChanged( _time );
}

void MainWindow::saveWidgets()
{
    QSettings settings("Test", "Test Dock Problem");
    settings.setValue("MainWindow/State", saveState());
}

void MainWindow::on_initialPlaceWidgets_triggered()
{
    QSettings settings("Test", "Test Dock Problem");
    restoreState(settings.value("MainWindow/State").toByteArray());
}

void MainWindow::changeTimeViewer( int newTime )
{
    _viewerImg->getTempsSlider()->setSliderPosition( newTime );

    std::string time;
    time = generateTimeData( newTime,_fps,24);
     _viewerImg->getTimeLabel()->setText( QString(time.c_str()));

    _timelineGraphic->getTimelineDataWrapper()._currentTime = newTime;
}

void MainWindow::changeFps(QString value)
{
    _fps = value.toInt();
}

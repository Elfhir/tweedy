#include "core/Application.hpp"

#include <tweedy/gui/Undo/UndoWidget.hpp>


#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtCore/QFile>
#include <iostream>

#include "gui/timesheet/TimeSheet.h"
#include "gui/MainWindow/mainwindow.h"
#include "core/command/clip/CmdClipSetTimeRange.hpp"


int main(int argc, char *argv[])
{
    //init the Application's singleton

    // pointeurs sur l'unique instance de la classe UniqueObject
      Application * application;

      // initialisation des pointeurs
      application =Application::getInstance ();

      //FOR TEST create a timeline
      //Timeline timeline01;
      //application->mapTimeline.insert(iterator before, const key_type $key,, std::auto_ptr<U>x)
      //application->mapTimeline.end()

      // affectation de la valeur 15 à l'objet pointé par obj1
      application->setValue (15);

      // affichage de _value
      std::cout << "obj1::_value = " << application->getValue () << std::endl;

      // destruction de l'instance unique
      application->kill ();

	// initialize resources, if needed
	// Q_INIT_RESOURCE(resfile);

	QApplication app(argc, argv);
	MainWindow mainWin;

	// Load css stylesheet
	const QString appCss( QCoreApplication::applicationDirPath() + "/resources/tweedy.css" );
	QFile appCssFile( appCss );
	if( ! appCssFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		std::cout << "Failed to open css file: " << appCss.toStdString() << std::endl;
	}
	else
	{
		const QString cssContent( appCssFile.readAll() );
		//std::cout << "cssContent: " << cssContent.toStdString() << std::endl;
		qApp->setStyleSheet(cssContent);
	}
        
        
        //test undo redo
        
        CommandManager cmdMan;
        Clip c1("img/tweedy0.jpg" );
    
        IUndoRedoCommand * cmd1 = new CmdClipSetTimeRange(c1,"Time Out + 2", 2);      
        
        cmdMan.pushNewCommand(cmd1);
        
        UndoView* undoView = new UndoView(&cmdMan);
    
        QWidget * undoWidget = new UndoWidget(undoView);
        undoWidget->setWindowTitle("Command List");
        undoWidget->show();

	mainWin.show();
	return app.exec();
}

/****************************************************************************
*	File: 	TacoSilvia.cpp
*  Author: 	Hartmut Gilde
*	(c) 2003 FRM-II
*	Purpose: Provide a GUI for the Labview Taco Interface Client for reasons of convenience
*	
*	Remarks:	This class depends on the LabviewClient class which interacts as the interface between Qt and Labview using TACO communication mechanism
*****************************************************************************/


/* Qt Includes */
#include "TacoSilvia.h"
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qerrormessage.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qtextedit.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtable.h>
#include <qstringlist.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qstylefactory.h>
#include <qpalette.h>
#include <qevent.h>
#include <qheader.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qapplication.h>
#include <qdialog.h>

/* TACO Includes */
#include <Admin.h>
#include <API.h>
#include <DevServer.h>

/* Labview Interface Includes */
#include "LabviewClient.h"
#include "Pixmaps.h"

int TacoSilvia::STATE_NOTIMPORTED 		= 0;
int TacoSilvia::STATE_IMPORTED 			= 1;
int TacoSilvia::STATE_CONTROLSLISTED	= 2;
int TacoSilvia::STATE_INDICATORSLISTED	= 3;

/* definition of the static column names */
QString QControlsTable::name_caption	= QString("Name");
QString QControlsTable::type_caption	= QString("Type"); 	
QString QControlsTable::value_caption	= QString("Value");	
QString QControlsTable::symbol_caption	= QString("Symbol");
QString QControlsTable::update_caption	= QString("Update");


//! The constructor initializes all the widgets and displays them.
/*!
 * A QGridLayout class is used for the arranging and alignment of the widgets.
*/
TacoSilvia::TacoSilvia( QWidget *parent, const char *name, WFlags fl )
    : QMainWindow( parent, name, fl ), appicon((const char **) xpm_icon_data)
{
	initPalettes();
	initIcons();
	initActions();
	initWindow(name);
	initMenus();
	initTables();
	initWidgets();
	addWidgets();
	setColors();
	initConnections();
	languageChange();
	stateMachine(TacoSilvia::STATE_NOTIMPORTED);
}

//! Defines a customized color palette for the widgets. 
/*! 
 * TODO: detailed description
 */
void TacoSilvia::initPalettes() 
{
	
	palette = ((QPalette&)(new QButton(this))->palette());
	palette.setColor(QColorGroup::Background, 		QColor(220,220,220)); 
	palette.setColor(QColorGroup::Foreground, 		QColor(20,20,20)); 
	palette.setColor(QColorGroup::Base, 				QColor(240,240,240)); 
	palette.setColor(QColorGroup::Text, 				QColor(0,0,0));
	palette.setColor(QColorGroup::Button, 				QColor(210,210,210));
	palette.setColor(QColorGroup::ButtonText, 		QColor(40,40,40));
	palette.setColor(QColorGroup::Light, 				QColor(220,220,220));
	palette.setColor(QColorGroup::Midlight, 			QColor(190,190,190));
	palette.setColor(QColorGroup::Dark, 				QColor(50,50,50));
	palette.setColor(QColorGroup::Mid, 					QColor(150,150,150));
	palette.setColor(QColorGroup::Shadow, 				QColor(40,40,40));
	palette.setColor(QColorGroup::Highlight, 			QColor(190,190,190));
	palette.setColor(QColorGroup::HighlightedText, 	QColor(30,30,30));
	palette.setColor(QColorGroup::BrightText, 		QColor(40,40,40));
	palette.setColor(QColorGroup::Link,					QColor(80,80,80));
	palette.setColor(QColorGroup::LinkVisited, 		QColor(60,60,60));
	
	((QColorGroup&)palette.disabled()).setColor(QColorGroup::Base, 	QColor(190,190,190));
	((QColorGroup&)palette.disabled()).setColor(QColorGroup::Text, 	QColor(100,100,100));
	((QColorGroup&)palette.disabled()).setColor(QColorGroup::Dark, 	QColor(150,150,150));
	((QColorGroup&)palette.disabled()).setColor(QColorGroup::Mid, 		QColor(190,190,190));
	((QColorGroup&)palette.disabled()).setColor(QColorGroup::Shadow,	QColor(90,90,90));
	
}

//! * Creates the Icons from ASCII bitmaps. 
/*! 
 * TODO: detailed description
 */
void TacoSilvia::initIcons()
{ 
	// construct icons
   p_device_import	= QPixmap ( xpm_device_import );
   p_device_close 	= QPixmap ( xpm_device_close 	);
                             
   p_vi_info			= QPixmap ( xpm_vi_info 		);
   p_list_controls	= QPixmap ( xpm_list_controls );
	p_list_indicators	= QPixmap ( xpm_list_indicators);
   p_get_value			= QPixmap ( xpm_get_value 		);
   p_set_value			= QPixmap ( xpm_set_value 		);
	p_get_ind_value 	= QPixmap ( xpm_get_ind_value	);
                             
   p_about				= QPixmap ( xpm_about 			);
   p_instructions		= QPixmap ( xpm_instructions 	);

	// make pixmaps transparent
	p_device_import	. setMask( p_device_import		. createHeuristicMask() );
	p_device_close		. setMask( p_device_close		. createHeuristicMask() );
	p_vi_info			. setMask( p_vi_info				. createHeuristicMask() );
	p_list_controls	. setMask( p_list_controls		. createHeuristicMask() );
	p_list_indicators	. setMask( p_list_indicators	. createHeuristicMask() );
	p_get_value			. setMask( p_get_value			. createHeuristicMask() );
	p_set_value			. setMask( p_set_value			. createHeuristicMask() );
	p_get_ind_value	. setMask( p_get_ind_value		. createHeuristicMask() );
	p_instructions		. setMask( p_instructions		. createHeuristicMask() );
	p_about				. setMask( p_about				. createHeuristicMask() );
}

//! Defines the Actions and assigns icons to them.
/*! 
 * TODO: detailed description
 */
void TacoSilvia::initActions()
{
	deviceImportAction 			= new QAction( this, "deviceImportAction" );
	deviceCloseAction 			= new QAction( this, "deviceCloseAction" 	);
	deviceExitAction 				= new QAction( this, "deviceExitAction" 	);
	
	commandsViInfoAction 		= new QAction( this, "commandsViInfoAction" 			);
	commandsControlListAction 	= new QAction( this, "commandsControlListAction" 	);
	commandsIndicatorListAction= new QAction( this, "commandsIndicatorListAction" );
	commandsGetValueAction 		= new QAction( this, "commandsGetValueAction" 		);
	commandsGetIndValueAction	= new QAction( this, "commandsGetIndValueAction"	);
	commandsSetValueAction 		= new QAction( this, "commandsSetValueAction" 		);

	helpInstructionsAction 		= new QAction( this, "helpInstructionsAction" 	);
	helpAboutAction 				= new QAction( this, "helpAboutAction" 			);

	// assign icons to actions	
	deviceImportAction			->setIconSet( QIconSet( p_device_import 	) );
	deviceCloseAction 			->setIconSet( QIconSet( p_device_close 	) );
	
	commandsViInfoAction 		->setIconSet( QIconSet( p_vi_info 			) );
	commandsControlListAction 	->setIconSet( QIconSet( p_list_controls 	) );
	commandsIndicatorListAction->setIconSet( QIconSet( p_list_indicators	) );
	commandsGetIndValueAction 	->setIconSet( QIconSet( p_get_ind_value	) );
	commandsGetValueAction 		->setIconSet( QIconSet( p_get_value 		) );
	commandsSetValueAction 		->setIconSet( QIconSet( p_set_value 		) );

	helpInstructionsAction 		->setIconSet( QIconSet( p_instructions 	) );
	helpAboutAction 				->setIconSet( QIconSet( p_about 				) );
}

//! Initializes the main window of the GUI.
/*!
 * Sets the name of the application, creates the layout and initializes the window size. 
 */
void TacoSilvia::initWindow(const char* name) 
{
	(void)statusBar();
   if ( !name ) {
		setName( "TACO Silvia" );
	}
	// construct main frame
	frame = new QFrame(this, "myframe");
	frame->setPalette(palette);
	
  	setCentralWidget(frame);
	g = new QGridLayout( frame, 5, 2, 20 );
	g->setName("mylayout");
	Q_CHECK_PTR(g);
	
	// used for simplified debug output
	errmsg = new QErrorMessage(this, "Error");

	// set GUI parameters
	setStyle(QStyleFactory::create("platinum"));
	setIcon(appicon);
	setUsesBigPixmaps	( true  );
	setUsesTextLabel 	( false );
   setMinimumSize( 1100, 600 );
   setFocusPolicy( QWidget::ClickFocus );
	lv_client = NULL;
}

//! Constructs menus and assign the corresponding actions. 
/*! 
 * TODO: detailed description
 */
void TacoSilvia::initMenus() 
{
   devicemenu = new QPopupMenu( this );
   Q_CHECK_PTR( devicemenu );
	devicemenu->setCaption("Device");
	deviceImportAction	-> addTo( devicemenu );
	deviceCloseAction		-> addTo( devicemenu );
	devicemenu->insertSeparator();
	deviceExitAction		-> addTo( devicemenu );

   commandsmenu = new QPopupMenu( this );
   Q_CHECK_PTR( commandsmenu );
   commandsmenu->setCaption("Commands");
	commandsViInfoAction			-> addTo( commandsmenu );
	commandsmenu->insertSeparator();
	commandsControlListAction	-> addTo( commandsmenu );
	commandsGetValueAction		-> addTo( commandsmenu );
	commandsSetValueAction		-> addTo( commandsmenu );
	commandsmenu->insertSeparator();
	commandsIndicatorListAction-> addTo( commandsmenu );
	commandsGetIndValueAction	-> addTo( commandsmenu );

   helpmenu = new QPopupMenu( this );
   Q_CHECK_PTR( helpmenu );
	helpInstructionsAction	-> addTo( helpmenu );
	helpAboutAction			-> addTo( helpmenu );
	
   // If we used a QMainWindow we could use its built-in menuBar().
   menubar = new QMenuBar( this );
   Q_CHECK_PTR( menubar );
   menubar->insertItem( "&Device", 		devicemenu 		);
   menubar->insertItem( "&Commands", 	commandsmenu 	);
   menubar->insertSeparator();
   menubar->insertItem( "&Help", 		helpmenu 	 	);


	// toolbar
   toolbar = new QToolBar( "", this, DockTop ); 
	Q_CHECK_PTR( toolbar );

   deviceImportAction			->addTo( toolbar );
	deviceCloseAction				->addTo( toolbar );
   toolbar->addSeparator();
   commandsViInfoAction			->addTo( toolbar );
	toolbar->addSeparator();
	commandsControlListAction	->addTo( toolbar );
	commandsGetValueAction		->addTo( toolbar );
   commandsSetValueAction		->addTo( toolbar );
	toolbar->addSeparator();
	commandsIndicatorListAction->addTo( toolbar );
	commandsGetIndValueAction	->addTo( toolbar );
	toolbar->addSeparator();
	
	helpInstructionsAction		->addTo( toolbar );
	helpAboutAction				->addTo( toolbar );
}

//! Creates and initializes the central QTable (actually a subclassed QTable) widgets that hold the controls and indicators. 
/*! 
 * TODO: detailed description
 */
void TacoSilvia::initTables() 
{
	/*QStringList itable_captions;
	itable_captions.append("Parameter");
	itable_captions.append("Value");

	itable = new QTable( 0, 2, frame );
	itable->setColumnLabels(itable_captions);
	itable->setLeftMargin(0);
	for (int i=0; i<itable->numCols();i++) {
		itable->setColumnReadOnly(i, true);
	}
	itable->setShowGrid(false);
	itable->setSelectionMode(QTable::NoSelection);*/

	QStringList table_captions;
	table_captions.append(QControlsTable::update_caption);
	table_captions.append(QControlsTable::symbol_caption);
	table_captions.append(QControlsTable::type_caption);
	table_captions.append(QControlsTable::name_caption);
	table_captions.append(QControlsTable::value_caption);
	
	table = new QControlsTable( 0, 5, frame );
	table->setName("Controls");
	table->setColumnLabels(table_captions);
	for (int i=0; i<table->numCols();i++) {
		if (!table->horizontalHeader()->label(i).compare(QControlsTable::update_caption) || !table->horizontalHeader()->label(i).compare(QControlsTable::value_caption)) {
			table->setColumnReadOnly(i, false);
		}	
		else {
			table->setColumnReadOnly(i, true);
		}
	}
	table->setSelectionMode(QTable::SingleRow);
	table->setFocusStyle(QTable::FollowStyle);
	table->setShowGrid(false);
	t = new QControlTip( table );


	indtable = new QControlsTable( 0, 5, frame );
	indtable->setName("Indicators");
	indtable->setColumnLabels(table_captions);
	t = new QControlTip( indtable );

	for (int i=0; i<indtable->numCols();i++) {
		if (!indtable->horizontalHeader()->label(i).compare(QControlsTable::update_caption)) {
			indtable->setColumnReadOnly(i, false);
		}	
		else {
			indtable->setColumnReadOnly(i, true);
		}
	}
	indtable->setSelectionMode(QTable::SingleRow);
	indtable->setFocusStyle(QTable::FollowStyle);
	indtable->setShowGrid(false);

}

//! Initializes all remaining widgets such as buttons, labels, checkboxes etc.
/*! 
 * TODO: detailed description
 */
void TacoSilvia::initWidgets() 
{
	hbl0 = new QHBoxLayout();
	statusLabel 	 	= new QLabel( "<b>Status:</b>"	,frame );
	statusMessage		= new QLabel(frame);
	statusMessage->setFrameStyle( QFrame::Box );

	//statusMessage = new QTextEdit(this);
	//statusMessage.setReadOnly(true);	

	bimport = new QPushButton( "Import Device", frame );
	hbl0->addWidget(statusLabel);
	hbl0->addWidget(statusMessage);
   hbl0->addWidget( bimport );

	//info_label 		  	= new QLabel( "<b>VI Parameters:</b>"	,frame );
	controls_label	  	= new QLabel( "<b>Controls:</b>"			,frame );
	indicators_label 	= new QLabel( "<b>Indicators:</b>"		,frame );


	hbl1 = new QHBoxLayout();
   binfo = new QPushButton( "Get VI Parameters", frame );
   hbl0->addWidget( binfo );

	hbl2 = new QHBoxLayout();
	blist = new QPushButton( "Get Controls", frame );
   hbl2->addWidget( blist );

   bgetv = new QPushButton( "Get Value", frame );
   hbl2->addWidget( bgetv );

	bsetv = new QPushButton( "Set Value", frame );
   hbl2->addWidget( bsetv );
	
	hbl3 = new QHBoxLayout();
	bilist = new QPushButton( "Get Indicators", frame );
   hbl3->addWidget( bilist );

   bgetiv = new QPushButton( "Get Value", frame );
   hbl3->addWidget( bgetiv );
	
	hbl4 = new QHBoxLayout();
   check_update_box = new QCheckBox("Update Control Values at Interval [sec]:" ,frame );
   hbl4->addWidget( check_update_box );

	interval_spinbox = new QSpinBox( 1, 3600, 1, frame );
   hbl4->addWidget( interval_spinbox );

	hbl5 = new QHBoxLayout();
   ind_check_update_box = new QCheckBox("Update Indicator Values at Interval [sec]:" ,frame );
   hbl5->addWidget( ind_check_update_box );

	ind_interval_spinbox = new QSpinBox( 1, 3600, 1, frame );
   hbl5->addWidget( ind_interval_spinbox );
}

//! Arranges the widgets in the main window.
/*! 
 * TODO: detailed description
 */
void TacoSilvia::addWidgets() 
{

	g->addLayout( hbl0, 	0, 0);
	//g->addLayout( hbl1, 0, 1 );

   //g->addWidget( info_label, 			1, 0 );
   g->addWidget( controls_label, 	1, 0 );
	g->addWidget( indicators_label, 	1, 1 );
	
	//g->addWidget( itable, 	2, 0 );
	g->addWidget( table, 	2, 0 );
	g->addWidget( indtable, 2, 1 );

	g->addLayout( hbl2, 3, 0 );
	g->addLayout( hbl3, 3, 1 );

	g->addLayout( hbl4, 4, 0 );
	g->addLayout( hbl5, 4, 1 );
}

//! Realizes the signals and slots connections between the actions and widgets.
void TacoSilvia::initConnections()
{
	// signals and slots connections
   connect( deviceImportAction, 			SIGNAL( activated() ), this, SLOT( importDevice() 		) );
	connect( deviceCloseAction,  			SIGNAL( activated() ), this, SLOT( closeDevice()  		) );
   connect( deviceExitAction,   			SIGNAL( activated() ), this, SLOT( exit()					) );

   connect( commandsViInfoAction, 		SIGNAL( activated() ), this, SLOT( getViInfo()			) );
   connect( commandsControlListAction, SIGNAL( activated() ), this, SLOT( getControlList() 	) );
   connect( commandsIndicatorListAction,SIGNAL( activated() ), this, SLOT( getIndicatorList()) );
   connect( commandsGetValueAction, 	SIGNAL( activated() ), this, SLOT( getControlValue() 	) );
   connect( commandsSetValueAction, 	SIGNAL( activated() ), this, SLOT( setControlValue()	) );
   connect( commandsGetIndValueAction, SIGNAL( activated() ), this, SLOT( getIndicatorValue()) );

   connect( helpInstructionsAction, 	SIGNAL( activated() ), this, SLOT( instructions()		) );
   connect( helpAboutAction, 				SIGNAL( activated() ), this, SLOT( about()	 			) );
   
   connect( table, SIGNAL( valueChanged(int, int) ), this, SLOT( setTextControlValue(int, int)	) );
   connect( table, SIGNAL( doubleClicked(int, int, int, const QPoint&) ), this, SLOT( clickTableItem(int, int, int, const QPoint&)	) );
   connect( table, SIGNAL( contextMenuRequested(int, int, const QPoint&) ), this, SLOT( showTableContextMenu(int, int, const QPoint&)	) );
   connect( indtable, SIGNAL( valueChanged(int, int) ), this, SLOT( setIndUpdateValue(int, int)	) );
	connect( indtable, SIGNAL( contextMenuRequested(int, int, const QPoint&) ), this, SLOT( showIndTableContextMenu(int, int, const QPoint&)	) );

   connect( bimport, SIGNAL( clicked() ), this, SLOT( deviceImportClose() ) );
   connect( binfo, SIGNAL( clicked() ), this, SLOT( getViInfo() ) );
   connect( blist, SIGNAL( clicked() ), this, SLOT( getControlList() ) );
   connect( bgetv, SIGNAL( clicked() ), this, SLOT( getControlValue() ) );
   connect( bsetv, SIGNAL( clicked() ), this, SLOT( setControlValue() ) );
   connect( bilist, SIGNAL( clicked() ), this, SLOT( getIndicatorList() ) );
   connect( bgetiv, SIGNAL( clicked() ), this, SLOT( getIndicatorValue() ) );
   connect( check_update_box, SIGNAL( stateChanged( int ) ), this, SLOT( updateControlsActivated(int) ));
   connect( interval_spinbox, SIGNAL( valueChanged( int ) ), this, SLOT( intervalControlsChanged(int) ));
   connect( ind_check_update_box, SIGNAL( stateChanged( int ) ), this, SLOT( updateIndicatorsActivated(int) ));
   connect( ind_interval_spinbox, SIGNAL( valueChanged( int ) ), this, SLOT( intervalIndicatorsChanged(int) ));

}

//! Assigns the custom palette to the widgets 
/*
 * (this is done to get rid of the ugly default light blue).
 */
void TacoSilvia::setColors() 
{
	QColor background = palette.color(QPalette::Active, QColorGroup::Background);
	devicemenu				->setPaletteBackgroundColor(background);
	commandsmenu			->setPaletteBackgroundColor(background);
	helpmenu					->setPaletteBackgroundColor(background);
	menubar					->setPaletteBackgroundColor(background);
	toolbar					->setPaletteBackgroundColor(background);
	bimport					->setPaletteBackgroundColor(background);
	binfo						->setPaletteBackgroundColor(background);
	blist						->setPaletteBackgroundColor(background);
	bgetv						->setPaletteBackgroundColor(background);
	bsetv						->setPaletteBackgroundColor(background);
	bilist					->setPaletteBackgroundColor(background);
	bgetiv					->setPaletteBackgroundColor(background);
	interval_spinbox		->setPaletteBackgroundColor(background);
	ind_interval_spinbox	->setPaletteBackgroundColor(background);
	statusMessage			->setPaletteForegroundColor(QColor("Red"));
	((QPalette&)check_update_box		->palette()).setColor(QColorGroup::Button, 	QColor(240,240,240));
	((QPalette&)ind_check_update_box	->palette()).setColor(QColorGroup::Button, 	QColor(240,240,240));
}





//! brief Destroys the object and frees any allocated resources
TacoSilvia::~TacoSilvia()
{
    // no need to delete child widgets, Qt does it all for us
}



//! Sets the strings of the subwidgets using the current language.
void TacoSilvia::languageChange()
{
   setCaption( tr( "Taco Silvia" ) );
   toolbar->setLabel( tr( "Toolbar" ) );

	// set Names for Actions
   deviceImportAction			-> setText( tr( "Import Device" 		) );
   deviceCloseAction				-> setText( tr( "Close Device" 		) );
   deviceExitAction				-> setText( tr( "Exit" 					) );
	commandsViInfoAction			-> setText( tr( "ViInfo" 				) );	
	commandsControlListAction	-> setText( tr( "Get Control List" 	) );	
	commandsIndicatorListAction-> setText( tr( "Get Indicator List") );	
	commandsGetValueAction		-> setText( tr( "Get Control Value" ) );	
	commandsSetValueAction		-> setText( tr( "Set Control Value" ) );	
	commandsGetIndValueAction	-> setText( tr( "Get Indicator Value") );	
	helpInstructionsAction		-> setText( tr( "Instructions" 		) );	
	helpAboutAction				-> setText( tr( "About" 				) );	

	// set Menu Texts for Actions
   deviceImportAction			-> setMenuText( tr( "&Import Device" 		) );
   deviceCloseAction				-> setMenuText( tr( "Close Device" 			) );
   deviceExitAction				-> setMenuText( tr( "&Exit" 					) );
   commandsViInfoAction			-> setMenuText( tr( "&Get VI Info" 			) );
   commandsControlListAction	-> setMenuText( tr( "&Get Control List" 	) );
   commandsIndicatorListAction-> setMenuText( tr( "&Get Indicator List"	) );
   commandsGetValueAction		-> setMenuText( tr( "&Get Control Value" 	) );
   commandsSetValueAction		-> setMenuText( tr( "&Set Control Value" 	) );
   commandsGetIndValueAction	-> setMenuText( tr( "&Get Indicator Value") );
   helpInstructionsAction		-> setMenuText( tr( "&Instructions" 		) );
   helpAboutAction				-> setMenuText( tr( "&About" 					) );

	// define Accelerators for Actions
   deviceImportAction			-> setAccel( tr( "Ctrl+D" 			) );
   deviceExitAction				-> setAccel( tr( "Ctrl+E" 			) );
   commandsViInfoAction			-> setAccel( tr( "Ctrl+P" 			) );
   commandsControlListAction	-> setAccel( tr( "Ctrl+L" 			) );
   commandsIndicatorListAction-> setAccel( tr( "Ctrl+I" 			) );
   commandsGetValueAction		-> setAccel( tr( "Ctrl+G" 			) );
   commandsSetValueAction		-> setAccel( tr( "Ctrl+S" 			) );
   commandsGetIndValueAction	-> setAccel( tr( "Ctrl+Shift+I"	) );
   helpInstructionsAction		-> setAccel( tr( "Ctrl+H" 			) );
   helpAboutAction				-> setAccel( tr( "Ctrl+A" 			) );

}

//! This function checks in what kind of state the application finds itself and adapts the widgets correspondigly.
/*
 * It keeps the widgets disabled when no device is imported or when no vi could be reached.
 * Four internal states have been defined (as static members):
 * STATE_NOTIMPORTED
 * STATE_IMPORTED
 * STATE_CONTROLSLISTED
 * STATE_INDICATORSLISTED
 * 
 */ 
int TacoSilvia::stateMachine(int state_flags) {
	int error=0;
	switch (state_flags) {
		case 0:
			state = TacoSilvia::STATE_NOTIMPORTED;
			statusMessage->setPaletteForegroundColor(QColor("Red"));
			statusMessage->setText("Device not imported");
			bimport->setText("Import Device");	
			//adapt widgets (turn disabled)
			deviceImportAction			->setEnabled(true);
			deviceCloseAction 			->setEnabled(false);
			commandsViInfoAction 		->setEnabled(false);
			commandsControlListAction 	->setEnabled(false);
			commandsIndicatorListAction->setEnabled(false);
			commandsGetValueAction 		->setEnabled(false);
			commandsSetValueAction 		->setEnabled(false);
			commandsGetIndValueAction	->setEnabled(false);
			helpInstructionsAction 		->setEnabled(true);
			helpAboutAction 				->setEnabled(true);

			//info_label						->setEnabled(false);
			controls_label					->setEnabled(false);
			indicators_label				->setEnabled(false);

			indtable							->setEnabled(false);
			table								->setEnabled(false);
			//itable							->setEnabled(false);

			ind_interval_spinbox			->setEnabled(false);
			ind_check_update_box			->setEnabled(false);
			interval_spinbox				->setEnabled(false);
			check_update_box				->setEnabled(false);

			bgetiv							->setEnabled(false);
			bilist							->setEnabled(false);
			bsetv								->setEnabled(false);
			bgetv								->setEnabled(false);
			blist								->setEnabled(false);
			binfo								->setEnabled(false);
			break;
		case 1:
			state = state | TacoSilvia::STATE_IMPORTED;
			statusMessage->setPaletteForegroundColor(QColor("DarkGreen"));
			statusMessage->setText("Device imported successfully");
			bimport->setText("Close Device");
			//adapt widgets (turn disabled or enabled)
			//bimport							->setEnabled(false);
			deviceImportAction			->setEnabled(false);
			deviceCloseAction 			->setEnabled(true);
			commandsViInfoAction 		->setEnabled(true);
			commandsControlListAction 	->setEnabled(true);
			commandsIndicatorListAction->setEnabled(true);
			commandsGetValueAction 		->setEnabled(false);
			commandsSetValueAction 		->setEnabled(false);
			commandsGetIndValueAction 	->setEnabled(false);
			helpInstructionsAction 		->setEnabled(true);
			helpAboutAction 				->setEnabled(true);

			//info_label						->setEnabled(true);
			controls_label					->setEnabled(true);
			indicators_label				->setEnabled(true);

			indtable							->setEnabled(true);
			table								->setEnabled(true);
			//itable							->setEnabled(true);

			indtable							->setPalette(palette);
			table								->setPalette(palette);
			//itable							->setPalette(palette);

			ind_interval_spinbox			->setEnabled(false);
			ind_check_update_box			->setEnabled(false);
			interval_spinbox				->setEnabled(false);
			check_update_box				->setEnabled(false);

			bgetiv							->setEnabled(false);
			bilist							->setEnabled(true);
			bsetv								->setEnabled(false);
			bgetv								->setEnabled(false);
			blist								->setEnabled(true);
			binfo								->setEnabled(true);
			break;
		case 2:
			if ((state & TacoSilvia::STATE_IMPORTED) == TacoSilvia::STATE_IMPORTED) {
				state = state | TacoSilvia::STATE_CONTROLSLISTED;

				//adapt widgets (turn disabled or enabled)
				deviceImportAction			->setEnabled(false);
				deviceCloseAction 			->setEnabled(true);
				commandsViInfoAction 		->setEnabled(true);
				commandsControlListAction 	->setEnabled(true);
				commandsIndicatorListAction->setEnabled(true);
				commandsGetValueAction 		->setEnabled(true);
				commandsSetValueAction 		->setEnabled(true);
				helpInstructionsAction 		->setEnabled(true);
				helpAboutAction 				->setEnabled(true);

				//info_label						->setEnabled(true);
				controls_label					->setEnabled(true);
				indicators_label				->setEnabled(true);

				indtable							->setEnabled(true);
				table								->setEnabled(true);
				//itable							->setEnabled(true);

				interval_spinbox				->setEnabled(true);
				check_update_box				->setEnabled(true);

				bilist							->setEnabled(true);
				bsetv								->setEnabled(true);
				bgetv								->setEnabled(true);
				blist								->setEnabled(true);
				binfo								->setEnabled(true);
			}
			break;
		case 4:
			if ((state & TacoSilvia::STATE_IMPORTED) == TacoSilvia::STATE_IMPORTED) {
				state = state | TacoSilvia::STATE_INDICATORSLISTED;

				//adapt widgets (turn disabled or enabled)
				deviceImportAction			->setEnabled(false);
				deviceCloseAction 			->setEnabled(true);
				commandsViInfoAction 		->setEnabled(true);
				commandsControlListAction 	->setEnabled(true);
				commandsIndicatorListAction->setEnabled(true);
				commandsGetIndValueAction	->setEnabled(true);
				helpInstructionsAction 		->setEnabled(true);
				helpAboutAction 				->setEnabled(true);

				//info_label						->setEnabled(true);
				controls_label					->setEnabled(true);
				indicators_label				->setEnabled(true);

				indtable							->setEnabled(true);
				table								->setEnabled(true);
				//itable							->setEnabled(true);

				ind_interval_spinbox			->setEnabled(true);
				ind_check_update_box			->setEnabled(true);

				bilist							->setEnabled(true);
				bgetiv							->setEnabled(true);
				blist								->setEnabled(true);
				binfo								->setEnabled(true);
			}
			break;
		case 6:
			if ((state & TacoSilvia::STATE_IMPORTED) == TacoSilvia::STATE_IMPORTED) {
				state = state | TacoSilvia::STATE_CONTROLSLISTED | TacoSilvia::STATE_INDICATORSLISTED;

				//adapt widgets (turn disabled or enabled)
				deviceImportAction			->setEnabled(false);
				deviceCloseAction 			->setEnabled(true);
				commandsViInfoAction 		->setEnabled(true);
				commandsControlListAction 	->setEnabled(true);
				commandsIndicatorListAction->setEnabled(true);
				commandsGetValueAction 		->setEnabled(true);
				commandsSetValueAction 		->setEnabled(true);
				commandsGetIndValueAction 	->setEnabled(true);
				helpInstructionsAction 		->setEnabled(true);
				helpAboutAction 				->setEnabled(true);

				//info_label						->setEnabled(true);
				controls_label					->setEnabled(true);
				indicators_label				->setEnabled(true);

				indtable							->setEnabled(true);
				table								->setEnabled(true);
				//itable							->setEnabled(true);

				ind_interval_spinbox			->setEnabled(true);
				ind_check_update_box			->setEnabled(true);
				interval_spinbox				->setEnabled(true);
				check_update_box				->setEnabled(true);

				bgetiv							->setEnabled(true);
				bilist							->setEnabled(true);
				bsetv								->setEnabled(true);
				bgetv								->setEnabled(true);
				blist								->setEnabled(true);
				binfo								->setEnabled(true);
			}
			break;
		case 3:
			state = TacoSilvia::STATE_IMPORTED | TacoSilvia::STATE_CONTROLSLISTED;
			statusMessage->setPaletteForegroundColor(QColor("DarkGreen"));
			statusMessage->setText("Device imported successfully");
			bimport->setText("Close Device");

			//adapt widgets (turn disabled or enabled)
			//bimport							->setEnabled(false);
			deviceImportAction			->setEnabled(false);
			deviceCloseAction 			->setEnabled(true);
			commandsViInfoAction 		->setEnabled(true);
			commandsControlListAction 	->setEnabled(true);
			commandsIndicatorListAction->setEnabled(true);
			commandsGetValueAction 		->setEnabled(true);
			commandsSetValueAction 		->setEnabled(true);
			helpInstructionsAction 		->setEnabled(true);
			helpAboutAction 				->setEnabled(true);

			//info_label						->setEnabled(true);
			controls_label					->setEnabled(true);
			indicators_label				->setEnabled(true);

			indtable							->setEnabled(true);
			table								->setEnabled(true);
			//itable							->setEnabled(true);

			indtable							->setPalette(palette);
			table								->setPalette(palette);
			//itable							->setPalette(palette);

			interval_spinbox				->setEnabled(true);
			check_update_box				->setEnabled(true);

			bilist							->setEnabled(true);
			bsetv								->setEnabled(true);
			bgetv								->setEnabled(true);
			blist								->setEnabled(true);
			binfo								->setEnabled(true);
			break;
		case 5:
			state = TacoSilvia::STATE_IMPORTED | TacoSilvia::STATE_INDICATORSLISTED;
			statusMessage->setPaletteForegroundColor(QColor("DarkGreen"));
			statusMessage->setText("Device imported successfully");
			bimport->setText("Close Device");

			//adapt widgets (turn disabled or enabled)
			//bimport							->setEnabled(false);
			deviceImportAction			->setEnabled(false);
			deviceCloseAction 			->setEnabled(true);
			commandsViInfoAction 		->setEnabled(true);
			commandsControlListAction 	->setEnabled(true);
			commandsIndicatorListAction->setEnabled(true);
			commandsGetIndValueAction 	->setEnabled(true);
			helpInstructionsAction 		->setEnabled(true);
			helpAboutAction 				->setEnabled(true);

			//info_label						->setEnabled(true);
			controls_label					->setEnabled(true);
			indicators_label				->setEnabled(true);

			indtable							->setEnabled(true);
			table								->setEnabled(true);
			//itable							->setEnabled(true);

			indtable							->setPalette(palette);
			table								->setPalette(palette);
			//itable							->setPalette(palette);

			ind_interval_spinbox			->setEnabled(true);
			ind_check_update_box			->setEnabled(true);

			bilist							->setEnabled(true);
			bgetiv							->setEnabled(true);
			blist								->setEnabled(true);
			binfo								->setEnabled(true);
			break;
		case 7:
			state = TacoSilvia::STATE_IMPORTED | TacoSilvia::STATE_IMPORTED | TacoSilvia::STATE_INDICATORSLISTED;
			statusMessage->setPaletteForegroundColor(QColor("DarkGreen"));
			statusMessage->setText("Device imported successfully");
			bimport->setText("Close Device");
			//adapt widgets (turn disabled or enabled)
			//bimport							->setEnabled(false);
			deviceImportAction			->setEnabled(false);
			deviceCloseAction 			->setEnabled(true);
			commandsViInfoAction 		->setEnabled(true);
			commandsControlListAction 	->setEnabled(true);
			commandsGetValueAction 		->setEnabled(true);
			commandsSetValueAction 		->setEnabled(true);
			commandsIndicatorListAction->setEnabled(true);
			commandsGetIndValueAction 	->setEnabled(true);
			helpInstructionsAction 		->setEnabled(true);
			helpAboutAction 				->setEnabled(true);

			//info_label						->setEnabled(true);
			controls_label					->setEnabled(true);
			indicators_label				->setEnabled(true);

			indtable							->setEnabled(true);
			table								->setEnabled(true);
			//itable							->setEnabled(true);

			indtable							->setPalette(palette);
			table								->setPalette(palette);
			//itable							->setPalette(palette);

			ind_interval_spinbox			->setEnabled(true);
			ind_check_update_box			->setEnabled(true);
			interval_spinbox				->setEnabled(true);
			check_update_box				->setEnabled(true);

			bgetiv							->setEnabled(true);
			bilist							->setEnabled(true);
			bsetv								->setEnabled(true);
			bgetv								->setEnabled(true);
			blist								->setEnabled(true);
			binfo								->setEnabled(true);
			break;
		default:
			error = -1;

	}
	return error;
}


//! Initializes the LabviewClient which encapsulates the communication with Labview.
void TacoSilvia::importDevice() 
{
	if (lv_client==NULL) {
		lv_client = new LabviewClient();
	}
	if (lv_client->InitDevice() != DS_OK) {
		QMessageBox::critical( this, "TacoSilvia Error", "Could not import Device\nPlease ensure that the device server is running properly");
		return;
		//QApplication::exit(-1);
	}
	stateMachine(TacoSilvia::STATE_IMPORTED);
	//getViInfo(); 
	//QMessageBox::information( this, "Labview Taco", "Device Server successfully imported");
}


//! Frees the TACO device and erases all content from widgets.
/*!
 * Invokes a call to stateMachine() to turn all widgets disabled.
 */
void TacoSilvia::closeDevice()
{
	/* clear parameters table
	QMemArray<int> rows(itable->numRows());
	for (unsigned int i=0; i<rows.size();i++) {
		rows[i] = i;
	}
	itable->removeRows(rows);*/

	// clear controls table
	QMemArray<int> rows = table->numRows();
	for (unsigned int i=0; i<rows.size();i++) {
		rows[i] = i;
	}
	table->removeRows(rows);

	// clear indicators table
	rows = indtable->numRows();
	for (unsigned int i=0; i<rows.size();i++) {
		rows[i] = i;
	}
	indtable->removeRows(rows);

	stateMachine(TacoSilvia::STATE_NOTIMPORTED);
	
	// The destructor of LabviewClient calls dev_free to close TACO device
	delete lv_client;
	lv_client = NULL;
}



//! Retrieves a list of VI related informations and displays them in a modal dialog.
/*!
 *	The List is made up of strings each of which contains a param/value pair.
 *
 *	In order to highlight it with different styles in the QTextEdit they have to be splitted
 */
void TacoSilvia::getViInfo() 
{
	if ((state & TacoSilvia::STATE_IMPORTED) != TacoSilvia::STATE_IMPORTED) {
		//illegal state
		return;
	}

   DevVarStringArray* cntl_list;
	cntl_list = new DevVarStringArray();

	int status = lv_client->GetViInfo(cntl_list);
	if (status < 0) {
	  	QMessageBox::warning( this, "Error", "GetViInfo() failed!!");
	}

	/*
	// clear table
	QMemArray<int> rows(itable->numRows());
	for (unsigned int i=0; i<rows.size();i++) {
		rows[i] = i;
	}
	itable->removeRows(rows);*/

	QString* line=NULL;
	QString* param=NULL;
	QString* value=NULL;
	int start=0;
	const char* tok = ": ";
	/*for(unsigned int i=0; i<cntl_list->length; i++) {
		line = new QString(cntl_list->sequence[i]);
		start = line->find(tok);
		param = new QString(line->left(start));
		value = new QString(line->mid(start+strlen(tok)));
		itable->insertRows(itable->numRows());
		itable->setItem(i,0, new QTableItem(itable, QTableItem::OnTyping, (*param)));
		itable->setItem(i,1, new QTableItem(itable, QTableItem::OnTyping, (*value)));
	}*/

	// construct info dialog
	QDialog *dlg = new QDialog(this, "VIInfo");
	QGridLayout *dg = new QGridLayout( dlg, cntl_list->length,2, 15 );
	QLabel* param_labels[cntl_list->length]; 
	QLabel* value_labels[cntl_list->length];
	for(unsigned int i=0; i<cntl_list->length; i++) {
		line = new QString(cntl_list->sequence[i]);
		start = line->find(tok);
		param = new QString(line->left(start)+":");
		value = new QString(line->mid(start+strlen(tok)));
		param_labels[i] = new QLabel(*param, dlg);
		param_labels[i]->setPaletteForegroundColor(QColor(50,50,50));
		value_labels[i] = new QLabel(*value, dlg);
		value_labels[i]->setPaletteForegroundColor(QColor(190, 90, 0));
		dg->addWidget(param_labels[i], i, 0);
		dg->addWidget(value_labels[i], i, 1);
	}
	dlg->exec();
}


 
//! Retrieves a list of all controls using the LabviewClient interface. 

/*! 
 * Then the type of each control is retrieved sequentially so that the control 
 * may be displayed properly in the table (with the right icon and the right table item) 
 * As the QTable model doesn't allow indexing by caption names, we always have to check 
 * the header captions sequentially in order to find the correct position for the insertion 
 * of the corresponding item entry. 
 * For this purpose and in order to keep things consistant, the table captions are generalized and kept as static members.
 * Before the table may be populated with vi data, we have to figure out the indices for 
 * the name, value, type field in the coresponding QTable
 */
void TacoSilvia::getControlList() 
{ 
	if ((state & TacoSilvia::STATE_IMPORTED) != TacoSilvia::STATE_IMPORTED) {
		//illegal state
		return;
	}
	DevVarStringArray* cntl_list;
	cntl_list = new DevVarStringArray();

	int status = lv_client->GetControlList(cntl_list);
	if (status < 0) {
	  	QMessageBox::warning( this, "Error", "GetControlList() failed!!");
		stateMachine(TacoSilvia::STATE_NOTIMPORTED);
	}
	
 	QPixmap* type_icon = NULL;
	QTableItem* item = NULL;
	int vstatus;

	// clear table
	QMemArray<int> rows(table->numRows());
	for (unsigned int i=0; i<rows.size();i++) {
		rows[i] = i;
	}
	table->removeRows(rows);

	for(unsigned int i=0; i<cntl_list->length; i++) {
		int type = lv_client->GetControlType(cntl_list->sequence[i]);
	 	if (type < 0) {
	   	QMessageBox::warning( this, "Error", "GetControlType() failed!!");
	 	}
		switch (type) {
			case D_DOUBLE_TYPE:	
				type_icon = new QPixmap( XPM_D_DOUBLE_TYPE );
				
				DevDouble dbl;	
				vstatus = lv_client->GetDoubleValue(cntl_list->sequence[i], &dbl);
				item = new FloatingTableItem((QTable*)table, QString::number(dbl));
				break;
			case D_FLOAT_TYPE:
				type_icon = new QPixmap( XPM_D_FLOAT_TYPE );
				
				DevFloat flt;	
				vstatus = lv_client->GetFloatValue(cntl_list->sequence[i], &flt);
				item = new FloatingTableItem(table, QString::number((double)flt));
				break;
			case D_SHORT_TYPE:
				type_icon = new QPixmap( XPM_D_SHORT_TYPE );
				
				DevShort shrt;	
				vstatus = lv_client->GetShortValue(cntl_list->sequence[i], &shrt);
				item = new IntegerTableItem(table, QString::number(shrt));
				break;
			case D_USHORT_TYPE:
				type_icon = new QPixmap( XPM_D_USHORT_TYPE );
				
				DevUShort ushrt;	
				vstatus = lv_client->GetUShortValue(cntl_list->sequence[i], &ushrt);
				item = new IntegerTableItem(table, QString::number(ushrt));
				break;
			case D_LONG_TYPE:
				type_icon = new QPixmap( XPM_D_LONG_TYPE );
				
				DevLong lng;	
				vstatus = lv_client->GetLongValue(cntl_list->sequence[i], &lng);
				item = new IntegerTableItem(table, QString::number(lng));
				break;
			case D_ULONG_TYPE:
				type_icon = new QPixmap( XPM_D_ULONG_TYPE );
				
				DevULong ulng;	
				vstatus = lv_client->GetULongValue(cntl_list->sequence[i], &ulng);
				item = new IntegerTableItem(table, QString::number(ulng));
				break;
			case D_STRING_TYPE:
				type_icon = new QPixmap( XPM_D_STRING_TYPE );
				{
				DevString str = new char[LabviewClient::MAX_STRING_LENGTH]; 
				vstatus = lv_client->GetStringValue(cntl_list->sequence[i], &str);
				item = new QTableItem(table, QTableItem::OnTyping, QString::fromLatin1(str));
				item->setWordWrap(true);
				}
				break;
			case D_BOOLEAN_TYPE:
				type_icon = new QPixmap( XPM_D_BOOLEAN_TYPE );
			  	{ 
				DevBoolean bl;	
				vstatus = lv_client->GetBooleanValue(cntl_list->sequence[i], &bl);
				QStringList bool_list;
				bool_list.append("TRUE");
				bool_list.append("FALSE");
				item = new QComboTableItem(table, bool_list);
				((QComboTableItem*)item)->setCurrentItem((bl)?0:1);
				}
				break;
			case D_VOID_TYPE:
				type_icon = new QPixmap( XPM_D_UNKNOWN_TYPE );

				item = new QTableItem(table, QTableItem::OnTyping,"");
				break;
			default:
				type_icon = new QPixmap( XPM_D_UNKNOWN_TYPE );
				
				item = new QTableItem(table, QTableItem::OnTyping,"");
				break;
		}
		table->insertRows(table->numRows());	

		for (int j=0; j<table->numCols();j++) {
			if (!table->horizontalHeader()->label(j).compare(QControlsTable::update_caption)) {
				table->setItem(i,j, new QCheckTableItem(table, "no")); 
			}	
			else if (!table->horizontalHeader()->label(j).compare(QControlsTable::name_caption)) {
				table->setItem(i,j, new QTableItem(table, QTableItem::OnTyping, cntl_list->sequence[i])); 
			}
			else if (!table->horizontalHeader()->label(j).compare(QControlsTable::type_caption)) {
				table->setItem(i,j, new QTableItem(table, QTableItem::OnTyping, lv_client->stringDType(type)));
			}
			else if (!table->horizontalHeader()->label(j).compare(QControlsTable::value_caption)) {
				if (item != NULL) {
					table->setItem(i,j, item);
				}	
			}
			else if (!table->horizontalHeader()->label(j).compare(QControlsTable::symbol_caption)) {
				table->setPixmap(i,j,*type_icon);
			}
		}

	}
	for (int i=0; i<table->numCols(); i++) {
		table->adjustColumn(i);
	}
	stateMachine(TacoSilvia::STATE_CONTROLSLISTED);
}



//! Retrieves a list of all indicators using the LabviewClient interface and populates the corresponding table widget (same as above, now for the indicators)
 
/*!
 * As the QTable model doesn't allow indexing by caption names, the header captions are scanned 
 * sequentially in order to find the correct position for the insertion of the corresponding item entry. 
 * For this purpose and in order to keep things maintainable, the table captions are generalized and kept as static members.
 * Before the table may be populated with vi data, the indices for the name, value, 
 * type fields have to be found in the coresponding QTable
 */
void TacoSilvia::getIndicatorList() 
{
	DevVarStringArray* cntl_list;
	cntl_list = new DevVarStringArray();

	int status = lv_client->GetControlList(cntl_list, TRUE);
	if (status < 0) {
	  	QMessageBox::warning( this, "Error", "GetControlList() failed!!");
		stateMachine(TacoSilvia::STATE_NOTIMPORTED);
	}
	
 	QPixmap* type_icon = NULL;
	QTableItem* item = NULL;
	int vstatus;

	// clear table
	QMemArray<int> rows(indtable->numRows());
	for (unsigned int i=0; i<rows.size();i++) {
		rows[i] = i;
	}
	indtable->removeRows(rows);

	for(unsigned int i=0; i<cntl_list->length; i++) {
		int type = lv_client->GetControlType(cntl_list->sequence[i], TRUE);
	 	if (type < 0) {
	   	QMessageBox::warning( this, "Error", "GetControlType() failed!!");
	 	}
		switch (type) {
			case D_DOUBLE_TYPE:
				type_icon = new QPixmap( XPM_D_DOUBLE_TYPE );
				
				DevDouble dbl;	
				vstatus = lv_client->GetDoubleValue(cntl_list->sequence[i], &dbl, TRUE);
				item = new FloatingTableItem((QTable*)table, QString::number(dbl));
				break;
			case D_FLOAT_TYPE:
				type_icon = new QPixmap( XPM_D_FLOAT_TYPE );
				
				DevFloat flt;	
				vstatus = lv_client->GetFloatValue(cntl_list->sequence[i], &flt, TRUE);
				item = new FloatingTableItem(table, QString::number((double)flt));
				break;
			case D_SHORT_TYPE:
				type_icon = new QPixmap( XPM_D_SHORT_TYPE );
				
				DevShort shrt;	
				vstatus = lv_client->GetShortValue(cntl_list->sequence[i], &shrt, TRUE);
				item = new IntegerTableItem(table, QString::number(shrt));
				break;
			case D_USHORT_TYPE:
				type_icon = new QPixmap( XPM_D_USHORT_TYPE );
				
				DevUShort ushrt;	
				vstatus = lv_client->GetUShortValue(cntl_list->sequence[i], &ushrt, TRUE);
				item = new IntegerTableItem(table, QString::number(ushrt));
				break;
			case D_LONG_TYPE:
				type_icon = new QPixmap( XPM_D_LONG_TYPE );
				
				DevLong lng;	
				vstatus = lv_client->GetLongValue(cntl_list->sequence[i], &lng, TRUE);
				item = new IntegerTableItem(table, QString::number(lng));
				break;
			case D_ULONG_TYPE:
				type_icon = new QPixmap( XPM_D_ULONG_TYPE );
				
				DevULong ulng;	
				vstatus = lv_client->GetULongValue(cntl_list->sequence[i], &ulng, TRUE);
				item = new IntegerTableItem(table, QString::number(ulng));
				break;
			case D_STRING_TYPE:
				type_icon = new QPixmap( XPM_D_STRING_TYPE );
				{
				DevString str = new char[100]; 
				vstatus = lv_client->GetStringValue(cntl_list->sequence[i], &str, TRUE);
				item = new QTableItem(table, QTableItem::OnTyping, QString::fromLatin1(str));
				item->setWordWrap(true);
				}
				break;
			case D_BOOLEAN_TYPE:
				type_icon = new QPixmap( XPM_D_BOOLEAN_TYPE );
			  	{ 
				DevBoolean bl;	
				vstatus = lv_client->GetBooleanValue(cntl_list->sequence[i], &bl, TRUE);
				QStringList bool_list;
				bool_list.append("TRUE");
				bool_list.append("FALSE");
				item = new QComboTableItem(table, bool_list);
				((QComboTableItem*)item)->setCurrentItem((bl)?0:1);
				}
				break;
			case D_VOID_TYPE:
				type_icon = new QPixmap( XPM_D_UNKNOWN_TYPE );

				item = new QTableItem(table, QTableItem::OnTyping,"");
				break;
			default:
				type_icon = new QPixmap( XPM_D_UNKNOWN_TYPE );
				
				item = new QTableItem(table, QTableItem::OnTyping,"");
				break;
		}
		
		indtable->insertRows(indtable->numRows());	

		for (int j=0; j<table->numCols();j++) {
			if (!indtable->horizontalHeader()->label(j).compare(QControlsTable::update_caption)) {
				indtable->setItem(i,j, new QCheckTableItem(indtable, "no")); 
			}	
			else if (!indtable->horizontalHeader()->label(j).compare(QControlsTable::name_caption)) {
				indtable->setItem(i,j, new QTableItem(indtable, QTableItem::OnTyping, cntl_list->sequence[i])); 
			}
			else if (!indtable->horizontalHeader()->label(j).compare(QControlsTable::type_caption)) {
				indtable->setItem(i,j, new QTableItem(indtable, QTableItem::OnTyping, lv_client->stringDType(type)));
			}
			else if (!indtable->horizontalHeader()->label(j).compare(QControlsTable::value_caption)) {
				if (item != NULL) {
					indtable->setItem(i,j, item);
				}	
			}
			else if (!indtable->horizontalHeader()->label(j).compare(QControlsTable::symbol_caption)) {
				indtable->setPixmap(i,j,*type_icon);
			}
		}
	}
	for (int i=0; i<indtable->numCols(); i++) {
		indtable->adjustColumn(i);
	}
	stateMachine(TacoSilvia::STATE_INDICATORSLISTED);
}


//! Displays the about window.
/*!
 * Shows some version info and the TACO Silvia Icon. 
 */
void TacoSilvia::about()
{
	QMessageBox aboutBox(this);
	aboutBox.setIconPixmap(appicon);
	aboutBox.setText("TACO SILVIA\nTACO Server Interfacing Labview Virtual Instrument Applications\n\nversion: 0.1\nauthor: Hartmut Gilde\n(c) 2003 FRM2 TUM");
	
	aboutBox.exec();
}

//! Displays a window with instructions how to use the client and server.
/*!
 * Provides the user with some infos of what it is and what you can do with it and how you should do it.
 */
void TacoSilvia::instructions()
{
	QMessageBox aboutBox(this);
	aboutBox.setText("INSTRUCTIONS FOR ACCESSING LABVIEW CONTROLS WITH TACOSILVIA:\
\n\n1) Run the Labview VIs and start the Labview TACO Server \
\n--------------------------------------------------------\
\n\t- Start Labview and open the Taco-Labview-Client application (LVTacoSelectViRemote.vi).\
\n\t1.1) Local VI Access: \
\n\t- In the upper panel choose the directory where your VI is located via FileChooserDialog.\
\n\t- Press <Update>, select your VI from the pulldown-list and run it via <Run> button.\
\n\t1.2) Remote VI Access: \
\n\t- Enter the machine name of the remote machine in the server list on the lower panel\
\n\t- Open the VI Server(Server.vi) on the corresponding machine. \
\n\t- Insert the desired VI in the export list and run the application. \
\n\t- After pressing <Update> the exported VIs should appear in the list now. \
\n\t- Select the VI from the list and press <Run>. \
\nNote: When you run the VI from within the Panel, the adequate TACO Server is exported automatically. \
\n\n2) Import The Device And Access VI Controls\
\n---------------------------------------------\
\n\t- In the TacoSilvia GUI just click <Import Device>. \
\n\t2.1) List the Controls: \
\n\t- Retrieve a list of the available controls and indicators by pressing <GetControlList> \
\n\t- Once listed, you can set and get the values of the controls and indicators in various ways.\
\n\t2.2) Get a value (control OR indicator):\
\n\t- Press the <GetControlValue> or the <GetIndicatorValue>button \
\n\t- Select the corresponding Pull-Down Menu Item\
\n\t- Press the adequate Toolbar Icon.\
\n\t- Let the data automatically be retrieved by activating the <Update> facility.\
\n\t2.3) Set a value (only control):\
\n\t- Double-click on an entry in the control list and fill in the value field. \
\n\t- Select the control in the list and press the <SetControlValue> button. \
\n\t- Select the corresponding Pull-Down Menu Item.\
\n\t- Press the adequate Toolbar Icon.\
\n\n3) Synchronize Data \
\n---------------------\
\n\t- If desired, the control values are regularly synchronized with labview. \
\n\t- Just mark the checkbox and enter the interval at which synchronization should occur.\
\n\t- The control data is then retrieved automatically.");
	
	aboutBox.exec();
}

//! Terminates the application.
/*! 
 * Shuts down the TACO device before closing the main window.
 */
void TacoSilvia::exit()
{
	// free the device
	closeDevice();

	// shut down gui
	close();
}


//! Retrieves the value of the labview indicator using the LabviewClient interface method 
/*!
 * As the QTable model doesn't allow indexing by caption names, we always have to check 
 * the header captions sequentially in order to find the correct position for the insertion 
 * of the corresponding item entry. 
 * For this purpose and in order to keep things consistant, the table captions are generalized and kept as static members.
 * Before the table may be populated with vi data, we have to figure out the indices for the name, value, type field in the coresponding QTable*/
void TacoSilvia::getIndicatorValue() 
{
	if (((state & TacoSilvia::STATE_IMPORTED) != TacoSilvia::STATE_IMPORTED) || ((state & TacoSilvia::STATE_INDICATORSLISTED) != TacoSilvia::STATE_INDICATORSLISTED)) {
		//illegal state
		return;
	}

	if (indtable->numSelections() == 0) {
		QMessageBox::warning( this, "Note", "Please Select an Indicator First");
		return;
	}	

	int vstatus;
	int type_index = -1;
	int name_index = -1;
	int update_index = -1;
	int value_index = -1;
	QTableItem* item = NULL;

	for (int j=0; j< indtable->horizontalHeader()->count(); j++) {
		if (!indtable->horizontalHeader()->label(j).compare(QControlsTable::type_caption)) {
			type_index = j;
		}		
		if (!indtable->horizontalHeader()->label(j).compare(QControlsTable::name_caption)) {
			name_index = j;
		}
		if (!indtable->horizontalHeader()->label(j).compare(QControlsTable::update_caption)) {
			update_index = j;
		}
		if (!indtable->horizontalHeader()->label(j).compare(QControlsTable::value_caption)) {
			value_index = j;
		}

	}

	for(int i=0; i<indtable->numRows(); i++) {	
		if (!indtable->isRowSelected(i)) {	
			continue;
		}

		int type = LabviewClient::typeFromString((DevString)(indtable->text(i, type_index).latin1()));
	 	if (type < 0) {
	   	QMessageBox::warning( this, "Error", "LabviewClient::typeFromString() failed!!");
	 	}
		switch (type) {
			case D_DOUBLE_TYPE:
				DevDouble dbl;	
				vstatus = lv_client->GetDoubleValue((DevString)(indtable->text(i, name_index).latin1()), &dbl, true);
				item = new FloatingTableItem(indtable, QString::number(dbl));
				break;
			case D_FLOAT_TYPE:
				DevFloat flt;	
				vstatus = lv_client->GetFloatValue((DevString)(indtable->text(i, name_index).latin1()), &flt, true);
				item = new FloatingTableItem(indtable, QString::number((double)flt));
				break;
			case D_SHORT_TYPE:
				DevShort shrt;	
				vstatus = lv_client->GetShortValue((DevString)(indtable->text(i, name_index).latin1()), &shrt, true);
				item = new IntegerTableItem(indtable, QString::number(shrt));
				break;
			case D_USHORT_TYPE:
				DevUShort ushrt;	
				vstatus = lv_client->GetUShortValue((DevString)(indtable->text(i, name_index).latin1()), &ushrt, true);
				item = new IntegerTableItem(indtable, QString::number(ushrt));
				break;
			case D_LONG_TYPE:
				DevLong lng;	
				vstatus = lv_client->GetLongValue((DevString)(indtable->text(i, name_index).latin1()), &lng, true);
				item = new IntegerTableItem(indtable, QString::number(lng));
				break;
			case D_ULONG_TYPE:
				DevULong ulng;	
				vstatus = lv_client->GetULongValue((DevString)(indtable->text(i, name_index).latin1()), &ulng, true);
				item = new IntegerTableItem(indtable, QString::number(ulng));
				break;
			case D_STRING_TYPE:
				{
				DevString str = new char[LabviewClient::MAX_STRING_LENGTH]; 
				vstatus = lv_client->GetStringValue((DevString)(indtable->text(i, name_index).latin1()), &str, true);
				item = new QTableItem(indtable, QTableItem::OnTyping, QString::fromLatin1(str));
				}
				break;
			case D_BOOLEAN_TYPE:
			  	{ 
				DevBoolean bl;	
				vstatus = lv_client->GetBooleanValue((DevString)(indtable->text(i, name_index).latin1()), &bl, true);
				QStringList bool_list;
				bool_list.append("TRUE");
				bool_list.append("FALSE");
				item = new QComboTableItem(indtable, bool_list);
				((QComboTableItem*)item)->setCurrentItem((bl)?0:1);
				}
				break;
			case D_VOID_TYPE:
				item = new QTableItem(indtable, QTableItem::OnTyping,"");
				break;
			default:
				item = new QTableItem(indtable, QTableItem::OnTyping,"");
				break;
		}
		if (vstatus<0) {
	  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
			stateMachine(TacoSilvia::STATE_NOTIMPORTED);
		}
		if (item != NULL) {
			indtable->setItem(i, value_index, item);
		}
		
		QMessageBox::information( this, "Result GetControlValue", QString("Current value of %1: %2").arg(indtable->text(i, name_index)).arg(indtable->text(i, value_index)));
	}
}



//! getControlValue() retrieves the value of the labview control using the LabviewClient interface method
/*! 
 * As the QTable model doesn't allow indexing by caption names, we always have to check 
 * the header captions sequentially in order to find the correct position for the insertion of the corresponding item entry. 
 * For this purpose and in order to keep things consistant, the table captions are generalized and kept as static members.
 * Before the table may be populated with vi data, we have to figure out the indices for the name, value, type field in the 
 * coresponding QTable
 */
void TacoSilvia::getControlValue()
{
	if (((state & TacoSilvia::STATE_IMPORTED) != TacoSilvia::STATE_IMPORTED) || ((state & TacoSilvia::STATE_CONTROLSLISTED) != TacoSilvia::STATE_CONTROLSLISTED)) {
		//illegal state
		return;
	}

	if (table->numSelections() == 0) {
		QMessageBox::warning( this, "Note", "Please Select a Control First");
		return;
	}	

	int vstatus;
	int type_index = -1;
	int name_index = -1;
	int update_index = -1;
	int value_index = -1;
	QTableItem* item = NULL;

	for (int j=0; j< table->horizontalHeader()->count(); j++) {
		if (!table->horizontalHeader()->label(j).compare(QControlsTable::type_caption)) {
			type_index = j;
		}		
		if (!table->horizontalHeader()->label(j).compare(QControlsTable::name_caption)) {
			name_index = j;
		}
		if (!table->horizontalHeader()->label(j).compare(QControlsTable::update_caption)) {
			update_index = j;
		}
		if (!table->horizontalHeader()->label(j).compare(QControlsTable::value_caption)) {
			value_index = j;
		}

	}

	for(int i=0; i<table->numRows(); i++) {	
		if (!table->isRowSelected(i)) {	
			continue;
		}

		int type = LabviewClient::typeFromString((DevString)(table->text(i, type_index).latin1()));
	 	if (type < 0) {
	   	QMessageBox::warning( this, "Error", "GetControlType() failed!!");
	 	}
		switch (type) {
			case D_DOUBLE_TYPE:
				DevDouble dbl;	
				vstatus = lv_client->GetDoubleValue((DevString)(table->text(i, name_index).latin1()), &dbl);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					stateMachine(TacoSilvia::STATE_NOTIMPORTED);
					return;
				}
				item = new FloatingTableItem(table, QString::number(dbl));
				break;
			case D_FLOAT_TYPE:
				DevFloat flt;	
				vstatus = lv_client->GetFloatValue((DevString)(table->text(i, name_index).latin1()), &flt);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					stateMachine(TacoSilvia::STATE_NOTIMPORTED);
					return;
				}
				item = new FloatingTableItem(table, QString::number((double)flt));
				break;
			case D_SHORT_TYPE:
				DevShort shrt;	
				vstatus = lv_client->GetShortValue((DevString)(table->text(i, name_index).latin1()), &shrt);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					stateMachine(TacoSilvia::STATE_NOTIMPORTED);
					return;
				}
				item = new IntegerTableItem(table, QString::number(shrt));
				break;
			case D_USHORT_TYPE:
				DevUShort ushrt;	
				vstatus = lv_client->GetUShortValue((DevString)(table->text(i, name_index).latin1()), &ushrt);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					stateMachine(TacoSilvia::STATE_NOTIMPORTED);
					return;
				}
				item = new IntegerTableItem(table, QString::number(ushrt));
				break;
			case D_LONG_TYPE:
				DevLong lng;	
				vstatus = lv_client->GetLongValue((DevString)(table->text(i, name_index).latin1()), &lng);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					stateMachine(TacoSilvia::STATE_NOTIMPORTED);
					return;
				}
				item = new IntegerTableItem(table, QString::number(lng));
				break;
			case D_ULONG_TYPE:
				DevULong ulng;	
				vstatus = lv_client->GetULongValue((DevString)(table->text(i, name_index).latin1()), &ulng);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					stateMachine(TacoSilvia::STATE_NOTIMPORTED);
					return;
				}
				item = new IntegerTableItem(table, QString::number(ulng));
				break;
			case D_STRING_TYPE:
				{
				DevString str = new char[LabviewClient::MAX_STRING_LENGTH]; 
				vstatus = lv_client->GetStringValue((DevString)(table->text(i, name_index).latin1()), &str);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					stateMachine(TacoSilvia::STATE_NOTIMPORTED);
					return;
				}
				item = new QTableItem(table, QTableItem::OnTyping, QString::fromLatin1(str));
				item->setWordWrap(true);
				}
				break;
			case D_BOOLEAN_TYPE:
			  	{ 
				DevBoolean bl;	
				vstatus = lv_client->GetBooleanValue((DevString)(table->text(i, name_index).latin1()), &bl);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					stateMachine(TacoSilvia::STATE_NOTIMPORTED);
					return;
				}
				QStringList bool_list;
				bool_list.append("TRUE");
				bool_list.append("FALSE");
				item = new QComboTableItem(table, bool_list);
				((QComboTableItem*)item)->setCurrentItem((bl)?0:1);
				}
				break;
			case D_VOID_TYPE:
				item = new QTableItem(table, QTableItem::OnTyping,"");
				break;
			default:
				item = new QTableItem(table, QTableItem::OnTyping,"");
				break;
		}
		if (item != NULL) {
			table->setItem(i, value_index, item);
		}
		
		QMessageBox::information( this, "Result GetControlValue", QString("Current value of %1: %2").arg(table->text(i, name_index)).arg(table->text(i, value_index)));
	}
}




//! Asks the user to input a value of the adequate type (corresponding to the selected control, and forwards it to the labview control using the LabviewClient interface 

/*! 
 *	This version opens a Dialog Box with an adequate edit widget for inserting a value 
 */
void TacoSilvia::setControlValue() 
{
	if (((state & TacoSilvia::STATE_IMPORTED) != TacoSilvia::STATE_IMPORTED) || ((state & TacoSilvia::STATE_CONTROLSLISTED) != TacoSilvia::STATE_CONTROLSLISTED)) {
		//illegal state
		return;
	}
	if (table->numSelections() == 0) {
		QMessageBox::warning( this, "Note", "Please Select a Control First");
		return;
	}	
	int status;
	int type_index = -1;
	int name_index = -1;
	int update_index = -1;
	int value_index = -1;
	
	for (int j=0; j< table->horizontalHeader()->count(); j++) {
		if (!table->horizontalHeader()->label(j).compare(QControlsTable::type_caption)) {
			type_index = j;
		}		
		if (!table->horizontalHeader()->label(j).compare(QControlsTable::name_caption)) {
			name_index = j;
		}
		if (!table->horizontalHeader()->label(j).compare(QControlsTable::update_caption)) {
			update_index = j;
		}
		if (!table->horizontalHeader()->label(j).compare(QControlsTable::value_caption)) {
			value_index = j;
		}
	}

	bool ok;

	for(int i=0; i<table->numRows(); i++) {	
		if (!table->isRowSelected(i)) {	
			continue;
		}
		//DevString cntl_name = (DevString)table->text(i, name_index).latin1();
		DevString cntl_value = (DevString)table->text(i, value_index).latin1();

		int type = LabviewClient::typeFromString((DevString)(table->text(i, type_index).latin1()));
	 	if (type < 0) {
	   	QMessageBox::warning( this, "Error", "GetControlType() failed!!");
	 	}
		switch (type) {
			case D_DOUBLE_TYPE: {				
				/* DevDouble */
				DevDouble dbl = LabviewClient::DevStringToFloat(cntl_value);
				dbl = (DevDouble)QInputDialog::getDouble("Set Control Value", "Enter a DevDouble", (double)dbl, -2147483647, 2147483647, 10, &ok, this );
				if (ok) {
					status = lv_client->SetDoubleValue((DevString)table->text(i, name_index).latin1(), &dbl);
					if (status < 0) {
						QMessageBox::warning( this, "Error", "SetControlValue() failed!!");
					} 
				} 
				else {
					//user entered nothing or pressed Cancel
					//QMessageBox::information( this, "You entered nothing", "Did you?");
				};
				}
				break;
			case D_FLOAT_TYPE: {
				/* DevFloat */
				DevFloat flt = LabviewClient::DevStringToFloat(cntl_value);
				flt = (DevFloat)QInputDialog::getDouble("Set Control Value", "Enter a DevFloat", (float)flt, -2147483647, 2147483647, 10, &ok, this );
				if (ok) {
					status = lv_client->SetFloatValue((DevString)table->text(i, name_index).latin1(), &flt);
					if (status < 0) {
						QMessageBox::warning( this, "Error", "SetControlValue() failed!!");
					} 
				} 
				else {
					//user entered nothing or pressed Cancel
					//QMessageBox::information( this, "You entered nothing", "Did you?");
				};
		}
				break;
			case D_SHORT_TYPE: {
				/* DevShort */
				DevShort shrt = LabviewClient::DevStringToShort(cntl_value);
				shrt = (DevShort)QInputDialog::getInteger("Set Control Value", "Enter a DevShort", shrt, -32768, 32767, 1, &ok, this );
				if (ok) {
					status = lv_client->SetShortValue((DevString)table->text(i, name_index).latin1(), &shrt);
					if (status < 0) {
						QMessageBox::warning( this, "Error", "SetControlValue() failed!!");
					} 
				} 
				else {
					//user entered nothing or pressed Cancel
					//QMessageBox::information( this, "You entered nothing", "Did you?");
				};
				}
				break;
			case D_USHORT_TYPE: {
				/* DevUShort */
				DevUShort ushrt = LabviewClient::DevStringToUShort(cntl_value);
				ushrt = (DevUShort)QInputDialog::getInteger("Set Control Value", "Enter a DevUShort", ushrt, 0, 65535, 1, &ok, this );
				if (ok) {
					status = lv_client->SetUShortValue((DevString)table->text(i, name_index).latin1(), &ushrt);
					if (status < 0) {
						QMessageBox::warning( this, "Error", "SetControlValue() failed!!");
					} 
				} 
				else {
					//user entered nothing or pressed Cancel
					//QMessageBox::information( this, "You entered nothing", "Did you?");
				};
				}
				break;
			case D_LONG_TYPE: {
				/* DevLong */
				DevLong lng = LabviewClient::DevStringToLong(cntl_value);
				lng = (DevLong)QInputDialog::getInteger("Set Control Value", "Enter a DevLong", lng, -32768, 32767, 1, &ok, this );
				if (ok) {
					status = lv_client->SetLongValue((DevString)table->text(i, name_index).latin1(), &lng);
					if (status < 0) {
						QMessageBox::warning( this, "Error", "SetControlValue() failed!!");
					} 
				} 
				else {
					//user entered nothing or pressed Cancel
					//QMessageBox::information( this, "You entered nothing", "Did you?");
				};
				}
				break;
			case D_ULONG_TYPE: {
				/* DevULong */
				DevULong ulong = LabviewClient::DevStringToULong(cntl_value);
				ulong = (DevULong)QInputDialog::getInteger("Set Control Value", "Enter a DevULong", ulong, 0, 65535, 1, &ok, this );
				if (ok) {
					status = lv_client->SetULongValue((DevString)table->text(i, name_index).latin1(), &ulong);
					if (status < 0) {
						QMessageBox::warning( this, "Error", "SetControlValue() failed!!");
					} 
				} 
				else {
					//user entered nothing or pressed Cancel
					//QMessageBox::information( this, "You entered nothing", "Did you?");
				};
				}
				break;
			case D_STRING_TYPE: {
				/* DevString */
				QString text = QInputDialog::getText("Set Control Value", "Enter a String: ", QLineEdit::Normal, table->text(i, name_index) , &ok, this );
				if (ok) {
					char *str = (char*)text.latin1();
					status = lv_client->SetStringValue((DevString)table->text(i, name_index).latin1(), (DevString*)&str);
					if (status < 0) {
						QMessageBox::warning( this, "Error", "SetControlValue() failed!!");
					} 
				} 
				else {
					//user entered nothing or pressed Cancel
					//QMessageBox::information( this, "You entered nothing", "Did you?");
				};
				}
				break;
			case D_BOOLEAN_TYPE: {
				/* DevBoolean */
				DevBoolean bl = LabviewClient::DevStringToBoolean(cntl_value);
				QStringList blist;
				blist.append("TRUE");
				blist.append("FALSE");
				QString text = QInputDialog::getItem("Set Control Value", "Choose a DevBoolean: ", blist, bl?0:1, false, &ok, this );
				if (ok) {
					DevBoolean boolean;
					(QString::compare(text, "TRUE"))?(boolean=FALSE):(boolean=TRUE);
					status = lv_client->SetBooleanValue((DevString)table->text(i, name_index).latin1(), &boolean);
					if (status < 0) {
						QMessageBox::warning( this, "Error", "SetControlValue() failed!!");
					} 
				} 
				else {
					//user entered nothing or pressed Cancel
					//QMessageBox::information( this, "You entered nothing", "Did you?");
				};
				}
				break;
			case D_VOID_TYPE:
				break;
			default:
				break;
		}
	}
}


//! setTextControlValue() is called when the user has edited a table item on-the-fly by double-clicking it 
/*! 
 * This function is called whenever a control value has been edited on-the-fly. 
 * So we just figure out the corresponding indices, and push the control value to 
 * labview via the LabviewClient interface
 */
void TacoSilvia::setTextControlValue(int row, int col) 
{
	if (((state & TacoSilvia::STATE_IMPORTED) != TacoSilvia::STATE_IMPORTED) || ((state & TacoSilvia::STATE_CONTROLSLISTED) != TacoSilvia::STATE_CONTROLSLISTED)) {
		//illegal state
		return;
	}
	if (!table->horizontalHeader()->label(col).compare(QControlsTable::update_caption)) {
		if	( ((QCheckTableItem*)table->item(row, col))->isChecked() ) {
			table->item(row, col)->setText("yes");
		}
		else {
			table->item(row, col)->setText("no");
		}
		return;
	}

	if (table->currentSelection() < 0 ) {
		QMessageBox::warning( this, "Note", "Please Select a Control First");
		return;
	}
	
	int arr_len=3;
   DevVarStringArray* cntl_list;
	cntl_list = new DevVarStringArray();
	cntl_list->length = arr_len;		
	cntl_list->sequence = new DevString[arr_len];
	for (int i=0; i<arr_len; i++) {
		cntl_list->sequence[i] = new char[200];
	}

	int value_index=-1;
	int name_index=-1;
	int type_index=-1;
	for (int i=0; i< table->horizontalHeader()->count(); i++) {
		if (!table->horizontalHeader()->label(i).compare(QControlsTable::value_caption)) {
			value_index=i;
		}
		else if (!table->horizontalHeader()->label(i).compare(QControlsTable::name_caption)) {
			name_index=i;
		}		
		else if (!table->horizontalHeader()->label(i).compare(QControlsTable::type_caption)) {
			type_index=i;
		}

	}
	
	DevString cntl_name = (DevString)((table->text(row, name_index)).latin1()); 
	DevString cntl_value = (DevString)((table->text(row, value_index)).latin1());
	DevString cntl_type = (DevString)((table->text(row, type_index)).latin1());

	int status;
	if (!strcmp("D_DOUBLE_TYPE", cntl_type)) {
		/* DevDouble */
		DevDouble dbl = LabviewClient::DevStringToDouble((DevString)cntl_value);
		status = lv_client->SetDoubleValue((DevString)cntl_name, &dbl);
		if (status < 0) {
			QMessageBox::warning( this, "Error", "SetDoubleValue() failed!!");
		} 
	}
	else if (!strcmp("D_FLOAT_TYPE", cntl_type)) {
		/* DevFloat */
		DevFloat flt = LabviewClient::DevStringToFloat((DevString)cntl_value);
		status = lv_client->SetFloatValue((DevString)cntl_name, &flt);
		if (status < 0) {
			QMessageBox::warning( this, "Error", "SetFloatValue() failed!!");
		} 
	}
	else if (!strcmp("D_SHORT_TYPE", cntl_type)) {
		/* DevShort */
		DevShort shrt = LabviewClient::DevStringToShort(cntl_value);
		status = lv_client->SetShortValue((DevString)cntl_name, &shrt);
		if (status < 0) {
			QMessageBox::warning( this, "Error", "SetShortValue() failed!!");
		} 
	}
	else if (!strcmp("D_USHORT_TYPE", cntl_type)) {
		/* DevUShort */
		DevUShort us = LabviewClient::DevStringToUShort(cntl_value);
		status = lv_client->SetUShortValue((DevString)cntl_name, &us);
		if (status < 0) {
			QMessageBox::warning( this, "Error", "SetUShortValue() failed!!");
		} 
	}
	else if (!strcmp("D_LONG_TYPE", cntl_type)) {
		/* DevLong */
		DevLong lng = LabviewClient::DevStringToLong(cntl_value);
		status = lv_client->SetLongValue((DevString)cntl_name, &lng);
		if (status < 0) {
			QMessageBox::warning( this, "Error", "SetLongValue() failed!!");
		} 
	}
	else if (!strcmp("D_ULONG_TYPE", cntl_type)) {
		/* DevULong */
		DevULong ulng = LabviewClient::DevStringToULong(cntl_value);
		status = lv_client->SetULongValue((DevString)cntl_name, &ulng);
		if (status < 0) {
			QMessageBox::warning( this, "Error", "SetULongValue() failed!!");
		} 
	}
	else if (!strcmp("D_STRING_TYPE", cntl_type)) {
		/* DevString */
		status = lv_client->SetStringValue((DevString)cntl_name, (DevString*)&cntl_value);
		if (status < 0) {
			QMessageBox::warning( this, "Error", "SetStringValue() failed!!");
		} 
	}
	else if (!strcmp("D_BOOLEAN_TYPE", cntl_type)) {
		/* DevBoolean */
		DevBoolean bl = LabviewClient::DevStringToBoolean(cntl_value);
		status = lv_client->SetBooleanValue((DevString)cntl_name, &bl);
		if (status < 0) {
			QMessageBox::warning( this, "Error", "SetBooleanValue() failed!!");
		} 
	}
}


//! setIndUpdateValue() is called when the user changes the update flag by clicking on it 
/*!
 * To make it clear the text is altered too.
*/
void TacoSilvia::setIndUpdateValue(int row, int col) 
{
	if (((state & TacoSilvia::STATE_IMPORTED) != TacoSilvia::STATE_IMPORTED) || ((state & TacoSilvia::STATE_CONTROLSLISTED) != TacoSilvia::STATE_CONTROLSLISTED)) {
		//illegal state
		return;
	}
	if (!indtable->horizontalHeader()->label(col).compare(QControlsTable::update_caption)) {
		if	( ((QCheckTableItem*)indtable->item(row, col))->isChecked() ) {
			indtable->item(row, col)->setText("yes");
		}
		else {
			indtable->item(row, col)->setText("no");
		}
		return;
	}
}

//! Called when user double clicks on the table.
/*!
 * Invokes customized table editing and enables the use of subclassed QTableItems. 
 */
void TacoSilvia::clickTableItem(int row, int col, int button, const QPoint & mousePos) {
	if ((button == 0) && (mousePos.x()==0)) {
	}
	if (!table->horizontalHeader()->label(col).compare(QControlsTable::update_caption)) {
		return;
	}
	for (int i=0; i< table->horizontalHeader()->count(); i++) {
		if (!table->horizontalHeader()->label(i).compare(QControlsTable::value_caption)) {
			table->setCurrentCell(row, i);
			table->p_beginEdit(row, i);
			break;
		}
	}
}

//! showContextMenu() is used to display a context menu when the user clicks on a control in the QTable.
/*! 
 *	The User may set or get a control value of the selected control. 
 */
void TacoSilvia::showTableContextMenu(int row, int col, const QPoint & pos) {
   QPopupMenu* contextMenu = new QPopupMenu( this );
   Q_CHECK_PTR( contextMenu );

	contextMenu->setPalette(palette);
	contextMenu->setCaption(QString("Exec Command at %1,%2").arg(row).arg(col));
   contextMenu->insertItem( "Get Value", this, SLOT(getControlValue()));
	contextMenu->insertItem( "Set Value", this, SLOT(setControlValue()));

	contextMenu->exec( pos );
   delete contextMenu;
	
}

//! showIndTableContextMenu() is used to display a context menu when the user clicks on an indicator in the QTable
/*
 * The User may get the value of the selected indicator. 
 */
void TacoSilvia::showIndTableContextMenu(int row, int col, const QPoint & pos) {
   QPopupMenu* contextMenu = new QPopupMenu( this );
   Q_CHECK_PTR( contextMenu );

	contextMenu->setPalette(palette);
	contextMenu->setCaption(QString("Exec Command at %1,%2").arg(row).arg(col));
   contextMenu->insertItem( "Get Value", this, SLOT(getIndicatorValue()));

	contextMenu->exec( pos );
   delete contextMenu;
	
}

//! Called whenever the check-button changes state.
/*! 
 *If activated, the timer for synchronizing the values has to be set
 */
void TacoSilvia::updateControlsActivated(int state ) {
	if (state == QButton::On) {
		table->startTimer(1000*interval_spinbox->value());
	}
	if (state == QButton::Off) {
		table->killTimers();
	}
}

//! Called whenever the check-button changes state.
/*! 
 *If activated, the timer for synchronizing the values has to be set
 */
void TacoSilvia::updateIndicatorsActivated(int state) {
	if (state == QButton::On) {
		indtable->startTimer(1000*ind_interval_spinbox->value());
	}
	if (state == QButton::Off) {
		indtable->killTimers();
	}
}

//! Grants access to the LabviewClient object.
/*! 
 * Just returns the LabviewClient member object.
 */
LabviewClient* TacoSilvia::getLabviewClient() {
	return lv_client;
}

//! Handles timer management.
/*!
 * Whenever the interval is altered, the timer has to be stopped and invoked with the recent values.
 */
void TacoSilvia::intervalControlsChanged(int value) {
	if(check_update_box->isChecked()) {
		table->killTimers();
		table->startTimer(1000*value);
	}
}

//! Handles timer management.
/*! 
 * Whenever the interval is altered, the timer has to be stopped and must be invoked with the recent parameters.
 */
void TacoSilvia::intervalIndicatorsChanged(int value) {	
	if(ind_check_update_box->isChecked()) {
		indtable->killTimers();
		indtable->startTimer(1000*value);
	}

}

//! Adapts the import button to the current state.
/*!
 * As only one button is used for importing and exporting the device, 
 * the label of the button changes in dependancy of the current state.
 */
void TacoSilvia::deviceImportClose() 
{
	if (!bimport->text().compare("Close Device") && ((state & TacoSilvia::STATE_IMPORTED) == TacoSilvia::STATE_IMPORTED)) {
		closeDevice();
	}	
	else if (!bimport->text().compare("Import Device") && ((state & TacoSilvia::STATE_IMPORTED) != TacoSilvia::STATE_IMPORTED)) {
		importDevice();
	}

}

//! Grants access to the state machine.
/*! 
 * Enables setting the state from outside of the class
 */
void TacoSilvia::setNotImportedState() {
	stateMachine(TacoSilvia::STATE_NOTIMPORTED);
}




/* Auxiliary Classes and customized widegt classes */


/* subclass QToolTip to customize tool tip
needed to display the corresponding control value when moving cursor over a table entry*/
QControlTip::QControlTip( QTable *table, QToolTipGroup *group ) : QToolTip( table->viewport(), group ), table( table )
{
}

/* decide whether to show and how to show the tool tip*/
void QControlTip::maybeTip( const QPoint &pos )
{
	if (table->numRows()==0 ) {
		return;
	}
	QPoint cp = table->viewportToContents( pos );
   int row = table->rowAt( cp.y() );
   int col = table->columnAt( cp.x() );

	int value_index = -1;
	int name_index = -1;
	
	for (int j=0; j< table->horizontalHeader()->count(); j++) {
		if (!table->horizontalHeader()->label(j).compare(QControlsTable::name_caption)) {
			name_index = j;
		}
		if (!table->horizontalHeader()->label(j).compare(QControlsTable::value_caption)) {
			value_index = j;
		}
	}
   QString tipString = table->text( row, name_index ).append(": ").append(table->text( row, value_index ));

   QRect crn = table->cellGeometry( row, name_index );
	QRect crv = table->cellGeometry( row, value_index );

	QRect cr = table->cellGeometry(row, col);
	cr.setWidth(crn.width()+crv.width()+10);
	cr.setHeight(crn.height()+crv.height()+10);

  	cr.moveTopLeft( table->contentsToViewport( cr.topLeft() ) );
	tip( cr, tipString, "This is a cell in a table" );
}

/* 
	constructor of subclassed QTable 
	just pass arguments to superclass
*/
QControlsTable::QControlsTable ( QWidget * parent, const char * name ) :  QTable(parent, name) {
}

/* 
* constructor of subclassed QTable 
* just pass arguments to superclass
*/
QControlsTable::QControlsTable ( int numRows, int numCols, QWidget * parent, const char * name ) :  QTable(numRows, numCols, parent, name) {
}

	
/* we have to override this virtual function in order to place our own widget for typing float values	with in-place-validation */
void QControlsTable::p_beginEdit(int row, int col ) {
	beginEdit(row, col, false);
}

/* we have to override this virtual function in order to place our own widget for typing float values	with in-place-validation 
same as above, except that we may specify if the old value within the table item should be replaced or not*/
QWidget* QControlsTable::beginEdit ( int row, int col, bool replace ) {
	return QTable::beginEdit(row, col, replace);
}





/* overload the timerEvent function of QObject */
/* this is used to keep track of the control and indicator values of the labview vi */
/* the user is enabled to synchronize the values with labview at regular intervals */
void QControlsTable::timerEvent ( QTimerEvent * te) {
	LabviewClient* lv_client = ((TacoSilvia*)(this->parentWidget()->parentWidget()))->getLabviewClient();
	
	int vstatus;
	int type_index = -1;
	int name_index = -1;
	int update_index = -1;
	QTableItem* item = NULL;

	for (int j=0; j< this->horizontalHeader()->count(); j++) {
		if (!this->horizontalHeader()->label(j).compare(QControlsTable::type_caption)) {
			type_index = j;
		}		
		if (!this->horizontalHeader()->label(j).compare(QControlsTable::name_caption)) {
			name_index = j;
		}
		if (!this->horizontalHeader()->label(j).compare(QControlsTable::update_caption)) {
			update_index = j;
		}
	}

	for(int i=0; i<this->numRows(); i++) {	
		if (!(((QCheckTableItem*)this->item(i, update_index))->isChecked())) {	
			continue;
		}

		int type = LabviewClient::typeFromString((DevString)(this->text(i, type_index).latin1()));
		bool ind = strcmp(this->name(), "Controls")?true:false;
	 	if (type < 0) {
	   	QMessageBox::warning( this, "Error", QString("GetControlType() failed!!: timerID:%1").arg(te->timerId()));
	 	}
		switch (type) {
			case D_DOUBLE_TYPE:
				DevDouble dbl;	
				vstatus = lv_client->GetDoubleValue((DevString)(this->text(i, name_index).latin1()), &dbl, ind);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					((TacoSilvia*)(this->parentWidget()->parentWidget()))->setNotImportedState();
					killTimers();
					return;
				}
				item = new FloatingTableItem(this, QString::number(dbl));
				break;
			case D_FLOAT_TYPE:
				DevFloat flt;	
				vstatus = lv_client->GetFloatValue((DevString)(this->text(i, name_index).latin1()), &flt, ind);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					((TacoSilvia*)(this->parentWidget()->parentWidget()))->setNotImportedState();
					killTimers();
					return;
				}
				item = new FloatingTableItem(this, QString::number((double)flt));
				break;
			case D_SHORT_TYPE:
				DevShort shrt;	
				vstatus = lv_client->GetShortValue((DevString)(this->text(i, name_index).latin1()), &shrt, ind);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					((TacoSilvia*)(this->parentWidget()->parentWidget()))->setNotImportedState();
					killTimers();
					return;
				}
				item = new IntegerTableItem(this, QString::number(shrt));
				break;
			case D_USHORT_TYPE:
				DevUShort ushrt;	
				vstatus = lv_client->GetUShortValue((DevString)(this->text(i, name_index).latin1()), &ushrt, ind);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					((TacoSilvia*)(this->parentWidget()->parentWidget()))->setNotImportedState();
					killTimers();
					return;
				}
				item = new IntegerTableItem(this, QString::number(ushrt));
				break;
			case D_LONG_TYPE:
				DevLong lng;	
				vstatus = lv_client->GetLongValue((DevString)(this->text(i, name_index).latin1()), &lng, ind);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					((TacoSilvia*)(this->parentWidget()->parentWidget()))->setNotImportedState();
					killTimers();
					return;
				}
				item = new IntegerTableItem(this, QString::number(lng));
				break;
			case D_ULONG_TYPE:
				DevULong ulng;	
				vstatus = lv_client->GetULongValue((DevString)(this->text(i, name_index).latin1()), &ulng, ind);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					((TacoSilvia*)(this->parentWidget()->parentWidget()))->setNotImportedState();
					killTimers();
					return;
				}
				item = new IntegerTableItem(this, QString::number(ulng));
				break;
			case D_STRING_TYPE:
				{
				DevString str = new char[LabviewClient::MAX_STRING_LENGTH]; 
				vstatus = lv_client->GetStringValue((DevString)(this->text(i, name_index).latin1()), &str, ind);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					((TacoSilvia*)(this->parentWidget()->parentWidget()))->setNotImportedState();
					killTimers();
					return;
				}
				item = new QTableItem(this, QTableItem::OnTyping, QString::fromLatin1(str));
				item->setWordWrap(true);
				}
				break;
			case D_BOOLEAN_TYPE:
			  	{ 
				DevBoolean bl;	
				vstatus = lv_client->GetBooleanValue((DevString)(this->text(i, name_index).latin1()), &bl, ind);
				if (vstatus<0) {
			  		QMessageBox::warning( this, "Error", "GetControlValue() failed!!");
					((TacoSilvia*)(this->parentWidget()->parentWidget()))->setNotImportedState();
					killTimers();
					return;
				}
				QStringList bool_list;
				bool_list.append("TRUE");
				bool_list.append("FALSE");
				item = new QComboTableItem(this, bool_list);
				((QComboTableItem*)item)->setCurrentItem((bl)?0:1);
				}
				break;
			case D_VOID_TYPE:
				item = new QTableItem(this, QTableItem::OnTyping,"");
				break;
			default:
				item = new QTableItem(this, QTableItem::OnTyping,"");
				break;
		}

		for (int j=0; j<this->numCols();j++) {
			if (!this->horizontalHeader()->label(j).compare(QControlsTable::value_caption)) {
				if (item != NULL) {
					this->setItem(i,j, item);
				}	
			}
		}

	}
	for (int i=0; i<this->numCols(); i++) {
		this->adjustColumn(i);
	}

}


/*! 
 * Constructor for the QTableItem representing float validating 
 */
FloatingTableItem::FloatingTableItem(QTable *table, QString text, double b, double t, int d) : QTableItem(table, QTableItem::OnTyping, text), bottom(b), top(t), decimals(d) 
{
} 

/*! 
 * The createEditor() function of QTableItem has to be overridden 
 * in order to place our own custom edit widgets whenever in-place-editing is requested 
 */
QWidget* FloatingTableItem::createEditor(void) const {
	QLineEdit *le = new QLineEdit(text(), table()->viewport());
	QDoubleValidator *dv = new QDoubleValidator(-999.0, 999.0, 2, le);
	le->setValidator(dv);
		
	return le;
}

/* the SetContentFromEditor has to be overridden too*/
/* however, it doesn't do much except from passing arguments to the superclass */
void FloatingTableItem::setContentFromEditor( QWidget *w ) {
	//update content
	QTableItem::setContentFromEditor( w );
}


/* The same as above except that this version is used for integer value in-place-editing and validation */
IntegerTableItem::IntegerTableItem(QTable *table, QString text, int b, int t) : QTableItem(table, QTableItem::OnTyping, text), bottom(b), top(t) 
{
} 


QWidget* IntegerTableItem::createEditor(void) const {
	QLineEdit *le = new QLineEdit(text(), table()->viewport());
	QIntValidator *iv = new QIntValidator(le);
	le->setValidator(iv);
	
	return le;
}


void IntegerTableItem::setContentFromEditor( QWidget *w ) {
	//update content
	QTableItem::setContentFromEditor( w );
}


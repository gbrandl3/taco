/****************************************************************************
* file: TacoSilvia.h   gui for the taco-labview interface                       
*
*****************************************************************************/


#ifndef TACO_SILVIA_H
#define TACO_SILVIA_H

class LabviewClient;

#include <qvariant.h>
#include <qpixmap.h>
#include <qmainwindow.h>
#include <qtable.h>
#include <qtooltip.h>

class QErrorMessage;
class QListBox;
class QListBoxItem;
class QListView;
class QLineEdit;
class QTable;
class QPushButton;
class QTextEdit;
class QToolBar;
class QPopupMenu;
class QAction;
class QActionGroup;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QTimerEvent;
class QSpinBox;
class QCheckBox;
class QFrame;

class QControlTip;
class QControlsTable;


/*! \mainpage  TACOSilvia - Linking Labview and TACO
 * 
 * \section intro Introduction
 * TACOSiliva is an application that provides an easy-to-use interface between TACO and Labview.
 *
 * LabVIEW is a powerful graphical programming language from National Instruments that uses 
 * icons instead of lines of text to create applications. In contrast to text-based programming 
 * languages, where instructions determine program execution, LabVIEW uses dataflow programming, 
 * where the flow of data determines execution.
 * 
 * TACO is an object oriented control system developed and used at the ESRF
 * (European Synchrotron Radiation Facility) and FRM2(Forschungs-Reaktor München 2) 
 * to control accelerators and beamlines and data acquisition systems.
 * 
 * TACOSiliva is a Qt-application that simplifies remote control of Labview VIs using the 
 * TACO client/server communication mechanism, enabling any TACO client to get access to local
 * OR remote virtual instruments running in a special Labview context. 
 *
 * <br><br>
 * \section install Installation 
 * In order for TACOSilvia to work you must have Qt and a working TACO environment and the Labview-TACO-Lib 
 * developed by Andy Götz (see TACO homepage http://www.esrf.fr/taco). 
 * 
 * \subsection step1 Build the TACO-Labview-Lib:  
 * To build the Taco-Labview-Lib simply execute the Makefile in the labview/server/src/ sub-directory.
 * (Don't forget to adapt the correct Labview installation Path in the Makefile)
 * 
 * \subsection step2 Build TACOSilvia:  
 * To build TACOSiliva ensure that all TACO and Qt environment variables are properly set 
 * and simply execute the Makefile in the labview/client/ subdirectory.
 * (Don't forget to adapt the correct Labview installation Path in the Makefile)
 *
 *
 * <br><br> 
 * \section gettingstarted Getting Started 
 * Just perform following steps to get the system running: 
 * \subsection runlabview Start Labview:  
 * Start Labview. Currently only Labview version 7.0 or higher are supported.
 * <br> 
 * \subsection rundevice Run Device Server:  
 * Open the TacoLabview.llb library from within Labview and select the LVTacoSelectViRemote.vi
 * When you run the VI from within the Panel, the adequate TACO Server is exported automatically
 * \subsubsection localaccess Local VI Access: 
 * \li In the upper panel choose the directory where your VI is located via FileChooserDialog.
 * \li Press <Update>, select your VI from the pulldown-list and run it via <Run> button.
 * <br>
 * \subsubsection remotaccess Remote VI Access: 
 * \li Enter the machine name of the remote machine in the server list on the lower panel
 * \li Open the VI Server(Server.vi) on the corresponding machine. 
 * \li Insert the desired VI in the export list and run the application. 
 * \li After pressing <Update> the exported VIs should appear in the list now. 
 * \li Select the VI from the list and press <i>Run</i>. 
 * <br>
 * \subsection importdb Import The Device And Access VI Controls
 * \li Start TacoSilvia
 * \li In the TacoSilvia GUI just click <i>Import Device</i>. 
 * <br>
 * \subsubsection listcontrols List the Controls: 
 * \li Retrieve a list of the available controls and indicators by pressing <i>GetControlList</i> 
 * \li Once listed, you can set and get the values of the controls and indicators in various ways.
 * <br>
 * \subsubsection getvalue Get a value (control OR indicator):
 * \li Press the <GetControlValue> or the <GetIndicatorValue>button 
 * \li Select the corresponding Pull-Down Menu Item
 * \li Press the adequate Toolbar Icon.
 * \li Let the data automatically be retrieved by activating the <Update> facility.
 * <br>
 * \subsubsection setvalue Set a value (only control):
 * \li Double-click on an entry in the control list and fill in the value field. 
 * \li Select the control in the list and press the <i>SetControlValue</i> button. 
 * \li Select the corresponding Pull-Down Menu Item.
 * \li Press the adequate Toolbar Icon.
 * <br>
 * \subsection synchronize Synchronize Data 
 * \li If desired, the control values are regularly synchronized with labview. 
 * \li Just mark the checkbox and enter the interval at which synchronization should occur.
 * \li The control data is then retrieved automatically at regular intervals.
 * 
 */

//! <b>TacoSilvia</b> means <b>TACO</b> <b>S</b>erver <b>I</b>nterfacing <b>L</b>abview <b>V</b>irtual <b>I</b>nstrument <b>A</b>pplications. It represents a GUI application built with Qt and allows communication with arbitrary Labview Virtual Instruments(VIs) by means of getting and setting the control and indicator values of selected VIs. It uses TACO and the Labview-Taco-Lib to offer an interface to Labview applications. 
    
 
/*!   The Labview-Taco-Lib is strongly tied to a corresponding particular Labview VI that itself loads the VI to be controlled remotely and it is able to handle TACO client requests in Labview context. In that context, this pair acts as the TACO Device Server whereas another class is responsible for the client side: <b>LabviewClient</b>.  The LabviewClient class encapsulates the communication with labview via TACO and does all the type conversion and checking. 
 *   TacoSilvia just delegates the requests to that class in order to contact labview.
 *   Once a connection has been established (the device server has been imported) the controls (and the indicators too) of the selected virtual instrument can be listed and modified using the GUI functionality.   
 */
class TacoSilvia : public QMainWindow 
{
   Q_OBJECT
public:
   TacoSilvia(QWidget *parent=0, const char *name=0, WFlags fl = WType_TopLevel);
	~TacoSilvia();

	QAction* deviceImportAction;
	QAction* deviceCloseAction;
	QAction* deviceExitAction;
	
	QAction* commandsViInfoAction;
	QAction* commandsControlListAction;
	QAction* commandsIndicatorListAction;
	QAction* commandsGetValueAction;
	QAction* commandsSetValueAction;
	QAction* commandsGetIndValueAction;

	QAction* helpInstructionsAction;
	QAction* helpAboutAction;

	LabviewClient* getLabviewClient();
	void	setNotImportedState();

public slots:
   void importDevice();
   void closeDevice();
	void exit();
	void getViInfo();
   void getControlList();
   void getIndicatorList();
   void getControlValue();
   void setControlValue();
	void setTextControlValue(int row, int col);
	void setIndUpdateValue(int row, int col);
   void getIndicatorValue();
	void instructions();
   void about();
	void showTableContextMenu(int row, int col, const QPoint & pos);
	void showIndTableContextMenu(int row, int col, const QPoint & pos);
	void clickTableItem(int row, int col, int button, const QPoint & mousePos);
	void updateControlsActivated(int state);
	void updateIndicatorsActivated(int state);
	void intervalControlsChanged(int value);
	void intervalIndicatorsChanged(int value);
	void deviceImportClose();

protected slots:
    virtual void languageChange();

protected:
	QFrame			*frame;
	QGridLayout 	*g;
   QMenuBar 		*menubar;
	QToolBar 		*toolbar;
	QPopupMenu 		*devicemenu;
	QPopupMenu 		*commandsmenu;
	QPopupMenu 		*helpmenu;		
	QListBox			*lbox;
	QListView		*lview;
	QControlsTable	*table;
	QTable			*indtable;
	//QTable			*itable;
   QLabel   		*label;
	//QLabel			*info_label;
	QLabel			*controls_label;
	QLabel			*indicators_label;

	QLabel			*statusLabel;
	QLabel			*statusMessage;
	//QTextEdit		*statusMessage;

	QSpinBox			*interval_spinbox;
	QCheckBox		*check_update_box;
	QSpinBox			*ind_interval_spinbox;
	QCheckBox		*ind_check_update_box;

	QPushButton 	*bimport;
	QPushButton 	*binfo;
	QPushButton 	*blist;
	QPushButton		*bilist;
	QPushButton 	*bgetv;
	QPushButton 	*bsetv;
	QPushButton 	*bgetiv;
	QErrorMessage 	*errmsg;
		
	QControlTip 	*t;

	QTextEdit		*vinfo;
	LabviewClient	*lv_client;

	QHBoxLayout 	*hbl0;
	QHBoxLayout 	*hbl1;
	QHBoxLayout 	*hbl2;
	QHBoxLayout 	*hbl3;
	QHBoxLayout 	*hbl4;
	QHBoxLayout 	*hbl5;

	QPalette	palette;
	QPixmap appicon;

	QPixmap p_device_import;		
	QPixmap p_device_close; 		
										
	QPixmap p_vi_info;				
	QPixmap p_list_controls;		
	QPixmap p_list_indicators;	
	QPixmap p_get_value;			
	QPixmap p_set_value;			
	QPixmap p_get_ind_value; 	
										
	QPixmap p_about;				
	QPixmap p_instructions;		

	static int STATE_NOTIMPORTED;
	static int STATE_IMPORTED;
	static int STATE_CONTROLSLISTED;
	static int STATE_INDICATORSLISTED;

	int state;

	void initPalettes();
	void initIcons();
	void initActions();
	void initWindow(const char* name=0);
	void initMenus();
	void initTables();
	void initWidgets();
	void addWidgets();
	void initConnections();
	void setColors();

	int stateMachine(int state_flags);
};


#ifndef DOXYGEN_SHOULD_SKIP_THIS 

class FloatingTableItem : public QTableItem {
public:
	FloatingTableItem(QTable *table, QString text, double b=-999.0, double t=999.0, int d=4);

	QWidget* createEditor(void) const;
	void setContentFromEditor( QWidget * w );
protected:
	double bottom;
	double top;
	int decimals;
};

class IntegerTableItem : public QTableItem {
public:
	IntegerTableItem(QTable *table, QString text, int b=0, int t=0);

	QWidget* createEditor(void) const;
	void setContentFromEditor( QWidget * w );
protected:
	int bottom;
	int top;
};



class QControlTip : public QToolTip
{
public:
	QControlTip( QTable* table, QToolTipGroup *group = 0  );

protected:
	void maybeTip( const QPoint &p );

private:
	QTable *table;
};


class QControlsTable : public QTable 
{
public:
	QControlsTable ( QWidget * parent = 0, const char * name = 0 );
	QControlsTable ( int numRows, int numCols, QWidget * parent = 0, const char * name = 0 );
	void p_beginEdit( int row, int col );
	
	static QString name_caption; 	
	static QString type_caption; 	
	static QString value_caption;	
	static QString symbol_caption;
	static QString update_caption;
protected:
	void timerEvent ( QTimerEvent * te);
	QWidget * beginEdit ( int row, int col, bool replace );
	QControlTip * t;
};

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
#endif /* TACO_SILVIA_H */

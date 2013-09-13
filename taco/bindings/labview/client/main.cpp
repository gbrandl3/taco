/****************************************************************************
** $Id: main.cpp,v 1.1 2004-01-26 08:44:22 hgilde1 Exp $
**
** Copyright (c) 2003-2013 FRM-II, www.frm2.tum.de
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "TacoSilvia.h"
#include <qapplication.h>
#include <qstylefactory.h>


int main( int argc, char ** argv )
{
	 QApplication::setStyle(QStyleFactory::create("platinum"));
    QApplication a( argc, argv );
	 TacoSilvia ts;
    ts.setCaption("TACO SILVIA");
    a.setMainWidget( &ts );
    ts.show();
    return a.exec();
}


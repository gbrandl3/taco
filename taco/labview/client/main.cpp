/****************************************************************************
** $Id: main.cpp,v 1.1 2004-01-12 17:14:31 hgilde1 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
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


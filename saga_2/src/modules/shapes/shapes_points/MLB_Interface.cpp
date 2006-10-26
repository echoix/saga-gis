
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_points                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     volaya@ya.com                          //
//                                                       //
//    contact:    Victor Olaya Ferrero                   //
//                Madrid                                 //
//                Spain                                  //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general module library informations here...

const char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Shapes - Points") );

	case MLB_INFO_Author:
		return( _TL("Victor Olaya (c) 2004") );

	case MLB_INFO_Description:
		return( _TL("Tools for the manipulation of point vector data.") );

	case MLB_INFO_Version:
		return( "1.0" );

	case MLB_INFO_Menu_Path:
		return( _TL("Shapes|Points") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "Points_From_Table.h"
#include "Points_From_Lines.h"
#include "CountPoints.h"
#include "CreatePointGrid.h"
#include "DistanceMatrix.h"
#include "FitNPointsToShape.h"
#include "AddCoordinates.h"

//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	// Don't forget to continuously enumerate the case switches
	// when adding new modules! Also bear in mind that the
	// enumeration always has to start with [case 0:] and
	// that [default:] must return NULL!...

	CSG_Module	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CPoints_From_Table;
		break;

	case 1:
		pModule	= new CCountPoints;
		break;

	case 2:
		pModule	= new CCreatePointGrid;
		break;

	case 3:
		pModule	= new CDistanceMatrix;
		break;

	case 4:
		pModule	= new CFitNPointsToShape;
		break;

	case 5:
		pModule	= new CPoints_From_Lines;
		break;

	case 6:
		pModule	= new CAddCoordinates;
		break;

	default:
		pModule	= NULL;
		break;
	}

	return( pModule );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//{{AFX_SAGA

	MLB_INTERFACE

//}}AFX_SAGA

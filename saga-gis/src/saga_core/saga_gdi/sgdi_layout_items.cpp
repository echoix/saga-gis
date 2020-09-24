
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 sgdi_layout_items.cpp                 //
//                                                       //
//                 Copyright (C) 2020 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sgdi_layout_items.h"

#include "RectTracker.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSGDI_Layout_Items::CSGDI_Layout_Item::CSGDI_Layout_Item(void)
{
	m_pTracker	= NULL;
	m_Scale		= 1.;
	m_Raster	= 0;
	m_bShow		= true;
	m_bSizer	= true;
	m_Ratio		= 0.;
}

//---------------------------------------------------------
CSGDI_Layout_Items::CSGDI_Layout_Item::~CSGDI_Layout_Item(void)
{
	if( m_pTracker )
	{
		delete(m_pTracker);
	}
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::Set_Sizer(bool bOn)
{
	if( m_bSizer != bOn )
	{
		m_bSizer	= bOn;

		if( m_pTracker )
		{
			m_pTracker->SetHandlerMask(m_bSizer ? RT_MASK_ALL : RT_MASK_NONE);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::Set_Fixed(bool bOn)
{
	if( m_bFixed != bOn )
	{
		m_bFixed	= bOn;

		if( m_pTracker )
		{
		//	m_pTracker->SetFixedSize(m_bFixed ? RT_MASK_ALL : RT_MASK_NONE);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::Set_Ratio(double Ratio)
{
	if( m_Ratio != Ratio )
	{
		m_Ratio	= Ratio;

		if( m_pTracker )
		{
			m_pTracker->SetRatio(m_Ratio);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::Set_Rect(const wxRect &Rect)
{
	if( !Rect.IsEmpty() && !Compare(Rect, m_Rect) )
	{
		m_Rect	= Rect;

		return( _Tracker_Set_Rect(Rect) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Create(wxWindow *pParent)
{
	if( m_pTracker )
	{
		delete(m_pTracker);

		m_pTracker	= NULL;
	}

	if( pParent )
	{
		m_pTracker	= new wxRectTrackerRatio(pParent);

		m_pTracker->Disable();	// disabled by default

		_Tracker_Set_Rect(m_Rect);

		m_pTracker->SetHandlerMask(m_bSizer ? RT_MASK_ALL : RT_MASK_NONE);
		m_pTracker->SetMinRect(wxRect(0, 0, 10, 10));
	//	m_pTracker->SetGreyOutColour(50);
	//	m_pTracker->SetHandlesWidth ( 5);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Set_Scale(double Scale)
{
	if( Scale > 0. && Scale != m_Scale )
	{
		m_Scale	= Scale;

		_Tracker_Set_Rect(m_Rect);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Changed(void)
{
	if( m_pTracker && m_Scale > 0. )
	{
		wxRect	Rect(Get_Scaled(m_pTracker->GetTrackerRect(), 1. / m_Scale));

		if( !Rect.IsEmpty() )
		{
			if( m_Raster > 1 )
			{
				if( Rect.x != m_Rect.x )
				{
					Rect.x	= m_Raster * (int)(0.5 + Rect.x / (double)m_Raster);
				}
				else if( Rect.width != m_Rect.width )
				{
					int	w = m_Raster * (int)(0.5 + (Rect.x + Rect.width) / (double)m_Raster);

					Rect.width  = w - Rect.x;
				}

				if( Rect.y != m_Rect.y )
				{
					Rect.y	= m_Raster * (int)(0.5 + Rect.y / (double)m_Raster);
				}
				else if( Rect.height != m_Rect.height )
				{
					int	h = m_Raster * (int)(0.5 + (Rect.y + Rect.height) / (double)m_Raster);

					Rect.height  = h - Rect.y;
				}
			}

			if( !Rect.IsEmpty() && !Compare(Rect, m_Rect) )
			{
				m_Rect	= Rect;

				_Tracker_Set_Rect(Rect);

				return( true );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Enable(void)
{
	if( m_pTracker )
	{
		_Tracker_Set_Rect(m_Rect);

		if( m_pTracker->IsEnabled() == false )
		{
		//	double	d	= m_Rect.GetWidth() / (double)m_Rect.GetHeight();

		//	m_pTracker->SetRatio      (0);
		//	m_pTracker->SetRatio      (d);
		//	m_pTracker->SetOrientation(d > 1. ? 1 : -1);

			m_pTracker->Enable();
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Disable(void)
{
	if( m_pTracker && m_pTracker->IsEnabled() == true )
	{
		m_pTracker->Disable();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Refresh(wxWindow *pParent, bool bErase)
{
	if( pParent )
	{
		wxRect	rTracker(Get_Scaled(m_Rect, m_Scale));

		pParent->Refresh(bErase, &rTracker);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
inline bool CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Contains(const wxPoint &Point)
{
	wxRect	rTracker(Get_Scaled(m_Rect, m_Scale));

	return( rTracker.Contains(Point) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Set_Rect(const wxRect &Rect)
{
	if( m_pTracker && m_Scale > 0. )
	{
		wxRect	rTracker(Get_Scaled(Rect, m_Scale));

		if( !Compare(rTracker, m_pTracker->GetTrackerRect()) )
		{
			m_pTracker->SetTrackerRect(rTracker);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
wxRect CSGDI_Layout_Items::CSGDI_Layout_Item::_Tracker_Get_Rect(void)
{
	return( Get_Scaled(m_Rect, m_Scale) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSGDI_Layout_Items::CSGDI_Layout_Items(void)
{
	m_pParent	= NULL;
	m_pActive	= NULL;

	m_Scale		= 1.;
	m_Raster	= 0;
}

//---------------------------------------------------------
CSGDI_Layout_Items::~CSGDI_Layout_Items(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Create(wxWindow *pParent)
{
	Destroy();

	m_pParent	= pParent;

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Destroy(bool bDetachItems)
{
	m_pActive	= NULL;

	if( bDetachItems == false )
	{
		for(size_t i=0; i<m_Items.Get_Size(); i++)
		{
			delete(Get_Item(i));
		}
	}

	m_Items.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxRect CSGDI_Layout_Items::Get_Scaled(const wxRect &Rect, double Scale)
{
	return( Scale == 1. ? Rect : wxRect(
		(int)(0.5 + Scale * Rect.x     ),
		(int)(0.5 + Scale * Rect.y     ),
		(int)(0.5 + Scale * Rect.width ),
		(int)(0.5 + Scale * Rect.height))
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::Set_Parent(wxWindow *pParent)
{
	if( m_pParent != pParent )
	{
		m_pParent	= pParent;

		for(size_t i=0; i<m_Items.Get_Size(); i++)
		{
			Get_Item(i)->_Tracker_Create(pParent);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Set_Raster(int Raster)
{
	m_Raster	= Raster;

	for(size_t i=0; i<m_Items.Get_Size(); i++)
	{
		Get_Item(i)->m_Raster	= m_Raster;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::Add(CSGDI_Layout_Item *pItem, const wxRect &Rect, bool bActivate)
{
	if( pItem )
	{
		pItem->m_Rect   = Rect;
		pItem->m_Raster = m_Raster;
		pItem->_Tracker_Set_Scale(m_Scale);
		pItem->_Tracker_Create(m_pParent);

		m_Items.Add(pItem);

		if( bActivate )
		{
			if( m_pActive && m_pActive->_Tracker_Disable() )
			{
				m_pActive->_Tracker_Refresh(m_pParent);
			}

			m_pActive	= pItem;

			if( m_pActive && m_pActive->_Tracker_Enable() )
			{
				m_pActive->_Tracker_Refresh(m_pParent);
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Add(CSGDI_Layout_Item *pItem, bool bActivate)
{
	return( pItem && Add(pItem, pItem->m_Rect, bActivate) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Del(CSGDI_Layout_Item *pItem, bool bDetachOnly)
{
	for(size_t i=0; i<m_Items.Get_Size(); i++)
	{
		if( pItem == m_Items[i] )
		{
			return( Del(i, bDetachOnly) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Del(size_t Index, bool bDetachOnly)
{
	if( Index < m_Items.Get_Size() )
	{
		CSGDI_Layout_Item	*pItem	= Get_Item(Index);

		if( m_pActive == pItem )
		{
			m_pActive	= NULL;
		}

		m_Items.Del(Index);

		pItem->_Tracker_Refresh(m_pParent);

		if( !bDetachOnly )
		{
			delete(pItem);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::Scale(double Scale)
{
	if( Scale > 0. && Scale != m_Scale )
	{
		m_Scale	= Scale;

		for(size_t i=0; i<m_Items.Get_Size(); i++)
		{
			Get_Item(i)->_Tracker_Set_Scale(m_Scale);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSGDI_Layout_Items::Get_Position(CSGDI_Layout_Item *pItem)	const
{
	if( pItem )
	{
		for(size_t i=0; i<m_Items.Get_Size(); i++)
		{
			if( pItem == m_Items[i] )
			{
				return( i );
			}
		}
	}

	return( m_Items.Get_Size() );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::is_Top(CSGDI_Layout_Item *pItem)	const
{
	return( Get_Position(pItem) == m_Items.Get_Size() - 1 );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::is_Bottom(CSGDI_Layout_Item *pItem)	const
{
	return( Get_Position(pItem) == 0 );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Move_Top(CSGDI_Layout_Item *pItem)
{
	size_t	Position	= Get_Position(pItem);

	if( Position < m_Items.Get_Size() - 1 )
	{
		for(size_t i=Position; i<m_Items.Get_Size() - 1; i++)
		{
			m_Items[i]	= m_Items[i + 1];
		}

		m_Items[m_Items.Get_Size() - 1]	= pItem;

		pItem->_Tracker_Refresh(m_pParent);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Move_Bottom(CSGDI_Layout_Item *pItem)
{
	size_t	Position	= Get_Position(pItem);

	if( Position > 0 )
	{
		for(size_t i=Position; i>0; i--)
		{
			m_Items[i]	= m_Items[i - 1];
		}

		m_Items[0]	= pItem;

		pItem->_Tracker_Refresh(m_pParent);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Move_Up(CSGDI_Layout_Item *pItem)
{
	size_t	Position	= Get_Position(pItem);

	if( Position < m_Items.Get_Size() - 1 )
	{
		m_Items[Position    ]	= m_Items[Position + 1];
		m_Items[Position + 1]	= pItem;

		pItem->_Tracker_Refresh(m_pParent);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Move_Down(CSGDI_Layout_Item *pItem)
{
	size_t	Position	= Get_Position(pItem);

	if( Position > 0 )
	{
		m_Items[Position    ]	= m_Items[Position - 1];
		m_Items[Position - 1]	= pItem;

		pItem->_Tracker_Refresh(m_pParent);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
size_t CSGDI_Layout_Items::Active_Get_Position(void)	const
{
	return( Get_Position(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_is_Top(void)	const
{
	return( is_Top(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_is_Bottom(void)	const
{
	return( is_Bottom(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_Move_Top(void)
{
	return( Move_Top(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_Move_Bottom(void)
{
	return( Move_Bottom(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_Move_Up(void)
{
	return( Move_Up(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_Move_Down(void)
{
	return( Move_Up(m_pActive) );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Active_Properties(void)
{
	if( m_pActive && m_pActive->Properties(m_pParent) )
	{
		if( m_pParent )
		{
			m_pParent->Refresh();
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::Hide(CSGDI_Layout_Item *pItem)
{
	if( pItem->is_Shown() )
	{
		if( m_pActive == pItem )
		{
			if( m_pActive->_Tracker_Disable() )
			{
				m_pActive->_Tracker_Refresh(m_pParent);
			}

			m_pActive	= NULL;
		}

		pItem->m_bShow	= false;
	}

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Show(CSGDI_Layout_Item *pItem)
{
	if( !pItem->is_Shown() )
	{
		if( m_pActive == pItem && m_pActive->_Tracker_Enable() )
		{
			m_pActive->_Tracker_Refresh(m_pParent);
		}

		pItem->m_bShow	= true;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Layout_Items::Draw(wxDC &dc)
{
	for(size_t i=0; i<m_Items.Get_Size(); i++)
	{
		if( Get_Item(i)->is_Shown() )
		{
			Get_Item(i)->Draw(dc);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::On_Tracker_Changed(void)
{
	if( m_pActive && m_pActive->_Tracker_Changed() )
	{
		m_pParent->Refresh(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::On_Key_Event(wxKeyEvent &event)
{
	if( m_pActive )
	{
		if( event.GetKeyCode() == WXK_RETURN )
		{
			if( Active_Properties() )
			{
				m_pActive->_Tracker_Refresh(m_pParent);
			}

			return( true );
		}

		//-------------------------------------------------
		wxRect	r(m_pActive->m_Rect), rMoved(m_pActive->m_Rect);

		int	Move	= event.ControlDown() ? 1 : !event.ShiftDown() ? 5 : 20;

		switch( event.GetKeyCode() )
		{
		case WXK_LEFT : rMoved.Offset(-Move,     0); break;
		case WXK_RIGHT: rMoved.Offset( Move,     0); break;
		case WXK_UP   : rMoved.Offset(    0, -Move); break;
		case WXK_DOWN : rMoved.Offset(    0,  Move); break;
		}

		if( !Compare(r, rMoved) )
		{
			m_pActive->Set_Rect(rMoved);

			r.Union(rMoved); r = Get_Scaled(r, m_Scale); r.Inflate(2);

			m_pParent->Refresh(true, &r);

			return( true );
		}
	}

	//-----------------------------------------------------
	event.Skip();

	return( false );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::On_Mouse_Event(wxMouseEvent &event)
{
	wxPoint	p(event.GetPosition());

	if( wxDynamicCast(m_pParent, wxScrolledWindow) )
	{
		p	= wxDynamicCast(m_pParent, wxScrolledWindow)->CalcUnscrolledPosition(p);
	}

	//-----------------------------------------------------
	if     ( event.LeftDown  () )
	{
		Select(p, true);
	}
	else if( event.LeftUp    () )
	{
		Select(p, false);
	}
	else if( event.RightUp   () )
	{
		Select(p, true);
		Select(p, false);
	}
	else if( event.LeftDClick() )
	{
		Select(p, true);
		Select(p, false);

		if( m_pActive && Active_Properties() )
		{
			m_pActive->_Tracker_Refresh(m_pParent);
		}
	}
	else
	{
		event.Skip();
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CSGDI_Layout_Items::Select(const wxPoint &p, bool bDown)
{
	if( bDown )
	{
		if( m_pActive )
		{
			switch( m_pActive->m_pTracker->HitTest(p.x, p.y) )
			{
			default:
				return( true );

			case RT_HANDLER_OUTSIDE:
			case RT_HANDLER_NONE:
				break;
			}
		}

		//-----------------------------------------------------
		CSGDI_Layout_Item	*pActivate	= NULL;

		for(size_t i=0, j=m_Items.Get_Size()-1; !pActivate && i<m_Items.Get_Size(); i++, j--)
		{
			if( Get_Item(j)->is_Shown() && Get_Item(j)->_Tracker_Contains(p) )
			{
				pActivate	= Get_Item(j);
			}
		}

		//-----------------------------------------------------
		if( pActivate == NULL || pActivate != m_pActive )
		{
			if( m_pActive )
			{
				m_pActive->_Tracker_Disable();
			}

			m_pActive	= NULL;
		}

		if( pActivate != NULL )
		{
			m_pActive	= pActivate;
		}
	}
	else
	{
		if( m_pActive )
		{
			if( m_pActive->_Tracker_Contains(p) )
			{
				if( m_pActive->_Tracker_Enable() )
				{
					m_pActive->_Tracker_Refresh(m_pParent);
				}
			}
			else
			{
				m_pActive	= NULL;
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

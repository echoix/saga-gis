
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 statistics_regression                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                gw_multi_regression.cpp                //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#include "gw_multi_regression.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGW_Multi_Regression::CGW_Multi_Regression(void)
{
	Set_Name		(_TL("GWR for Multiple Predictors (Gridded Model Output)"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Geographically Weighted Regression for multiple predictors. "
		"Predictors have to be supplied as attributes of ingoing points data. "
		"Regression model parameters are generated as continuous fields, i.e. as grids. "
	));

	GWR_Add_References(false);

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"DEPENDENT"	, _TL("Dependent Variable"),
		_TL("")
	);

	Parameters.Add_Table_Fields("POINTS",
		"PREDICTORS", _TL("Predictors"),
		_TL("")
	);

	Parameters.Add_Shapes("",
		"REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Bool("",
		"LOGISTIC"	, _TL("Logistic Regression"),
		_TL(""),
		false
	);

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false, "", "TARGET_");

	m_Grid_Target.Add_Grid("QUALITY"  , _TL("Quality"  ), false);
	m_Grid_Target.Add_Grid("INTERCEPT", _TL("Intercept"), false);

	Parameters.Add_Grid_List("",
		"SLOPES"		, _TL("Slopes"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	m_Weighting.Set_Weighting(SG_DISTWGHT_GAUSS);
	m_Weighting.Create_Parameters(Parameters);

	//-----------------------------------------------------
	m_Search.Create(&Parameters, "NODE_SEARCH", 16);

	Parameters("SEARCH_RANGE"     )->Set_Value(1);
	Parameters("SEARCH_POINTS_ALL")->Set_Value(1);

	//-----------------------------------------------------
	m_iPredictor	= NULL;
	m_pSlopes		= NULL;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGW_Multi_Regression::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("POINTS") )
	{
		m_Grid_Target.Set_User_Defined(pParameters, pParameter->asShapes());

		m_Search.On_Parameter_Changed(pParameters, pParameter);

		pParameters->Set_Parameter("DW_BANDWIDTH", GWR_Fit_To_Density(pParameter->asShapes(), 4.0, 1));
	}

	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGW_Multi_Regression::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Weighting.Enable_Parameters(*pParameters);

	m_Search     .On_Parameters_Enable(pParameters, pParameter);
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression::Initialize(void)
{
	CSG_Parameter_Table_Fields	*pFields	= Parameters("PREDICTORS")->asTableFields();

	if( (m_nPredictors = pFields->Get_Count()) > 0 )
	{
		m_iPredictor	= new int[m_nPredictors];

		for(int i=0; i<m_nPredictors; i++)
		{
			m_iPredictor[i]	= pFields->Get_Index(i);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CGW_Multi_Regression::Finalize(void)
{
	SG_DELETE_ARRAY(m_iPredictor);
	SG_FREE_SAFE   (m_pSlopes);

	m_Search.Finalize();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression::On_Execute(void)
{
	//-----------------------------------------------------
	m_pPoints		= Parameters("POINTS"   )->asShapes();
	m_iDependent	= Parameters("DEPENDENT")->asInt   ();

	//-----------------------------------------------------
	if( !Initialize() )
	{
		Finalize();

		return( false );
	}

	if( !m_Search.Initialize(m_pPoints, -1) )
	{
		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	m_Weighting.Set_Parameters(Parameters);

	m_pQuality		= m_Grid_Target.Get_Grid("QUALITY"  );
	m_pIntercept	= m_Grid_Target.Get_Grid("INTERCEPT");

	if( !m_pQuality || !m_pIntercept )
	{
		Finalize();

		return( false );
	}

	m_pQuality  ->Fmt_Name("%s (%s)", Parameters("DEPENDENT")->asString(), _TL("GWR Quality"  ));
	m_pIntercept->Fmt_Name("%s (%s)", Parameters("DEPENDENT")->asString(), _TL("GWR Intercept"));

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pSlopes	= Parameters("SLOPES")->asGridList();

	m_pSlopes	= (CSG_Grid **)SG_Calloc(m_nPredictors, sizeof(CSG_Grid *));

	for(int i=0; i<m_nPredictors; i++)
	{
		pSlopes->Add_Item(m_pSlopes[i] = SG_Create_Grid(m_pQuality->Get_System()));

		m_pSlopes[i]->Fmt_Name("%s (%s)", Parameters("DEPENDENT")->asString(), m_pPoints->Get_Field_Name(m_iPredictor[i]));
	}

	//-----------------------------------------------------
	bool	bLogistic	= Parameters("LOGISTIC")->asBool();

	for(int y=0; y<m_pIntercept->Get_NY() && Set_Progress(y, m_pIntercept->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<m_pIntercept->Get_NX(); x++)
		{
			CSG_Regression_Weighted	Model;

			if( Get_Model(x, y, Model, bLogistic) )
			{
				m_pQuality  ->Set_Value(x, y, Model.Get_R2());
				m_pIntercept->Set_Value(x, y, Model[0]);

				for(int i=0; i<m_nPredictors; i++)
				{
					m_pSlopes[i]->Set_Value(x, y, Model[i + 1]);
				}
			}
			else
			{
				m_pQuality  ->Set_NoData(x, y);
				m_pIntercept->Set_NoData(x, y);

				for(int i=0; i<m_nPredictors; i++)
				{
					m_pSlopes[i]->Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGW_Multi_Regression::Get_Model(int x, int y, CSG_Regression_Weighted &Model, bool bLogistic)
{
	Model.Destroy(); TSG_Point Point = m_pIntercept->Get_System().Get_Grid_to_World(x, y); CSG_Vector Predictors(m_nPredictors);

	//-----------------------------------------------------
	if( m_Search.Do_Use_All() )
	{
		for(sLong iPoint=0; iPoint<m_pPoints->Get_Count(); iPoint++)
		{
			CSG_Shape *pPoint = m_pPoints->Get_Shape(iPoint);

			if( !pPoint->is_NoData(m_iDependent) )
			{
				bool bOkay = true;

				for(int iPredictor=0; iPredictor<m_nPredictors && bOkay; iPredictor++)
				{
					if( (bOkay = !pPoint->is_NoData(m_iPredictor[iPredictor])) == true )
					{
						Predictors[iPredictor] = pPoint->asDouble(m_iPredictor[iPredictor]);
					}
				}

				if( bOkay )
				{
					Model.Add_Sample(m_Weighting.Get_Weight(SG_Get_Distance(Point, pPoint->Get_Point(0))),
						pPoint->asDouble(m_iDependent), Predictors
					);
				}
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Array_Int Index; CSG_Vector Distance;

		if( !m_Search.Get_Points(Point, Index, Distance) )
		{
			return( false );
		}

		for(sLong iPoint=0; iPoint<Index.Get_Size(); iPoint++)
		{
			CSG_Shape *pPoint = m_pPoints->Get_Shape(Index[iPoint]);

			if( !pPoint->is_NoData(m_iDependent) )
			{
				bool bOkay = true;

				for(int iPredictor=0; iPredictor<m_nPredictors && bOkay; iPredictor++)
				{
					if( (bOkay = !pPoint->is_NoData(m_iPredictor[iPredictor])) == true )
					{
						Predictors[iPredictor] = pPoint->asDouble(m_iPredictor[iPredictor]);
					}
				}

				if( bOkay )
				{
					Model.Add_Sample(m_Weighting.Get_Weight(Distance[iPoint]),
						pPoint->asDouble(m_iDependent), Predictors
					);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( Model.Calculate(bLogistic) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

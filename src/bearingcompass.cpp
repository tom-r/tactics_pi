/***************************************************************************
* $Id: bearingcompass.cpp, v1.0 2016/06/07 tom_BigSpeedy Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Thomas Rauch
*     
***************************************************************************
*   Copyright (C) 2010 by David S. Register                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
***************************************************************************
*/

#include "bearingcompass.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "wx/tokenzr.h"
#include "performance.h"
extern int g_iMinLaylineWidth;
extern int g_iMaxLaylineWidth;
extern Polar* BoatPolar;
extern PlugIn_Waypoint *m_pMark;
extern int g_iDashDistanceUnit;
extern int g_iDashSpeedUnit;

extern double g_dalphaDeltCoG;
extern double  getDegRange(double max, double min);
/***************************************************************************************
****************************************************************************************/
TacticsInstrument_BearingCompass::TacticsInstrument_BearingCompass(wxWindow *parent, wxWindowID id, wxString title, int cap_flag) :
TacticsInstrument_Dial(parent, id, title, cap_flag, 0, 360, 0, 360)
{
	SetOptionMarker(5, DIAL_MARKER_SIMPLE, 2);
	SetOptionLabel(20, DIAL_LABEL_ROTATED);
	SetOptionMainValue(_T("%.0f"), DIAL_POSITION_INSIDE);
	//    Get a pointer to the opencpn configuration object

	m_pconfig = GetOCPNConfigObject();

	LoadConfig();
	m_Bearing = NAN;
    m_lat = NAN;
    m_lon = NAN;
	m_CurrDir = NAN;
	m_CurrSpeed = NAN;
	m_ExtraValueDTW = NAN;
    m_MainValue = NAN;
	m_Leeway = 0;
	m_AngleStart = 0;
    m_ExpSmoothDegRange = 0;
    mExpSmDegRange = new ExpSmooth(g_dalphaDeltCoG);
	m_Cog = -999;
	m_Hdt = -999;
	m_diffCogHdt = 0;
	m_predictedSog = NAN;
	m_TWA = NAN;
	m_AWA = -999;
	m_TWS = NAN;
    m_TWD = NAN;
    m_StW = 0.0;
    m_ToWpt = _T("---");
	alpha_diffCogHdt = 0.1;
	m_ExpSmoothDiffCogHdt = 0;
	m_oldExpSmoothDiffCogHdt = 0;
	for (int i = 0; i < COGRANGE; i++) m_COGRange[i] = 0;
}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_BearingCompass::SetData(int st, double data, wxString unit)
{
	if (st == OCPN_DBP_STC_COG) {
		m_Cog = data;
	}
	else if (st == OCPN_DBP_STC_HDT) {
		m_AngleStart = -data; 
		m_MainValue = data; 
		m_MainValueUnit = unit;
		m_Hdt = data;
	}
	else if (st == OCPN_DBP_STC_CURRDIR) {
		m_CurrDir = data;
		m_CurrDirUnit = unit;
	}
	else if (st == OCPN_DBP_STC_CURRSPD) {
		m_CurrSpeed = data;
		m_CurrSpeedUnit = unit;
	}

	else if (st == OCPN_DBP_STC_DTW) {
		if (!GetSingleWaypoint(_T("TacticsWP"), m_pMark)){
			m_ExtraValueDTW = data;
			m_ExtraValueDTWUnit = unit;
		}
	}
	else if (st == OCPN_DBP_STC_TWA) {
		m_curTack = unit;
		m_TWA = data;
	}
    else if (st == OCPN_DBP_STC_TWD) {
      m_TWD = data;
    }
    else if (st == OCPN_DBP_STC_AWA) {
		m_AWA = data;
	}
	else if (st == OCPN_DBP_STC_TWS) {
		m_TWS = data;
	}

	else if (st == OCPN_DBP_STC_LEEWAY) {
		m_Leeway = data;
        m_LeewayUnit = unit;
	}
	else if (st == OCPN_DBP_STC_LAT) {
		m_lat = data;
	}
	else if (st == OCPN_DBP_STC_LON) {
		m_lon = data;
	}
	else if (st == OCPN_DBP_STC_STW) {
		m_StW = data;
	}

	if (m_Cog != -999 && m_Hdt != -999){
		m_diffCogHdt = m_Cog - m_Hdt;
    }
	if (st == OCPN_DBP_STC_BRG) {
		//if (!GetSingleWaypoint(_T("TacticsWP"), m_pMark)){
			m_Bearing = data;
			m_ToWpt = unit;
		/*}
		else{
			if (m_pMark) {
				double dist;
				DistanceBearingMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, m_lat, m_lon, &m_Bearing, &dist);
				m_ToWpt = _T("TacticsWP");
				m_ExtraValueDTW = toUsrDistance_Plugin(dist, g_iDashDistanceUnit);
				m_ExtraValueDTWUnit = getUsrDistanceUnit_Plugin(g_iDashDistanceUnit);
			}
		}*/
		m_BearingUnit = _T("\u00B0");
	}
    if (!GetSingleWaypoint(_T("TacticsWP"), m_pMark)) m_pMark = NULL;
    if (m_pMark && !wxIsNaN(m_lat) && !wxIsNaN(m_lon)) {
      double dist;
      DistanceBearingMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, m_lat, m_lon, &m_Bearing, &dist);
      m_ToWpt = _T("TacticsWP");
      m_ExtraValueDTW = toUsrDistance_Plugin(dist, g_iDashDistanceUnit);
      m_ExtraValueDTWUnit = getUsrDistanceUnit_Plugin(g_iDashDistanceUnit);
      m_BearingUnit = _T("\u00B0");
    }
    if (!m_pMark && wxIsNaN(m_Bearing)){
      m_ToWpt = _T("---");
      m_ExtraValueDTW = NAN;
      m_predictedSog = NAN;
      m_ExtraValueDTWUnit = getUsrDistanceUnit_Plugin(g_iDashDistanceUnit);
      m_BearingUnit = _T("\u00B0");
    }
	CalculateLaylineDegreeRange();
}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_BearingCompass::Draw(wxGCDC* bdc)
{
	wxColour c1;
	GetGlobalColor(_T("DASHB"), &c1);
	wxBrush b1(c1);
	bdc->SetBackground(b1);
	bdc->Clear();

	wxSize size = GetClientSize();
	m_cx = size.x / 2;
	int availableHeight = size.y - m_TitleHeight - 6;
	int width, height;
	bdc->GetTextExtent(_T("000"), &width, &height, 0, 0, g_pFontLabel);
	m_cy = m_TitleHeight + 2;
	m_cy += availableHeight / 2;
	m_radius = availableHeight / 2 *0.80;


	DrawLabels(bdc);
	DrawFrame(bdc);
	DrawMarkers(bdc);
	DrawBackground(bdc);
    if (!wxIsNaN(m_Bearing)){
      DrawData(bdc, m_Bearing, m_BearingUnit, _T("BRG:%.f"), DIAL_POSITION_TOPLEFT);
      DrawData(bdc, 0, m_ToWpt, _T(""), DIAL_POSITION_TOPRIGHT);
    }
    if (!wxIsNaN(m_CurrSpeed)) DrawData(bdc, m_CurrSpeed, m_CurrSpeedUnit, _T("Curr:%.2f"), DIAL_POSITION_INSIDE);
    if (!wxIsNaN(m_ExtraValueDTW)) DrawData(bdc, m_ExtraValueDTW, m_ExtraValueDTWUnit, _T("DTW:%.1f"), DIAL_POSITION_BOTTOMLEFT);
    if (!wxIsNaN(m_CurrDir) && m_CurrDir >= 0 && m_CurrDir < 360)
		DrawCurrent(bdc);
	DrawForeground(bdc);

	DrawLaylines(bdc);
    if (!wxIsNaN(m_MainValue)) DrawData(bdc, m_MainValue, m_MainValueUnit, _T("%.0f"), DIAL_POSITION_TOPINSIDE);

    if (!wxIsNaN(m_predictedSog)) DrawData(bdc, m_predictedSog, getUsrSpeedUnit_Plugin(g_iDashSpeedUnit), _T("prd.SOG: ~%.1f"), DIAL_POSITION_BOTTOMRIGHT);


}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_BearingCompass::DrawBackground(wxGCDC* dc)
{
    DrawCompassRose( dc, m_cx, m_cy, 0.7 * m_radius, m_AngleStart, true );
	DrawBoat(dc, m_cx, m_cy, m_radius);
}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_BearingCompass::DrawWindAngles(wxGCDC* dc)
{
	// draw the wind needles for AWA and TWA. We don't use the standard ones, as they are to big.
	// 
	// True Wind Angle first (it's in the background then)
	if (!wxIsNaN(m_TWA)) {
		wxColour cl;
		GetGlobalColor(_T("DASH2"), &cl);
		wxPen pen1;
		pen1.SetStyle(wxSOLID);
		pen1.SetColour(cl);
		pen1.SetWidth(2);
		dc->SetPen(pen1);
		GetGlobalColor(_T("DASH1"), &cl);
		wxBrush brush1;
		brush1.SetStyle(wxSOLID);
		brush1.SetColour(cl);
		dc->SetBrush(brush1);

		dc->SetPen(*wxTRANSPARENT_PEN);

		GetGlobalColor(_T("BLUE3"), &cl);
		wxBrush brush;
		brush.SetStyle(wxSOLID);
		brush.SetColour(cl);
		dc->SetBrush(brush);

		/* this is fix for a +/-180° round instrument, when m_MainValue is supplied as <0..180><L | R>
		* for example TWA & AWA */
		double data, TwaCog;
		// head-up = COG, but TWA is based on Hdt --> add the diff here for a correct display
		//TwaCog = m_TWA - m_diffCogHdt; //alt: für COG
		TwaCog = m_TWA; //neu, jetzt HDt 

		/* this is fix for a +/-180° round instrument, when m_MainValue is supplied as <0..180><L | R>
		* for example TWA & AWA */
		if (m_curTack == _T("\u00B0L"))
			data = 360 - TwaCog;
		else
			data = TwaCog;

		// The arrow should stay inside fixed limits
		double val;
		if (data < m_MainValueMin)
			val = m_MainValueMin;
		else if (data > m_MainValueMax)
			val = m_MainValueMax;
		else
			val = data;

		double value = deg2rad((val - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);
		double value1 = deg2rad((val + 5 - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);
		double value2 = deg2rad((val - 5 - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);

		/*
		*           0
		*          /\
		*         /  \
		*        /    \
		*     2 /_ __ _\ 1
		*
		*           X
		*/
		wxPoint points[4];
		points[0].x = m_cx + (m_radius * 0.95 * cos(value));
		points[0].y = m_cy + (m_radius * 0.95 * sin(value));
		points[1].x = m_cx + (m_radius * 0.75 * cos(value1));
		points[1].y = m_cy + (m_radius * 0.75 * sin(value1));
		points[2].x = m_cx + (m_radius * 0.75 * cos(value2));
		points[2].y = m_cy + (m_radius * 0.75 * sin(value2));
		dc->DrawPolygon(3, points, 0, 0);

		wxPoint brg[2];
		brg[0].x = m_cx;
		brg[0].y = m_cy;
		brg[1].x = m_cx + (m_radius * 0.75 * cos(value));
		brg[1].y = m_cy + (m_radius * 0.75 * sin(value));

		wxPen pen2;
		pen2.SetStyle(wxSOLID);
		pen2.SetColour(cl);
		pen2.SetWidth(1);
		dc->SetPen(pen2);

		dc->DrawLine(brg[0], brg[1]);
		dc->SetPen(*wxTRANSPARENT_PEN);
	}
	// now almost the same for AWA
	if (m_AWA != -999) {
		wxColour cl;
		GetGlobalColor(_T("DASH2"), &cl);
		wxPen pen1;
		pen1.SetStyle(wxSOLID);
		pen1.SetColour(cl);
		pen1.SetWidth(2);
		dc->SetPen(pen1);
		GetGlobalColor(_T("DASH1"), &cl);
		wxBrush brush1;
		brush1.SetStyle(wxSOLID);
		brush1.SetColour(cl);
		dc->SetBrush(brush1);

		dc->SetPen(*wxTRANSPARENT_PEN);

		GetGlobalColor(_T("DASHN"), &cl);
		wxBrush brush;
		brush.SetStyle(wxSOLID);
		brush.SetColour(cl);
		dc->SetBrush(brush);

		double data;
		/* this is fix for a +/-180° round instrument, when m_MainValue is supplied as <0..180><L | R>
		* for example TWA & AWA */
		if (m_curTack == _T("\u00B0L"))
			data = 360 - m_AWA;
		else
			data = m_AWA;

		// The arrow should stay inside fixed limits
		double val;
		if (data < m_MainValueMin)
			val = m_MainValueMin;
		else if (data > m_MainValueMax)
			val = m_MainValueMax;
		else
			val = data;

		double value = deg2rad((val - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);
		double value1 = deg2rad((val + 5 - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);
		double value2 = deg2rad((val - 5 - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);

		/*
		*           0
		*          /\
		*         /  \
		*        /    \
		*     2 /_ __ _\ 1
		*
		*           X
		*/
		wxPoint points[4];
		points[0].x = m_cx + (m_radius * 0.95 * cos(value));
		points[0].y = m_cy + (m_radius * 0.95 * sin(value));
		points[1].x = m_cx + (m_radius * 0.75 * cos(value1));
		points[1].y = m_cy + (m_radius * 0.75 * sin(value1));
		points[2].x = m_cx + (m_radius * 0.75 * cos(value2));
		points[2].y = m_cy + (m_radius * 0.75 * sin(value2));
		dc->DrawPolygon(3, points, 0, 0);

		wxPoint brg[2];
		brg[0].x = m_cx;
		brg[0].y = m_cy;
		brg[1].x = m_cx + (m_radius * 0.75 * cos(value));
		brg[1].y = m_cy + (m_radius * 0.75 * sin(value));

		wxPen pen2;
		pen2.SetStyle(wxSOLID);
		pen2.SetColour(cl);
		pen2.SetWidth(1);
		dc->SetPen(pen2);

		dc->DrawLine(brg[0], brg[1]);
		dc->SetPen(*wxTRANSPARENT_PEN);
	}
}
/***************************************************************************************
Draw pointers for the optimum target VMG- and CMG Angle (if bearing is available)
****************************************************************************************/
void TacticsInstrument_BearingCompass::DrawTargetxMGAngle(wxGCDC* dc){
  if (!wxIsNaN(m_TWS)) {
    // get Target VMG Angle from Polar
    TargetxMG tvmg_up = BoatPolar->GetTargetVMGUpwind(m_TWS);
    TargetxMG tvmg_dn = BoatPolar->GetTargetVMGDownwind(m_TWS);
    TargetxMG TCMGMax;
    TargetxMG TCMGMin;

    if (tvmg_up.TargetAngle > 0){
      DrawTargetAngle(dc, m_curTack == _T("\u00B0L") ? 360 - tvmg_up.TargetAngle : tvmg_up.TargetAngle, _T("BLUE3"), 2);
    }
    if (tvmg_dn.TargetAngle > 0) {
      DrawTargetAngle(dc, m_curTack == _T("\u00B0L") ? 360 - tvmg_dn.TargetAngle : tvmg_dn.TargetAngle, _T("BLUE3"), 2);
    }
    if (!wxIsNaN(m_Bearing)){
      if (m_Bearing >= 0 && m_Bearing < 360 && !wxIsNaN(m_TWD)){
        //       TargetxMG tcmg = BoatPolar->Calc_TargetCMG(m_TWS, m_TWD, m_Bearing);
        BoatPolar->Calc_TargetCMG2(m_TWS, m_TWD, m_Bearing, &TCMGMax, &TCMGMin);
        if (!wxIsNaN(TCMGMax.TargetAngle))      DrawTargetAngle(dc, 360 - TCMGMax.TargetAngle, _T("URED"), 2);
        if (!wxIsNaN(TCMGMin.TargetAngle))      DrawTargetAngle(dc, 360 - TCMGMin.TargetAngle, _T("URED"), 1);
      }
    }
  }
}
/***************************************************************************************
Draw pointers for the optimum target VMG- and CMG Angle (if bearing is available)
****************************************************************************************/
void TacticsInstrument_BearingCompass::DrawTargetAngle(wxGCDC* dc, double TargetAngle, wxString color, int size){
    if (TargetAngle > 0){
      wxColour cl;
      dc->SetPen(*wxTRANSPARENT_PEN);
      GetGlobalColor(color, &cl);
      wxBrush brush;
      brush.SetStyle(wxSOLID);
      brush.SetColour(cl);
      dc->SetBrush(brush);

      /* this is fix for a +/-180° round instrument, when m_MainValue is supplied as <0..180><L | R>
      * for example TWA & AWA */
      double data, TwaCog;
      // head-up = COG, but TWA is based on Hdt --> add the diff here for a correct display
      TwaCog = TargetAngle;


      /* this is fix for a +/-180° round instrument, when m_MainValue is supplied as <0..180><L | R>
      * for example TWA & AWA */
      /*if (m_curTack == _T("\u00B0L"))
        data = 360 - TwaCog;
      else*/
        data = TwaCog;

      // The arrow should stay inside fixed limits
      double val;
      if (data < m_MainValueMin)
        val = m_MainValueMin;
      else if (data > m_MainValueMax)
        val = m_MainValueMax;
      else
        val = data;
      double sizefactor, widthfactor;
      if (size == 1) {
        sizefactor = 0.935;
        widthfactor = 0.90;
      }
      else{
        sizefactor = 1;
        widthfactor = 1;
      }

      double value = deg2rad((val - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);
      double value1 = deg2rad((val + 5*widthfactor - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);
      double value2 = deg2rad((val - 5 * widthfactor - m_MainValueMin) * m_AngleRange / (m_MainValueMax - m_MainValueMin)) + deg2rad(0 - ANGLE_OFFSET);

      /*
      *           0
      *          /\
      *         /  \
      *        /    \
      *     2 /_ __ _\ 1
      *
      *           X
      */
      wxPoint points[4];
      points[0].x = m_cx + (m_radius * 0.95 * cos(value));
      points[0].y = m_cy + (m_radius * 0.95 * sin(value));
      points[1].x = m_cx + (m_radius * 1.15*sizefactor * cos(value1));
      points[1].y = m_cy + (m_radius * 1.15*sizefactor * sin(value1));
      points[2].x = m_cx + (m_radius * 1.15*sizefactor * cos(value2));
      points[2].y = m_cy + (m_radius * 1.15*sizefactor * sin(value2));
      dc->DrawPolygon(3, points, 0, 0);
    //}
  }
}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_BearingCompass::DrawForeground(wxGCDC* dc)
{
	if (!wxIsNaN(m_Bearing))  
		DrawBearing(dc);
    if (!wxIsNaN(m_TWS) && !wxIsNaN(m_TWA)) {
      //DrawPolar(dc);
      DrawWindAngles(dc);
      DrawTargetxMGAngle(dc);
    }
}
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_BearingCompass::DrawBearing(wxGCDC* dc)
{
	wxColour cl;
	// no border for the circle 
	dc->SetPen(*wxTRANSPARENT_PEN);

	GetGlobalColor(_T("URED"), &cl);
	wxBrush brush;
	brush.SetStyle(wxSOLID);
	brush.SetColour(cl);
	dc->SetBrush(brush);

	double value = deg2rad(m_Bearing) + deg2rad(m_AngleStart - ANGLE_OFFSET);

	dc->DrawCircle(m_cx + (m_radius * 0.8 * cos(value)), m_cy + (m_radius * 0.8 * sin(value)), m_radius / 16);
	wxPoint brg[2];
	brg[0].x = m_cx + (m_radius * 0.8 * cos(value));
	brg[0].y = m_cy + (m_radius * 0.8 * sin(value));
	brg[1].x = m_cx + (m_radius * 0.98 * cos(value));
	brg[1].y = m_cy + (m_radius * 0.98 * sin(value));

	wxPen pen2;
	pen2.SetStyle(wxSOLID);
	pen2.SetColour(cl);
	pen2.SetWidth(2);
	dc->SetPen(pen2);

	dc->DrawLine(brg[0],brg[1]);
	dc->SetPen(*wxTRANSPARENT_PEN);

}
/***************************************************************************************
****************************************************************************************/
/*void TacticsInstrument_BearingCompass::DrawPolar(wxGCDC*dc)
{
  if (!wxIsNaN(m_TWS)) {
    wxColour cl;
    GetGlobalColor(_T("UBLCK"), &cl);
    wxPen pen1;
    pen1.SetStyle(wxSOLID);
    pen1.SetColour(cl);
    pen1.SetWidth(2);
    dc->SetPen(pen1);
    double polval[72];
    double max = 0;
    for (int i = 0; i < 72; i++){
      polval[i] = BoatPolar->GetPolarSpeed(i * 5, m_TWS);
      if (wxIsNaN(polval[i])) polval[i] = 0.0;
      if (polval[i]>max) max = polval[i];
    }
    // double anglevalue = deg2rad(m_Bearing) + deg2rad(m_AngleStart - ANGLE_OFFSET);
    wxPoint currpoints[72];
    double rad, anglevalue;
    for (int i = 0; i < 72; i++){
      //anglevalue = deg2rad(m_Bearing+i*5) + deg2rad(m_AngleStart - ANGLE_OFFSET);
      anglevalue = deg2rad(m_Hdt + i * 5) + deg2rad(m_AngleStart - ANGLE_OFFSET);
      rad = m_radius*0.69*polval[i] / max;
      // wxLogMessage("polval[%d]=%.2f, rad=%.2f",i,polval[i],rad);

      currpoints[i].x = m_cx + (rad * cos(anglevalue));
      currpoints[i].y = m_cy + (rad * sin(anglevalue));

    }
    wxBrush currbrush;
    currbrush.SetColour(wxColour(7, 107, 183, 0));
    currbrush.SetStyle(wxSOLID);
    dc->SetBrush(currbrush);
    dc->DrawPolygon(72, currpoints, 0, 0);
  }
}*/
/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_BearingCompass::DrawCurrent(wxGCDC* dc)
{
	wxColour cl;

	GetGlobalColor(_T("BLUE2"), &cl);

	wxBrush currbrush;
	currbrush.SetColour(wxColour(7, 107, 183, 192));
	currbrush.SetStyle(wxSOLID);
	dc->SetBrush(currbrush);

	double currvalue = deg2rad(m_CurrDir) + deg2rad(m_AngleStart - ANGLE_OFFSET);

	/*
	*           0
	*          /\
	*         /  \
	*        /    \
	*     6 /_  X _\ 1
	*        5|  |2
	*         |__|
	*        4    3
	*/

	wxPoint currpoints[7];
	currpoints[0].x = m_cx + (m_radius * .40 * cos(currvalue));
	currpoints[0].y = m_cy + (m_radius * .40 * sin(currvalue));
	currpoints[1].x = m_cx + (m_radius * .18 * cos(currvalue + 1.5));
	currpoints[1].y = m_cy + (m_radius * .18 * sin(currvalue + 1.5));
	currpoints[2].x = m_cx + (m_radius * .10 * cos(currvalue + 1.5));
	currpoints[2].y = m_cy + (m_radius * .10 * sin(currvalue + 1.5));

	currpoints[3].x = m_cx + (m_radius * .3 * cos(currvalue + 2.8));
	currpoints[3].y = m_cy + (m_radius * .3 * sin(currvalue + 2.8));
	currpoints[4].x = m_cx + (m_radius * .3 * cos(currvalue - 2.8));
	currpoints[4].y = m_cy + (m_radius * .3 * sin(currvalue - 2.8));

	currpoints[5].x = m_cx + (m_radius * .10 * cos(currvalue - 1.5));
	currpoints[5].y = m_cy + (m_radius * .10 * sin(currvalue - 1.5));
	currpoints[6].x = m_cx + (m_radius * .18 * cos(currvalue - 1.5));
	currpoints[6].y = m_cy + (m_radius * .18 * sin(currvalue - 1.5));

	dc->SetPen(*wxTRANSPARENT_PEN);

	dc->DrawPolygon(7, currpoints, 0, 0);
}

/***************************************************************************************
****************************************************************************************/
void TacticsInstrument_BearingCompass::DrawData(wxGCDC* dc, double value,
	wxString unit, wxString format, DialPositionOption position)
{
	if (position == DIAL_POSITION_NONE)
		return;

	dc->SetFont(*g_pFontLabel);
	wxColour cl;
	GetGlobalColor(_T("DASHF"), &cl);
	dc->SetTextForeground(cl);

	wxSize size = GetClientSize();

	wxString text;
	if (!wxIsNaN(value))
	{
		if (unit == _T("\u00B0"))
			text = wxString::Format(format, value) + DEGREE_SIGN;
		else if (unit == _T("\u00B0L")) // No special display for now, might be XX°< (as in text-only instrument)
			text = wxString::Format(format, value) + DEGREE_SIGN;
		else if (unit == _T("\u00B0R")) // No special display for now, might be >XX°
			text = wxString::Format(format, value) + DEGREE_SIGN;
		else if (unit == _T("\u00B0T"))
			text = wxString::Format(format, value) + DEGREE_SIGN + _T("T");
		else if (unit == _T("\u00B0M"))
			text = wxString::Format(format, value) + DEGREE_SIGN + _T("M");
		else if (unit == _T("N")) // Knots
			text = wxString::Format(format, value) + _T(" Kts");
		else
			text = wxString::Format(format, value) + _T(" ") + unit;
	}
	else
		text = _T("---");

	int width, height;
	dc->GetMultiLineTextExtent(text, &width, &height, NULL, g_pFontLabel);

	wxRect TextPoint;
	TextPoint.width = width;
	TextPoint.height = height;
	wxColour c3;

	switch (position)
	{
	case DIAL_POSITION_NONE:
		GetGlobalColor(_T("DASHF"), &c3);
		// This case was already handled before, it's here just
		// to avoid compiler warning.
		return;
	case DIAL_POSITION_INSIDE:
	{
		GetGlobalColor(_T("BLUE2"), &c3);
		TextPoint.x = m_cx - (width / 2) - 1;
		TextPoint.y = (size.y * .75) - height;
		GetGlobalColor(_T("DASHL"), &cl);
		int penwidth = size.x / 100;
		wxPen* pen = wxThePenList->FindOrCreatePen(cl, penwidth, wxPENSTYLE_SOLID);
		dc->SetPen(*pen);
		GetGlobalColor(_T("DASHB"), &cl);
		dc->SetBrush(cl);
		// There might be a background drawn below
		// so we must clear it first.
		dc->DrawRoundedRectangle(TextPoint.x - 2, TextPoint.y - 2, width + 4, height + 4, 3);
		break;
	}
	case DIAL_POSITION_TOPINSIDE:
	{
		GetGlobalColor(_T("DASHF"), &c3);
		TextPoint.x = m_cx - (width / 2) - 1;
		TextPoint.y = m_cy- 0.95*m_radius - height;
		GetGlobalColor(_T("UBLCK"), &cl);
		int penwidth = size.x / 100;
		wxPen* pen = wxThePenList->FindOrCreatePen(cl, penwidth, wxPENSTYLE_SOLID);
		dc->SetPen(*pen);
		GetGlobalColor(_T("DASHB"), &cl);
		dc->SetBrush(cl);
		// There might be a background drawn below
		// so we must clear it first.
		dc->DrawRoundedRectangle(TextPoint.x - 2, TextPoint.y - 2, width + 4, height + 4, 3);
		break;
	}
	case DIAL_POSITION_TOPLEFT:
		GetGlobalColor(_T("DASHF"), &c3);
		TextPoint.x = 0;
		TextPoint.y = m_TitleHeight;
		break;
	case DIAL_POSITION_TOPRIGHT:
		GetGlobalColor(_T("DASHF"), &c3);
		TextPoint.x = size.x - width - 1;
		TextPoint.y = m_TitleHeight;
		break;
	case DIAL_POSITION_BOTTOMLEFT:
		GetGlobalColor(_T("DASHF"), &c3);
		TextPoint.x = 0;
		TextPoint.y = size.y - height;
		break;
	case DIAL_POSITION_BOTTOMRIGHT:
		GetGlobalColor(_T("DASHF"), &c3);
		TextPoint.x = size.x - width - 1;
		TextPoint.y = size.y - height;
		break;
	}

	wxColour c2;
	GetGlobalColor(_T("DASHB"), &c2);

	wxStringTokenizer tkz(text, _T("\n"));
	wxString token;

	token = tkz.GetNextToken();
	while (token.Length()) {
		dc->GetTextExtent(token, &width, &height, NULL, NULL, g_pFontLabel);

#ifdef __WXMSW__
		if (g_pFontLabel->GetPointSize() <= 12) {
			wxBitmap tbm(width, height, -1);
			wxMemoryDC tdc(tbm);

			tdc.SetBackground(c2);
			tdc.Clear();
			tdc.SetFont(*g_pFontLabel);
			tdc.SetTextForeground(c3);

			tdc.DrawText(token, 0, 0);
			tdc.SelectObject(wxNullBitmap);

			dc->DrawBitmap(tbm, TextPoint.x, TextPoint.y, false);
		}
		else
#endif
			dc->DrawText(token, TextPoint.x, TextPoint.y);


		TextPoint.y += height;
		token = tkz.GetNextToken();
	}
}
/***************************************************************************************
  Calculate & Draw the laylines for the bearing compass 
****************************************************************************************/
void TacticsInstrument_BearingCompass::DrawLaylines(wxGCDC* dc)
{
  if (!wxIsNaN(m_Cog) && !wxIsNaN(m_Hdt) && !wxIsNaN(m_lat) && !wxIsNaN(m_lon) && !wxIsNaN(m_TWA) && !wxIsNaN(m_CurrDir) && !wxIsNaN(m_CurrSpeed)){


		wxColour cl;
		GetGlobalColor(_T("DASH2"), &cl);
		wxPen pen1;
		pen1.SetStyle(wxSOLID);
		pen1.SetColour(cl);
		pen1.SetWidth(2);
		dc->SetPen(pen1);
		GetGlobalColor(_T("DASH1"), &cl);
		wxBrush brush1;
		brush1.SetStyle(wxSOLID);
		brush1.SetColour(cl);
		dc->SetBrush(brush1);

		dc->SetPen(*wxTRANSPARENT_PEN);

		GetGlobalColor(_T("DASHN"), &cl);
		wxBrush vbrush, tackbrush;
		vbrush.SetStyle(wxSOLID);
		tackbrush.SetStyle(wxSOLID);
		//m_curTack = TWA unit
		//it shows L= wind from left = port tack or R=wind from right = starboard tack
		//we're on port tack, so vertical layline is red
		if (m_curTack == _T("\u00B0L")) {
			vbrush.SetColour(wxColour(204, 41, 41, 128));  //red, transparent
			tackbrush.SetColour(wxColour(0, 200, 0, 128)); //green, transparent
			m_targetTack = _("R");
		}
		else if (m_curTack == _T("\u00B0R"))  {// we're on starboard tack, so vertical layline is green
			vbrush.SetColour(wxColour(0, 200, 0, 128));      //green, transparent
			tackbrush.SetColour(wxColour(204, 41, 41, 128)); //red, transparent
			m_targetTack = _("L");
		}
		double value1 = deg2rad(m_Cog - m_ExpSmoothDegRange / 2. ) + deg2rad(m_AngleStart - ANGLE_OFFSET);
		double value2 = deg2rad(m_Cog + m_ExpSmoothDegRange / 2. ) + deg2rad(m_AngleStart - ANGLE_OFFSET);

		//draw the vertical layline
		dc->SetBrush(vbrush);
		wxPoint vpoints[3];
		vpoints[0].x = m_cx;
		vpoints[0].y = m_cy;
		vpoints[1].x = m_cx + (m_radius * cos(value1));//neu : für HEadup = HDT
		vpoints[1].y = m_cy + (m_radius * sin(value1));//neu : für HEadup = HDT
		vpoints[2].x = m_cx + (m_radius * cos(value2));//neu : für HEadup = HDT
		vpoints[2].y = m_cy + (m_radius * sin(value2));//neu : für HEadup = HDT
		dc->DrawArc(vpoints[2], vpoints[1], vpoints[0]);

		/*****************************************************************************************
		Caclulate and draw  the second layline (for other tack) :
		---------------------------------------------------------
		Approach : in the bearing compass display, "head up" = COG. And TWA is based on boat heading (Hdt).
		to calculate the layline of the other tack, sum up
		diff_btw_Cog_and_HDG(now we're on Hdt) ; this is NOT NEEDED if Head-Up = Hdt !!! 
		+ 2 x TWA
		+ Leeway
		---------
		= predictedHdt
		+ current_angle
		======================
		= newCog (on other tack)
		Calculation of (sea) current angle :
		1. from actual pos. calculate the endpoint of predictedHdt (out: predictedLatHdt, predictedLonHdt),
		assuming same StW on other tack
		2. at that point apply current : startpoint predictedLatHdt, predictedLonHdt + bearing + speed; out : predictedLatCog, predictedLonCog
		3. calculate angle (and speed) from curr pos to predictedLatCog, predictedLonCog; out : newCog + newSOG
		********************************************************************************************/
		dc->SetBrush(tackbrush);
        double predictedKdW; //==predicted Course Through Water
		/*********************************************
        Old: with BearingCompass Head-Up = COG
		double diffCogHdt = m_Cog - m_Hdt;
		m_oldExpSmoothDiffCogHdt = m_ExpSmoothDiffCogHdt;
		m_ExpSmoothDiffCogHdt = alpha_diffCogHdt*diffCogHdt + (1 - alpha_diffCogHdt)*m_oldExpSmoothDiffCogHdt;

		if (m_targetTack == _T("R")){ // currently wind is from port ...
			predictedKdW = m_Cog - m_ExpSmoothDiffCogHdt - 2 * m_TWA - m_Leeway;
		}
		else if (m_targetTack == _T("L")){ //currently wind from starboard
			predictedKdW = m_Cog + m_ExpSmoothDiffCogHdt + 2 * m_TWA + m_Leeway;
		}
        *******************************************/
		//New: with BearingCompass in Head-Up mode = Hdt
        double Leeway = (m_LeewayUnit == _T("\u00B0L")) ? -m_Leeway : m_Leeway;
        if (m_targetTack == _T("R")){ // so currently wind is from port ...
			//predictedKdW = m_Hdt - 2 * m_TWA - m_Leeway;
          predictedKdW = m_Hdt - 2 * m_TWA - Leeway;
        }
		else if (m_targetTack == _T("L")){ //so, currently wind from starboard
			//predictedKdW = m_Hdt + 2 * m_TWA + m_Leeway;
          predictedKdW = m_Hdt + 2 * m_TWA - Leeway;
        }
		else {
          predictedKdW = (m_TWA < 10) ? 180 : 0; // should never happen, but is this correct ???
		}
        if (predictedKdW >= 360) predictedKdW -= 360;
        if (predictedKdW < 0) predictedKdW += 360;
        double predictedLatHdt, predictedLonHdt, predictedLatCog, predictedLonCog;
		double predictedCoG;
        //standard triangle calculation to get predicted CoG / SoG
        //get endpoint from boat-position by applying  KdW, StW
        PositionBearingDistanceMercator_Plugin(m_lat, m_lon, predictedKdW, fromUsrSpeed_Plugin(m_StW, g_iDashSpeedUnit), &predictedLatHdt, &predictedLonHdt);
		   //wxLogMessage(_T("Step1: m_lat=%f,m_lon=%f, predictedKdW=%f,m_StW=%f --> predictedLatHdt=%f,predictedLonHdt=%f\n"), m_lat, m_lon, predictedKdW, m_StW, predictedLatHdt, predictedLonHdt);
        //apply surface current with direction & speed to endpoint from above
		PositionBearingDistanceMercator_Plugin(predictedLatHdt, predictedLonHdt, m_CurrDir, m_CurrSpeed, &predictedLatCog, &predictedLonCog);
		   //wxLogMessage(_T("Step2: predictedLatHdt=%f,predictedLonHdt=%f, m_CurrDir=%f,m_CurrSpeed=%f --> predictedLatCog=%f,predictedLonCog=%f\n"), predictedLatHdt, predictedLonHdt, m_CurrDir, m_CurrSpeed, predictedLatCog, predictedLonCog);
		//now get predicted CoG & SoG as difference between the 2 endpoints (coordinates) from above
        DistanceBearingMercator_Plugin(predictedLatCog, predictedLonCog, m_lat, m_lon, &predictedCoG, &m_predictedSog);
           //wxLogMessage("m_Leeway=%f, m_LeewayUnit=%s,m_targetTack=%s,predictedKdW=%.2f,predictedCoG=%f,predictedSog=%f ", m_Leeway, m_LeewayUnit, m_targetTack, predictedKdW, predictedCoG, m_predictedSog);
           //wxLogMessage(_T("Step3: predictedLatCog=%f,predictedLonCog=%f, m_lat=%f,m_lon=%f --> predictedCoG=%f,predictedSog=%f, AngleStart=%d\n"), predictedLatCog, predictedLonCog, m_lat, m_lon, predictedCoG, m_predictedSog, m_AngleStart);


		value1 = deg2rad(predictedCoG - m_ExpSmoothDegRange / 2.) + deg2rad(m_AngleStart - ANGLE_OFFSET);
		value2 = deg2rad(predictedCoG + m_ExpSmoothDegRange / 2.) + deg2rad(m_AngleStart - ANGLE_OFFSET);

		wxPoint tackpoints[3];
		tackpoints[0].x = m_cx;
		tackpoints[0].y = m_cy;
		tackpoints[1].x = m_cx + (m_radius * cos(value1));
		tackpoints[1].y = m_cy + (m_radius * sin(value1));
		tackpoints[2].x = m_cx + (m_radius * cos(value2));
		tackpoints[2].y = m_cy + (m_radius * sin(value2));
		dc->DrawArc(tackpoints[2], tackpoints[1], tackpoints[0]);
	}
}
/***************************************************************************************
Calculates the width of the laylines.
This depends on the boat's yawing around COG and should give you a hint on the accuracy
of the predicted layline
****************************************************************************************/
void TacticsInstrument_BearingCompass::CalculateLaylineDegreeRange(void)
{
	//calculate degree-range for laylines
	//do some exponential smoothing on degree range of COGs
	if (!wxIsNaN(m_Cog) && m_Cog != m_COGRange[0]){
		double mincog = 360, maxcog = 0;
		for (int i = 0; i < COGRANGE; i++){
			if (m_COGRange[i] < mincog) mincog = m_COGRange[i];
			if (m_COGRange[i] > maxcog) maxcog = m_COGRange[i];
		}
         m_LaylineDegRange = getDegRange(maxcog, mincog);

		for (int i = 0; i < COGRANGE - 1; i++) m_COGRange[i + 1] = m_COGRange[i];
		m_COGRange[0] = m_Cog;
		if (m_LaylineDegRange < g_iMinLaylineWidth){
			m_LaylineDegRange = g_iMinLaylineWidth;
		}
		else if (m_LaylineDegRange > g_iMaxLaylineWidth){
			m_LaylineDegRange = g_iMaxLaylineWidth;
		}
		mExpSmDegRange->SetAlpha(g_dalphaDeltCoG);
		m_ExpSmoothDegRange = mExpSmDegRange->GetSmoothVal(m_LaylineDegRange);


	}

}
/***************************************************************************************
****************************************************************************************/
bool TacticsInstrument_BearingCompass::LoadConfig(void)
{
	wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

	if (pConf) {
		pConf->SetPath(_T("/PlugIns/Tactics/BearingCompass"));

		return true;
	}
	else
		return false;
}
/***************************************************************************************
****************************************************************************************/
bool TacticsInstrument_BearingCompass::SaveConfig(void)
{
	wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

	if (pConf)
	{
		pConf->SetPath(_T("/PlugIns/Tactics/BearingCompass"));
		return true;
	}
	else
		return false;
}

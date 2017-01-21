/******************************************************************************
* $Id: wind_history.cpp, v1.0 2010/08/30 tom_BigSpeedy Exp $
*
* Project:  OpenCPN
* Purpose:  Tactics_pi Plugin
* Author:   Thomas Rauch
* 
***************************************************************************
*   Copyright (C) 2010 by David S. Register   *
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
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
***************************************************************************
*/

#include "avg_wind.h"
#include "wx28compat.h"

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

extern int g_iDashWindSpeedUnit;
//************************************************************************************************************************
// History of wind direction
//************************************************************************************************************************

TacticsInstrument_AvgWindDir::TacticsInstrument_AvgWindDir(wxWindow *parent, wxWindowID id, wxString title) :
TacticsInstrument(parent, id, title, OCPN_DBP_STC_TWD)
{
  SetDrawSoloInPane(true);
  m_WindDir = NAN;
  m_WindDirRange = 90;
  m_TopLineHeight = 30;
  m_IsRunning = false;
  m_SampleCount = 0;
  m_Legend = 3;
   m_AvgTime = 360; //6 min
   m_DegRangePort = 0.0;
   m_DegRangeStb = 0.0;
   for (int i = 0; i < AVG_WIND_RECORDS; i++){
     m_WindDirArray[i] = NAN;
     m_signedWindDirArray[i] = NAN;
     m_ExpsinSmoothArrayWindDir[i] = NAN;
     m_ExpcosSmoothArrayWindDir[i] = NAN;
     m_ExpSmoothSignedWindDirArray[i] = NAN;
   }
   mDblsinExpSmoothWindDir = new DoubleExpSmooth(0.06);
   mDblcosExpSmoothWindDir = new DoubleExpSmooth(0.06);
   /* for (int idx = 0; idx < AVG_WIND_RECORDS; idx++) {
    m_ArrayRecTime[idx] = wxDateTime::Now();
  }*/
  alpha = 0.1;  //smoothing constant
  wxSize size = GetClientSize();
  m_cx = size.x / 2;

  m_AvgTimeSlider = new wxSlider(this, wxID_ANY, m_AvgTime / 60, 6, 30, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS | wxSL_BOTTOM | wxSL_HORIZONTAL | wxFULL_REPAINT_ON_RESIZE | wxSL_VALUE_LABEL);
  m_AvgTimeSlider->SetPageSize(2);
  m_AvgTimeSlider->SetLineSize(2);
  m_AvgTimeSlider->SetTickFreq(2);
  m_AvgTimeSlider->SetValue(m_AvgTime/60);
  m_AvgTimeSlider->Connect(wxEVT_COMMAND_SLIDER_UPDATED, wxCommandEventHandler(TacticsInstrument_AvgWindDir::OnAvgTimeSliderUpdated), NULL, this);
  int w;
  m_AvgTimeSlider->GetSize(&w, &m_SliderHeight);

//we process data 1/s ...
  m_avgWindUpdTimer.Start(1000, wxTIMER_CONTINUOUS);
  m_avgWindUpdTimer.Connect(wxEVT_TIMER, wxTimerEventHandler(TacticsInstrument_AvgWindDir::OnAvgWindUpdTimer), NULL, this);


}
void TacticsInstrument_AvgWindDir::OnAvgWindUpdTimer(wxTimerEvent & event)
{
  if (!wxIsNaN(m_WindDir))
    CalcAvgWindDir(m_WindDir);
}
void TacticsInstrument_AvgWindDir::OnAvgTimeSliderUpdated(wxCommandEvent& event)
{  // slider increments in step of 2 only
 /* int val = m_AvgTimeSlider->GetValue();

  int remainder = val % 2; // The step interval. 

  // If the value is not evenly divisible by the step interval,
  // snap the value to an even interval.
  if (remainder != 0){
    val -= remainder;
    m_AvgTimeSlider->SetValue(val);
  }*/
  m_AvgTime = m_AvgTimeSlider->GetValue()*60;
}

wxSize TacticsInstrument_AvgWindDir::GetSize(int orient, wxSize hint)
{
  wxClientDC dc(this);
  int w;
  dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
  if (orient == wxHORIZONTAL) {
    return wxSize(DefaultWidth, wxMax(m_TitleHeight + 140, hint.y));
  }
  else {
    return wxSize(wxMax(hint.x, DefaultWidth), wxMax(m_TitleHeight + 140, hint.y));
  }
}
void TacticsInstrument_AvgWindDir::SetData(int st, double data, wxString unit)
{
    if (st == OCPN_DBP_STC_TWD ) { 
      m_WindDir = data;
    }
    m_IsRunning = wxIsNaN(m_WindDir)? false:true;
}

double TacticsInstrument_AvgWindDir::GetAvgWindDir()
{
  return m_AvgWindDir;
}
void TacticsInstrument_AvgWindDir::CalcAvgWindDir(double CurWindDir)
{
  int i;
  m_SampleCount = m_SampleCount<AVG_WIND_RECORDS ? m_SampleCount + 1 : AVG_WIND_RECORDS;
  //fill the array, perform data shifting in case the array is completely filled.
  // we always fill the whole array, independent of which time average is set.
  // --> once the array is filled up, we can dynamically change the time average w/o the need to
  // wait for another full set of data.
  for (i = AVG_WIND_RECORDS - 1; i > 0; i--) {
    m_WindDirArray[i]=m_WindDirArray[i - 1]  ;
    m_ExpsinSmoothArrayWindDir[i] = m_ExpsinSmoothArrayWindDir[i-1];
    m_ExpcosSmoothArrayWindDir[i] = m_ExpcosSmoothArrayWindDir[i - 1];
  }
  m_WindDirArray[0] = CurWindDir;
  double rad = (90 - CurWindDir)*M_PI / 180.;

  m_ExpsinSmoothArrayWindDir[0] = mDblsinExpSmoothWindDir->GetSmoothVal(sin(rad));
  m_ExpcosSmoothArrayWindDir[0] = mDblcosExpSmoothWindDir->GetSmoothVal(cos(rad));

  //Problem Norddurchgang: 355° - 10° ...
  //solution via atan2 function...
  //calculation of arithmetical mean value
  double sinAvgDir = 0;
  double cosAvgDir = 0;
   rad = 0;
   int samples = m_SampleCount < m_AvgTime ? m_SampleCount : m_AvgTime;
  for (i = 0; i < samples; i++){
    rad = (90. - m_WindDirArray[i])*M_PI / 180.;
    sinAvgDir += sin(rad);
    cosAvgDir += cos(rad);
  }
  m_AvgWindDir = (90. - (atan2( sinAvgDir,cosAvgDir)*180. / M_PI) + 360.);
 while (m_AvgWindDir >= 360) m_AvgWindDir -= 360;
 //m_AvgDegRange ermitteln
 m_DegRangePort = 360;
 m_DegRangeStb = -360;
 double val,smval,smWDir;
 for (i = 0; i < samples && !wxIsNaN(m_WindDirArray[i]); i++){
  val= getSignedDegRange(m_AvgWindDir, m_WindDirArray[i]);
  m_signedWindDirArray[i] = val;
  smWDir = (90. - (atan2(m_ExpsinSmoothArrayWindDir[i], m_ExpcosSmoothArrayWindDir[i])*180. / M_PI) + 360.);
  while (smWDir >= 360) smWDir -= 360;
  smval = getSignedDegRange(m_AvgWindDir, smWDir);
  m_ExpSmoothSignedWindDirArray[i] = smval;
  if (val < m_DegRangePort) m_DegRangePort=val;
  if (val > m_DegRangeStb) m_DegRangeStb=val;
 }
}

void TacticsInstrument_AvgWindDir::Draw(wxGCDC* dc)
{
  wxColour c1;
  GetGlobalColor(_T("DASHB"), &c1);
  wxBrush b1(c1);
  dc->SetBackground(b1);
  dc->Clear();

  wxSize size = GetClientSize();
  m_cx = size.x / 2;

  m_AvgTimeSlider->SetSize(10, 0, size.x-20,5);
  int w,h;
  m_AvgTimeSlider->GetSize(&w, &m_SliderHeight);
//  m_width = size.x;
  m_height = size.y;

  dc->GetTextExtent(_T("30"), &w, &h, 0, 0, g_pFontSmall);
  m_Legend = w;
  m_width = size.x - 2 * m_Legend-2;

  m_availableHeight = size.y - m_TopLineHeight - m_SliderHeight- 1 - h;
  DrawBackground(dc);
  DrawForeground(dc);

}

// *********************************************************************************
//draw background
// *********************************************************************************
void TacticsInstrument_AvgWindDir::DrawBackground(wxGCDC* dc)
{
  wxString label;
  wxColour cl;
  wxPen pen;
  //---------------------------------------------------------------------------------
  // horizontal lines
  //---------------------------------------------------------------------------------
  GetGlobalColor(_T("UBLCK"), &cl);
  pen.SetColour(cl);
  pen.SetStyle(wxPENSTYLE_SOLID);
  dc->SetPen(pen);
  int width, height;
  int time = m_AvgTime / 60;
  cl = wxColour(0, 0, 0, 255); //black, solid

  dc->SetTextForeground(cl);
  dc->SetFont(*g_pFontSmall);

  dc->DrawLine(m_cx, m_TopLineHeight + m_SliderHeight, m_cx, (int)(m_TopLineHeight + m_SliderHeight + m_availableHeight)); // the vertical center line

  //top horzontal line
  label = wxString::Format(_T("%2d"), 0);
  dc->GetTextExtent(label, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label, 1, (int)(m_TopLineHeight + m_SliderHeight - height / 2.));
  dc->DrawLine(m_Legend + 1, m_TopLineHeight + m_SliderHeight, m_Legend + 1 + m_width, m_TopLineHeight + m_SliderHeight);
//bottom line + legend
  label = wxString::Format(_T("%2d"), time);
  dc->GetTextExtent(label, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label, 1, (int)(m_TopLineHeight + m_SliderHeight + m_availableHeight - height / 2.)); 
  dc->DrawLine(m_Legend + 1, (int)(m_TopLineHeight + m_SliderHeight + m_availableHeight), m_Legend + 1 + m_width, (int)(m_TopLineHeight + m_SliderHeight + m_availableHeight));
  int x1 = 0;
  int x2 = 0;
  for (int i = 1; i < time; i++){
    if (i % 5 == 0){
      x1 = m_cx;
      x2 = m_cx;
      label = wxString::Format(_T("%2d"), i);
      dc->DrawText(label, 1, (int)(m_TopLineHeight + m_SliderHeight - height / 2. + m_availableHeight / (double)time * i));
    }
    else{
      //      x1 = m_cx - 10;
      //      x2 = m_cx + 10;
      x1 = m_Legend + 11;
      x2 = m_Legend + 1 + m_width - 10;
    }
    dc->DrawLine(m_Legend + 1 , (int)(m_TopLineHeight + m_SliderHeight + m_availableHeight / (double)time * i), x1, (int)(m_TopLineHeight + m_SliderHeight + m_availableHeight / (double)time * i));
    dc->DrawLine(x2, (int)(m_TopLineHeight + m_SliderHeight + m_availableHeight / (double)time * i), m_Legend + 1 + m_width, (int)(m_TopLineHeight + m_SliderHeight + m_availableHeight / (double)time * i));
  }
}


//*********************************************************************************
//draw foreground
//*********************************************************************************
void TacticsInstrument_AvgWindDir::DrawForeground(wxGCDC* dc)
{
  wxColour col;
  int degw, degh;
  double dir;
  wxString avgWindAngle, minAngle, maxAngle;
  wxPen pen;
  wxString label;

  //---------------------------------------------------------------------------------
  // average wind direction
  //---------------------------------------------------------------------------------
  dc->SetFont(*g_pFontData);
  col = wxColour(255, 0, 0, 255); //red, solid
  dc->SetTextForeground(col);
  if (!m_IsRunning || wxIsNaN(m_WindDir))
    avgWindAngle = _T("---");
  else {
    dir = wxRound(m_AvgWindDir);
    while (dir > 360) dir -= 360;
    while (dir <0) dir += 360;
    avgWindAngle = wxString::Format(_T("%3.0f"), dir) + DEGREE_SIGN;
  }
  dc->GetTextExtent(avgWindAngle, &degw, &degh, 0, 0, g_pFontData);
  dc->DrawText(avgWindAngle, m_Legend + 1+m_width / 2 - degw / 2, m_TopLineHeight + m_SliderHeight - degh);

  // das WindDirArray durchgehen
  // --> aktuelle Werte als größer/kleiner AverageWindDir, also +-Werte.
  // min und max Werte aus dem Array (0.. m_AvgTime) ergeben den horizontalen Zoomfaktor m_ratioW
  // Darzustellende Averagezeit m_AvgTime ergibt vertikalen Zoomfaktor ratioH
  m_ratioH = (double)(m_availableHeight) / (double)(m_AvgTime);// Höhe durch Anzahl Sekunden
 //take the bigger value of both and double up, Average is always centered
  double maxDegRange = 2.* wxMax(m_DegRangeStb, abs(m_DegRangePort));

  m_ratioW = double(m_width) / maxDegRange;
  pen.SetStyle(wxPENSTYLE_SOLID);
  //---------------------------------------------------------------------------------
  // live direction data
  //---------------------------------------------------------------------------------
  wxPoint points, pointAngle_old;
  pointAngle_old.x = m_width / 2. + m_signedWindDirArray[0] * m_ratioW + m_Legend + 1;
  pointAngle_old.y = m_TopLineHeight + m_SliderHeight + 1;
  pen.SetColour(wxColour(0, 0, 255,40)); //blue, opague
  dc->SetPen(pen);
  int samples = m_SampleCount < m_AvgTime ? m_SampleCount : m_AvgTime;

  for (int idx = 1; idx < samples; idx++) {
    points.x = m_width / 2. + m_signedWindDirArray[idx] * m_ratioW + m_Legend + 1;
    points.y = (int)((double)m_TopLineHeight + m_SliderHeight + 1. + (double)idx * m_ratioH);
    dc->DrawLine(pointAngle_old,points);
    pointAngle_old.x = points.x;
    pointAngle_old.y = points.y;
  }
  
  //---------------------------------------------------------------------------------
  //exponential smoothing of direction
  //---------------------------------------------------------------------------------
  pen.SetStyle(wxPENSTYLE_SOLID);
  pen.SetColour(wxColour(0, 0, 255, 128));
  pen.SetWidth(2);
  dc->SetPen(pen);
  wxBrush greenbrush, redbrush;
  greenbrush.SetStyle(wxBRUSHSTYLE_SOLID);
  greenbrush.SetColour(wxColour(0, 200, 0, 128));
  redbrush.SetStyle(wxBRUSHSTYLE_SOLID);
  redbrush.SetColour(wxColour(204, 41, 41, 128));
  wxPoint fill[4];
  pointAngle_old.x = m_width / 2. + m_ExpSmoothSignedWindDirArray[0] * m_ratioW + m_Legend + 1;
  pointAngle_old.y = m_TopLineHeight + m_SliderHeight + 1;
  for (int idx = 1; idx < samples; idx++) {
    points.x = m_width / 2. + m_ExpSmoothSignedWindDirArray[idx] * m_ratioW + m_Legend + 1;
    points.y = m_TopLineHeight + m_SliderHeight + 1 + idx * m_ratioH;
    fill[0].x = pointAngle_old.x;
    fill[0].y = pointAngle_old.y;
    fill[1].x = points.x;
    fill[1].y = points.y;
    fill[2].x = m_cx;
    fill[2].y = points.y;
    fill[3].x = m_cx;
    fill[3].y = pointAngle_old.y;
    dc->SetPen(pen);
    dc->DrawLine(pointAngle_old, points);
    dc->SetPen(*wxTRANSPARENT_PEN);
    if (points.x>=m_cx && pointAngle_old.x>=m_cx)
       dc->SetBrush(greenbrush);
    else
      dc->SetBrush(redbrush);
    dc->DrawPolygon(4,fill,0,0);
    pointAngle_old.x = points.x;
    pointAngle_old.y = points.y;
  }
  
  //---------------------------------------------------------------------------------
  // wind speed
  //---------------------------------------------------------------------------------
  dc->SetFont(*g_pFontData);
  if (!m_IsRunning || wxIsNaN(m_WindDir)){
    minAngle = _T("---");
    maxAngle = _T("---");
  }
  else{
    double leftAngle = wxRound(m_AvgWindDir + m_DegRangePort);
    while (leftAngle > 360) leftAngle -= 360;
    while (leftAngle <0) leftAngle += 360;

    minAngle = wxString::Format(_T("%3.0f"), leftAngle) + DEGREE_SIGN;
    double rightAngle = wxRound(m_AvgWindDir + m_DegRangeStb);
    while (rightAngle > 360) rightAngle -= 360;
    while (rightAngle <0) rightAngle += 360;
    maxAngle = wxString::Format(_T("%3.0f"), rightAngle) + DEGREE_SIGN;
  }
  dc->GetTextExtent(minAngle, &degw, &degh, 0, 0, g_pFontData);
  col = wxColour(wxColour(204, 41, 41, 128)); //red, opaque
  dc->SetTextForeground(col);
  dc->DrawText(minAngle, m_Legend + 1, m_TopLineHeight + m_SliderHeight - degh);
  dc->GetTextExtent(maxAngle, &degw, &degh, 0, 0, g_pFontData);
  col = wxColour(wxColour(0, 200, 0, 192)); //green, opaque
  dc->SetTextForeground(col);
  dc->DrawText(maxAngle, m_width - degw + m_Legend +1, m_TopLineHeight + m_SliderHeight - degh);
}

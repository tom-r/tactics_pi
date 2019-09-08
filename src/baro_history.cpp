/******************************************************************************
 * $Id: baro_history.cpp, v1.0 2014/02/10 tom-r Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Tactics Plugin
 * Author:   stedy
 * Based on code from  Thomas Rauch
 * Updated and code cleanup Thomas Rauch
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

#include <wx/fileconf.h>
#include "baro_history.h"
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
extern wxString g_sDataExportSeparator;
extern bool g_bDataExportUTC;
extern bool g_bDataExportClockticks;

#define ID_EXPORTRATE_10 11110
#define ID_EXPORTRATE_20 11120
#define ID_EXPORTRATE_60 11160


//************************************************************************************************************************
// History of barometic pressure
//************************************************************************************************************************

TacticsInstrument_BaroHistory::TacticsInstrument_BaroHistory(wxWindow *parent, wxWindowID id, wxString title) :
  TacticsInstrument(parent, id, title, OCPN_DBP_STC_MDA)
{
  SetDrawSoloInPane(true);
  m_MaxPress = 0.0;
  m_MinPress = 1200.00;
  m_TotalMaxPress = 0.0;
  m_TotalMinPress = 1200.00;
  m_Press = 0.0;
  m_MaxPressScale = NAN;
  m_ratioW = NAN;
  m_TopLineHeight = 35;
  m_IsRunning = false;
  m_SampleCount = 0;
  m_LeftLegend = 3;
  m_RightLegend = 3;
  m_TitleHeight = 10;
  for (int idx = 0; idx < BARO_RECORD_COUNT; idx++) {
    m_ArrayPressHistory[idx] = -1;
    m_ArrayRecTime[idx] = wxDateTime::Now().GetTm();
    m_ArrayRecTime[idx].year = 999;

  }
  m_WindowRect = GetClientRect();
  m_DrawAreaRect = GetClientRect();
  m_DrawAreaRect.SetHeight(m_WindowRect.height - m_TopLineHeight - m_TitleHeight);
  m_BaroHistUpdTimer.Start(1000, wxTIMER_CONTINUOUS); //start with 1/s, on full exportinterval set to 5 sec
  m_TimerSynched = false;
  m_BaroHistUpdTimer.Connect(wxEVT_TIMER, wxTimerEventHandler(TacticsInstrument_BaroHistory::OnBaroHistUpdTimer), NULL, this);

  //data export
  m_isExporting = false;
  wxPoint pos;
  pos.x = pos.y = 0;
  m_LogButton = new wxButton(this, wxID_ANY, _(">"), pos, wxDefaultSize, wxBU_TOP | wxBU_EXACTFIT | wxFULL_REPAINT_ON_RESIZE | wxBORDER_NONE);
  m_LogButton->SetToolTip(_("'>' starts data export and creates a new or appends to an existing file,\n'X' stops data export"));
  m_LogButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TacticsInstrument_BaroHistory::OnLogDataButtonPressed), NULL, this);
  m_pconfig = GetOCPNConfigObject();
  if (LoadConfig() == false) {
    m_exportInterval = 10;
    SaveConfig();
  }
  m_pExportmenu = new wxMenu();
  // this is a dummy menu required by Windows as parent to item created
  //wxMenuItem *pmi = new wxMenuItem(m_pExportmenu, -1, _T("Data Export"));
  btn10Sec = m_pExportmenu->AppendRadioItem(ID_EXPORTRATE_10, _("Exportrate 10 Seconds"));
  btn20Sec = m_pExportmenu->AppendRadioItem(ID_EXPORTRATE_20, _("Exportrate 20 Seconds"));
  btn60Sec = m_pExportmenu->AppendRadioItem(ID_EXPORTRATE_60, _("Exportrate 60 Seconds"));

  if (m_exportInterval == 10) btn10Sec->Check(true);
  if (m_exportInterval == 20) btn20Sec->Check(true);
  if (m_exportInterval == 60) btn60Sec->Check(true);
}
TacticsInstrument_BaroHistory::~TacticsInstrument_BaroHistory(void) {
  if (m_isExporting)
    m_ostreamlogfile.Close();
  m_BaroHistUpdTimer.Stop();
  m_LogButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TacticsInstrument_BaroHistory::OnLogDataButtonPressed), NULL, this);

}
wxSize TacticsInstrument_BaroHistory::GetSize( int orient, wxSize hint )
{
      wxClientDC dc(this);
      int w;
      dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
      if( orient == wxHORIZONTAL ) {
        return wxSize( DefaultWidth, wxMax(m_TitleHeight+140, hint.y) );
      }
      else {
        return wxSize( wxMax(hint.x, DefaultWidth), wxMax(m_TitleHeight+140, hint.y) );
      }
}
void TacticsInstrument_BaroHistory::SetData(int st, double data, wxString unit)
{
    if (st == OCPN_DBP_STC_MDA)  m_Press = data;
}
// 5/sec should do for barometric pressure ...
void TacticsInstrument_BaroHistory::OnBaroHistUpdTimer(wxTimerEvent & event)
{
  // as long as the timer is not synched to a full minute ...
  if (m_TimerSynched == false) {
    if (wxDateTime::Now().GetSecond() % 10 == 0) {
      m_BaroHistUpdTimer.Stop();        //stop and restart the timer with 5s 
      m_BaroHistUpdTimer.Start(5000, wxTIMER_CONTINUOUS);
      m_TimerSynched = true;
    }
    else  //do nothing and return ...
      return;
  }
  wxDateTime localTimeNow = wxDateTime::Now().GetTm();

  if (m_Press > 0.0) {
    m_IsRunning = true;
    m_SampleCount = m_SampleCount < BARO_RECORD_COUNT ? m_SampleCount + 1 : BARO_RECORD_COUNT;
    m_MaxPress = 0.0;
    ;
    //data shifting
    for (int idx = 1; idx < BARO_RECORD_COUNT; idx++) {
      if (BARO_RECORD_COUNT - m_SampleCount <= idx)
        m_MaxPress = wxMax(m_ArrayPressHistory[idx - 1], m_MaxPress);
      m_MinPress = wxMin(m_ArrayPressHistory[idx - 1], m_MinPress);
      m_ArrayPressHistory[idx - 1] = m_ArrayPressHistory[idx];
      m_ArrayRecTime[idx - 1] = m_ArrayRecTime[idx];
    }
    m_ArrayPressHistory[BARO_RECORD_COUNT - 1] = m_Press;
    if (m_SampleCount < 2) {
      m_ArrayPressHistory[BARO_RECORD_COUNT - 2] = m_Press;

    }
    m_ArrayRecTime[BARO_RECORD_COUNT - 1] = wxDateTime::Now().GetTm();
    m_MaxPress = wxMax(m_Press, m_MaxPress);

    m_MinPress = wxMin(m_MinPress, m_Press);
    if (wxMin(m_Press, m_MinPress) == -1) {
      m_MinPress = wxMin(m_Press, 1200); // to make a OK inital value
    }
    //get the overall max min pressure
    m_TotalMaxPress = wxMax(m_Press, m_TotalMaxPress);
    m_TotalMinPress = wxMin(m_Press, m_TotalMinPress);
    if (localTimeNow.GetSecond() % m_exportInterval == 0)
      ExportData();
  }
}
void TacticsInstrument_BaroHistory::Draw(wxGCDC* dc)
{
   m_WindowRect = GetClientRect();
   m_DrawAreaRect=GetClientRect();
   m_DrawAreaRect.SetHeight(m_WindowRect.height-m_TopLineHeight-m_TitleHeight);
   m_DrawAreaRect.SetX (m_LeftLegend+3);
   DrawBackground(dc);
   DrawForeground(dc);
}



//*********************************************************************************
// draw pressure scale
//*********************************************************************************
void  TacticsInstrument_BaroHistory::DrawPressureScale(wxGCDC* dc)
{
  wxString label1,label2,label3,label4,label5;
  wxColour cl;
  int width, height;
  cl=wxColour(61,61,204,255);
  dc->SetTextForeground(cl);
  dc->SetFont(*g_pFontSmall);
  //round m_MaxPress up to the next hpa ...
  if (m_MaxPress > 1100)
  m_MaxPress=1100;

  if (m_TotalMinPress < 930)
  m_TotalMinPress=930;


  m_MaxPressScale= (int)((m_MaxPress+15)-(m_TotalMinPress-15));

  if(!m_IsRunning) {
    label1=_T("-- hPa");
    label2=_T("-- hPa");
    label3=_T("-- hPa");
    label4=_T("-- hPa");
    label5=_T("-- hPa");
  }
  else {
/*
 The goal is to draw the legend with decimals only, if we really have them !
*/
    // top legend for max press
    label1.Printf(_T("%.0f hPa"), m_MaxPressScale +(m_TotalMinPress-18)  );

    // 3/4 legend

      label2.Printf(_T("%.0f hPa"), m_MaxPressScale *3./4 + (m_TotalMinPress-18)  );

    // center legend

      label3.Printf(_T("%.0f hPa"), m_MaxPressScale /2 +(m_TotalMinPress-18));

    // 1/4 legend

      label4.Printf(_T("%.0f hPa"), m_MaxPressScale /4 +(m_TotalMinPress-18)  );

    //bottom legend for min pressure
    label5.Printf(_T("%.0f hPa"), (m_TotalMinPress-18));
  }
  dc->GetTextExtent(label1, &m_LeftLegend, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label1, 4, (int)(m_TopLineHeight-height/2));
  dc->GetTextExtent(label2, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label2, 4, (int)(m_TopLineHeight+m_DrawAreaRect.height/4-height/2));
  m_LeftLegend = wxMax(width,m_LeftLegend);
  dc->GetTextExtent(label3, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label3, 4, (int)(m_TopLineHeight+m_DrawAreaRect.height/2-height/2));
  m_LeftLegend = wxMax(width,m_LeftLegend);
  dc->GetTextExtent(label4, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label4, 4, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.75-height/2));
  m_LeftLegend = wxMax(width,m_LeftLegend);
  dc->GetTextExtent(label5, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label5, 4,  (int)(m_TopLineHeight+m_DrawAreaRect.height-height/2));
  m_LeftLegend = wxMax(width,m_LeftLegend);
  m_LeftLegend+=4;
}

//*********************************************************************************
//draw background
//*********************************************************************************
void TacticsInstrument_BaroHistory::DrawBackground(wxGCDC* dc)
{
  wxString label,label1,label2,label3,label4,label5;
  wxColour cl;
  wxPen pen;
  //---------------------------------------------------------------------------------
  // draw legend for pressure
  //---------------------------------------------------------------------------------

  DrawPressureScale(dc);

  //---------------------------------------------------------------------------------
  // horizontal lines
  //---------------------------------------------------------------------------------
  GetGlobalColor(_T("UBLCK"), &cl);
  pen.SetColour(cl);
  dc->SetPen(pen);
  dc->DrawLine(m_LeftLegend+3, m_TopLineHeight, m_WindowRect.width-3-m_RightLegend, m_TopLineHeight); // the upper line
  dc->DrawLine(m_LeftLegend+3, (int)(m_TopLineHeight+m_DrawAreaRect.height), m_WindowRect.width-3-m_RightLegend, (int)(m_TopLineHeight+m_DrawAreaRect.height));
  pen.SetStyle(wxPENSTYLE_DOT);
  dc->SetPen(pen);
  dc->DrawLine(m_LeftLegend+3, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.25), m_WindowRect.width-3-m_RightLegend, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.25));
  dc->DrawLine(m_LeftLegend+3, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.75), m_WindowRect.width-3-m_RightLegend, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.75));
#ifdef __WXMSW__  
  pen.SetStyle(wxPENSTYLE_SHORT_DASH);
  dc->SetPen(pen);
#endif  
  dc->DrawLine(m_LeftLegend+3, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.5), m_WindowRect.width-3-m_RightLegend, (int)(m_TopLineHeight+m_DrawAreaRect.height*0.5));
}


//*********************************************************************************
//draw foreground
//*********************************************************************************
void TacticsInstrument_BaroHistory::DrawForeground(wxGCDC* dc)
{
  wxColour col;
  double ratioH;
  int degw,degh;
  int width,height,sec,min,hour;
  wxString BaroPressure;
  wxPen pen;
  wxString label;


  //---------------------------------------------------------------------------------
  // Pressure
  //---------------------------------------------------------------------------------
  col=wxColour(61,61,204,255); //blue, opaque
  dc->SetFont(*g_pFontData);
  dc->SetTextForeground(col);
  BaroPressure=wxString::Format(_T("hPa %4.1f  "), m_Press);
  dc->GetTextExtent(BaroPressure, &degw, &degh, 0, 0, g_pFontData);
  dc->DrawText(BaroPressure, m_LeftLegend+3, m_TopLineHeight-degh);
  dc->SetFont(*g_pFontLabel);
  //determine the time range of the available data (=oldest data value)
  int i=0;
  while(m_ArrayRecTime[i].year == 999 && i<BARO_RECORD_COUNT-1) i++;
  if (i == BARO_RECORD_COUNT -1) {  
    min=0;
    hour=0;
  }
  else {
    wxDateTime localTime( m_ArrayRecTime[i] );
    min=localTime.GetMinute( );
    hour=localTime.GetHour( );
  }
  // Avoid writing outside of drawing aread , available area = LeftLegend - 3 - draw_area_width - 5
 // m_ratioW = double(m_DrawAreaRect.width - m_LeftLegend - 3 - 5) / (BARO_RECORD_COUNT - 1);
 // dc->DrawText(wxString::Format(_(" Max %.1f Min %.1f since %02d:%02d  Overall Max %.1f Min %.1f "),m_MaxPress,m_MinPress,hour,min,m_TotalMaxPress,m_TotalMinPress), m_LeftLegend+3+2+degw, m_TopLineHeight-degh+5);
 // Cant get the min sice to work...
 // Single text var to facilitate correct translations:
 wxString s_Max = _("Max");
 wxString s_Since = _("since");
 wxString s_OMax = _("Overall Max");
 wxString s_Min = _("Min");
 dc->DrawText(wxString::Format(_T(" %s %.1f %s %02d:%02d  %s %.1f %s %.1f "), s_Max, m_MaxPress, s_Since, hour, min, s_OMax, m_TotalMaxPress, s_Min, m_TotalMinPress), m_LeftLegend + 3 + 2 + degw, m_TopLineHeight - degh + 2);
 //dc->DrawText(wxString::Format(_(" Max %.1f since %02d:%02d  Overall Max %.1f Min %.1f "),m_MaxPress,hour,min,m_TotalMaxPress,m_TotalMinPress), m_LeftLegend+3+2+degw, m_TopLineHeight-degh+5);
  pen.SetStyle(wxPENSTYLE_SOLID);
  pen.SetColour(wxColour(61,61,204,255)); //blue, opague
  pen.SetWidth(1);
  dc->SetPen( pen );
  ratioH = (double)m_DrawAreaRect.height / (double)m_MaxPressScale ;
  m_DrawAreaRect.SetWidth(m_WindowRect.width - 6 - m_LeftLegend - m_RightLegend);
  m_ratioW = double(m_DrawAreaRect.width) / (BARO_RECORD_COUNT - 1);

  wxPoint  pointPressure[BARO_RECORD_COUNT+2],pointPressure_old;
  pointPressure_old.x=m_LeftLegend+3;
//TR  pointPressure_old.y = m_TopLineHeight+m_DrawAreaRect.height - m_ArrayPressHistory[0] * ratioH;
  pointPressure_old.y = m_TopLineHeight + m_DrawAreaRect.height - (m_ArrayPressHistory[0] - (double)m_TotalMinPress + 18) * ratioH;

  //---------------------------------------------------------------------------------
  // live pressure data
  //---------------------------------------------------------------------------------

  for (int idx = 1; idx < BARO_RECORD_COUNT; idx++) {
    pointPressure[idx].y = m_TopLineHeight+m_DrawAreaRect.height - ((m_ArrayPressHistory[idx]-(double)m_TotalMinPress+18) * ratioH);
//    pointPressure[idx].x = idx * m_ratioW -3 ;//- 30 + m_LeftLegend;
    pointPressure[idx].x = idx * m_ratioW + 3 + m_LeftLegend;
    if(BARO_RECORD_COUNT-m_SampleCount <= idx && pointPressure[idx].y > m_TopLineHeight && pointPressure_old.y > m_TopLineHeight && pointPressure[idx].y <=m_TopLineHeight+m_DrawAreaRect.height && pointPressure_old.y<=m_TopLineHeight+m_DrawAreaRect.height)
       dc->DrawLine( pointPressure_old.x, pointPressure_old.y, pointPressure[idx].x,pointPressure[idx].y );
    pointPressure_old.x=pointPressure[idx].x;
    pointPressure_old.y=pointPressure[idx].y;
  }

  //---------------------------------------------------------------------------------
  //draw vertical timelines every 15 minutes
  //---------------------------------------------------------------------------------
  GetGlobalColor(_T("UBLCK"), &col);
  pen.SetColour(col);
  pen.SetStyle(wxPENSTYLE_DOT);
  dc->SetPen(pen);
  dc->SetTextForeground(col);
  dc->SetFont(*g_pFontSmall);
  int done=-1;
  wxPoint pointTime;
  int prevfiverfit = -15;
  for (int idx = 0; idx < BARO_RECORD_COUNT; idx++) {
    if (m_ArrayRecTime[idx].year != 999) {
      wxDateTime localTime( m_ArrayRecTime[idx] );
      min=localTime.GetMinute( );
      hour=localTime.GetHour( );
      sec=localTime.GetSecond( );
//      if ( (hour*100+min) != done && (min % 5 == 0 ) && (sec == 0 || sec == 1) ) {
      if ((hour * 100 + min) != done && (min % 15 == 0)) {
        if (min != prevfiverfit) {
          pointTime.x = idx * m_ratioW + 3 + m_LeftLegend;
        dc->DrawLine( pointTime.x, m_TopLineHeight+1, pointTime.x,(m_TopLineHeight+m_DrawAreaRect.height+1) );
        label.Printf(_T("%02d:%02d"), hour,min);
        dc->GetTextExtent(label, &width, &height, 0, 0, g_pFontSmall);
        dc->DrawText(label, pointTime.x-width/2, m_WindowRect.height-height);
        done=hour*100+min;
        prevfiverfit = min;
        } // then avoid double printing in faster devices

      }
    }
  }
}
void TacticsInstrument_BaroHistory::OnLogDataButtonPressed(wxCommandEvent& event) {

  if (m_isExporting == false) {
    wxPoint pos;
    m_LogButton->GetSize(&pos.x, &pos.y);
    pos.x = 0;
    this->PopupMenu(m_pExportmenu, pos);
    if (btn10Sec->IsChecked()) m_exportInterval = 10;
    if (btn20Sec->IsChecked()) m_exportInterval = 20;
    if (btn60Sec->IsChecked()) m_exportInterval = 60;

    wxFileDialog fdlg(GetOCPNCanvasWindow(), _("Choose a new or existing file"), wxT(""), m_logfile, wxT("*.*"), wxFD_SAVE);
    if (fdlg.ShowModal() != wxID_OK) {
      return;
    }
    m_logfile.Clear();
    m_logfile = fdlg.GetPath();
    bool exists = m_ostreamlogfile.Exists(m_logfile);
    m_ostreamlogfile.Open(m_logfile, wxFile::write_append);
    if (!exists) {
      wxString str_ticks = g_bDataExportClockticks ? wxString::Format(_("ClockTicks%s"), g_sDataExportSeparator) : _("");
      wxString str_utc = g_bDataExportUTC ? wxString::Format(_("UTC-ISO8601%s"), g_sDataExportSeparator) : _("");

      wxString str = wxString::Format(_T("%s%s%s%s%s%s%s\n"), str_ticks, str_utc, "Date", g_sDataExportSeparator, "local Time", g_sDataExportSeparator, "Pressure");
      m_ostreamlogfile.Write(str);
    }
    SaveConfig(); //save the new export-rate &filename to opencpn.ini
    m_isExporting = true;
    m_LogButton->SetLabel(_("X"));
    m_LogButton->Refresh();
  }
  else if (m_isExporting == true) {
    m_isExporting = false;
    m_ostreamlogfile.Close();
    m_LogButton->SetLabel(_(">"));
    m_LogButton->Refresh();
  }
}
/***************************************************************************************
****************************************************************************************/
bool TacticsInstrument_BaroHistory::LoadConfig(void)
{
  wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

  if (pConf) {
    pConf->SetPath(_T("/PlugIns/Tactics/BaroHistory"));
    pConf->Read(_T("Exportrate"), &m_exportInterval, 5);
    pConf->Read(_T("BaroHistoryExportfile"), &m_logfile, wxEmptyString);
    return true;
  }
  else
    return false;
}
/***************************************************************************************
****************************************************************************************/
bool TacticsInstrument_BaroHistory::SaveConfig(void)
{
  wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

  if (pConf)
  {
    pConf->SetPath(_T("/PlugIns/Tactics/BaroHistory"));
    pConf->Write(_T("Exportrate"), m_exportInterval);
    pConf->Write(_T("BaroHistoryExportfile"), m_logfile);
    return true;
  }
  else
    return false;
}
void TacticsInstrument_BaroHistory::ExportData(void)
{
  if (m_isExporting == true) {
    wxDateTime localTime(m_ArrayRecTime[BARO_RECORD_COUNT - 1]);
/*    if (m_TimerSynched ==false) {
      
      if (localTime.GetSecond() % m_exportInterval == 0) {//remember the second of the first run
        m_BaroHistUpdTimer.Stop();
        m_BaroHistUpdTimer.Start(5000, wxTIMER_CONTINUOUS);
        m_TimerSynched = true;
      }
    }
    if (localTime.GetSecond() % m_exportInterval == 0) { */
        wxString str_utc, ticks;
      if (g_bDataExportUTC) {
        wxDateTime utc = localTime.ToUTC();
        str_utc = wxString::Format(_T("%sZ%s"), utc.FormatISOCombined('T'), g_sDataExportSeparator);
      }
      else
        str_utc = _T("");
      if (g_bDataExportClockticks) {
        wxLongLong ti = localTime.GetValue();
        ticks = wxString::Format(_T("%s%s"), ti.ToString(), g_sDataExportSeparator);
      }
      else
        ticks = _T("");
      wxString str = wxString::Format(_T("%s%s%s%s%s%s%4.1f\n"), ticks, str_utc, localTime.FormatDate(), g_sDataExportSeparator, localTime.FormatTime(), g_sDataExportSeparator, m_Press);
      m_ostreamlogfile.Write(str);
    //}
  }
}

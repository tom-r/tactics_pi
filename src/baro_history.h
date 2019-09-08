/******************************************************************************
 * $Id: baro_history.h, v1.0 2014/02/10 tom-r Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Tactics Plugin
 * Author:   stedy
 * Based on code from  Thomas Rauch
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

#ifndef __BARO_HISTORY_H__
#define __BARO_HISTORY_H__

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

// Warn: div by 0 if count == 1
#define BARO_RECORD_COUNT 3000

#include "instrument.h"
#include "dial.h"
#include <wx/filename.h>


class TacticsInstrument_BaroHistory: public TacticsInstrument
{
      public:
            TacticsInstrument_BaroHistory( wxWindow *parent, wxWindowID id, wxString title);

            ~TacticsInstrument_BaroHistory(void);

            void SetData(int, double, wxString);
            wxSize GetSize( int orient, wxSize hint );


      private:
           int m_soloInPane ;
           wxFileConfig  *m_pconfig;
           bool LoadConfig(void);
           bool SaveConfig(void);
           bool m_TimerSynched;
      protected:
           double m_ArrayPressHistory[BARO_RECORD_COUNT];

           wxDateTime::Tm m_ArrayRecTime[BARO_RECORD_COUNT];

           double m_MaxPress;  //...in array
           double m_MinPress;  //...in array
           double m_TotalMaxPress; // since O is started
           double m_TotalMinPress;
           double m_Press;
           double m_MaxPressScale;
           double m_ratioW;

           bool m_IsRunning;
           int m_SampleCount;
           wxTimer m_BaroHistUpdTimer;

           wxRect m_WindowRect;
           wxRect m_DrawAreaRect; //the coordinates of the real darwing area
           int m_DrawingWidth,m_TopLineHeight,m_DrawingHeight;
           int m_width,m_height;
           int m_LeftLegend, m_RightLegend;
        
           wxString    m_logfile;        //for data export
           wxFile      m_ostreamlogfile; //for data export
           bool        m_isExporting;      //for data export
           int         m_exportInterval; //for data export
           wxButton    *m_LogButton;     //for data export
           wxMenu      *m_pExportmenu;//for data export
           wxMenuBar   *m_pExportmenuBar;//for data export
           wxMenuItem* btn10Sec;
           wxMenuItem* btn20Sec;
           wxMenuItem* btn60Sec;

           void Draw(wxGCDC* dc);
           void DrawBackground(wxGCDC* dc);
           void DrawForeground(wxGCDC* dc);
           void DrawPressureScale(wxGCDC* dc);
           void OnLogDataButtonPressed(wxCommandEvent& event);
           void OnBaroHistUpdTimer(wxTimerEvent & event);
           void ExportData(void);

};



#endif // __BARO_HISTORY_H__


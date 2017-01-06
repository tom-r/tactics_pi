/***************************************************************************
* $Id: bearingcompass.h, v1.0 2016/06/07 tomBigSpeedy Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Thomas Rauch
*       (Inspired by original work from Jean-Eudes Onfray)
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

#ifndef __PolarCompass_H__
#define __PolarCompass_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#include <wx/fileconf.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "dial.h"

class ExpSmooth;
class DoubleExpSmooth;

#define COGRANGE 60
//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    TacticsInstrument_PolarCompass
//|
//| DESCRIPTION:
//|    This class creates a compass style control with Bearing Pointer
//|
//+------------------------------------------------------------------------------

class TacticsInstrument_PolarCompass : public TacticsInstrument_Dial
{
      public:
		  TacticsInstrument_PolarCompass(wxWindow *parent, wxWindowID id, wxString title, int cap_flag);

		  ~TacticsInstrument_PolarCompass(void){ SaveConfig(); }

            void SetData(int, double, wxString);
			bool SaveConfig(void);
			double m_Bearing,  m_ExtraValueDTW, m_CurrDir, m_CurrSpeed, m_currAngleStart, m_TWA, m_TWD, m_AWA,m_TWS, m_Hdt, m_Leeway;
            double m_PolSpd,m_PolSpd_Percent;
            //double m_RMBlat, m_RMBlon;
			double m_diffCogHdt;
			double m_lat, m_lon,m_StW,m_predictedSog;
			//double m_AngleStart;
            wxString m_BearingUnit, m_ExtraValueDTWUnit, m_ToWpt, m_CurrDirUnit, m_CurrSpeedUnit, m_StWUnit, m_curTack, m_targetTack, m_LeewayUnit;
			//double m_ExpSmoothDegRange, m_oldExpSmoothDegRange, m_alphaDeltaCog, alpha_diffCogHdt ;
			double m_ExpSmoothDegRange, alpha_diffCogHdt;
			//double m_LaylineDegRange, m_minLaylineDegRange, m_maxLaylineDegRange, m_COGRange[COGRANGE], m_Cog;
			double m_LaylineDegRange, m_COGRange[COGRANGE], m_Cog;
			double	m_ExpSmoothDiffCogHdt, m_oldExpSmoothDiffCogHdt;
			ExpSmooth  *mExpSmDegRange;

      private:
		  bool LoadConfig(void);
		  wxFileConfig     *m_pconfig;

      protected:
            void DrawBackground(wxGCDC* dc);
			void DrawForeground(wxGCDC* dc);
			void DrawBearing(wxGCDC* dc);
			void DrawWindAngles(wxGCDC* dc);
            void DrawBoat(wxGCDC* dc, int cx, int cy, int radius);
            void DrawPolar(wxGCDC* dc);
            void DrawTargetxMGAngle(wxGCDC* dc);
            void DrawTargetAngle(wxGCDC* dc, double TargetAngle, wxString color1, int size);
			void DrawLaylines(wxGCDC* dc);
			virtual void DrawData(wxGCDC* dc, double value, wxString unit, wxString format, DialPositionOption position);
			virtual void Draw(wxGCDC* dc);
			void CalculateLaylineDegreeRange(void);
};

#endif // __PolarCompass_H__


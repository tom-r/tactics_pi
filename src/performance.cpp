/**************************************************************************
* $Id: performance.cpp, v1.0 2016/06/07 tom_BigSpeedy Exp $
*
* Project:  OpenCPN
* Purpose:  tactics Plugin
* Author:   Thomas Rauch
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
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 
#include <wx/math.h>
#include <wx/stdpaths.h>
#include <wx/progdlg.h>
#include <wx/gdicmn.h>
#include <wx/fileconf.h>
#include "nmea0183/nmea0183.h"

#include "performance.h"
#include <map>

extern Polar* BoatPolar;
extern wxString g_path_to_PolarLookupOutputFile;
extern wxString g_path_to_PolarFile;
extern int g_iDashWindSpeedUnit;
extern int g_iDashSpeedUnit;
extern PlugIn_Waypoint *m_pMark;
extern int g_iSpeedFormat;

// ----------------------------------------------------------------
//
//    TacticsInstrument_Simple Implementation
//
//----------------------------------------------------------------
TacticsInstrument_PerformanceSingle::TacticsInstrument_PerformanceSingle(wxWindow *pparent, wxWindowID id, wxString title, int cap_flag, wxString format)
	:TacticsInstrument(pparent, id, title, cap_flag)
{
	m_format = format;
	m_data = _T("---");
	m_pconfig = GetOCPNConfigObject();
//	BoatPolar = new Polar(this);
//	BoatPolar->loadPolar(_T(""));
	mTWS = NAN;
	mHDT = NAN;
	mTWA = NAN;
	mSTW = NAN;
	mSOG = NAN;
	mCOG = NAN;
	mBRG = -1;
    mTWD = NAN;
	stwunit = _T("");

}
/***********************************************************************************

************************************************************************************/
wxSize TacticsInstrument_PerformanceSingle::GetSize(int orient, wxSize hint)
{
	wxClientDC dc(this);
	int w;
	dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
	dc.GetTextExtent(_T("000"), &w, &m_DataHeight, 0, 0, g_pFontData);

	if (orient == wxHORIZONTAL) {
		return wxSize(DefaultWidth, wxMax(hint.y, m_TitleHeight + m_DataHeight));
	}
	else {
		return wxSize(wxMax(hint.x, DefaultWidth), m_TitleHeight + m_DataHeight);
	}
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_PerformanceSingle::Draw(wxGCDC* dc)
{
	wxColour cl;
#ifdef __WXMSW__
	wxBitmap tbm(dc->GetSize().x, m_DataHeight, -1);
	wxMemoryDC tdc(tbm);
	wxColour c2;
	GetGlobalColor(_T("DASHB"), &c2);
	tdc.SetBackground(c2);
	tdc.Clear();

	tdc.SetFont(*g_pFontData);
	GetGlobalColor(_T("DASHF"), &cl);
	tdc.SetTextForeground(cl);

	tdc.DrawText(m_data, 10, 0);

	tdc.SelectObject(wxNullBitmap);

	dc->DrawBitmap(tbm, 0, m_TitleHeight, false);
#else
	dc->SetFont(*g_pFontData);
	GetGlobalColor(_T("DASHF"), &cl);
	dc->SetTextForeground(cl);

	dc->DrawText(m_data, 10, m_TitleHeight);

#endif

}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_PerformanceSingle::SetDisplayType(int type){
	m_displaytype = type;
}
/***********************************************************************************

************************************************************************************/
void TacticsInstrument_PerformanceSingle::SetData(int st, double data, wxString unit)
{

			if (st == OCPN_DBP_STC_STW){

              //mSTW = data;
              //convert to knots first
              mSTW = fromUsrSpeed_Plugin(data, g_iDashSpeedUnit);
			  stwunit = unit;
			}
			else if (st == OCPN_DBP_STC_TWA){
				mTWA = data;
			}
            else if (st == OCPN_DBP_STC_COG){
              mCOG = data;
			}
			else if (st == OCPN_DBP_STC_SOG){
              //convert to knots first
              //mSOG = data;
              mSOG = fromUsrSpeed_Plugin(data, g_iDashSpeedUnit);
            }
            else if (st == OCPN_DBP_STC_LAT) {
              m_lat = data;
            }
            else if (st == OCPN_DBP_STC_LON) {
              m_lon = data;
            }
			else if (st == OCPN_DBP_STC_BRG){
			  mBRG = data;
              /*if (!GetSingleWaypoint(_T("TacticsWP"), m_pMark)){
                mBRG = data;
                //m_ToWpt = unit;
                //wxLogMessage(_T("RMB mBRG=%.2f"), mBRG);
              }
              else{
                if (m_pMark && m_lat>0 && m_lon>0) {
                  double dist;
                  DistanceBearingMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, m_lat, m_lon, &mBRG, &dist);
                  //m_ToWpt = _T("TacticsWP");
                 // wxLogMessage(_T("TacticsWP m_pMark->m_lat=%f, m_pMark->m_lon=%f, m_lat=%f, m_lon=%f, mBRG=%.2f"), m_pMark->m_lat, m_pMark->m_lon, m_lat, m_lon,mBRG);

                }
              }*/
			}
			else if (st == OCPN_DBP_STC_TWS){
              //mTWS = data;
              //convert to knots
              mTWS = fromUsrSpeed_Plugin(data, g_iDashWindSpeedUnit);
			}
			else if (st == OCPN_DBP_STC_HDT){
				mHDT = data;
			}
            else if (st == OCPN_DBP_STC_TWD){
              mTWD = data;
            }
            if (!GetSingleWaypoint(_T("TacticsWP"), m_pMark)) m_pMark = NULL;
            if (m_pMark && m_lat > 0 && m_lon > 0) {
                double dist;
                DistanceBearingMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, m_lat, m_lon, &mBRG, &dist);
                //m_ToWpt = _T("TacticsWP");
            }
			if (!wxIsNaN(mSTW) && !wxIsNaN(mTWA) && !wxIsNaN(mTWS)){

				if (m_displaytype == POLARSPEED){
                  double targetspeed = BoatPolar->GetPolarSpeed(mTWA, mTWS);
                  //double avgtargetspeed = BoatPolar->GetAvgPolarSpeed(mTWA, mTWS);

					if (wxIsNaN(targetspeed) || mSTW == 0)
						m_data = _T("no polar data");
					else {
                        double percent = mSTW / targetspeed * 100;
                        double user_targetSpeed = toUsrSpeed_Plugin(targetspeed, g_iDashSpeedUnit);
                        m_data = wxString::Format("%d", wxRound(percent)) + _T(" % / ") + wxString::Format("%.2f ", user_targetSpeed) + stwunit;
                        //m_data = wxString::Format("%.2f / ", avgtargetspeed) + wxString::Format("%.2f", user_targetSpeed) + _T(" ") + stwunit;
                    }
				}
				else if (m_displaytype == POLARVMG){
					double VMG = BoatPolar->Calc_VMG(mTWA,mSTW);
                    double user_VMG = toUsrSpeed_Plugin(VMG, g_iDashSpeedUnit);

					m_data = wxString::Format("%.2f", user_VMG) + _T(" ") + stwunit;

				}
				else if (m_displaytype == POLARTARGETVMG){
                  TargetxMG targetVMG = BoatPolar->Calc_TargetVMG(mTWA, mTWS);
					if (targetVMG.TargetSpeed > 0) {
						double VMG = BoatPolar->Calc_VMG(mTWA, mSTW);
						double percent = fabs(VMG / targetVMG.TargetSpeed * 100.);
                        targetVMG.TargetSpeed = toUsrSpeed_Plugin(targetVMG.TargetSpeed, g_iDashSpeedUnit);

						m_data = wxString::Format("%d", wxRound(percent)) + _T(" % / ") + wxString::Format("%.2f", targetVMG.TargetSpeed) + _T(" ") + stwunit;
					}
					else
						m_data =  _T("--- % / --- ") + stwunit;

				}
				else if (m_displaytype == POLARTARGETVMGANGLE){
					TargetxMG targetVMG = BoatPolar->Calc_TargetVMG(mTWA, mTWS);
					if (!wxIsNaN(targetVMG.TargetAngle))
					    m_data = wxString::Format("%.0f", targetVMG.TargetAngle) + _T("\u00B0");
					else
						m_data = _T("no polar data");
				}
			}
    		else
			  m_data = _T("---");

			if (m_displaytype == POLARCMG){
				if (!wxIsNaN(mSOG) && !wxIsNaN(mCOG) && mBRG>=0) {
				  mCMG = BoatPolar->Calc_CMG(mCOG, mSOG, mBRG);
                  double user_CMG = toUsrSpeed_Plugin(mCMG, g_iDashSpeedUnit);
				   m_data = wxString::Format("%.2f", user_CMG) + _T(" ") + stwunit;
				}
				else
				   m_data = _T("no bearing");

			}
			else if (m_displaytype == POLARTARGETCMG){
              //TargetxMG targetCMG = BoatPolar->Calc_TargetCMG(mTWS, mTWD, mBRG);
              TargetxMG TCMGMax, TCMGMin;
	          TCMGMax.TargetSpeed = NAN;
              if (!wxIsNaN(mTWS) && !wxIsNaN(mTWD) && mBRG>=0)
		        BoatPolar->Calc_TargetCMG2 (mTWS, mTWD, mBRG, &TCMGMax, &TCMGMin);
                //if (!wxIsNaN(targetCMG.TargetSpeed) && targetCMG.TargetSpeed > 0) {
              if (!wxIsNaN(TCMGMax.TargetSpeed) && TCMGMax.TargetSpeed > 0 && !wxIsNaN(mHDT) && !wxIsNaN(mSTW)) {
					double cmg = BoatPolar->Calc_CMG(mHDT, mSTW, mBRG);
                    if (!wxIsNaN(cmg) )//&& cmg >=0)
                    {
                      //double percent = fabs(cmg / targetCMG.TargetSpeed * 100.);
                      double percent = cmg / TCMGMax.TargetSpeed * 100.;
                      TCMGMax.TargetSpeed = toUsrSpeed_Plugin(TCMGMax.TargetSpeed, g_iDashSpeedUnit);
                      m_data = wxString::Format("%d", wxRound(percent)) + _T(" % / ") + wxString::Format("%.2f", TCMGMax.TargetSpeed) + _T(" ") + stwunit;
                    }
                    else
                      m_data = _T("--- % / --- ") + stwunit;
				}
				else
					m_data = _T("--- % / --- ") + stwunit;

			}
            else if (m_displaytype == POLARTARGETCMGANGLE){
              if (!wxIsNaN(mSTW) && mBRG >= 0 && !wxIsNaN(mHDT)) {
                double cmg = BoatPolar->Calc_CMG(mHDT, mSTW, mBRG);
                TargetxMG TCMGMax, TCMGMin;
                TCMGMax.TargetAngle = NAN;
                if (!wxIsNaN(mTWS) && !wxIsNaN(mTWD) && mBRG >= 0)
                  BoatPolar->Calc_TargetCMG2(mTWS, mTWD, mBRG, &TCMGMax, &TCMGMin);
                if (!wxIsNaN(TCMGMax.TargetAngle))
                  m_data = wxString::Format("%.0f", TCMGMax.TargetAngle) + _T("\u00B0");
                else
                  m_data = _T("no polar data");
              }
              else
                m_data = _T("no data");
            }
            else if (m_displaytype == TWAMARK){
              if (mBRG>=0 && !wxIsNaN(mTWD)) {
                double markBrG = getDegRange(mBRG, mTWD);
                m_data = wxString::Format("%.0f",(double) markBrG) + _T("\u00B0");
              }
              else
                m_data = _T("---");
            }
}
/***********************************************************************************

************************************************************************************/
Polar::Polar(TacticsInstrument_PerformanceSingle* parent)
{

	m_pconfig = GetOCPNConfigObject();
	mode = 0;
	windSpeed = -1;
	windAngle = -1;
	windReference = wxEmptyString;

	timeout = 5;
	wxString s = wxFileName::GetPathSeparator();

	wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
#ifdef __WXMSW__
	wxString stdPath = std_path.GetConfigDir();
#endif
#ifdef __WXGTK__
	wxString stdPath = std_path.GetUserDataDir();
#endif
#ifdef __WXOSX__
	wxString stdPath = std_path.GetUserConfigDir();   // should be ~/Library/Preferences
	stdPath += s + _T("opencpn");
#endif

	wxString basePath = stdPath + s + _T("plugins") + s + _T("tactics_pi") + s + _T("data") + s;
	logbookDataPath = basePath;

	reset();
}
Polar::Polar(tactics_pi* parent)
{

  m_pconfig = GetOCPNConfigObject();
  mode = 0;
  windSpeed = -1;
  windAngle = -1;
  windReference = wxEmptyString;

  timeout = 5;
  wxString s = wxFileName::GetPathSeparator();

  wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
#ifdef __WXMSW__
  wxString stdPath = std_path.GetConfigDir();
#endif
#ifdef __WXGTK__
  wxString stdPath = std_path.GetUserDataDir();
#endif
#ifdef __WXOSX__
  wxString stdPath = std_path.GetUserConfigDir();   // should be ~/Library/Preferences
  stdPath += s + _T("opencpn");
#endif

  wxString basePath = stdPath + s + _T("plugins") + s + _T("tactics_pi") + s + _T("data") + s;
  logbookDataPath = basePath;

  reset();
}
/***********************************************************************************
Destructor
************************************************************************************/
Polar::~Polar(void)
{
}
/***********************************************************************************
Load a new polar file and read it into the polar array
Parameter
(input) FilePath = path to polar file;
_T("")     : read from opencpn.ini file;
_T("NULL") : just initialize array (to work w/o polar)
************************************************************************************/
void Polar::loadPolar(wxString FilePath)
{
	wxString filePath = _T("NULL");
    wxString fname = _T("");
	wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

	if (FilePath == _T("")) { //input parameter empty, read from config

		if (pConf) {

			pConf->SetPath(_T("/PlugIns/Tactics/Performance"));

			pConf->Read(_T("PolarFile"), &filePath, _T("NULL"));
			fname = filePath;

		}
		if (filePath == _T("NULL")) {
			wxFileDialog fdlg(GetOCPNCanvasWindow(), _("tactics_pi: Select a Polar-File"), _T(""));
			if (fdlg.ShowModal() == wxID_CANCEL) return;
			filePath = fdlg.GetPath();
			fname = fdlg.GetFilename();

		}
	}
	else{
		filePath = FilePath;
        fname = filePath;
	}

	reset();

    if (filePath != _T("NULL")) {  //TR23.04.
      wxFileInputStream stream(filePath);
      wxTextInputStream in(stream);
      wxString wdirstr, wsp;

      bool first = true;
      int mode = -1, row = -1, sep = -1;
      wxArrayString WS, WSS;

      while (!stream.Eof())
      {
        int col = 0, i = 0, x = 0;
        wxString s;

        wxString str = in.ReadLine();				// read line by line
        if (stream.Eof()) break;
        if (first)
        {
          WS = wxStringTokenize(str, _T(";,\t "));
          WS[0] = WS[0].Upper();
          if (WS[0].Find(_T("TWA\\TWS")) != -1 || WS[0].Find(_T("TWA/TWS")) != -1 || WS[0].Find(_T("TWA")) != -1)
          {
            mode = 1;
            sep = 1;
          }
          else if (WS[0].IsNumber())
          {
            mode = 2;
            sep = 1;
            //x = wxAtoi(WS[0]);
            //col = (x + 1) / 2 - 1;
            col = wxAtoi(WS[0]);

            for (i = 1; i < (int)WS.GetCount(); i += 2)
            {
              //x = wxAtoi(WS[i]);
              //row = (x + 2) / 5 - 1;
              row = wxAtoi(WS[i]);
              s = WS[i + 1];

              if (col > WINDSPEED - 1) break;
              if (s == _T("0") || s == _T("0.00") || s == _T("0.0") || s == _T("0.000")){
                continue;
              }
              if (col < WINDSPEED + 1) {
                setValue(s, row, col);
              }
            }
          }
          else if (!WS[0].IsNumber()){
            continue;
          }

          if (sep == -1){
            wxMessageBox(_("Format in this file not recognised"));
            return;
          }

          first = false;
          if (mode != 0)
            continue;
        }
        if (mode == 1) // Formats OCPN/QTVlm/MAXSea/CVS 
        {
          WSS = wxStringTokenize(str, _T(";,\t "));
          if (WSS[0] == _T("0") && mode == 1)
          {
            row++; continue;
          }
          else if (row == -1)
            row++;
          row = wxAtoi(WSS[0]);
          for (i = 1; i < (int)WSS.GetCount(); i++)
          {
            s = WSS[i];
            if (col > WINDSPEED - 1) break;
            if (s == _T("0") || s == _T("0.00") || s == _T("0.0") || s == _T("0.000")){
              continue;
            }
            col = wxAtoi(WS[i]);
            setValue(s, row, col);
          }
        }

        if (mode == 2) // Format Expedition
        {
          WS = wxStringTokenize(str, _T(";,\t "));
          //x = wxAtoi(WS[0]);
          //col = (x + 1) / 2 - 1;
          col = wxAtoi(WS[0]);

          for (i = 1; i < (int)WS.GetCount(); i += 2)
          {
            //x = wxAtoi(WS[i]);
            //row = (x + 2) / 5 - 1;
            row = wxAtoi(WS[i]);
            s = WS[i + 1];
            if (col > WINDSPEED - 1) break;
            if (s == _T("0") || s == _T("0.00") || s == _T("0.0") || s == _T("0.000"))
            {
              continue;
            }
            //if (col < 21)
            if (col < WINDSPEED + 1){
              setValue(s, row, col);
            }
          }
        }
      }
      completePolar();
      g_path_to_PolarFile = filePath;
      if (pConf) {
        pConf->SetPath(_T("/PlugIns/Tactics/Performance"));
        pConf->Write(_T("PolarFile"), g_path_to_PolarFile);
      }

    } //TR23.04.
// temporary output of Polar-Lookuptable to file ...
	wxString path_to_PolarLookupOutputFile = _T("NULL");
	if (pConf) {
		pConf->SetPath(_T("/PlugIns/Tactics/Performance"));
		pConf->Read(_T("PolarLookupTableOutputFile"), &path_to_PolarLookupOutputFile, _T("NULL"));
	}

	//wxString file = _T("C:/TEMP/Polar.txt");
	//wxFileOutputStream outstream(file);
	if (path_to_PolarLookupOutputFile != _T("NULL")){
		wxFileOutputStream outstream(path_to_PolarLookupOutputFile);
		wxTextOutputStream out(outstream);

		wxString str = _T("d/s");
        for (int i = 0; i <= WINDSPEED; i++){
			str = wxString::Format(_T("%s\t%02d"), str, i);
		}
		str = str + _T("\n");
		out.WriteString(str);				// write line by line
		for (int n = 0; n < WINDDIR; n++){
			str = wxString::Format(_T("%d\t"), n);
            for (int i = 0; i <= WINDSPEED; i++){
				str = wxString::Format(_T("%s\t%.2f"), str, windsp[i].winddir[n]);
			}
			str = str + _T("\n");
			out.WriteString(str);				// write line by line

		}
		outstream.Close();
	}
	//wxMessageBox(_T("Polar ") + fname + _T(" loaded"));
}
/***********************************************************************************

************************************************************************************/
void Polar::setValue(wxString s, int dir, int spd)
{
	s.Replace(_T(","), _T("."));
	double speed = wxAtof(s);
   
    //if (speed > 0.0 && speed <= WINDSPEED && dir >= 0 && dir <WINDDIR)
    if (spd > 0 && spd <= WINDSPEED && dir >= 0 && dir <WINDDIR)
      {
		windsp[spd].winddir[dir] = speed;
		windsp[spd].isfix[dir] = true;
//for cmg : fill the second half of the polar
        windsp[spd].winddir[360-dir] = speed;
        windsp[spd].isfix[360- dir] = true;
    }
}
/***********************************************************************************
Clear the polar lookup table with default values
************************************************************************************/
void Polar::reset()
{
	for (int n = 0; n < WINDDIR; n++) {
			windsp[0].winddir[n] = 0;
			windsp[0].isfix[n] = false;
	}
	for (int i = 1; i <= WINDSPEED; i++) {
		for (int n = 0; n < WINDDIR; n++) {
			windsp[i].winddir[n] = NAN;
			windsp[i].isfix[n] = false;
		}
	}
    for (int i = 0; i <= WINDSPEED; i++) {
      tws[i].tvmg_up.TargetAngle = tws[i].tvmg_up.TargetSpeed = 0;
      tws[i].tvmg_dn.TargetAngle = tws[i].tvmg_dn.TargetSpeed = 0;
    }
}

/***********************************************************************************
Complete the boat polar, i.e. create the lookup table
Calculates the intermediate values per degree & knot of the given polar
No extrapolation.
************************************************************************************/
void Polar::completePolar()
{
	for (int n = 0; n < WINDDIR; n++)
	{
		int min_index = WINDSPEED;
		int max_index = 0;
		int i = 0;
		bool ret = false;
		//get min/max index (i) with data.
		//first we fill the gaps in the lines, between existing values
		while (i <= WINDSPEED) {
			if (!wxIsNaN(windsp[i].winddir[n]))
			{
				if (i < min_index) min_index = i;
				if (i > max_index) max_index = i;
				ret = true;
			}
			if (ret == true) CalculateLineAverages(n, min_index, max_index);
			i++;
		}
	}
	for (int i = 0; i <= WINDSPEED; i++)
	{
		int min_index = WINDDIR;
		int max_index = 0;
		int n = 0;
		bool ret = false;
		//get min/max index (i) with data.
		//now we fill the gaps in the rows, between existing values
		while (n < WINDDIR) {
			if (!wxIsNaN(windsp[i].winddir[n]))
			{
				if (n < min_index) min_index = n;
				if (n > max_index) max_index = n;
				ret = true;
			}
			if (ret == true) CalculateRowAverages(i, min_index, max_index);
			n++;
		}
	}
    //fill the TargetVMG lookup table now
    for (int i = 0; i <= WINDSPEED; i++){
      tws[i].tvmg_up = Calc_TargetVMG(45.0, (double)i);
      tws[i].tvmg_dn = Calc_TargetVMG(120.0, (double)i);
    }
    //for (int j = 0; j <= WINDSPEED; j++){
    //  wxLogMessage("TWS=%d, UP: TargetAngle=%.2f, TargetSpeed=%.2f, DOWN: TargetAngle=%.2f, TargetSpeed=%.2f", j, tws[j].tvmg_up.TargetAngle, tws[j].tvmg_up.TargetSpeed, tws[j].tvmg_dn.TargetAngle, tws[j].tvmg_dn.TargetSpeed);
    //}
}
TargetxMG Polar::GetTargetVMGUpwind(double TWS)
{
  return( tws[wxRound(TWS)].tvmg_up);
}
TargetxMG Polar::GetTargetVMGDownwind(double TWS)
{
  return(tws[wxRound(TWS)].tvmg_dn);
}

/***********************************************************************************

************************************************************************************/
void Polar::CalculateLineAverages(int n, int min, int max)
{
	int j;
	int cur_min;
	int count;
	j = min;
	cur_min = min;
	while (j <= max) {
		j++;
		count = 0;
		while (j <= max && wxIsNaN(windsp[j].winddir[n])) // find next cell which is NOT empty
		{
			j++;
		}
		count = j - cur_min;
        for (int k = cur_min + 1, m = 1; k < cur_min + count; k++, m++){
          windsp[k].winddir[n] = windsp[cur_min].winddir[n] + (windsp[j].winddir[n] - windsp[cur_min].winddir[n]) / count * m;
//TR temp, fill 2nd half
         // windsp[k].winddir[360-n] = windsp[k].winddir[n];
        }
		cur_min = j;
	}

}
/***********************************************************************************

************************************************************************************/
void Polar::CalculateRowAverages(int i, int min, int max)
{
	int j;
	int cur_min;
	int count;
	j = min;
	cur_min = min;
	while (j <= max) {
		j++;
		count = 0;
		while (j <= max && wxIsNaN(windsp[i].winddir[j])) // find next cell which is NOT empty
		{
			j++;
		}
		count = j - cur_min;
		for (int k = cur_min + 1, m = 1; k < cur_min + count; k++, m++)
			windsp[i].winddir[k] = windsp[i].winddir[cur_min] + (windsp[i].winddir[j] - windsp[i].winddir[cur_min]) / count * m;

		cur_min = j;
	}

}
/***********************************************************************************
Return the polar speed with averaging of wind speed.
We're still roúnding the TWA, as this is a calculated value anyway and I doubt
it will have an accuracy < 1°.
With this simplified approach of averaging only TWS we can reduce some load ...
************************************************************************************/
double Polar::GetPolarSpeed(double twa, double tws)
{
  //original w/o averaging:
  //return (windsp[wxRound(tws)].winddir[wxRound(twa)]);
  double  fws, avspd1, avspd2;
  int twsmin, i_twa;

//wxLogMessage("-- GetPolarSpeed() - twa=%f tws=%f", twa, tws);
  if (wxIsNaN(twa) || wxIsNaN(tws))
      return NAN;
  // to do : limits to be checked (0°, 180°, etc.)
  i_twa = wxRound(twa); //the next lower full true wind angle value of the polar array
  twsmin = (int)tws; //the next lower full true wind speed value of the polar array
  fws = tws - twsmin; // factor tws (how much are we above twsmin)
  //do the vertical averaging btw. the 2 surrounding polar twa angles
  avspd1 = windsp[twsmin].winddir[i_twa] ;
  avspd2 = windsp[twsmin + 1].winddir[i_twa];
  // now do the horizontal averaging btw. the 2 surrounding polar tws values ...
  //if (wxIsNaN(avspd1) || wxIsNaN(avspd1))
    return ((wxIsNaN(avspd1) || wxIsNaN(avspd1))?NAN: avspd1 + (avspd2 - avspd1)*fws);
}
/***********************************************************************************
Get the polar speed with full averaging of the input data of both TWA and TWS.
The polar is stored as a lookup table (2dim array) in steps of 1 kt / 1°.
Instead of rounding up/down to the next full value as done in original GetPolarSpeed() we're
averaging both TWA & TWS.
Currently not used ...
************************************************************************************/
double Polar::GetAvgPolarSpeed(double twa, double tws)
{
  double fangle, fws,  avspd1, avspd2, av_Spd;
  int twsmin, twamin;

  // to do : limits to be checked (0°, 180°, etc.)
  twamin = (int)twa; //the next lower full true wind angle value of the polar array
  twsmin = (int)tws; //the next lower full true wind speed value of the polar array
  fangle = twa - twamin; //factor twa (how much are we above twamin)
  fws = tws - twsmin; // factor tws (how much are we above twsmin)
  //do the vertical averaging btw. the 2 surrounding polar twa angles
  avspd1 = windsp[twsmin].winddir[twamin] + (windsp[twsmin].winddir[twamin + 1] - windsp[twsmin].winddir[twamin])*fangle;
  avspd2 = windsp[twsmin + 1].winddir[twamin] + (windsp[twsmin + 1].winddir[twamin + 1] - windsp[twsmin + 1].winddir[twamin])*fangle;
  // now do the horizontal averaging btw. the 2 surrounding polar tws values.
  av_Spd = avspd1 + (avspd2 - avspd1)*fws;
  //wxLogMessage("TWA=%.1f,TWS=%.1f, =%f, av_Spd=%f", twa, tws, av_Spd);
  return av_Spd;
}

/***********************************************************************************
Basic VMG(Velocity made good) measured against the wind direction
************************************************************************************/
double Polar::Calc_VMG(double TWA, double StW)
{
	return fabs(StW * cos(TWA * M_PI / 180.) );
}
/***********************************************************************************
Calculate opt. VMG (angle & speed) for up- and downwind courses (w/o a bearing to a mark)
************************************************************************************/
TargetxMG Polar::Calc_TargetVMG(double TWA, double TWS)
{
	TargetxMG TVMG;
	TVMG.TargetAngle = -999;
	TVMG.TargetSpeed = -999;
	double calcvmg=0;
	int i_tws = wxRound(TWS);
	int k=0;
	if (TWA <90) { //upwind
		for (k = 1; k < 90; k++){
			if (!wxIsNaN(windsp[i_tws].winddir[k])){
				calcvmg = windsp[i_tws].winddir[k] * cos((double)(k*M_PI / 180.));
                if (calcvmg < 0) calcvmg = -calcvmg;
				if (calcvmg > TVMG.TargetSpeed ){
					TVMG.TargetSpeed = calcvmg;
					TVMG.TargetAngle = (double)k;
				}
			}
		}
	}
	if (TWA >= 90) {  //downwind
		for ( k = 180; k > 90; k--){
			if (!wxIsNaN(windsp[i_tws].winddir[k] ) ){
				calcvmg = windsp[i_tws].winddir[k] * cos((double)k*M_PI / 180.);
                if (calcvmg < 0) calcvmg = -calcvmg;
				//wxLogMessage("cosval=%f, calcvmg=%f", cosval, calcvmg);
				if (calcvmg > TVMG.TargetSpeed ) {
					TVMG.TargetSpeed = calcvmg;
					TVMG.TargetAngle = (double) k;
				}

			}
		}

	}
	if (TVMG.TargetAngle == -999)TVMG.TargetAngle = NAN;
	if (TVMG.TargetSpeed == -999)TVMG.TargetSpeed = NAN;
	return TVMG;
}
/***********************************************************************************
 Calculate CMG (Course made good) to bearing = the speed towards the bearing
************************************************************************************/
double Polar::Calc_CMG(double heading, double speed, double Brg)
{
	//double Cmg = speed * cos((heading - Brg)* M_PI / 180.);
  double Cmg = speed * cos((getDegRange(heading,Brg))* M_PI / 180.);
	//return fabs(Cmg);
    return Cmg;
}
/***********************************************************************************
Calculate opt. CMG (angle & speed) for up- and downwind courses with bearing to a mark

       TWD
 CMG    |   .  * * *
   o    .  * /       *
    \.  |*  /          *
     \  |  /            *  
      \ | /              *
       \|/               * 
                        *
************************************************************************************/

/*
  Calculate the target CMG (=VMC)
  The theoretical approach is to calculate the tangens from the bearing line to the polar curve.
  As this is not (easily) possible (or I don't know how to do), I use another approach :
  The procedure is to determine the diff-angle btw. TWD and BRG. Then we "rotate" the polar
  by this diff-angle. For the given windspeed, we can now query all boatspeeds from the polar
  in a range of -90°..diff-angle..+90° around the new vertical point (diff-angle), and find the max speed 
  with "boatspeed * cos (angle)"; the returned angle is the TWA-angle for opt. CMG
  with reference to TWD
*/

TargetxMG Polar::Calc_TargetCMG(double TWS, double TWD,  double BRG)
{
	TargetxMG TCMG,tcmg2;
	TCMG.TargetAngle = -999;
	TCMG.TargetSpeed = -999;
	double cmg = 0;

	int i_tws = wxRound(TWS);
    double range = getSignedDegRange(TWD, BRG);
    double absrange = range < 0 ? -range : range;
    double diffAngle = 0;
    int vPolarAngle = wxRound(range);  //polar is rotated by this angle, this is "vertical" now
	int k = 0;
    int curAngle = 0;
    int start = 0;
    int polang;
    int iIargetAngle = -999;
    start = vPolarAngle - 90; 
    if (start < 0) start += 360;  // oder 180 ?
    for (k = 0; k <= 180; k++){
      curAngle = k + start;
      if (curAngle > 359) curAngle -= 360;
      polang = curAngle;
      diffAngle = curAngle - range;
      if (diffAngle > 359) diffAngle -= 360;
      if (diffAngle < -359) diffAngle += 360;
      if (!wxIsNaN(windsp[i_tws].winddir[polang])){
        cmg = windsp[i_tws].winddir[polang] * cos(diffAngle*M_PI / 180.);
        if (cmg > TCMG.TargetSpeed) {
          TCMG.TargetSpeed = cmg;

          iIargetAngle = curAngle;
        }
      }
    }
    if (TCMG.TargetSpeed == -999)TCMG.TargetSpeed = NAN;
    if (iIargetAngle == -999)
      TCMG.TargetAngle = NAN;
    else
      TCMG.TargetAngle = (double)iIargetAngle;

    if (TCMG.TargetAngle > 180) TCMG.TargetAngle = 360. - TCMG.TargetAngle;
	return TCMG;
}
/**********************************************************************************
in certain cases there exists a second, lower cmg on the other tack
This routine returns both cmg's if available, otherwise NAN

in the small chart below :
cmg-max : is the higher of both cmg's, *TCMGMax in the routine below
cmg-2   : the second possible cmg
in general :
cmg = boat_speed*cos(hdg - brg)

HDG     : target - heading, based on polar
BRG     : Bearing to waypoint
TWD     : True Wind Direction

boat_speed = boat_speed at target-hdg = speed from polar

As the polar is rotated now (polar-0° is in TWD direction)--> hdg = polarangle + diffangle
with diffangle = angle btw.TWD and BRG

                ^
       TWD      | BRG   / HDG
         \      |______x______cmg-max
cmg-min   \     |  *  /   *
   __x_____\____|*   /      *
  *     *   \  *|   /        *
 *          *\* |  /         *
 *            \ | /         *
  *            \|/         *
   *            \         *
     *           \      *
        *         *   *
           *   *

**********************************************************************************************/
void Polar::Calc_TargetCMG2(double TWS, double TWD, double BRG, TargetxMG *TCMGMax, TargetxMG* TCMGMin)
{
  TargetxMG* TCMG1 = new TargetxMG;
  TargetxMG* TCMG2 = new TargetxMG;
  TCMG1->TargetAngle = -999;
  TCMG1->TargetSpeed = -999;
  TCMG2->TargetAngle = -999;
  TCMG2->TargetSpeed = -999;
  double cmg = 0;

  int i_tws = wxRound(TWS);  //still rounding here, not averaging ...to be done
  // wxLogMessage("-- Calc_TargetCMG2() - range?");
  double range = getSignedDegRange(TWD, BRG);
  // wxLogMessage("range =%f", range);
  double diffAngle = 0;
//  int vPolarAngle = wxRound(range);  //polar is rotated by this angle, this is "vertical" now
  int k = 0;
  int curAngle = 0;
  int start = 0;
  //start = vPolarAngle - 180;
  start = 0;
//  if (start < 0) start += 360;  // oder 180 ?
  for (k = 0; k <= 180; k++){
    curAngle = k + start;
    if (curAngle > 359) curAngle -= 360;
    diffAngle = curAngle - range;
    if (diffAngle > 359) diffAngle -= 360;
    if (diffAngle < -359) diffAngle += 360;
    if (!wxIsNaN(windsp[i_tws].winddir[curAngle])){
      cmg = windsp[i_tws].winddir[curAngle] * cos(diffAngle*M_PI / 180.);
      if (cmg > TCMG1->TargetSpeed){
        TCMG1->TargetSpeed = cmg;
        TCMG1->TargetAngle = (double)curAngle;
       }
    }
  }
  if (TCMG1->TargetSpeed <= 0){
    TCMG1->TargetSpeed = -999;
    TCMG1->TargetAngle = NAN;
  }

//  start = vPolarAngle ;
  start = 180;
//  if (start < 0) start += 360;  // oder 180 ?
  for (k = 0; k <= 180; k++){
    curAngle = k + start;
    if (curAngle > 359) curAngle -= 360;
    diffAngle = curAngle - range;
    if (diffAngle > 359) diffAngle -= 360;
    if (diffAngle < -359) diffAngle += 360;
    if (!wxIsNaN(windsp[i_tws].winddir[curAngle])){
      cmg = windsp[i_tws].winddir[curAngle] * cos(diffAngle*M_PI / 180.);
      if (cmg > TCMG2->TargetSpeed) {
        TCMG2->TargetSpeed = cmg;
        TCMG2->TargetAngle = (double)curAngle;
      }
    }
  }
  if (TCMG2->TargetSpeed <= 0){
    TCMG2->TargetSpeed = -999;
    TCMG2->TargetAngle = NAN;
  }
  if (TCMG1->TargetSpeed > TCMG2->TargetSpeed){
    TCMGMax->TargetSpeed = TCMG1->TargetSpeed;
    TCMGMax->TargetAngle = TCMG1->TargetAngle;
    TCMGMin->TargetAngle = TCMG2->TargetAngle;
    TCMGMin->TargetSpeed = TCMG2->TargetSpeed;
  }
  else {
    TCMGMax->TargetSpeed = TCMG2->TargetSpeed;
    TCMGMax->TargetAngle = TCMG2->TargetAngle;
    TCMGMin->TargetAngle = TCMG1->TargetAngle;
    TCMGMin->TargetSpeed = TCMG1->TargetSpeed;
  }
  if (TCMGMax->TargetSpeed == -999) TCMGMax->TargetSpeed = NAN;
  if (TCMGMin->TargetSpeed == -999) TCMGMin->TargetSpeed = NAN;
 /* wxLogMessage("--");
    wxLogMessage("TCMG1.TargetSpeed =%f, TCMG2.TargetSpeed =%f", TCMG1->TargetSpeed, TCMG2->TargetAngle);
    wxLogMessage("-->TCMGMax.TargetSpeed =%f, TCMGMax.TargetAngle =%f", TCMGMax->TargetSpeed, TCMGMax->TargetAngle);
    wxLogMessage("-->TCMGMin.TargetSpeed =%f, TCMGMin.TargetAngle =%f", TCMGMin->TargetSpeed, TCMGMin->TargetAngle);
    */
}
/*
test, doesn't work ...
/*
TargetxMG Polar::Calc_TargetCMG2(double TWS, double TWD, double BRG, TargetxMG *cmg2)
{
  TargetxMG TCMG, *TCMG2;
  TCMG.TargetAngle = -999;
  TCMG.TargetSpeed = -999;
  TCMG2->TargetAngle = -999;
  TCMG2->TargetSpeed = -999;
  double cmg = 0;

  int i_tws = wxRound(TWS);
  //double range = 
    int  diffAngle = wxRound(getSignedDegRange(TWD, BRG));
//  int vPolarAngle = wxRound(range);  //polar is rotated by this angle, this is "vertical" now
  int i = 0;
  int polarAngle;
  // first half of the polar
  for (i = 1; i <= 180; i++){
     polarAngle=i+diffAngle;
     wxLogMessage("polarAngle (i+diffAngle) = %d + %d = %d", i, diffAngle,polarAngle);
//     if (!wxIsNaN(windsp[i_tws].winddir[polarAngle])){
//       cmg = windsp[i_tws].winddir[polarAngle] * cos((polarAngle-BRG)*M_PI / 180.);
       if (!wxIsNaN(windsp[i_tws].winddir[i])){
         cmg = windsp[i_tws].winddir[i] * cos((polarAngle - BRG)*M_PI / 180.);
         //          wxLogMessage("k=%d, curAngle=%d, polarspeed=%f, curAngle-range=%f, cmg=%f", k, curAngle, windsp[i_tws].winddir[polang], diffAngle, cmg);
      if (cmg > TCMG.TargetSpeed) {
        TCMG.TargetSpeed = cmg;
        TCMG.TargetAngle = polarAngle;
      }
    }
  }
  //}
  if (TCMG.TargetSpeed == -999){
    TCMG.TargetSpeed = NAN;
    TCMG.TargetAngle = NAN;
  }
  //second half of the polar
  cmg = 0;
  for (i = 181; i <= 359; i++){
    polarAngle = i + diffAngle;
    if (!wxIsNaN(windsp[i_tws].winddir[polarAngle])){
      cmg = windsp[i_tws].winddir[polarAngle] * cos((polarAngle - BRG)*M_PI / 180.);
      if (cmg > TCMG2->TargetSpeed) {
        TCMG2->TargetSpeed = cmg;
        TCMG2->TargetAngle = polarAngle;
      }
    }
  }
  //}
  if (TCMG2->TargetSpeed == -999){
    TCMG2->TargetSpeed = NAN;
    TCMG2->TargetAngle = NAN;
  }
  //          wxLogMessage("k=%d, curAngle=%d, polarspeed=%f, curAngle-range=%f, cmg=%f", k, curAngle, windsp[i_tws].winddir[polang], diffAngle, cmg);

  if (TCMG.TargetSpeed > TCMG2->TargetSpeed) {
    cmg2 = TCMG2;
    return (TCMG);
  }
  else {
    cmg2 = &TCMG;
    return (*TCMG2);
  }
}*/
/***********************************************************************************

************************************************************************************/
void Polar::showDlg()
{
	dlg->ShowModal();
}


/***********************************************************************************
simple class for exonential smoothing
************************************************************************************/
ExpSmooth::ExpSmooth(double a)
{
	alpha = a;
	SmoothedValue = NAN;
	oldSmoothedValue = NAN;
}
/***********************************************************************************

************************************************************************************/
ExpSmooth::~ExpSmooth(void)
{
}
/***********************************************************************************

************************************************************************************/
double ExpSmooth::GetSmoothVal(double input)
{
	if (wxIsNaN(SmoothedValue)) SmoothedValue = input;
	oldSmoothedValue = SmoothedValue;
	SmoothedValue = alpha*input + (1 - alpha)*oldSmoothedValue;
	return SmoothedValue;
}
/***********************************************************************************

************************************************************************************/
void ExpSmooth::SetAlpha(double newalpha)
{
	alpha = newalpha;
}
/***********************************************************************************

************************************************************************************/
double ExpSmooth::GetAlpha()
{
	return alpha;
}
/***********************************************************************************

************************************************************************************/
void ExpSmooth::SetInitVal(double init)
{
	SmoothedValue = init;
	oldSmoothedValue = init;
}
/***********************************************************************************
Class for double exonential Smoothing, DES
------------------------------------------------------------------------------------
Formula taken from 
Double Exponential Smoothing: An Alternative to Kalman Filter-Based Predictive Tracking
Joseph J. LaViola Jr.
Brown University Technology Center
for Advanced Scientific Computing and Visualization
PO Box 1910, Providence, RI, 02912, USA
jjl@cs.brown.edu
--------------------------------------------------------------------------------------
T = 1;
Spt = alpha*pt + (1 - alpha)*Sptmin1;
Sp2t = alpha*Spt + (1 - alpha)*Sp2tmin1;
ptplusT = (2 + alpha*T / (1 - alpha))*Spt - (1 + alpha*T / (1 - alpha)) * Sp2t;

************************************************************************************/
DoubleExpSmooth::DoubleExpSmooth(double newalpha)
{
  alpha = newalpha;
  T = 1;
  //SmoothedValue = NAN;
  SpT = NAN;
  Sp2T = NAN;
}
/***********************************************************************************

************************************************************************************/
DoubleExpSmooth::~DoubleExpSmooth(void)
{
}
/***********************************************************************************

************************************************************************************/
double DoubleExpSmooth::GetSmoothVal(double input)
{
  
  if (wxIsNaN(SpT)) SpT = input;
  if (wxIsNaN(Sp2T)) Sp2T = input;

  oldSpT = SpT;
  oldSp2T = Sp2T;

  SpT = alpha*input + (1 - alpha)*oldSpT;
  Sp2T = alpha*SpT + (1 - alpha)*oldSp2T;
  predPosT = (2 + alpha*T / (1 - alpha))*SpT - (1 + alpha*T / (1 - alpha)) * Sp2T;
  return predPosT;
}
/***********************************************************************************

************************************************************************************/
void DoubleExpSmooth::SetAlpha(double newalpha)
{
  alpha = newalpha;
}
/***********************************************************************************

************************************************************************************/
double DoubleExpSmooth::GetAlpha()
{
  return alpha;
}
/***********************************************************************************

************************************************************************************/
void DoubleExpSmooth::SetInitVal(double init)
{
  SpT = init;
  Sp2T = init;
}

//************************************************************************************************************************
// Polar Performance instrument
//************************************************************************************************************************

TacticsInstrument_PolarPerformance::TacticsInstrument_PolarPerformance(wxWindow *parent, wxWindowID id, wxString title) :
TacticsInstrument(parent, id, title, OCPN_DBP_STC_STW | OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS)
{
  SetDrawSoloInPane(true);
  m_TWA = NAN;
  m_TWS = NAN;
  m_STW = NAN;
  m_PolarSpeedPercent = 0;
  m_MaxPercent = 0;
  m_MinBoatSpd = 0;
  m_MaxBoatSpd = 0;
  m_STWUnit = _T("--");
  m_PercentUnit = _T("%");
  m_TotalMaxSpdPercent = 0;
  m_TopLineHeight = 30;
  m_SpdStartVal = -1;
  m_IsRunning = false;
  m_SampleCount = 0;
  m_LeftLegend = 3;
  m_RightLegend = 3;
  for (int idx = 0; idx < DATA_RECORD_COUNT; idx++) {
    m_ArrayPercentSpdHistory[idx] = -1;
    m_ExpSmoothArrayPercentSpd[idx] = -1;
    m_ArrayRecTime[idx] = wxDateTime::Now();
    m_ArrayRecTime[idx].SetYear(999);
  }
  alpha = 0.01;  //smoothing constant
  m_WindowRect = GetClientRect();
  m_DrawAreaRect = GetClientRect();
  m_DrawAreaRect.SetHeight(m_WindowRect.height - m_TopLineHeight - m_TitleHeight);
}

wxSize TacticsInstrument_PolarPerformance::GetSize(int orient, wxSize hint)
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
void TacticsInstrument_PolarPerformance::SetData(int st, double data, wxString unit)
{
  if (st == OCPN_DBP_STC_STW || st == OCPN_DBP_STC_TWA || st == OCPN_DBP_STC_TWS) {
    if (st == OCPN_DBP_STC_TWA) {
      m_TWA = data;
    }
    if (st == OCPN_DBP_STC_TWS) {
      //m_TWS = data;
      //convert to knots first
      m_TWS = fromUsrSpeed_Plugin(data, g_iDashWindSpeedUnit);
    }

    if (st == OCPN_DBP_STC_STW) {
      //m_STW = data;
      //convert to knots first
      m_STW = fromUsrSpeed_Plugin(data, g_iDashSpeedUnit);

      if (!wxIsNaN(m_STW) && !wxIsNaN(m_TWA) && !wxIsNaN(m_TWS)){
        double m_PolarSpeed = BoatPolar->GetPolarSpeed(m_TWA, m_TWS);

        if (wxIsNaN(m_PolarSpeed))
          m_PercentUnit = _T("no polar data");
        else if (m_PolarSpeed == 0)
          m_PercentUnit = _T("--");
        else {
          m_PolarSpeedPercent = m_STW / m_PolarSpeed * 100;
          m_PercentUnit = _T("%");
        }
        m_STWUnit = unit;
        m_IsRunning = true;
        m_SampleCount = m_SampleCount < DATA_RECORD_COUNT ? m_SampleCount + 1 : DATA_RECORD_COUNT;
        m_MaxPercent = 0;
        m_MaxBoatSpd = 0;
        m_MinBoatSpd = 0;
        //data shifting
        for (int idx = 1; idx < DATA_RECORD_COUNT; idx++) {
          m_MaxPercent = wxMax(m_ArrayPercentSpdHistory[idx - 1], m_MaxPercent);
          m_ArrayPercentSpdHistory[idx - 1] = m_ArrayPercentSpdHistory[idx];
          m_ExpSmoothArrayPercentSpd[idx - 1] = m_ExpSmoothArrayPercentSpd[idx];
          m_MaxBoatSpd = wxMax(m_ArrayBoatSpdHistory[idx - 1], m_MaxBoatSpd);
          m_ArrayBoatSpdHistory[idx - 1] = m_ArrayBoatSpdHistory[idx];
          m_ExpSmoothArrayBoatSpd[idx - 1] = m_ExpSmoothArrayBoatSpd[idx];
          m_ArrayRecTime[idx - 1] = m_ArrayRecTime[idx];
        }
        m_ArrayPercentSpdHistory[DATA_RECORD_COUNT - 1] = m_PolarSpeedPercent;
        m_ArrayBoatSpdHistory[DATA_RECORD_COUNT - 1] = m_STW;
        if (m_SampleCount < 2) {
          m_ArrayPercentSpdHistory[DATA_RECORD_COUNT - 2] = m_PolarSpeedPercent;
          m_ExpSmoothArrayPercentSpd[DATA_RECORD_COUNT - 2] = m_PolarSpeedPercent;
          m_ArrayBoatSpdHistory[DATA_RECORD_COUNT - 2] = m_STW;
          m_ExpSmoothArrayBoatSpd[DATA_RECORD_COUNT - 2] = m_STW;
        }
        m_ExpSmoothArrayPercentSpd[DATA_RECORD_COUNT - 1] = alpha*m_ArrayPercentSpdHistory[DATA_RECORD_COUNT - 2] + (1 - alpha)*m_ExpSmoothArrayPercentSpd[DATA_RECORD_COUNT - 2];
        m_ExpSmoothArrayBoatSpd[DATA_RECORD_COUNT - 1] = alpha*m_ArrayBoatSpdHistory[DATA_RECORD_COUNT - 2] + (1 - alpha)*m_ExpSmoothArrayBoatSpd[DATA_RECORD_COUNT - 2];
        m_ArrayRecTime[DATA_RECORD_COUNT - 1] = wxDateTime::Now();
        //include the new/latest value in the max/min value test too
        m_MaxPercent = wxMax(m_PolarSpeedPercent, m_MaxPercent);
        m_MaxBoatSpd = wxMax(m_STW, m_MaxBoatSpd);
        //get the overall max Wind Speed
        m_TotalMaxSpdPercent = wxMax(m_PolarSpeedPercent, m_TotalMaxSpdPercent);
        //m_BoatSpeedRange = m_MaxBoatSpd - m_MinBoatSpd;
      }
    }
  }
}
void TacticsInstrument_PolarPerformance::Draw(wxGCDC* dc)
{
  m_WindowRect = GetClientRect();
  m_DrawAreaRect = GetClientRect();
  m_DrawAreaRect.SetHeight(m_WindowRect.height - m_TopLineHeight - m_TitleHeight);
  m_DrawAreaRect.SetX(m_LeftLegend + 3);
  DrawBackground(dc);
  DrawForeground(dc);
}

//*********************************************************************************
// draw boat speed legend (right side)
//*********************************************************************************
void  TacticsInstrument_PolarPerformance::DrawBoatSpeedScale(wxGCDC* dc)
{
  wxString label1, label2, label3, label4, label5;
  wxColour cl;
  wxPen pen;
  int width, height;
  double BoatSpdScale;
  int tmpval = (int)(m_MaxBoatSpd + 2) % 2;
  m_MaxBoatSpdScale = (int)(m_MaxBoatSpd + 2 - tmpval);

  cl = wxColour(204, 41, 41, 255); //red, opague

  dc->SetTextForeground(cl);
  dc->SetFont(*g_pFontSmall);
  if (!m_IsRunning) {
    label1.Printf(_T("--- %s"), m_STWUnit);
    label2 = label1;
    label3 = label1;
    label4 = label1;
    label5 = label1;
  }
  else {
    /*we round the speed up to the next full knot ==> the top and bottom line have full numbers as legend (e.g. 23 kn -- 0 kn)
    but the intermediate lines may have decimal values (e.g. center line : 23/2=11.5 or quarter line 23/4=5.75), so in worst case
    we end up with 23 - 17.25 - 11.5 - 5.75 - 0
    The goal is to draw the legend with decimals only, if we really have them !
    */
    // label 1 : legend for bottom line. By definition always w/o decimals
    label1.Printf(_T("%.0f %s"), toUsrSpeed_Plugin(m_MinBoatSpd, g_iDashSpeedUnit), m_STWUnit.c_str());
    // label 2 : 1/4
    BoatSpdScale = m_MaxBoatSpdScale / 4.;
    label2.Printf(_T("%.1f %s"), toUsrSpeed_Plugin(BoatSpdScale, g_iDashSpeedUnit), m_STWUnit.c_str());
    // label 3 : legend for center line
    BoatSpdScale = m_MaxBoatSpdScale / 2.;
    label3.Printf(_T("%.0f %s"), toUsrSpeed_Plugin(BoatSpdScale, g_iDashSpeedUnit), m_STWUnit.c_str());
    // label 4 :  3/4
    BoatSpdScale = m_MaxBoatSpdScale*3. / 4.;
    label4.Printf(_T("%.1f %s"), toUsrSpeed_Plugin(BoatSpdScale, g_iDashSpeedUnit), m_STWUnit.c_str());
    // label 5 : legend for top line
    label5.Printf(_T("%.0f %s"), toUsrSpeed_Plugin(m_MaxBoatSpdScale, g_iDashSpeedUnit), m_STWUnit.c_str());
  }
  //draw the legend with the labels; find the widest string and store it in m_RightLegend.
  // m_RightLegend is the basis for the horizontal lines !
  dc->GetTextExtent(label5, &width, &height, 0, 0, g_pFontSmall);
  m_RightLegend = width;
  dc->GetTextExtent(label4, &width, &height, 0, 0, g_pFontSmall);
  m_RightLegend = wxMax(width, m_RightLegend);
  dc->GetTextExtent(label3, &width, &height, 0, 0, g_pFontSmall);
  m_RightLegend = wxMax(width, m_RightLegend);
  dc->GetTextExtent(label2, &width, &height, 0, 0, g_pFontSmall);
  m_RightLegend = wxMax(width, m_RightLegend);
  dc->GetTextExtent(label1, &width, &height, 0, 0, g_pFontSmall);
  m_RightLegend = wxMax(width, m_RightLegend);

  m_RightLegend += 4; //leave some space to the edge
  dc->DrawText(label5, m_WindowRect.width - m_RightLegend, m_TopLineHeight - height / 2);
  dc->DrawText(label4, m_WindowRect.width - m_RightLegend, (int)(m_TopLineHeight + m_DrawAreaRect.height / 4 - height / 2));
  dc->DrawText(label3, m_WindowRect.width - m_RightLegend, (int)(m_TopLineHeight + m_DrawAreaRect.height / 2 - height / 2));
  dc->DrawText(label2, m_WindowRect.width - m_RightLegend, (int)(m_TopLineHeight + m_DrawAreaRect.height*0.75 - height / 2));
  dc->DrawText(label1, m_WindowRect.width - m_RightLegend, (int)(m_TopLineHeight + m_DrawAreaRect.height - height / 2));
}

//*********************************************************************************
// draw percent boat speed scale (left side)
//*********************************************************************************
void  TacticsInstrument_PolarPerformance::DrawPercentSpeedScale(wxGCDC* dc)
{
  wxString label1, label2, label3, label4, label5;
  wxColour cl;
  int width, height;
  double val1;
  double BoatSpdScale;

  cl = wxColour(61, 61, 204, 255);
  dc->SetTextForeground(cl);
  dc->SetFont(*g_pFontSmall);
  //round maxWindSpd up to the next full knot; nicer view ...
//  m_MaxPercentScale = (int)100;
  //m_MaxPercentScale = (int)m_MaxPercent + 1;
  int tmpval = (int)(m_MaxPercent + 10) % 10;
  m_MaxPercentScale = (int)(m_MaxPercent + 10 - tmpval);
  if (!m_IsRunning) {
    label1.Printf(_T("--- %s"), m_PercentUnit);
    label2 = label1;
    label3 = label1;
    label4 = label1;
    label5 = label1;
  }
  else {
    /*we round the speed up to the next full knot ==> the top and bottom line have full numbers as legend (e.g. 23 kn -- 0 kn)
    but the intermediate lines may have decimal values (e.g. center line : 23/2=11.5 or quarter line 23/4=5.75), so in worst case
    we end up with 23 - 17.25 - 11.5 - 5.75 - 0
    The goal is to draw the legend with decimals only, if we really have them !
    */
    // top legend for max %
    label1.Printf(_T("%.0f %s"), m_MaxPercentScale, m_PercentUnit);
    // 3/4 legend
    BoatSpdScale = m_MaxPercentScale*3. / 4.;
    // do we need a decimal ?
    val1 = (int)((BoatSpdScale - (int)BoatSpdScale) * 100);
    if (val1 == 25 || val1 == 75)  // it's a .25 or a .75
      label2.Printf(_T("%.2f %s"), BoatSpdScale, m_PercentUnit);
    else if (val1 == 50)
      label2.Printf(_T("%.1f %s"), BoatSpdScale, m_PercentUnit);
    else
      label2.Printf(_T("%.0f %s"), BoatSpdScale, m_PercentUnit);
    // center legend
    BoatSpdScale = m_MaxPercentScale / 2.;
    // center line can either have a .0 or .5 value !
    if ((int)(BoatSpdScale * 10) % 10 == 5)
      label3.Printf(_T("%.1f %s"), BoatSpdScale, m_PercentUnit);
    else
      label3.Printf(_T("%.0f %s"), BoatSpdScale, m_PercentUnit);

    // 1/4 legend
    BoatSpdScale = m_MaxPercentScale / 4.;
    // do we need a decimal ?
    val1 = (int)((BoatSpdScale - (int)BoatSpdScale) * 100);
    if (val1 == 25 || val1 == 75)
      label4.Printf(_T("%.2f %s"), BoatSpdScale, m_PercentUnit);
    else if (val1 == 50)
      label4.Printf(_T("%.1f %s"), BoatSpdScale, m_PercentUnit);
    else
      label4.Printf(_T("%.0f %s"), BoatSpdScale, m_PercentUnit);

    //bottom legend for min wind, always 0
    label5.Printf(_T("%.0f %s"), 0.0, m_PercentUnit);
  }
  dc->GetTextExtent(label1, &m_LeftLegend, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label1, 4, (int)(m_TopLineHeight - height / 2));
  dc->GetTextExtent(label2, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label2, 4, (int)(m_TopLineHeight + m_DrawAreaRect.height / 4 - height / 2));
  m_LeftLegend = wxMax(width, m_LeftLegend);
  dc->GetTextExtent(label3, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label3, 4, (int)(m_TopLineHeight + m_DrawAreaRect.height / 2 - height / 2));
  m_LeftLegend = wxMax(width, m_LeftLegend);
  dc->GetTextExtent(label4, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label4, 4, (int)(m_TopLineHeight + m_DrawAreaRect.height*0.75 - height / 2));
  m_LeftLegend = wxMax(width, m_LeftLegend);
  dc->GetTextExtent(label5, &width, &height, 0, 0, g_pFontSmall);
  dc->DrawText(label5, 4, (int)(m_TopLineHeight + m_DrawAreaRect.height - height / 2));
  m_LeftLegend = wxMax(width, m_LeftLegend);
  m_LeftLegend += 4;
}

//*********************************************************************************
//draw background
//*********************************************************************************
void TacticsInstrument_PolarPerformance::DrawBackground(wxGCDC* dc)
{
  wxString label, label1, label2, label3, label4, label5;
  wxColour cl;
  wxPen pen;
  //---------------------------------------------------------------------------------
  // draw legends for speed and direction
  //---------------------------------------------------------------------------------
  DrawBoatSpeedScale(dc);
  DrawPercentSpeedScale(dc);

  //---------------------------------------------------------------------------------
  // horizontal lines
  //---------------------------------------------------------------------------------
  GetGlobalColor(_T("UBLCK"), &cl);
  pen.SetColour(cl);
  dc->SetPen(pen);
  dc->DrawLine(m_LeftLegend + 3, m_TopLineHeight, m_WindowRect.width - 3 - m_RightLegend, m_TopLineHeight); // the upper line
  dc->DrawLine(m_LeftLegend + 3, (int)(m_TopLineHeight + m_DrawAreaRect.height), m_WindowRect.width - 3 - m_RightLegend, (int)(m_TopLineHeight + m_DrawAreaRect.height));
  pen.SetStyle(wxPENSTYLE_DOT);
  dc->SetPen(pen);
  dc->DrawLine(m_LeftLegend + 3, (int)(m_TopLineHeight + m_DrawAreaRect.height*0.25), m_WindowRect.width - 3 - m_RightLegend, (int)(m_TopLineHeight + m_DrawAreaRect.height*0.25));
  dc->DrawLine(m_LeftLegend + 3, (int)(m_TopLineHeight + m_DrawAreaRect.height*0.75), m_WindowRect.width - 3 - m_RightLegend, (int)(m_TopLineHeight + m_DrawAreaRect.height*0.75));
#ifdef __WXMSW__  
  pen.SetStyle(wxSHORT_DASH);
  dc->SetPen(pen);
#endif  
  dc->DrawLine(m_LeftLegend + 3, (int)(m_TopLineHeight + m_DrawAreaRect.height*0.5), m_WindowRect.width - 3 - m_RightLegend, (int)(m_TopLineHeight + m_DrawAreaRect.height*0.5));
}


//*********************************************************************************
//draw foreground
//*********************************************************************************
void TacticsInstrument_PolarPerformance::DrawForeground(wxGCDC* dc)
{
  wxColour col;
  double ratioH;
  int degw, degh;
  int width, height, min, hour;
  wxString  BoatSpeed, PercentSpeed;
  wxPen pen;
  wxString label;

  //---------------------------------------------------------------------------------
  // boat speed
  //---------------------------------------------------------------------------------
  dc->SetFont(*g_pFontData);
  col = wxColour(204, 41, 41, 255); //red, opaque
  dc->SetTextForeground(col);
  if (!m_IsRunning)
    BoatSpeed = _T("STW ---");
  else {
    BoatSpeed = wxString::Format(_T("STW %3.2f %s"), toUsrSpeed_Plugin(m_STW, g_iDashSpeedUnit), m_STWUnit.c_str());
  }
  dc->GetTextExtent(BoatSpeed, &degw, &degh, 0, 0, g_pFontData);
  dc->DrawText(BoatSpeed, m_WindowRect.width - degw - m_RightLegend - 3, m_TopLineHeight - degh);
  pen.SetStyle(wxPENSTYLE_SOLID);
  pen.SetColour(wxColour(204, 41, 41, 96)); //red, transparent
  pen.SetWidth(1);
  dc->SetPen(pen);
  ratioH = (double)m_DrawAreaRect.height / m_MaxBoatSpdScale;
  m_DrawAreaRect.SetWidth(m_WindowRect.width - 6 - m_LeftLegend - m_RightLegend);
  m_ratioW = double(m_DrawAreaRect.width) / (DATA_RECORD_COUNT - 1);

  //---------------------------------------------------------------------------------
  // live boat speed data
  //---------------------------------------------------------------------------------
  wxPoint points[DATA_RECORD_COUNT + 2], pointAngle_old;
  pointAngle_old.x = 3 + m_LeftLegend;
  pointAngle_old.y = m_TopLineHeight + m_DrawAreaRect.height - (m_ArrayBoatSpdHistory[0] - m_MinBoatSpd) * ratioH;
  // wxLogMessage("Live:pointAngle_old.x=%d, pointAngle_old.y=%d", pointAngle_old.x, pointAngle_old.y);
  for (int idx = 1; idx < DATA_RECORD_COUNT; idx++) {
    points[idx].x = idx * m_ratioW + 3 + m_LeftLegend;
    points[idx].y = m_TopLineHeight + m_DrawAreaRect.height - (m_ArrayBoatSpdHistory[idx] - m_MinBoatSpd) * ratioH;
    //  wxLogMessage("Live:points[%d].y=%d", idx, points[idx].y);
    if (DATA_RECORD_COUNT - m_SampleCount <= idx && points[idx].y > m_TopLineHeight && pointAngle_old.y> m_TopLineHeight && points[idx].y <= m_TopLineHeight + m_DrawAreaRect.height && pointAngle_old.y <= m_TopLineHeight + m_DrawAreaRect.height)
      dc->DrawLine(pointAngle_old.x, pointAngle_old.y, points[idx].x, points[idx].y);
    pointAngle_old.x = points[idx].x;
    pointAngle_old.y = points[idx].y;
  }

  //---------------------------------------------------------------------------------
  //exponential smoothing of boat speed
  //---------------------------------------------------------------------------------
  pen.SetStyle(wxPENSTYLE_SOLID);
  pen.SetColour(wxColour(204, 41, 41, 255));
  pen.SetWidth(2);
  dc->SetPen(pen);
  pointAngle_old.x = 3 + m_LeftLegend;
  pointAngle_old.y = m_TopLineHeight + m_DrawAreaRect.height - (m_ExpSmoothArrayBoatSpd[0] - m_MinBoatSpd) * ratioH;
  //  wxLogMessage("Smoothed:pointAngle_old.x=%d, pointAngle_old.y=%d", pointAngle_old.x, pointAngle_old.y);
  for (int idx = 1; idx < DATA_RECORD_COUNT; idx++) {
    points[idx].x = idx * m_ratioW + 3 + m_LeftLegend;
    points[idx].y = m_TopLineHeight + m_DrawAreaRect.height - (m_ExpSmoothArrayBoatSpd[idx] - m_MinBoatSpd) * ratioH;
    //  wxLogMessage("Smoothed:points[%d].y=%d", idx,points[idx].y);
    if (DATA_RECORD_COUNT - m_SampleCount <= idx && points[idx].y > m_TopLineHeight && pointAngle_old.y > m_TopLineHeight && points[idx].y <= m_TopLineHeight + m_DrawAreaRect.height && pointAngle_old.y <= m_TopLineHeight + m_DrawAreaRect.height)
      dc->DrawLine(pointAngle_old.x, pointAngle_old.y, points[idx].x, points[idx].y);
    pointAngle_old.x = points[idx].x;
    pointAngle_old.y = points[idx].y;
  }

  //---------------------------------------------------------------------------------
  // boat speed in percent polar
  //---------------------------------------------------------------------------------
  col = wxColour(61, 61, 204, 255); //blue, opaque
  dc->SetFont(*g_pFontData);
  dc->SetTextForeground(col);
  PercentSpeed = wxString::Format(_T("Polar Perf. %3.2f %s "), m_PolarSpeedPercent, m_PercentUnit);
  dc->GetTextExtent(PercentSpeed, &degw, &degh, 0, 0, g_pFontData);
  dc->DrawText(PercentSpeed, m_LeftLegend + 3, m_TopLineHeight - degh);
  dc->SetFont(*g_pFontLabel);
  //determine the time range of the available data (=oldest data value)
  int i = 0;
  while (m_ArrayRecTime[i].GetYear() == 999 && i<DATA_RECORD_COUNT - 1) i++;
  if (i == DATA_RECORD_COUNT - 1) {
    min = 0;
    hour = 0;
  }
  else {
    min = m_ArrayRecTime[i].GetMinute();
    hour = m_ArrayRecTime[i].GetHour();
  }
  // Single text var to facilitate correct translations:
  wxString s_Max = _("Max");
  wxString s_Since = _("since");
  wxString s_OMax = _("Overall");
  dc->DrawText(wxString::Format(_T("%s %.1f %s %s %02d:%02d  %s %.1f %s"), s_Max, m_MaxPercent, m_PercentUnit, s_Since, hour, min, s_OMax, m_TotalMaxSpdPercent, m_PercentUnit), m_LeftLegend + 3 + 2 + degw, m_TopLineHeight - degh + 5);
  //dc->DrawText(wxString::Format(_("Max %.1f %s since %02d:%02d  Overall %.1f %s"), m_MaxPercent, m_PercentUnit, hour, min, m_TotalMaxSpdPercent, m_PercentUnit), m_LeftLegend + 3 + 2 + degw, m_TopLineHeight - degh + 5);
  pen.SetStyle(wxPENSTYLE_SOLID);
  pen.SetColour(wxColour(61, 61, 204, 96)); //blue, transparent
  pen.SetWidth(1);
  dc->SetPen(pen);
  ratioH = (double)m_DrawAreaRect.height / m_MaxPercentScale;
  m_DrawAreaRect.SetWidth(m_WindowRect.width - 6 - m_LeftLegend - m_RightLegend);
  m_ratioW = double(m_DrawAreaRect.width) / (DATA_RECORD_COUNT - 1);
  wxPoint  pointsSpd[DATA_RECORD_COUNT + 2], pointSpeed_old;
  pointSpeed_old.x = m_LeftLegend + 3;
  pointSpeed_old.y = m_TopLineHeight + m_DrawAreaRect.height - m_ArrayPercentSpdHistory[0] * ratioH;

  //---------------------------------------------------------------------------------
  // live speed data in percent polar
  //---------------------------------------------------------------------------------
  for (int idx = 1; idx < DATA_RECORD_COUNT; idx++) {
    pointsSpd[idx].x = idx * m_ratioW + 3 + m_LeftLegend;
    pointsSpd[idx].y = m_TopLineHeight + m_DrawAreaRect.height - m_ArrayPercentSpdHistory[idx] * ratioH;
    if (DATA_RECORD_COUNT - m_SampleCount <= idx && pointsSpd[idx].y > m_TopLineHeight && pointSpeed_old.y > m_TopLineHeight && pointsSpd[idx].y <= m_TopLineHeight + m_DrawAreaRect.height && pointSpeed_old.y <= m_TopLineHeight + m_DrawAreaRect.height)
      dc->DrawLine(pointSpeed_old.x, pointSpeed_old.y, pointsSpd[idx].x, pointsSpd[idx].y);
    pointSpeed_old.x = pointsSpd[idx].x;
    pointSpeed_old.y = pointsSpd[idx].y;
  }

  //---------------------------------------------------------------------------------
  //exponential smoothing of percentage
  //---------------------------------------------------------------------------------
  pen.SetStyle(wxPENSTYLE_SOLID);
  pen.SetColour(wxColour(61, 61, 204, 255)); //blue, opaque
  pen.SetWidth(2);
  dc->SetPen(pen);
  pointSpeed_old.x = m_LeftLegend + 3;
  pointSpeed_old.y = m_TopLineHeight + m_DrawAreaRect.height - m_ExpSmoothArrayPercentSpd[0] * ratioH;
  for (int idx = 1; idx < DATA_RECORD_COUNT; idx++) {
    pointsSpd[idx].x = idx * m_ratioW + 3 + m_LeftLegend;
    pointsSpd[idx].y = m_TopLineHeight + m_DrawAreaRect.height - m_ExpSmoothArrayPercentSpd[idx] * ratioH;
    if (DATA_RECORD_COUNT - m_SampleCount <= idx && pointsSpd[idx].y > m_TopLineHeight && pointSpeed_old.y > m_TopLineHeight && pointsSpd[idx].y <= m_TopLineHeight + m_DrawAreaRect.height && pointSpeed_old.y <= m_TopLineHeight + m_DrawAreaRect.height)
      dc->DrawLine(pointSpeed_old.x, pointSpeed_old.y, pointsSpd[idx].x, pointsSpd[idx].y);
    pointSpeed_old.x = pointsSpd[idx].x;
    pointSpeed_old.y = pointsSpd[idx].y;
  }

  //---------------------------------------------------------------------------------
  //draw vertical timelines every 5 minutes
  //---------------------------------------------------------------------------------
  GetGlobalColor(_T("UBLCK"), &col);
  pen.SetColour(col);
  pen.SetStyle(wxPENSTYLE_DOT);
  dc->SetPen(pen);
  dc->SetTextForeground(col);
  dc->SetFont(*g_pFontSmall);
  int done = -1;
  wxPoint pointTime;
  for (int idx = 0; idx < DATA_RECORD_COUNT; idx++) {
    min = m_ArrayRecTime[idx].GetMinute();
    hour = m_ArrayRecTime[idx].GetHour();
    if (m_ArrayRecTime[idx].GetYear() != 999) {
      if ((hour * 100 + min) != done && (min % 5 == 0) && (m_ArrayRecTime[idx].GetSecond() == 0 || m_ArrayRecTime[idx].GetSecond() == 1)) {
        pointTime.x = idx * m_ratioW + 3 + m_LeftLegend;
        dc->DrawLine(pointTime.x, m_TopLineHeight + 1, pointTime.x, (m_TopLineHeight + m_DrawAreaRect.height + 1));
        label.Printf(_T("%02d:%02d"), hour, min);
        dc->GetTextExtent(label, &width, &height, 0, 0, g_pFontSmall);
        dc->DrawText(label, pointTime.x - width / 2, m_WindowRect.height - height);
        done = hour * 100 + min;
      }
    }
  }
}

/***************************************************************************
* $Id: performance.h, v1.0 2016/06/07 tomBigSpeedy Exp $
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

#ifndef __PERFORMANCE_H__
#define __PERFORMANCE_H__

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/dynarray.h>
#include <wx/grid.h> //TR
#include <wx/filename.h> //TR
#include <map>

#include "instrument.h"
#include "dial.h"

//#include "nmea0183/nmea0183.h"

WX_DEFINE_ARRAY_DOUBLE(int, ArrayOfDouble);


//TR #define WINDDIR 180
#define WINDDIR 360
#define WINDSPEED 60

enum{
	POLARSPEED, POLARVMG, POLARTARGETVMG, POLARTARGETVMGANGLE, POLARCMG, POLARTARGETCMG, POLARTARGETCMGANGLE,TWAMARK
};
struct TargetxMG{
	double TargetAngle=0;
	double TargetSpeed=0;
};
class tactics_pi;
class Polar;
double getDegRange(double max, double min);
double getSignedDegRange(double max, double min);

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    TacticsInstrument_Performance
//|
//| DESCRIPTION:
//|    This class creates a simple Performance Instrument
//|
//+------------------------------------------------------------------------------
class TacticsInstrument_PerformanceSingle : public TacticsInstrument
{
public:
	TacticsInstrument_PerformanceSingle(wxWindow *pparent, wxWindowID id, wxString title, int cap, wxString format);
	~TacticsInstrument_PerformanceSingle(){}

	wxSize GetSize(int orient, wxSize hint);
	void SetData(int st, double data, wxString unit);
	void SetDisplayType(int displaytype);
	double mTWS;
	double mTWA;
	double mSTW;
	double mCMG,mSOG,mCOG,mBRG,mHDT,mTWD,m_lat,m_lon;
	wxString stwunit;
	int m_displaytype;

protected:
	wxString          m_data;
	wxString          m_format;
	int               m_DataHeight;
	
	void Draw(wxGCDC* dc);
private :
	wxFileConfig     *m_pconfig;

};




//*************************************************************************************
class Polar
{

public:
	Polar(TacticsInstrument_PerformanceSingle* parent);
    Polar(tactics_pi *parent);
    ~Polar(void);
	wxFileConfig     *m_pconfig;
	struct pol
	{
	    double   winddir[WINDDIR+1];
		bool     isfix[WINDDIR+1];   //one of the values from the original polar
	} windsp[WINDSPEED+1];
	
    struct optAngle
    {
      TargetxMG tvmg_up; //upwind
      TargetxMG tvmg_dn; //downwind
    }tws[WINDSPEED + 1];

	wxDC*			dc;

	wxArrayString	sails;
	bool			filterSails;
	double			knots;
	int				degrees;

	int				mode;
	double			sog;
	double			windAngle;
	double			windSpeed;
	wxString		windReference;
	double			gpsSpeed;
	double			speedoSpeed;
	int				timeout;
	bool			nmea;


	void showDlg();
	void setValue(wxString s, int row, int col);
	void reset();
	bool insert();
	void loadPolar(wxString FilePath);        //fill the polar values from file in the lookup table
	void completePolar();    //complete the empty spots in the lookup table with simple average calculation
	void CalculateLineAverages(int n, int min, int max);
	void CalculateRowAverages(int i, int min, int max);
	double GetPolarSpeed(double twa, double tws);
    TargetxMG GetTargetVMGUpwind(double TWS);
    TargetxMG GetTargetVMGDownwind(double TWS);
    double GetAvgPolarSpeed(double twa, double tws);
	double Calc_VMG(double TWA, double StW);
    double Calc_CMG(double heading, double speed, double Brg);
	TargetxMG Calc_TargetVMG(double TWA, double TWS);
    TargetxMG Calc_TargetCMG(double TWS, double TWD, double BRG);
    void Calc_TargetCMG2(double TWS, double TWD, double BRG, TargetxMG *TCMG1, TargetxMG*TCMG2);

private:
	wxDialog*	dlg;

	wxString		logbookDataPath;
	wxSize			center;
	double			dist;

	double toRad(int angle);
};

/*************************************************************************************
Class for exponential smoothing

Usage :
ExpSmooth       *mp_expsmooth;
double alpha,  unsmoothed_data, smoothed_val;
...
mp_expsmooth = new ExpSmooth(alpha);
start_of_any_loop{
  smoothed_val = mp_expsmooth->GetSmoothVal(unsmoothed_data);
  optional:
  mp_expsmooth->SetAlpha(new_alpha);
}
*************************************************************************************/
class ExpSmooth
{
public:
	ExpSmooth(double newalpha);
	~ExpSmooth(void);

	double GetSmoothVal(double input);
	void SetAlpha(double newalpha);
	void SetInitVal(double init);
	double GetAlpha(void);
protected:

private:
	double alpha;
	double oldSmoothedValue;
	double SmoothedValue;

};
/***********************************************************************************
Class for double exonential smoothing, DES
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

Note :
There is only ONE damping factor alpha in this special implementation of DES. 
The second factor beta dropped out. Search the net on the title above, there is
a theoretical description on this
Usage : 
see "Class for exponential smoothing" above

************************************************************************************/
class DoubleExpSmooth
{
public:
  DoubleExpSmooth(double newalpha);
  ~DoubleExpSmooth(void);

  double GetSmoothVal(double input);
  void SetAlpha(double newalpha);
  void SetInitVal(double init);
  double GetAlpha(void);
protected:

private:
  double alpha;
  int T;
  double SpT, oldSpT, Sp2T, oldSp2T, predPosT;

};

// Warn: div by 0 if count == 1
#define DATA_RECORD_COUNT 1500

class TacticsInstrument_PolarPerformance : public TacticsInstrument
{
public:
  TacticsInstrument_PolarPerformance(wxWindow *parent, wxWindowID id, wxString title);
  ~TacticsInstrument_PolarPerformance(void){}
  void SetData(int, double, wxString);
  wxSize GetSize(int orient, wxSize hint);

private:
  int m_soloInPane;
  int    m_SpdRecCnt, m_SpdStartVal, m_DirStartVal;
  int m_isNULL;

protected:
  double alpha;
  double m_ArrayPercentSpdHistory[DATA_RECORD_COUNT];
  double m_ArrayBoatSpdHistory[DATA_RECORD_COUNT];
  double m_ExpSmoothArrayBoatSpd[DATA_RECORD_COUNT];
  double m_ExpSmoothArrayPercentSpd[DATA_RECORD_COUNT];
  wxDateTime::Tm m_ArrayRecTime[DATA_RECORD_COUNT];

  double m_MaxBoatSpd;
  double m_MinBoatSpd;
  double m_BoatSpeedRange;
  double m_MaxPercent;  //...in array
  double m_AvgSpdPercent;
  ExpSmooth  *mExpSmAvgSpdPercent;
  double m_TWA, m_TWS, m_STW, m_PolarSpeedPercent, m_PolarSpeed;
  double m_MaxPercentScale, m_MaxBoatSpdScale;
  int num_of_scales;
  double m_ratioW;
  //double m_oldDirVal;
  bool m_IsRunning;
  int m_SampleCount;
  wxString m_STWUnit, m_PercentUnit;

  wxRect m_WindowRect;
  wxRect m_DrawAreaRect; //the coordinates of the real darwing area
  int m_DrawingWidth, m_TopLineHeight, m_DrawingHeight;
  int m_width, m_height;
  int m_LeftLegend, m_RightLegend;
  int m_currSec, m_lastSec, m_SpdCntperSec, m_DirCntperSec;
  double m_cntSpd, m_cntDir, m_avgSpd, m_avgDir;

  void Draw(wxGCDC* dc);
  void DrawBackground(wxGCDC* dc);
  void DrawForeground(wxGCDC* dc);
  void SetMinMaxWindScale();
  //void DrawWindDirScale(wxGCDC* dc);
  void DrawBoatSpeedScale(wxGCDC* dc);
  void DrawPercentSpeedScale(wxGCDC* dc);
  //wxString GetWindDirStr(wxString WindDir);
};
#endif
/*
class NKEPerformanceData
{
public:
  NKEPerformanceData(void);
  ~NKEPerformanceData(void);
  wxString ComputeChecksum(wxString sentence);
  void SendNMEASentence(wxString sentence);
  void createPNKEP_NMEA(int sentence, double data1, double data2, double data3, double data4);
protected:

private:
  double mPolarTargetSpeed;
};*/

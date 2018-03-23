/***************************************************************************
* $Id: tactics_pi.cpp, v1.0 2016/06/07 tomBigSpeedy Exp $
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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

// xw 2.8
#include <wx/filename.h>

#include <typeinfo>
#include "tactics_pi.h"
#include "icons.h"
#include "wx/jsonreader.h"
#include "wx/jsonwriter.h"
#ifdef __WXMSW__
#include "GL/gl.h"            // Windows
#include <GL/glu.h>
#else
#ifndef __OCPN__ANDROID__
//#include <GL/gl.h>
//#include <GL/glu.h>
#else
#include "qopengl.h"                  // this gives us the qt runtime gles2.h
#include "GL/gl_private.h"
#endif
#endif

#ifdef ocpnUSE_GL
#include <wx/glcanvas.h>
#endif

#include "ocpn_plugin.h"
#include <wx/glcanvas.h>
#include <wx/wfstream.h> //TR temp
#include <wx/txtstrm.h>   //TR temp


wxFont *g_pFontTitle;
wxFont *g_pFontData;
wxFont *g_pFontLabel;
wxFont *g_pFontSmall;
int g_iDashSpeedMax;
int g_iDashCOGDamp;
int g_iDashSpeedUnit;
int g_iDashSOGDamp;
int g_iDashDepthUnit;
int g_iDashDistanceUnit;  //0="Nautical miles", 1="Statute miles", 2="Kilometers", 3="Meters"
int g_iDashWindSpeedUnit; //0="Kts", 1="mph", 2="km/h", 3="m/s"
//TR
double g_dalphaDeltCoG;
double g_dLeewayFactor;
double g_dfixedLeeway;
double g_dalpha_currdir;
int g_iMinLaylineWidth;
int g_iMaxLaylineWidth;
double g_dLaylineLengthonChart;
Polar* BoatPolar;
bool g_bDisplayCurrentOnChart;
wxString g_path_to_PolarFile;
PlugIn_Route *m_pRoute = NULL;
PlugIn_Waypoint *m_pMark = NULL;
double g_dmark_lat = NAN;
double g_dmark_lon = NAN;
double g_dcur_lat = NAN;
double g_dcur_lon = NAN;
double g_dheel[6][5];
bool g_bUseHeelSensor;
bool g_bUseFixedLeeway;
bool g_bManHeelInput;
bool g_bCorrectSTWwithLeeway;  //if true STW is corrected with Leeway (in case Leeway is available)
bool g_bCorrectAWwithHeel;    //if true, AWS/AWA will be corrected with Heel-Angle
bool g_bForceTrueWindCalculation;    //if true, NMEA Data for TWS,TWA,TWD is not used, but the plugin calculated data is used
bool g_bUseSOGforTWCalc; //if true, use SOG instead of STW to calculate TWS,TWA,TWD
bool g_bShowWindbarbOnChart;
bool g_bShowPolarOnChart;
bool g_bExpPerfData01;
bool g_bExpPerfData02;
bool g_bExpPerfData03;
bool g_bExpPerfData04;
bool g_bExpPerfData05;
bool b_tactics_dc_message_shown = false;
wxString g_sCMGSynonym, g_sVMGSynonym;

#if !defined(NAN)
static const long long lNaN = 0xfff8000000000000;
#define NAN (*(double*)&lNaN)
#endif


// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
	return (opencpn_plugin *) new tactics_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
	delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    Tactics PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------
// !!! WARNING !!!
// do not change the order, add new instruments at the end, before ID_DBP_LAST_ENTRY!
// otherwise, for users with an existing opencpn.ini file, their instruments are changing !

enum {
	ID_DBP_I_POS, ID_DBP_I_SOG, ID_DBP_D_SOG, ID_DBP_I_COG, ID_DBP_D_COG, ID_DBP_I_STW,
	ID_DBP_I_HDT, ID_DBP_D_AW, ID_DBP_D_AWA, ID_DBP_I_AWS, ID_DBP_D_AWS, ID_DBP_D_TW,
	ID_DBP_I_DPT, ID_DBP_D_DPT, ID_DBP_I_TMP, ID_DBP_I_VMG, ID_DBP_D_VMG,
	ID_DBP_I_CLK, ID_DBP_I_SUN, ID_DBP_I_ATMP, ID_DBP_I_AWA, ID_DBP_I_TWA, ID_DBP_I_TWD, ID_DBP_I_TWS,
	ID_DBP_D_TWD, ID_DBP_I_HDM, ID_DBP_D_HDT, ID_DBP_D_WDH, ID_DBP_I_TWAMARK, ID_DBP_D_MDA, ID_DBP_I_MDA, ID_DBP_D_BPH, ID_DBP_I_FOS,
	ID_DBP_M_COG, ID_DBP_I_PITCH, ID_DBP_I_HEEL, ID_DBP_D_AWA_TWA, ID_DBP_I_LEEWAY, ID_DBP_I_CURRDIR,
	ID_DBP_I_CURRSPD, ID_DBP_D_BRG, ID_DBP_I_POLSPD, ID_DBP_I_POLVMG, ID_DBP_I_POLTVMG,
	ID_DBP_I_POLTVMGANGLE, ID_DBP_I_POLCMG, ID_DBP_I_POLTCMG, ID_DBP_I_POLTCMGANGLE, ID_DBP_D_POLPERF, ID_DBP_D_AVGWIND, ID_DBP_D_POLCOMP,
	ID_DBP_LAST_ENTRY //this has a reference in one of the routines; defining a "LAST_ENTRY" and setting the reference to it, is one codeline less to change (and find) when adding new instruments :-)
};

/********************************************************************************************************/
//   Distance measurement for simple sphere
/********************************************************************************************************/

/* currently not used, but maybe less costy in terms of CPU usage ? To be investigated ...*/
static double local_distance(double lat1, double lon1, double lat2, double lon2) {
	// Spherical Law of Cosines
	double theta, dist;

	theta = lon2 - lon1;
	dist = sin(lat1 * M_PI / 180.) * sin(lat2 * M_PI / 180.) + cos(lat1 * M_PI / 180.) * cos(lat2* M_PI / 180.) * cos(theta* M_PI / 180.);
	dist = acos(dist);		// radians
	dist = dist * 180. / M_PI;
	dist = fabs(dist) * 60.;    // nautical miles/degree
	return (dist);
}

static double local_bearing(double lat1, double lon1, double lat2, double lon2) //FES
{
	double angle = atan2((lat2 - lat1)*M_PI / 180., ((lon2 - lon1)* M_PI / 180. * cos(lat1 *M_PI / 180.)));

	angle = angle * 180. / M_PI;
	angle = 90.0 - angle;
	if (angle < 0) angle = 360 + angle;
	return (angle);
}

bool IsObsolete(int id) {
	switch (id) {
	case ID_DBP_D_AWA: return true;
	default: return false;
	}
}

wxString getInstrumentCaption(unsigned int id)
{
	switch (id){
	case ID_DBP_I_POS:
		return _("Position");
	case ID_DBP_I_SOG:
		return _("SOG");
	case ID_DBP_D_SOG:
		return _("Speedometer");
	case ID_DBP_I_COG:
		return _("COG");
	case ID_DBP_M_COG:
		return _("Mag COG");
	case ID_DBP_D_COG:
		return _("GPS Compass");
	case ID_DBP_D_HDT:
		return _("True Compass");
	case ID_DBP_I_STW:
		return _("STW");
	case ID_DBP_I_HDT:
		return _("True HDG");
	case ID_DBP_I_HDM:
		return _("Mag HDG");
	case ID_DBP_D_AW:
	case ID_DBP_D_AWA:
		return _("App. Wind Angle & Speed");
	case ID_DBP_D_AWA_TWA:
		return _("App & True Wind Angle");
	case ID_DBP_I_AWS:
		return _("App. Wind Speed");
	case ID_DBP_D_AWS:
		return _("App. Wind Speed");
	case ID_DBP_D_TW:
		return _("True Wind Angle & Speed");
	case ID_DBP_I_DPT:
		return _("Depth");
	case ID_DBP_D_DPT:
		return _("Depth");
	case ID_DBP_D_MDA:
		return _("Barometric pressure");
	case ID_DBP_I_MDA:
		return _("Barometric pressure");
	case ID_DBP_I_TMP:
		return _("Water Temp.");
	case ID_DBP_I_ATMP:
		return _("Air Temp.");
	case ID_DBP_I_AWA:
		return _("App. Wind Angle");
	case ID_DBP_I_TWA:
		return _("True Wind Angle");
	case ID_DBP_I_TWD:
		return _("True Wind Direction");
	case ID_DBP_I_TWS:
		return _("True Wind Speed");
	case ID_DBP_D_TWD:
		return _("True Wind Dir. & Speed");
	case ID_DBP_I_VMG:
		//            return _("VMG");
		return g_sVMGSynonym;
	case ID_DBP_D_VMG:
		//            return _("VMG");
		return g_sVMGSynonym;
		//case ID_DBP_I_RSA:
		//    return _("Rudder Angle");
		//case ID_DBP_D_RSA:
		//    return _("Rudder Angle");
		//case ID_DBP_I_SAT:
		//    return _("GPS in View");
		//case ID_DBP_D_GPS:
		//    return _("GPS Status");
		//case ID_DBP_I_PTR:
		//    return _("Cursor");
	case ID_DBP_I_CLK:
		return _("Clock");
	case ID_DBP_I_SUN:
		return _("Sunrise/Sunset");
		//case ID_DBP_D_MON:
		//    return _("Moon phase");
	case ID_DBP_D_WDH:
		return _("Wind history");
	case ID_DBP_D_BPH:
		return  _("Barometric history");
		//case ID_DBP_I_VLW1:
		//    return _("Trip Log");
		//case ID_DBP_I_VLW2:
		//    return _("Sum Log");
	case ID_DBP_I_TWAMARK:
		return _("TWA to Waypoint");
	case ID_DBP_I_FOS:
		return _("From Ownship");
	case ID_DBP_I_PITCH:
		return _("Pitch");
	case ID_DBP_I_HEEL:
		return _("Heel");
	case  ID_DBP_I_LEEWAY:
		return _("Leeway");
	case ID_DBP_I_CURRDIR:
		return _("Current Direction");
	case ID_DBP_I_CURRSPD:
		return _("Current Speed");
	case ID_DBP_D_BRG:
		return _("Bearing Compass");
	case	ID_DBP_I_POLSPD:
		return _("Polar Speed");
	case	ID_DBP_I_POLVMG:
		//			return _("Actual VMG");
		return _("Actual ") + g_sVMGSynonym;
	case	ID_DBP_I_POLTVMG:
		//          return _("Target VMG");
		return _("Target ") + g_sVMGSynonym;
	case	ID_DBP_I_POLTVMGANGLE:
		//			return _("Target VMG-Angle");
		return _("Target ") + g_sVMGSynonym + _("-Angle");
	case	ID_DBP_I_POLCMG:
		//          return _("Actual CMG");
		return _("Actual ") + g_sCMGSynonym;
	case	ID_DBP_I_POLTCMG:
		//          return _("Target CMG");
		return _("Target ") + g_sCMGSynonym;
	case	ID_DBP_I_POLTCMGANGLE:
		//			return _("Target CMG-Angle");
		return _("Target ") + g_sCMGSynonym + _("-Angle");
	case ID_DBP_D_POLPERF:
		return _("Polar Performance");
	case ID_DBP_D_AVGWIND:
		return _("Average Wind");
	case ID_DBP_D_POLCOMP:
		return _("Polar Compass");

	}
	return _T("");
}

void getListItemForInstrument(wxListItem &item, unsigned int id)
{
	item.SetData(id);
	item.SetText(getInstrumentCaption(id));
	switch (id){
	case ID_DBP_I_POS:
	case ID_DBP_I_SOG:
	case ID_DBP_I_COG:
	case ID_DBP_M_COG:
	case ID_DBP_I_STW:
	case ID_DBP_I_HDT:
	case ID_DBP_I_HDM:
	case ID_DBP_I_AWS:
	case ID_DBP_I_DPT:
	case ID_DBP_I_MDA:
	case ID_DBP_I_TMP:
	case ID_DBP_I_ATMP:
	case ID_DBP_I_TWA:
	case ID_DBP_I_TWD:
	case ID_DBP_I_TWS:
	case ID_DBP_I_AWA:
	case ID_DBP_I_VMG:
		//case ID_DBP_I_RSA:
		//case ID_DBP_I_SAT:
		//case ID_DBP_I_PTR:
	case ID_DBP_I_CLK:
	case ID_DBP_I_SUN:
		//       case ID_DBP_I_VLW1:
		//case ID_DBP_I_VLW2:
	case ID_DBP_I_TWAMARK:
	case ID_DBP_I_FOS:
	case ID_DBP_I_PITCH:
	case ID_DBP_I_HEEL:
	case  ID_DBP_I_LEEWAY:
	case ID_DBP_I_CURRDIR:
	case ID_DBP_I_CURRSPD:
	case ID_DBP_I_POLSPD:
	case ID_DBP_I_POLVMG:
	case ID_DBP_I_POLTVMG:
	case ID_DBP_I_POLTVMGANGLE:
	case ID_DBP_I_POLCMG:
	case ID_DBP_I_POLTCMG:
	case ID_DBP_I_POLTCMGANGLE:
		item.SetImage(0);
		break;
	case ID_DBP_D_SOG:
	case ID_DBP_D_COG:
	case ID_DBP_D_AW:
	case ID_DBP_D_AWA:
	case ID_DBP_D_AWS:
	case ID_DBP_D_TW:
	case ID_DBP_D_AWA_TWA:
	case ID_DBP_D_TWD:
	case ID_DBP_D_DPT:
	case ID_DBP_D_MDA:
	case ID_DBP_D_VMG:
		//case ID_DBP_D_RSA:
		//case ID_DBP_D_GPS:
	case ID_DBP_D_HDT:
		//case ID_DBP_D_MON:
	case ID_DBP_D_WDH:
	case ID_DBP_D_BPH:
		//case ID_DBP_D_CURRDIR:
	case ID_DBP_D_BRG:
	case ID_DBP_D_POLPERF:
	case ID_DBP_D_AVGWIND:
	case ID_DBP_D_POLCOMP:
		item.SetImage(1);
		break;
	}
}

/*  These two function were taken from gpxdocument.cpp */
int GetRandomNumber(int range_min, int range_max)
{
	long u = (long)wxRound(((double)rand() / ((double)(RAND_MAX)+1) * (range_max - range_min)) + range_min);
	return (int)u;
}

// RFC4122 version 4 compliant random UUIDs generator.
wxString GetUUID(void)
{
	wxString str;
	struct {
		int time_low;
		int time_mid;
		int time_hi_and_version;
		int clock_seq_hi_and_rsv;
		int clock_seq_low;
		int node_hi;
		int node_low;
	} uuid;

	uuid.time_low = GetRandomNumber(0, 2147483647);//FIXME: the max should be set to something like MAXINT32, but it doesn't compile un gcc...
	uuid.time_mid = GetRandomNumber(0, 65535);
	uuid.time_hi_and_version = GetRandomNumber(0, 65535);
	uuid.clock_seq_hi_and_rsv = GetRandomNumber(0, 255);
	uuid.clock_seq_low = GetRandomNumber(0, 255);
	uuid.node_hi = GetRandomNumber(0, 65535);
	uuid.node_low = GetRandomNumber(0, 2147483647);

	/* Set the two most significant bits (bits 6 and 7) of the
	* clock_seq_hi_and_rsv to zero and one, respectively. */
	uuid.clock_seq_hi_and_rsv = (uuid.clock_seq_hi_and_rsv & 0x3F) | 0x80;

	/* Set the four most significant bits (bits 12 through 15) of the
	* time_hi_and_version field to 4 */
	uuid.time_hi_and_version = (uuid.time_hi_and_version & 0x0fff) | 0x4000;

	str.Printf(_T("%08x-%04x-%04x-%02x%02x-%04x%08x"),
		uuid.time_low,
		uuid.time_mid,
		uuid.time_hi_and_version,
		uuid.clock_seq_hi_and_rsv,
		uuid.clock_seq_low,
		uuid.node_hi,
		uuid.node_low);

	return str;
}

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

tactics_pi::tactics_pi(void *ppimgr) :
wxTimer(this), opencpn_plugin_112(ppimgr)
{
	// Create the PlugIn icons
	initialize_images();

}

tactics_pi::~tactics_pi(void)
{
	delete _img_tactics_pi;
	delete _img_tactics;
	delete _img_dial;
	delete _img_instrument;
	delete _img_minus;
	delete _img_plus;
}

int tactics_pi::Init(void)
{

    AddLocaleCatalog( _T("opencpn-tactics_pi") );


    mVar = NAN;
    mPriPosition = 99;
    mPriCOGSOG = 99;
    mPriHeadingT = 99; // True heading
    mPriHeadingM = 99; // Magnetic heading
    mPriVar = 99;
    mPriDateTime = 99;
    mPriAWA = 99; // Relative wind
    mPriTWA = 99; // True wind
    mPriDepth = 99;
    m_config_version = -1;
    mHDx_Watchdog = 2;
    mHDT_Watchdog = 2;
    mGPS_Watchdog = 5;
    mVar_Watchdog = 2;
    mBRG_Watchdog = 2;
    mTWS_Watchdog = 5;
    mTWD_Watchdog = 5;
    mAWS_Watchdog = 2;
    //************TR

	alpha_currspd = 0.2;  //smoothing constant for current speed
	alpha_CogHdt = 0.1; // smoothing constant for diff. btw. Cog & Hdt
	m_alphaLaylineCog = 0.2; //0.1
	m_ExpSmoothCurrSpd = NAN;
	m_ExpSmoothCurrDir = NAN;
	m_ExpSmoothSog = NAN;
	m_ExpSmoothSinCurrDir = NAN;
	m_ExpSmoothCosCurrDir = NAN;
	m_ExpSmoothSinCog = NAN;
	m_ExpSmoothCosCog = NAN;
	m_CurrentDirection = NAN;
	m_LaylineSmoothedCog = NAN;
	m_LaylineDegRange = 0;
	mSinCurrDir = new DoubleExpSmooth(g_dalpha_currdir);
	mCosCurrDir = new DoubleExpSmooth(g_dalpha_currdir);
	mExpSmoothCurrSpd = new ExpSmooth(alpha_currspd);
	mExpSmoothSog = new DoubleExpSmooth(0.4);
	mExpSmSinCog = new ExpSmooth(m_alphaLaylineCog);
	mExpSmCosCog = new ExpSmooth(m_alphaLaylineCog);
    m_ExpSmoothDegRange = 0;
	mExpSmDegRange = new ExpSmooth(g_dalphaDeltCoG);
	mExpSmDegRange->SetInitVal(g_iMinLaylineWidth);
	mExpSmDiffCogHdt = new ExpSmooth(alpha_CogHdt);
	mExpSmDiffCogHdt->SetInitVal(0);
	m_bShowPolarOnChart = false;
	m_bShowWindbarbOnChart = false;
	m_bDisplayCurrentOnChart = false;
	m_LeewayOK = false;
	mHdt = NAN;
	mStW = NAN;
	mTWA = NAN;
	mTWD = NAN;
	mTWS = NAN;
	m_calcTWA = NAN;
	m_calcTWD = NAN;
	m_calcTWS = NAN;
	mSOG = NAN;
	mCOG = NAN;
	mlat = NAN;
	mlon = NAN;
	mheel = NAN;
	mLeeway = NAN;
	mPolarTargetSpeed = NAN;
	mBRG = NAN;
	mVMGGain = mCMGGain = mVMGoptAngle = mCMGoptAngle = 0.0;
	mPredictedCoG = NAN;
	for (int i = 0; i < COGRANGE; i++) m_COGRange[i] = NAN;

	m_bTrueWind_available = false;
	//*****************
	g_pFontTitle = new wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL);
	g_pFontData = new wxFont(14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	g_pFontLabel = new wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	g_pFontSmall = new wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	m_pauimgr = GetFrameAuiManager();
	m_pauimgr->Connect(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(tactics_pi::OnPaneClose),
		NULL, this);

	//    Get a pointer to the opencpn configuration object
	m_pconfig = GetOCPNConfigObject();

	//    And load the configuration items
	LoadConfig();

	BoatPolar = new Polar(this);
	if (g_path_to_PolarFile != _T("NULL"))
		BoatPolar->loadPolar(g_path_to_PolarFile);
	else
		BoatPolar->loadPolar(_T("NULL"));
	//    This PlugIn needs a toolbar icon
	wxString shareLocn = *GetpSharedDataLocation() +
		_T("plugins") + wxFileName::GetPathSeparator() +
		_T("tactics_pi") + wxFileName::GetPathSeparator()
		+ _T("data") + wxFileName::GetPathSeparator();

	wxString normalIcon = shareLocn + _T("Tactics.svg");
	wxString toggledIcon = shareLocn + _T("Tactics_toggled.svg");
	wxString rolloverIcon = shareLocn + _T("Tactics_rollover.svg");

	//  For journeyman styles, we prefer the built-in raster icons which match the rest of the toolbar.
	/* if (GetActiveStyleName().Lower() != _T("traditional")){
	normalIcon = _T("");
	toggledIcon = _T("");
	rolloverIcon = _T("");
	}*/

	m_toolbar_item_id = InsertPlugInToolSVG(_T(""), normalIcon, rolloverIcon, toggledIcon, wxITEM_CHECK,
		_("Tactics"), _T(""), NULL, TACTICS_TOOL_POSITION, 0, this);


	ApplyConfig();

	//  If we loaded a version 1 config setup, convert now to version 2
	if (m_config_version == 1) {
		SaveConfig();
	}

	Start(1000, wxTIMER_CONTINUOUS);
	/* TR */
	// Context menue for making marks    
	m_pmenu = new wxMenu();
	// this is a dummy menu required by Windows as parent to item created
	wxMenuItem *pmi = new wxMenuItem(m_pmenu, -1, _T("Set Tactics Mark "));
	int miid = AddCanvasContextMenuItem(pmi, this);
	SetCanvasContextMenuItemViz(miid, true);

	return (WANTS_CURSOR_LATLON |
		WANTS_TOOLBAR_CALLBACK |
		INSTALLS_TOOLBAR_TOOL |
		WANTS_PREFERENCES |
		WANTS_CONFIG |
		WANTS_NMEA_SENTENCES |
		WANTS_NMEA_EVENTS |
		USES_AUI_MANAGER |
		WANTS_PLUGIN_MESSAGING |
		WANTS_OPENGL_OVERLAY_CALLBACK |
		WANTS_OVERLAY_CALLBACK
		);
}

bool tactics_pi::DeInit(void)
{
	SaveConfig();
	if (IsRunning()) // Timer started?
		Stop(); // Stop timer

	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindow *tactics_window = m_ArrayOfTacticsWindow.Item(i)->m_pTacticsWindow;
		if (tactics_window) {
			m_pauimgr->DetachPane(tactics_window);
			tactics_window->Close();
			tactics_window->Destroy();
			m_ArrayOfTacticsWindow.Item(i)->m_pTacticsWindow = NULL;
		}
	}

	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindowContainer *pdwc = m_ArrayOfTacticsWindow.Item(i);
		delete pdwc;
	}

	delete g_pFontTitle;
	delete g_pFontData;
	delete g_pFontLabel;
	delete g_pFontSmall;
	//TR
	if (m_pRoute){
		m_pRoute->pWaypointList->DeleteContents(true);
		DeletePlugInRoute(m_pRoute->m_GUID);
	}

	return true;
}
//*********************************************************************************
void tactics_pi::Notify()
{
	SendUtcTimeToAllInstruments(mUTCDateTime);
	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindow *tactics_window = m_ArrayOfTacticsWindow.Item(i)->m_pTacticsWindow;
		if (tactics_window) tactics_window->Refresh();
	}
	//  Manage the watchdogs
	mHDx_Watchdog--;
	if (mHDx_Watchdog <= 0) {
		mHdm = NAN;
		SendSentenceToAllInstruments(OCPN_DBP_STC_HDM, mHdm, _T("\u00B0"));
	}

	mHDT_Watchdog--;
	if (mHDT_Watchdog <= 0) {
		mHdt = NAN;

        SendSentenceToAllInstruments( OCPN_DBP_STC_HDT, NAN, _T("\u00B0T") );
    }

    mVar_Watchdog--;
    if( mVar_Watchdog <= 0 ) {
        mVar = NAN;
        mPriVar = 99;
        SendSentenceToAllInstruments( OCPN_DBP_STC_HMV, NAN, _T("\u00B0T") );
    }

    mGPS_Watchdog--;
    if( mGPS_Watchdog <= 0 ) {
        SAT_INFO sats[4];
        for(int i=0 ; i < 4 ; i++) {
            sats[i].SatNumber = 0;
            sats[i].SignalToNoiseRatio = 0;
        }
        SendSatInfoToAllInstruments( 0, 1, sats );
        SendSatInfoToAllInstruments( 0, 2, sats );
        SendSatInfoToAllInstruments( 0, 3, sats );

        mSatsInView = 0;
        //SendSentenceToAllInstruments( OCPN_DBP_STC_SAT, 0, _T("") );
		
    }
    mBRG_Watchdog--;
    if (mBRG_Watchdog <= 0) {
      SendSentenceToAllInstruments(OCPN_DBP_STC_BRG, NAN, _T("\u00B0"));
    }
    mTWS_Watchdog--;
    if (mTWS_Watchdog <= 0) {
      mTWS = NAN;
      SendSentenceToAllInstruments(OCPN_DBP_STC_TWS, NAN, _T(""));
    }
    mTWD_Watchdog--;
    if (mTWD_Watchdog <= 0) {
      mTWD = NAN;
      mTWA = NAN;
      SendSentenceToAllInstruments(OCPN_DBP_STC_TWD, NAN, _T("\u00B0"));
      SendSentenceToAllInstruments(OCPN_DBP_STC_TWA, NAN, _T("\u00B0"));
    }
    mAWS_Watchdog--;
    if (mAWS_Watchdog <= 0) {
      SendSentenceToAllInstruments(OCPN_DBP_STC_AWS, NAN, _T(""));
    }

    ExportPerformanceData();

}
//*********************************************************************************
int tactics_pi::GetAPIVersionMajor()
{
	return MY_API_VERSION_MAJOR;
}
//*********************************************************************************
int tactics_pi::GetAPIVersionMinor()
{
	return MY_API_VERSION_MINOR;
}
//*********************************************************************************
int tactics_pi::GetPlugInVersionMajor()
{
	return PLUGIN_VERSION_MAJOR;
}
//*********************************************************************************
int tactics_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}
//*********************************************************************************
wxBitmap *tactics_pi::GetPlugInBitmap()
{
	return _img_tactics_pi;
}

wxString tactics_pi::GetCommonName()
{
	return _("Tactics");
}
//*********************************************************************************
wxString tactics_pi::GetShortDescription()
{
	return _("Tactics PlugIn for OpenCPN");
}
//*********************************************************************************
wxString tactics_pi::GetLongDescription()
{
	return _("Tactics PlugIn for OpenCPN\n\
			 Provides performance data & instrument display from NMEA source and polar file.");

}
//*********************************************************************************
void tactics_pi::SendSentenceToAllInstruments(int st, double value, wxString unit)
{
	if (st == OCPN_DBP_STC_AWS){
		//Correct AWS with heel if global variable set and heel is available
		//correction only makes sense if you use a heel sensor 
		//AWS_corrected = AWS_measured * cos(AWA_measured) / cos(AWA_corrected)
		if (g_bCorrectAWwithHeel == true && g_bUseHeelSensor && !wxIsNaN(mheel) && !wxIsNaN(value))
			value = value / cos(mheel*M_PI / 180.);
	}
	if (st == OCPN_DBP_STC_STW){
		//Correct STW with Leeway if global variable set and heel is available
		//correction only makes sense if you use a heel sensor 
		if (g_bCorrectSTWwithLeeway == true && g_bUseHeelSensor && !wxIsNaN(mLeeway) && !wxIsNaN(mheel))
			value = value / cos(mLeeway *M_PI / 180.0);
	}
	if (st == OCPN_DBP_STC_BRG){
      if (m_pMark && !wxIsNaN(mlat) && !wxIsNaN(mlon)) {
			double dist;
			DistanceBearingMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, mlat, mlon, &value, &dist);
			unit = _T("TacticsWP");
			//m_BearingUnit = _T("\u00B0");
		}
	}
	if (st == OCPN_DBP_STC_AWA){
		if (g_bCorrectAWwithHeel == true && g_bUseHeelSensor && !wxIsNaN(mLeeway) && !wxIsNaN(mheel)){
			//Correct AWA with heel if global variable set and heel is available
			//correction only makes sense if you use a heel sensor 
			double tan_awa = tan(value * M_PI / 180.);
			double awa_heel;
			if (wxIsNaN(tan_awa))
				awa_heel = value;
			else
			{
				double cos_heel = cos(mheel * M_PI / 180.);
				awa_heel = atan(tan_awa / cos_heel) *180. / M_PI;
				if (value >= 0.0){
					if (value > 90.0)
						awa_heel += 180.0;
				}
				else{
					if (value < -90.0)
						awa_heel -= 180.0;
				}
			}
			value = awa_heel;
		}
	}
	//}

	//    if (g_bForceTrueWindCalculation && ((st == OCPN_DBP_STC_TWS && !wxIsNaN(value)) || st == OCPN_DBP_STC_TWA || st == OCPN_DBP_STC_TWD)){
	if (g_bForceTrueWindCalculation && ((st == OCPN_DBP_STC_TWS || st == OCPN_DBP_STC_TWA || st == OCPN_DBP_STC_TWD) && !wxIsNaN(value))){
		//do nothing, if we force TW calculation-> the distribution to the plugin instruments is done in CalculateTrueWind()
		//for all other sentences and usage of the original NMEA-TW, see else ...
		;
	}
	else{
		for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
			TacticsWindow *tactics_window = m_ArrayOfTacticsWindow.Item(i)->m_pTacticsWindow;
			if (tactics_window)   tactics_window->SendSentenceToAllInstruments(st, value, unit);
		}
	}
	// calculate some data and distribute to all instruments as well
	SetCalcVariables(st, value, unit);
	CalculateTrueWind(st, value, unit);
	CalculateLeeway(st, value, unit);
	CalculateCurrent(st, value, unit);
	CalculateLaylineDegreeRange();
	CalculatePerformanceData();
	//ExportPerformanceData();
}
/*********************************************************************************
Draw the OpenGL overlay
Called by Plugin Manager on main system process cycle
**********************************************************************************/
bool tactics_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
	b_tactics_dc_message_shown = false; // show message box if RenderOverlay() is called again
	if (m_bLaylinesIsVisible || m_bDisplayCurrentOnChart || m_bShowWindbarbOnChart || m_bShowPolarOnChart){
		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_POLYGON_BIT | GL_HINT_BIT);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glPushMatrix();
		DoRenderLaylineGLOverlay(pcontext, vp);
		DoRenderCurrentGLOverlay(pcontext, vp);
		glPopMatrix();
		glPopAttrib();
	}
	return true;

}
/*********************************************************************************
Taken from cutil
**********************************************************************************/
inline int myCCW(wxRealPoint p0, wxRealPoint p1, wxRealPoint p2) {
	double dx1, dx2;
	double dy1, dy2;

	dx1 = p1.x - p0.x; dx2 = p2.x - p0.x;
	dy1 = p1.y - p0.y; dy2 = p2.y - p0.y;

	/* This is basically a slope comparison: we don't do divisions because

	* of divide by zero possibilities with pure horizontal and pure
	* vertical lines.
	*/
	return ((dx1 * dy2 > dy1 * dx2) ? 1 : -1);

}
/*********************************************************************************
returns true if we have a line intersection.
Taken from cutil, but with double variables
**********************************************************************************/
inline bool IsLineIntersect(wxRealPoint p1, wxRealPoint p2, wxRealPoint p3, wxRealPoint p4)
{
	return (((myCCW(p1, p2, p3) * myCCW(p1, p2, p4)) <= 0)
		&& ((myCCW(p3, p4, p1) * myCCW(p3, p4, p2) <= 0)));

}
/********************************************************************************
calculate Line intersection between 2 lines, each described by 2 points
return lat/lon of intersection point
basic calculation:
int p1[] = { -4,  5, 1 };
int p2[] = { -2, -5, 1 };
int p3[] = { -6,  2, 1 };
int p4[] = {  5,  4, 1 };
int l1[3], l2[3], s[3];
double sch[2];
l1[0] = p1[1] * p2[2] - p1[2] * p2[1];
l1[1] = p1[2] * p2[0] - p1[0] * p2[2];
l1[2] = p1[0] * p2[1] - p1[1] * p2[0];
l2[0] = p3[1] * p4[2] - p3[2] * p4[1];
l2[1] = p3[2] * p4[0] - p3[0] * p4[2];
l2[2] = p3[0] * p4[1] - p3[1] * p4[0];
s[0] = l1[1] * l2[2] - l1[2] * l2[1];
s[1] = l1[2] * l2[0] - l1[0] * l2[2];
s[2] = l1[0] * l2[1] - l1[1] * l2[0];
sch[0] = (double)s[0] / (double)s[2];
sch[1] = (double)s[1] / (double)s[2];
*********************************************************************************/
wxRealPoint GetLineIntersection(wxRealPoint line1point1, wxRealPoint line1point2, wxRealPoint line2point1, wxRealPoint line2point2)
{
	wxRealPoint intersect;
	intersect.x = -999.;
	intersect.y = -999.;
	if (IsLineIntersect(line1point1, line1point2, line2point1, line2point2)){
		double line1[3], line2[3], s[3];
		line1[0] = line1point1.y * 1. - 1. * line1point2.y;
		line1[1] = 1. * line1point2.x - line1point1.x * 1.;
		line1[2] = line1point1.x * line1point2.y - line1point1.y * line1point2.x;
		line2[0] = line2point1.y * 1. - 1. * line2point2.y;
		line2[1] = 1. * line2point2.x - line2point1.x * 1.;
		line2[2] = line2point1.x * line2point2.y - line2point1.y * line2point2.x;
		s[0] = line1[1] * line2[2] - line1[2] * line2[1];
		s[1] = line1[2] * line2[0] - line1[0] * line2[2];
		s[2] = line1[0] * line2[1] - line1[1] * line2[0];
		intersect.x = s[0] / s[2];
		intersect.y = s[1] / s[2];
	}
	return intersect;
}
/*********************************************************************************
Function calculates the time to sail for a given distance, TWA and TWS, based on
the polar data
**********************************************************************************/
double CalcPolarTimeToMark(double distance, double twa, double tws)
{
	double pspd = BoatPolar->GetPolarSpeed(twa, tws);
	return distance / pspd;
}
/*********************************************************************************
Function returns the (smaller) TWA of a given TWD and Course.
Used for Target-CMG calculation.
It covers the 359 - 0 degree problem
e.g. : TWD = 350, ctm = 10; the TWA is returned as 20 degrees
(and not 340 if we'd do a simple TWD - ctm)
**********************************************************************************/
double getMarkTWA(double twd, double ctm)
{
	double val, twa;
	if (twd > 180)
	{
		val = twd - 180;
		if (ctm < val)
			twa = 360 - twd + ctm;
		else
			twa = twd > ctm ? twd - ctm : ctm - twd;
	}
	else
	{
		val = twd + 180;
		if (ctm > val)
			twa = 360 - ctm + twd;
		else
			twa = twd > ctm ? twd - ctm : ctm - twd;
	}
	return twa;
}
/*********************************************************************************
Function returns the (smaller) degree range of 2 angular values
on the compass rose (without sign)
It covers the 359 - 0 degree problem
e.g. : max = 350, min = 10; the rage is returned as 20 degrees
(and not 340 if we'd do a simple max - min)
**********************************************************************************/
double getDegRange(double max, double min)
{
	double val, range;
	if (max > 180)
	{
		val = max - 180;
		if (min < val)
			range = 360 - max + min;
		else
			range = max > min ? max - min : min - max;
	}
	else
	{
		val = max + 180;
		if (min > val)
			range = 360 - min + max;
		else
			range = max > min ? max - min : min - max;
	}
	return range;
}
/*********************************************************************************
Function returns the (smaller) signed degree range of 2 angular values
on the compass rose (clockwise is +)
It covers the 359 - 0 degree problem
e.g. : fromAngle = 350, toAngle = 10; the range is returned as +20 degrees
(and not 340 if we'd do a simple fromAngle - toAngle)
**********************************************************************************/
double getSignedDegRange(double fromAngle, double toAngle)
{
	double val, range;
	if (fromAngle > 180)
	{
		val = fromAngle - 180;
		if (toAngle < val)
			range = 360 - fromAngle + toAngle;
		else
			range = toAngle - fromAngle;
	}
	else
	{
		val = fromAngle + 180;
		if (toAngle > val)
			range = -(360 - toAngle + fromAngle);
		else
			range = toAngle - fromAngle;
	}
	return range;
}
/*********************************************************************************
Draw the OpenGL Layline overlay
**********************************************************************************/
void tactics_pi::DoRenderLaylineGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
	wxPoint  mark_center;
	wxPoint boat;
	if (!wxIsNaN(mlat) && !wxIsNaN(mlon)) {
		GetCanvasPixLL(vp, &vpoints[0], mlat, mlon);
		boat = vpoints[0];
		/*****************************************************************************************
		Draw wind barb on boat position
		******************************************************************************************/
        //mTWD=NAN caught in subroutines
		DrawWindBarb(boat, vp);
		DrawPolar(vp, boat, mTWD);
	}
	//wxString GUID = _T("TacticsWP");
	if (!GetSingleWaypoint(_T("TacticsWP"), m_pMark)) m_pMark = NULL;
	if (m_pMark){
		/*****************************************************************************************
		Draw wind barb on mark position
		******************************************************************************************/
		GetCanvasPixLL(vp, &mark_center, m_pMark->m_lat, m_pMark->m_lon);
		DrawWindBarb(mark_center, vp);
		/*****************************************************************************************
		Draw direct line from boat to mark as soon as mark is dropped. Reduces problems to find it ...
		******************************************************************************************/
		glColor4ub(255, 128, 0, 168); //orange
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2d(boat.x, boat.y);
		glVertex2d(mark_center.x, mark_center.y);
		glEnd();

	}

	if (m_bLaylinesIsVisible){
      if (!wxIsNaN(mlat) && !wxIsNaN(mlon) && !wxIsNaN(mCOG) && !wxIsNaN(mHdt) && !wxIsNaN(mStW) && !wxIsNaN(mTWS) && !wxIsNaN(mTWA)) {
			if (wxIsNaN(m_LaylineSmoothedCog)) m_LaylineSmoothedCog = mCOG;
            if (wxIsNaN(mLeeway)) mLeeway = 0.0;
			/*****************************************************************************************
			Draw the boat laylines, independent from the "Temp. Tactics WP"

			The first (foreward) layline is on the COG pointer
			******************************************************************************************/
			wxString curTack = mAWAUnit;
			wxString targetTack = _T("");
			//it shows '°L'= wind from left = port tack or '°R'=wind from right = starboard tack
			//we're on port tack, so vertical layline is red
			if (curTack == _T("\u00B0L")) {
				GLubyte red(204), green(41), blue(41), alpha(128);
				glColor4ub(204, 41, 41, 128);                 	// red, green, blue,  alpha
				targetTack = _T("R");
			}
			else if (curTack == _T("\u00B0R"))  {// we're on starboard tack, so vertical layline is green
				GLubyte red(0), green(200), blue(0), alpha(128);
				glColor4ub(0, 200, 0, 128);                 	// red, green, blue,  alpha
				targetTack = _T("L");
			}
			double tmplat1, tmplon1, tmplat2, tmplon2;
			PositionBearingDistanceMercator_Plugin(mlat, mlon, m_LaylineSmoothedCog - m_ExpSmoothDegRange / 2., g_dLaylineLengthonChart, &tmplat1, &tmplon1);
			GetCanvasPixLL(vp, &vpoints[1], tmplat1, tmplon1);
			PositionBearingDistanceMercator_Plugin(mlat, mlon, m_LaylineSmoothedCog + m_ExpSmoothDegRange / 2., g_dLaylineLengthonChart, &tmplat2, &tmplon2);
			GetCanvasPixLL(vp, &vpoints[2], tmplat2, tmplon2);
			glBegin(GL_TRIANGLES);
			glVertex2d(vpoints[0].x, vpoints[0].y);
			glVertex2d(vpoints[1].x, vpoints[1].y);
			glVertex2d(vpoints[2].x, vpoints[2].y);
			glEnd();
			/*****************************************************************************************
			Calculate and draw  the second boat layline (for other tack) :
			---------------------------------------------------------
			Approach : we're drawing the first layline on COG, but TWA is based on boat heading (Hdt).
			To calculate the layline of the other tack, sum up
			diff_btw_Cog_and_Hdt (now we're on Hdt)
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
			double  diffCogHdt;
			double tws_kts = fromUsrSpeed_Plugin(mTWS, g_iDashWindSpeedUnit);
			double stw_kts = fromUsrSpeed_Plugin(mStW, g_iDashSpeedUnit);
			double currspd_kts = wxIsNaN(m_ExpSmoothCurrSpd) ? 0.0 : fromUsrSpeed_Plugin(m_ExpSmoothCurrSpd, g_iDashSpeedUnit);
			double currdir = wxIsNaN(m_CurrentDirection) ? 0.0 : m_CurrentDirection;
			diffCogHdt = getDegRange(mCOG, mHdt);
			mExpSmDiffCogHdt->SetAlpha(alpha_CogHdt);
			m_ExpSmoothDiffCogHdt = mExpSmDiffCogHdt->GetSmoothVal((diffCogHdt < 0 ? -diffCogHdt : diffCogHdt));
			if (targetTack == _T("R")){ // currently wind is from port ...now
				mPredictedHdG = m_LaylineSmoothedCog - m_ExpSmoothDiffCogHdt - 2 * mTWA - fabs(mLeeway); //Leeway is signed 
				GLubyte red(0), green(200), blue(0), alpha(128);
				glColor4ub(0, 200, 0, 128);                 	// red, green, blue,  alpha
			}
			else if (targetTack == _T("L")){ //currently wind from starboard
				mPredictedHdG = m_LaylineSmoothedCog + m_ExpSmoothDiffCogHdt + 2 * mTWA + fabs(mLeeway); //Leeway is signed 
				GLubyte red(204), green(41), blue(41), alpha(128);
				glColor4ub(204, 41, 41, 128);                 	// red, green, blue,  alpha
			}
			else {
				mPredictedHdG = (mTWA < 10) ? 180 : 0;
			}
			if (mPredictedHdG < 0) mPredictedHdG += 360;
			if (mPredictedHdG >= 360) mPredictedHdG -= 360;
			double predictedLatHdt, predictedLonHdt, predictedLatCog, predictedLonCog;
			double  predictedSog;
			//apply current on predicted Heading
			PositionBearingDistanceMercator_Plugin(mlat, mlon, mPredictedHdG, stw_kts, &predictedLatHdt, &predictedLonHdt);
			PositionBearingDistanceMercator_Plugin(predictedLatHdt, predictedLonHdt, currdir, currspd_kts, &predictedLatCog, &predictedLonCog);
			DistanceBearingMercator_Plugin(predictedLatCog, predictedLonCog, mlat, mlon, &mPredictedCoG, &predictedSog);

			tackpoints[0] = vpoints[0];
			double tmplat3, tmplon3, tmplat4, tmplon4;
			PositionBearingDistanceMercator_Plugin(mlat, mlon, mPredictedCoG - m_ExpSmoothDegRange / 2., g_dLaylineLengthonChart, &tmplat3, &tmplon3);
			GetCanvasPixLL(vp, &tackpoints[1], tmplat3, tmplon3);
			PositionBearingDistanceMercator_Plugin(mlat, mlon, mPredictedCoG + m_ExpSmoothDegRange / 2., g_dLaylineLengthonChart, &tmplat4, &tmplon4);
			GetCanvasPixLL(vp, &tackpoints[2], tmplat4, tmplon4);
			glBegin(GL_TRIANGLES);
			glVertex2d(tackpoints[0].x, tackpoints[0].y);
			glVertex2d(tackpoints[1].x, tackpoints[1].y);
			glVertex2d(tackpoints[2].x, tackpoints[2].y);
			glEnd();

//            wxLogMessage("mlat=%f, mlon=%f,currspd=%f,predictedCoG=%f, mTWA=%f,mLeeway=%f, g_iDashSpeedUnit=%d", mlat, mlon, currspd_kts, mPredictedCoG, mTWA, mLeeway,g_iDashSpeedUnit);
            //wxLogMessage("tackpoints[0].x=%d, tackpoints[0].y=%d,tackpoints[1].x=%d, tackpoints[1].y=%d,tackpoints[2].x=%d, tackpoints[2].y=%d", tackpoints[0].x, tackpoints[0].y, tackpoints[1].x, tackpoints[1].y, tackpoints[2].x, tackpoints[2].y);
            //wxString GUID = _T("TacticsWP");

			//if (!GetSingleWaypoint(_T("TacticsWP"), m_pMark)) m_pMark = NULL;
			if (m_pMark)
			{
				/*********************************************************************************************************
				Draw the laylines btw. mark and boat with max 1 tack
				Additionally calculate if sailing the directline to mark is faster.
				This direct line calculation is based on the theoretical polardata for the TWA when on 'course to mark',
				TWS and 'distance to mark'
				Idea:
				* we draw the VMG laylines on mark and boat position
				* currently (for simplicity) I'm drawing/calculating the laylines up- AND downwind ! Room for improvement ...
				* per layline pair ..
				* check, if they intersect, if yes
				* calculate time to sail
				* calculate time to sail on direct line, based on polar data
				* either draw layline or direct line
				**********************************************************************************************************/
				//calculate Course to Mark = CTM
				double CTM, DistToMark, directLineTimeToMark, directLineTWA;
				DistanceBearingMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, mlat, mlon, &CTM, &DistToMark);
				//calc time-to-mark on direct line, versus opt. TWA and intersection
				directLineTWA = getMarkTWA(mTWD, CTM);
				directLineTimeToMark = CalcPolarTimeToMark(DistToMark, directLineTWA, tws_kts);
				if (wxIsNaN(directLineTimeToMark)) directLineTimeToMark = 99999;
				//use target VMG calculation for laylines-to-mark
				TargetxMG tvmg = BoatPolar->Calc_TargetVMG(directLineTWA, tws_kts); // directLineTWA <= 90° --> upwind, >90 --> downwind
				//optional : use target CMG calculation for laylines-to-mark
				//TargetxMG tvmg = BoatPolar->Calc_TargetCMG(mTWS,mTWD,CTM); // directLineTWA <= 90° --> upwind, >90 --> downwind
				double sigCTM_TWA = getSignedDegRange(CTM, mTWD);
				double cur_tacklinedir, target_tacklinedir;
				if (!wxIsNaN(tvmg.TargetAngle))
				{
					if (curTack == _T("\u00B0L")){
						cur_tacklinedir = mTWD - tvmg.TargetAngle - fabs(mLeeway);  //- m_ExpSmoothDiffCogHdt
						target_tacklinedir = mTWD + tvmg.TargetAngle + fabs(mLeeway);//+ m_ExpSmoothDiffCogHdt
					}
					else{
						cur_tacklinedir = mTWD + tvmg.TargetAngle + fabs(mLeeway);//+ m_ExpSmoothDiffCogHdt
						target_tacklinedir = mTWD - tvmg.TargetAngle - fabs(mLeeway);//- m_ExpSmoothDiffCogHdt
					}
					while (cur_tacklinedir < 0) cur_tacklinedir += 360;
					while (cur_tacklinedir > 359) cur_tacklinedir -= 360;
					while (target_tacklinedir < 0) target_tacklinedir += 360;
					while (target_tacklinedir > 359) target_tacklinedir -= 360;
					double lat, lon, curlat, curlon, act_sog;
					wxRealPoint m_end, m_end2, c_end, c_end2;
					//apply current on foreward layline
					PositionBearingDistanceMercator_Plugin(mlat, mlon, cur_tacklinedir, stw_kts, &lat, &lon);
					PositionBearingDistanceMercator_Plugin(lat, lon, currdir, currspd_kts, &curlat, &curlon);
					DistanceBearingMercator_Plugin(curlat, curlon, mlat, mlon, &cur_tacklinedir, &act_sog);
					//cur_tacklinedir=local_bearing(curlat, curlon, mlat, mlon);
					//apply current on mark layline
					PositionBearingDistanceMercator_Plugin(mlat, mlon, target_tacklinedir, stw_kts, &lat, &lon);
					PositionBearingDistanceMercator_Plugin(lat, lon, currdir, currspd_kts, &curlat, &curlon);
					DistanceBearingMercator_Plugin(curlat, curlon, mlat, mlon, &target_tacklinedir, &act_sog);
					//target_tacklinedir=local_bearing(curlat, curlon, mlat, mlon);
					double cur_tacklinedir2 = cur_tacklinedir > 180 ? cur_tacklinedir - 180 : cur_tacklinedir + 180;

					//Mark : get an end of the current dir
					PositionBearingDistanceMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, cur_tacklinedir, DistToMark * 2, &m_end.y, &m_end.x);
					//Mark : get the second end of the same line on the opposite direction
					PositionBearingDistanceMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, cur_tacklinedir2, DistToMark * 2, &m_end2.y, &m_end2.x);

					double boat_fwTVMGDir = target_tacklinedir > 180 ? target_tacklinedir - 180 : target_tacklinedir + 180;
					//Boat : get an end of the predicted layline
					PositionBearingDistanceMercator_Plugin(mlat, mlon, boat_fwTVMGDir, DistToMark * 2, &c_end.y, &c_end.x);
					//Boat : get the second end of the same line on the opposite direction
					PositionBearingDistanceMercator_Plugin(mlat, mlon, target_tacklinedir, DistToMark * 2, &c_end2.y, &c_end2.x);

					// see if we have an intersection of the 2 laylines
					wxRealPoint intersection_pos;
					intersection_pos = GetLineIntersection(c_end, c_end2, m_end, m_end2);

					if (intersection_pos.x > 0 && intersection_pos.y > 0) {
						//calc time-to-mark on direct line, versus opt. TWA and intersection
						// now calc time-to-mark via intersection
						double TimeToMarkwithIntersect, tempCTM, DistToMarkwInt, dist1, dist2;
						//distance btw. Boat and intersection
						DistanceBearingMercator_Plugin(mlat, mlon, intersection_pos.y, intersection_pos.x, &tempCTM, &dist1);
						//dist1 = local_distance(mlat, mlon, intersection_pos.y, intersection_pos.x);

						//distance btw. Intersection - Mark
						DistanceBearingMercator_Plugin(intersection_pos.y, intersection_pos.x, m_pMark->m_lat, m_pMark->m_lon, &tempCTM, &dist2);
						//dist2 = local_distance(intersection_pos.y, intersection_pos.x, m_pMark->m_lat, m_pMark->m_lon);

						//Total distance as sum of dist1 + dist2
						//Note : current is NOT yet taken into account here !
						DistToMarkwInt = dist1 + dist2;
						TimeToMarkwithIntersect = CalcPolarTimeToMark(DistToMarkwInt, tvmg.TargetAngle, tws_kts);
						if (wxIsNaN(TimeToMarkwithIntersect))TimeToMarkwithIntersect = 99999;
						if (TimeToMarkwithIntersect > 0 && directLineTimeToMark > 0){
							//only draw the laylines with intersection, if they are faster than the direct course
							if (TimeToMarkwithIntersect < directLineTimeToMark){
								if (curTack == _T("\u00B0L"))
									glColor4ub(255, 0, 0, 255);
								else
									glColor4ub(0, 200, 0, 255);
								glLineWidth(2);
								wxPoint inter;
								GetCanvasPixLL(vp, &inter, intersection_pos.y, intersection_pos.x);
								glBegin(GL_LINES); // intersect from forward layline --> target VMG --> mark
								glVertex2d(boat.x, boat.y); // from boat with target VMG-Angle sailing forward  to intersection
								glVertex2d(inter.x, inter.y);
								if (curTack == _T("\u00B0L"))
									glColor4ub(0, 200, 0, 255);
								else
									glColor4ub(255, 0, 0, 255);
								glVertex2d(inter.x, inter.y); // from intersection with target VMG-Angle to mark
								glVertex2d(mark_center.x, mark_center.y);
								glEnd();
							}
							else{ // otherwise highlight the direct line
								if (sigCTM_TWA < 0)
									glColor4ub(255, 0, 0, 255);
								else
									glColor4ub(0, 200, 0, 255);
								glLineWidth(2);
								glBegin(GL_LINES);
								glVertex2d(boat.x, boat.y);
								glVertex2d(mark_center.x, mark_center.y);
								glEnd();
							}
						}
					}
					else { //no intersection at all
						if (directLineTimeToMark < 99999.){ //but direct line may be valid
							if (sigCTM_TWA <0)
								glColor4ub(255, 0, 0, 255);
							else
								glColor4ub(0, 200, 0, 255);
							glLineWidth(2);
							glBegin(GL_LINES);
							glVertex2d(boat.x, boat.y);
							glVertex2d(mark_center.x, mark_center.y);
							glEnd();
						}
						else { //no intersection and no valid direct line
							// convert from coordinates to screen values
							wxPoint cogend, mark_end;
							GetCanvasPixLL(vp, &mark_end, m_end.y, m_end.x);
							GetCanvasPixLL(vp, &cogend, c_end.y, c_end.x);
							if (curTack == _T("\u00B0L"))glColor4ub(255, 0, 0, 255);
							else  glColor4ub(0, 200, 0, 255);
							glLineWidth(2);
							glLineStipple(4, 0xAAAA);
							glEnable(GL_LINE_STIPPLE);
							glBegin(GL_LINES); // intersect from forward layline --> target VMG --> mark
							glVertex2d(boat.x, boat.y); // from boat with target VMG-Angle sailing forward  to intersection
							glVertex2d(cogend.x, cogend.y);
							if (curTack == _T("\u00B0L"))glColor4ub(0, 200, 0, 255);
							else  glColor4ub(255, 0, 0, 255);
							glVertex2d(mark_end.x, mark_end.y); // from intersection with target VMG-Angle to mark
							glVertex2d(mark_center.x, mark_center.y);
							glEnd();
							glDisable(GL_LINE_STIPPLE);
						}
					}
				}
				while (target_tacklinedir < 0) target_tacklinedir += 360;
				while (target_tacklinedir > 359) target_tacklinedir -= 360;
				double target_tacklinedir2 = target_tacklinedir > 180 ? target_tacklinedir - 180 : target_tacklinedir + 180;
				wxRealPoint pm_end, pm_end2, pc_end, pc_end2;

				//Mark : get an end of the predicted layline
				PositionBearingDistanceMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, target_tacklinedir, DistToMark * 2, &pm_end.y, &pm_end.x);
				//Mark : get the second end of the same predicted layline on the opposite direction
				PositionBearingDistanceMercator_Plugin(m_pMark->m_lat, m_pMark->m_lon, target_tacklinedir2, DistToMark * 2, &pm_end2.y, &pm_end2.x);
				//PositionBearingDistanceMercator_Plugin(mlat, mlon, predictedCoG, g_dLaylineLengthonChart, &tmplat1, &tmplon1);
				double boat_tckTVMGDir = cur_tacklinedir > 180 ? cur_tacklinedir - 180 : cur_tacklinedir + 180;
				//Boat : get an end of the predicted layline
				PositionBearingDistanceMercator_Plugin(mlat, mlon, boat_tckTVMGDir, DistToMark * 2, &pc_end.y, &pc_end.x);
				//Boat : get the second end of the same predicted layline on the opposite direction
				PositionBearingDistanceMercator_Plugin(mlat, mlon, cur_tacklinedir, DistToMark * 2, &pc_end2.y, &pc_end2.x);

				// see if we have an intersection of the 2 laylines
				wxRealPoint pIntersection_pos;
				pIntersection_pos = GetLineIntersection(pc_end, pc_end2, pm_end, pm_end2);

				if (pIntersection_pos.x > 0 && pIntersection_pos.y > 0){
					//calc time-to-mark on direct line, versus opt. TWA and intersection
					// now calc time-to-mark via intersection
					double TimeToMarkwInt, tempCTM, DistToMarkwInt, dist1, dist2;
					//distance btw. boat and intersection
					DistanceBearingMercator_Plugin(mlat, mlon, pIntersection_pos.y, pIntersection_pos.x, &tempCTM, &dist1);
					//dist1 = local_distance(mlat, mlon, pIntersection_pos.y, pIntersection_pos.x);

					//distance btw. Intersection - Mark
					DistanceBearingMercator_Plugin(pIntersection_pos.y, pIntersection_pos.x, m_pMark->m_lat, m_pMark->m_lon, &tempCTM, &dist2);
					//dist2 = local_distance(pIntersection_pos.y, pIntersection_pos.x, m_pMark->m_lat, m_pMark->m_lon);
					//Total distance as sum of dist1 + dist2
					DistToMarkwInt = dist1 + dist2;
					TimeToMarkwInt = CalcPolarTimeToMark(DistToMarkwInt, tvmg.TargetAngle, tws_kts);
					if (wxIsNaN(TimeToMarkwInt))TimeToMarkwInt = 99999;
					if (TimeToMarkwInt > 0 && directLineTimeToMark > 0){
						//only draw the laylines with intersection, if they are faster than the direct course
						if (TimeToMarkwInt < directLineTimeToMark){
							if (curTack == _T("\u00B0L"))
								glColor4ub(0, 200, 0, 255);
							else
								glColor4ub(255, 0, 0, 255);
							glLineWidth(2);
							wxPoint pinter;
							GetCanvasPixLL(vp, &pinter, pIntersection_pos.y, pIntersection_pos.x);

							glBegin(GL_LINES); // intersect from target layline --> target other tack VMG --> mark
							glVertex2d(boat.x, boat.y);   //from boat to intersection with Target VMG-Angle, but sailing on other tack
							glVertex2d(pinter.x, pinter.y);
							if (curTack == _T("\u00B0L"))
								glColor4ub(255, 0, 0, 255);
							else
								glColor4ub(0, 200, 0, 255);
							glVertex2d(pinter.x, pinter.y);//from intersection to mark with Target VMG-Angle, but sailing on other tack
							glVertex2d(mark_center.x, mark_center.y);
							glEnd();
						}
						else { // otherwise highlight the direct line 
							if (sigCTM_TWA <0)
								glColor4ub(255, 0, 0, 255);
							else
								glColor4ub(0, 200, 0, 255);
							glLineWidth(2);
							glBegin(GL_LINES);
							glVertex2d(boat.x, boat.y);
							glVertex2d(mark_center.x, mark_center.y);
							glEnd();
						}
					}
				}
				else { //no intersection ...
					if (directLineTimeToMark < 99999.){ //but direct line may be valid
						if (sigCTM_TWA <0)
							glColor4ub(255, 0, 0, 255);
						else
							glColor4ub(0, 200, 0, 255);
						glLineWidth(2);
						glBegin(GL_LINES);
						glVertex2d(boat.x, boat.y);
						glVertex2d(mark_center.x, mark_center.y);
						glEnd();
					}
					else{
						wxPoint pcogend, pmarkend;
						GetCanvasPixLL(vp, &pmarkend, pm_end.y, pm_end.x);
						GetCanvasPixLL(vp, &pcogend, pc_end.y, pc_end.x);

						if (curTack == _T("\u00B0L"))glColor4ub(0, 200, 0, 255);
						else  glColor4ub(255, 0, 0, 255);
						glLineWidth(2);
						glLineStipple(4, 0xAAAA);
						glEnable(GL_LINE_STIPPLE);
						glBegin(GL_LINES); // intersect from target layline --> target other tack VMG --> mark
						glVertex2d(boat.x, boat.y);   //from boat to intersection with Target VMG-Angle, but sailing on other tack
						glVertex2d(pcogend.x, pcogend.y);
						if (curTack == _T("\u00B0L"))glColor4ub(255, 0, 0, 255);
						else glColor4ub(0, 200, 0, 255);
						glVertex2d(pmarkend.x, pmarkend.y);//from intersection to mark with Target VMG-Angle, but sailing on other tack
						glVertex2d(mark_center.x, mark_center.y);
						glEnd();
						glDisable(GL_LINE_STIPPLE);
					}
				}
			}
		}
	}

}
/*********************************************************************************
Draw the OpenGL Layline overlay
**********************************************************************************/
void tactics_pi::DoRenderCurrentGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
	if (m_bDisplayCurrentOnChart && !wxIsNaN(mlat) && !wxIsNaN(mlon) && !wxIsNaN(m_CurrentDirection)) {
		//draw the current on the chart here
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
		wxPoint boat;
		GetCanvasPixLL(vp, &boat, mlat, mlon);

		double m_radius = 100;
		wxRealPoint currpoints[7];
		double currval = m_CurrentDirection;
		double rotate = vp->rotation;
		double currvalue = ((currval - 90.) * M_PI / 180) + rotate;

		currpoints[0].x = boat.x + (m_radius * .40 * cos(currvalue));
		currpoints[0].y = boat.y + (m_radius * .40 * sin(currvalue));
		currpoints[1].x = boat.x + (m_radius * .18 * cos(currvalue + 1.5));
		currpoints[1].y = boat.y + (m_radius * .18 * sin(currvalue + 1.5));
		currpoints[2].x = boat.x + (m_radius * .10 * cos(currvalue + 1.5));
		currpoints[2].y = boat.y + (m_radius * .10 * sin(currvalue + 1.5));

		currpoints[3].x = boat.x + (m_radius * .3 * cos(currvalue + 2.8));
		currpoints[3].y = boat.y + (m_radius * .3 * sin(currvalue + 2.8));
		currpoints[4].x = boat.x + (m_radius * .3 * cos(currvalue - 2.8));
		currpoints[4].y = boat.y + (m_radius * .3 * sin(currvalue - 2.8));

		currpoints[5].x = boat.x + (m_radius * .10 * cos(currvalue - 1.5));
		currpoints[5].y = boat.y + (m_radius * .10 * sin(currvalue - 1.5));
		currpoints[6].x = boat.x + (m_radius * .18 * cos(currvalue - 1.5));
		currpoints[6].y = boat.y + (m_radius * .18 * sin(currvalue - 1.5));
 //below 0.2 knots the current generally gets inaccurate, as the error level gets too high.
        // as a hint, fade the current transparency below 0.25 knots...
//        int curr_trans = 164;
//        if (m_ExpSmoothCurrSpd <= 0.20) {
//          fading : 0.2 * 5 = 1 --> we set full transp. >=0.2
//          curr_trans = m_ExpSmoothCurrSpd * 5 * curr_trans;
//          if (curr_trans < 50) curr_trans = 50;  //lower limit
//        }
//        GLubyte red(7), green(107), blue(183), alpha(curr_trans);
//        glColor4ub(7, 107, 183, curr_trans);                 	// red, green, blue,  alpha

		GLubyte red(7), green(107), blue(183), alpha(164);
		glColor4ub(7, 107, 183, 164);                 	// red, green, blue,  alpha
//        glLineWidth(2);
//		glBegin(GL_POLYGON | GL_LINES);
        glBegin(GL_POLYGON);
		glVertex2d(currpoints[0].x, currpoints[0].y);
		glVertex2d(currpoints[1].x, currpoints[1].y);
		glVertex2d(currpoints[2].x, currpoints[2].y);
		glVertex2d(currpoints[3].x, currpoints[3].y);
		glVertex2d(currpoints[4].x, currpoints[4].y);
		glVertex2d(currpoints[5].x, currpoints[5].y);
		glVertex2d(currpoints[6].x, currpoints[6].y);
		glEnd();
	}
}
/*********************************************************************************
Draw the OpenGL Windbarb on the ships position overlay
Basics taken from tackandlay_pi and adopted
**********************************************************************************/
void tactics_pi::DrawWindBarb(wxPoint pp, PlugIn_ViewPort *vp)
{
  if (m_bShowWindbarbOnChart && !wxIsNaN(mTWD) && !wxIsNaN(mTWS)){
    if (mTWD >= 0 && mTWD < 360){
      glColor4ub(0, 0, 255, 192);	// red, green, blue,  alpha (byte values)
      double rad_angle;
      double shaft_x, shaft_y;
      double barb_0_x, barb_0_y, barb_1_x, barb_1_y;
      double barb_2_x, barb_2_y;
      double barb_length_0_x, barb_length_0_y, barb_length_1_x, barb_length_1_y;
      double barb_length_2_x, barb_length_2_y;
      double barb_3_x, barb_3_y, barb_4_x, barb_4_y, barb_length_3_x, barb_length_3_y, barb_length_4_x, barb_length_4_y;
      double tws_kts = fromUsrSpeed_Plugin(mTWS, g_iDashWindSpeedUnit);

      double barb_length[50] = {
        0, 0, 0, 0, 0,    //  0 knots
        0, 0, 0, 0, 5,    //  5 knots
        0, 0, 0, 0, 10,    // 10 knots
        0, 0, 0, 5, 10,    // 15 knots
        0, 0, 0, 10, 10,   // 20 knots
        0, 0, 5, 10, 10,   // 25 knots
        0, 0, 10, 10, 10,  // 30 knots
        0, 5, 10, 10, 10,  // 35 knots
        0, 10, 10, 10, 10,  // 40 knots
        5, 10, 10, 10, 10  // 45 knots
      };

      int p = 0;
      if (tws_kts < 3.)
        p = 0;
      else if (tws_kts >= 3. && tws_kts < 8.)
        p = 1;
      else if (tws_kts >= 8. && tws_kts < 13.)
        p = 2;
      else if (tws_kts >= 13. && tws_kts < 18.)
        p = 3;
      else if (tws_kts >= 18. && tws_kts < 23.)
        p = 4;
      else if (tws_kts >= 23. && tws_kts < 28.)
        p = 5;
      else if (tws_kts >= 28. && tws_kts < 33.)
        p = 6;
      else if (tws_kts >= 33. && tws_kts < 38.)
        p = 7;
      else if (tws_kts >= 38. && tws_kts < 43.)
        p = 8;
      else if (tws_kts >= 43. && tws_kts < 48.)
        p = 9;
      else if (tws_kts >= 48.)
        p = 9;
      //wxLogMessage("mTWS=%.2f --> p=%d", mTWS, p);
      p = 5 * p;

      double rotate = vp->rotation;
      rad_angle = ((mTWD - 90.) * M_PI / 180) + rotate;

      shaft_x = cos(rad_angle) * 90;
      shaft_y = sin(rad_angle) * 90;

      barb_0_x = pp.x + .6 * shaft_x;
      barb_0_y = (pp.y + .6 * shaft_y);
      barb_1_x = pp.x + .7 * shaft_x;
      barb_1_y = (pp.y + .7 * shaft_y);
      barb_2_x = pp.x + .8 * shaft_x;
      barb_2_y = (pp.y + .8 * shaft_y);
      barb_3_x = pp.x + .9 * shaft_x;
      barb_3_y = (pp.y + .9 * shaft_y);
      barb_4_x = pp.x + shaft_x;
      barb_4_y = (pp.y + shaft_y);

      barb_length_0_x = cos(rad_angle + M_PI / 4) * barb_length[p] * 3;
      barb_length_0_y = sin(rad_angle + M_PI / 4) * barb_length[p] * 3;
      barb_length_1_x = cos(rad_angle + M_PI / 4) * barb_length[p + 1] * 3;
      barb_length_1_y = sin(rad_angle + M_PI / 4) * barb_length[p + 1] * 3;
      barb_length_2_x = cos(rad_angle + M_PI / 4) * barb_length[p + 2] * 3;
      barb_length_2_y = sin(rad_angle + M_PI / 4) * barb_length[p + 2] * 3;
      barb_length_3_x = cos(rad_angle + M_PI / 4) * barb_length[p + 3] * 3;
      barb_length_3_y = sin(rad_angle + M_PI / 4) * barb_length[p + 3] * 3;
      barb_length_4_x = cos(rad_angle + M_PI / 4) * barb_length[p + 4] * 3;
      barb_length_4_y = sin(rad_angle + M_PI / 4) * barb_length[p + 4] * 3;

      glLineWidth(2);
      glBegin(GL_LINES);
      glVertex2d(pp.x, pp.y);
      glVertex2d(pp.x + shaft_x, pp.y + shaft_y);
      glVertex2d(barb_0_x, barb_0_y);
      glVertex2d(barb_0_x + barb_length_0_x, barb_0_y + barb_length_0_y);
      glVertex2d(barb_1_x, barb_1_y);
      glVertex2d(barb_1_x + barb_length_1_x, barb_1_y + barb_length_1_y);
      glVertex2d(barb_2_x, barb_2_y);
      glVertex2d(barb_2_x + barb_length_2_x, barb_2_y + barb_length_2_y);
      glVertex2d(barb_3_x, barb_3_y);
      glVertex2d(barb_3_x + barb_length_3_x, barb_3_y + barb_length_3_y);
      glVertex2d(barb_4_x, barb_4_y);
      glVertex2d(barb_4_x + barb_length_4_x, barb_4_y + barb_length_4_y);
      glEnd();
    }
  }
}

/*********************************************************************************
Draw the OpenGL Polar on the ships position overlay
Polar is normalized (always same size)
What should be drawn:
* the actual polar curve for the actual TWS
* 0/360° point (directly upwind)
* the rest of the polar currently in 2° steps
**********************************************************************************/
#define STEPS  180 //72

void tactics_pi::DrawPolar(PlugIn_ViewPort *vp, wxPoint pp, double PolarAngle)
{
	if (m_bShowPolarOnChart && !wxIsNaN(mTWS) && !wxIsNaN(mTWD) && !wxIsNaN(mBRG)){
		glColor4ub(0, 0, 255, 192);	// red, green, blue,  alpha (byte values)
		double polval[STEPS];
		double max = 0;
		double rotate = vp->rotation;
		int i;
		if (mTWS > 0){
			TargetxMG vmg_up = BoatPolar->GetTargetVMGUpwind(mTWS);
			TargetxMG vmg_dn = BoatPolar->GetTargetVMGDownwind(mTWS);
			TargetxMG CmGMax, CmGMin;
			BoatPolar->Calc_TargetCMG2(mTWS, mTWD, mBRG, &CmGMax, &CmGMin);  //CmGMax = the higher value, CmGMin the lower cmg value

			for (i = 0; i < STEPS / 2; i++){ //0...179
				polval[i] = BoatPolar->GetPolarSpeed(i * 2 + 1, mTWS); //polar data is 1...180 !!! i*2 : draw in 2° steps
				polval[STEPS - 1 - i] = polval[i];
				//if (wxIsNaN(polval[i])) polval[i] = polval[STEPS-1 - i] = 0.0;
				if (polval[i]>max) max = polval[i];
			}
			wxPoint currpoints[STEPS];
			double rad, anglevalue;
			for (i = 0; i < STEPS; i++){
				anglevalue = deg2rad(PolarAngle + i * 2) + deg2rad(0. - ANGLE_OFFSET); //i*2 : draw in 2° steps
				rad = 81 * polval[i] / max;
				currpoints[i].x = pp.x + (rad * cos(anglevalue));
				currpoints[i].y = pp.y + (rad * sin(anglevalue));
			}
			glLineWidth(1);
			glBegin(GL_LINES);

			if (wxIsNaN(polval[0])){ //always draw the 0° point (directly upwind)
				currpoints[0].x = pp.x;
				currpoints[0].y = pp.y;
			}
			glVertex2d(currpoints[0].x, currpoints[0].y);

			for (i = 1; i < STEPS; i++){
				if (!wxIsNaN(polval[i])){  //only draw, if we have a real data value (NAN is init status, w/o data)
					glVertex2d(currpoints[i].x, currpoints[i].y);
					glVertex2d(currpoints[i].x, currpoints[i].y);
				}
			}
			glVertex2d(currpoints[0].x, currpoints[0].y); //close the curve

			//dc->DrawPolygon(STEPS, currpoints, 0, 0);
			glEnd();
			//draw Target-VMG Angles now
			if (!wxIsNaN(vmg_up.TargetAngle)){
				rad = 81 * BoatPolar->GetPolarSpeed(vmg_up.TargetAngle, mTWS) / max;
				DrawTargetAngle(vp, pp, PolarAngle + vmg_up.TargetAngle, _T("BLUE3"), 1, rad);
				DrawTargetAngle(vp, pp, PolarAngle - vmg_up.TargetAngle, _T("BLUE3"), 1, rad);
			}
			if (!wxIsNaN(vmg_dn.TargetAngle)){
				rad = 81 * BoatPolar->GetPolarSpeed(vmg_dn.TargetAngle, mTWS) / max;
				DrawTargetAngle(vp, pp, PolarAngle + vmg_dn.TargetAngle, _T("BLUE3"), 1, rad);
				DrawTargetAngle(vp, pp, PolarAngle - vmg_dn.TargetAngle, _T("BLUE3"), 1, rad);
			}
			if (!wxIsNaN(CmGMax.TargetAngle)){
				rad = 81 * BoatPolar->GetPolarSpeed(CmGMax.TargetAngle, mTWS) / max;
				DrawTargetAngle(vp, pp, PolarAngle + CmGMax.TargetAngle, _T("URED"), 2, rad);
			}
			if (!wxIsNaN(CmGMin.TargetAngle)){
				rad = 81 * BoatPolar->GetPolarSpeed(CmGMin.TargetAngle, mTWS) / max;
				DrawTargetAngle(vp, pp, PolarAngle + CmGMin.TargetAngle, _T("URED"), 1, rad);
			}
			//Hdt line
			if (!wxIsNaN(mHdt)){
				wxPoint hdt;
				anglevalue = deg2rad(mHdt) + deg2rad(0. - ANGLE_OFFSET) + rotate;
				rad = 81 * 1.1;
				hdt.x = pp.x + (rad * cos(anglevalue));
				hdt.y = pp.y + (rad * sin(anglevalue));
				glColor4ub(0, 0, 255, 255);	// red, green, blue,  alpha (byte values)
				glLineWidth(3);
				glBegin(GL_LINES);
				glVertex2d(pp.x, pp.y);
				glVertex2d(hdt.x, hdt.y);
				glEnd();
			}
		}
	}
}
/***************************************************************************************
Draw pointers for the optimum target VMG- and CMG Angle (if bearing is available)
****************************************************************************************/
void tactics_pi::DrawTargetAngle(PlugIn_ViewPort *vp, wxPoint pp, double Angle, wxString color, int size, double rad){
	//  if (TargetAngle > 0){
	double rotate = vp->rotation;
	//    double value = deg2rad(PolarAngle + TargetAngle) + deg2rad(0 - ANGLE_OFFSET) + rotate;
	//    double value1 = deg2rad(PolarAngle + 5 + TargetAngle) + deg2rad(0 - ANGLE_OFFSET) + rotate;
	//    double value2 = deg2rad(PolarAngle - 5 + TargetAngle) + deg2rad(0 - ANGLE_OFFSET) + rotate;
	double sizefactor, widthfactor;
	if (size == 1) {
		sizefactor = 1.05;
		widthfactor = 1.05;
	}
	else{
		sizefactor = 1.12;
		widthfactor = 1.4;
	}
	double value = deg2rad(Angle) + deg2rad(0 - ANGLE_OFFSET) + rotate;
	double value1 = deg2rad(Angle + 5 * widthfactor) + deg2rad(0 - ANGLE_OFFSET) + rotate;
	double value2 = deg2rad(Angle - 5 * widthfactor) + deg2rad(0 - ANGLE_OFFSET) + rotate;

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
	points[0].x = pp.x + (rad * 0.95 * cos(value));
	points[0].y = pp.y + (rad * 0.95 * sin(value));
	points[1].x = pp.x + (rad * 1.15 * sizefactor * cos(value1));
	points[1].y = pp.y + (rad * 1.15 * sizefactor * sin(value1));
	points[2].x = pp.x + (rad * 1.15 * sizefactor * cos(value2));
	points[2].y = pp.y + (rad * 1.15 * sizefactor * sin(value2));
	/*    points[1].x = pp.x + (rad * 1.15 * cos(value1));
	points[1].y = pp.y + (rad * 1.15 * sin(value1));
	points[2].x = pp.x + (rad * 1.15 * cos(value2));
	points[2].y = pp.y + (rad * 1.15 * sin(value2));*/
	if (color == _T("BLUE3")) glColor4ub(0, 0, 255, 128);
	else if (color == _T("URED")) glColor4ub(255, 0, 0, 128);
	else glColor4ub(255, 128, 0, 168);

	glLineWidth(1);
	glBegin(GL_TRIANGLES);
	glVertex2d(points[0].x, points[0].y);
	glVertex2d(points[1].x, points[1].y);
	glVertex2d(points[2].x, points[2].y);
	glEnd();

	//}
	//  }
}
/*********************************************************************************
Toggle Layline Render overlay
**********************************************************************************/
void tactics_pi::ToggleLaylineRender(wxWindow* parent)
{
	m_bLaylinesIsVisible = m_bLaylinesIsVisible ? false : true;
}
void tactics_pi::ToggleCurrentRender(wxWindow* parent)
{
	m_bDisplayCurrentOnChart = m_bDisplayCurrentOnChart ? false : true;
}
void tactics_pi::TogglePolarRender(wxWindow* parent)
{
	m_bShowPolarOnChart = m_bShowPolarOnChart ? false : true;
}
void tactics_pi::ToggleWindbarbRender(wxWindow* parent)
{
	m_bShowWindbarbOnChart = m_bShowWindbarbOnChart ? false : true;
}

/*********************************************************************************
**********************************************************************************/
bool tactics_pi::GetLaylineVisibility(wxWindow* parent)
{
	return m_bLaylinesIsVisible;
}
bool tactics_pi::GetCurrentVisibility(wxWindow* parent)
{
	return m_bDisplayCurrentOnChart;
}
bool tactics_pi::GetWindbarbVisibility(wxWindow* parent)
{
	return m_bShowWindbarbOnChart;
}
bool tactics_pi::GetPolarVisibility(wxWindow* parent)
{
	return m_bShowPolarOnChart;
}
bool tactics_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp)
{
	if (b_tactics_dc_message_shown == false) {
		b_tactics_dc_message_shown = true;

		wxString message(_("You have to turn on OpenGL to use chart overlay "));
		wxMessageDialog dlg(GetOCPNCanvasWindow(), message, _T("tactics_pi message"), wxOK);
		dlg.ShowModal();
		//wxMessageBox(_("You have to turn on OpenGL to use chart overlay "));
	}
	return false;
}
/*********************************************************************************
**********************************************************************************/
void tactics_pi::CalculateLaylineDegreeRange(void)
{
	//calculate degree-range for laylines
	//do some exponential smoothing on degree range of COGs and  COG itself
  if (!wxIsNaN(mCOG)){
    if (mCOG != m_COGRange[0]){
      if (wxIsNaN(m_ExpSmoothSinCog)) m_ExpSmoothSinCog = 0;
      if (wxIsNaN(m_ExpSmoothCosCog)) m_ExpSmoothCosCog = 0;


      double mincog = 360, maxcog = 0;
      for (int i = 0; i < COGRANGE; i++){
        if (!wxIsNaN(m_COGRange[i])){
          mincog = wxMin(mincog, m_COGRange[i]);
          maxcog = wxMax(maxcog, m_COGRange[i]);
        }
      }
      m_LaylineDegRange = getDegRange(maxcog, mincog);
      for (int i = 0; i < COGRANGE - 1; i++) m_COGRange[i + 1] = m_COGRange[i];
      m_COGRange[0] = mCOG;
      if (m_LaylineDegRange < g_iMinLaylineWidth){
        m_LaylineDegRange = g_iMinLaylineWidth;
      }
      else if (m_LaylineDegRange > g_iMaxLaylineWidth){
        m_LaylineDegRange = g_iMaxLaylineWidth;
      }

      //shifting
      double rad = (90 - mCOG)*M_PI / 180.;
      mExpSmSinCog->SetAlpha(m_alphaLaylineCog);
      mExpSmCosCog->SetAlpha(m_alphaLaylineCog);
      m_ExpSmoothSinCog = mExpSmSinCog->GetSmoothVal(sin(rad));
      m_ExpSmoothCosCog = mExpSmCosCog->GetSmoothVal(cos(rad));

      m_LaylineSmoothedCog = (int)(90. - (atan2(m_ExpSmoothSinCog, m_ExpSmoothCosCog)*180. / M_PI) + 360.) % 360;


      mExpSmDegRange->SetAlpha(g_dalphaDeltCoG);
      m_ExpSmoothDegRange = mExpSmDegRange->GetSmoothVal(m_LaylineDegRange);
    }
  }
}

void tactics_pi::SendUtcTimeToAllInstruments(wxDateTime value)
{
	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindow *tactics_window = m_ArrayOfTacticsWindow.Item(i)->m_pTacticsWindow;
		if (tactics_window) tactics_window->SendUtcTimeToAllInstruments(value);
	}
}

void tactics_pi::SendSatInfoToAllInstruments(int cnt, int seq, SAT_INFO sats[4])
{
	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindow *tactics_window = m_ArrayOfTacticsWindow.Item(i)->m_pTacticsWindow;
		if (tactics_window) tactics_window->SendSatInfoToAllInstruments(cnt, seq, sats);
	}
}

void tactics_pi::SetNMEASentence(wxString &sentence)
{
	m_NMEA0183 << sentence;
	if (m_NMEA0183.PreParse()) {
		if (m_NMEA0183.LastSentenceIDReceived == _T("DBT")) {
			if (m_NMEA0183.Parse()) {
				if (mPriDepth >= 2) {
					mPriDepth = 2;

					/*
					double m_NMEA0183.Dbt.DepthFeet;
					double m_NMEA0183.Dbt.DepthMeters;
					double m_NMEA0183.Dbt.DepthFathoms;
					*/
					double depth = 999.;
					if (m_NMEA0183.Dbt.DepthMeters != 999.) depth = m_NMEA0183.Dbt.DepthMeters;
					else if (m_NMEA0183.Dbt.DepthFeet != 999.) depth = m_NMEA0183.Dbt.DepthFeet
						* 0.3048;
					else if (m_NMEA0183.Dbt.DepthFathoms != 999.) depth =
						m_NMEA0183.Dbt.DepthFathoms * 1.82880;
					SendSentenceToAllInstruments(OCPN_DBP_STC_DPT, toUsrDistance_Plugin(depth / 1852.0, g_iDashDepthUnit), getUsrDistanceUnit_Plugin(g_iDashDepthUnit));
				}
			}
		}

		else if (m_NMEA0183.LastSentenceIDReceived == _T("DPT")) {
			if (m_NMEA0183.Parse()) {
				if (mPriDepth >= 1) {
					mPriDepth = 1;

					/*
					double m_NMEA0183.Dpt.DepthMeters
					double m_NMEA0183.Dpt.OffsetFromTransducerMeters
					*/
					double depth = 999.;
					if (m_NMEA0183.Dpt.DepthMeters != 999.) depth = m_NMEA0183.Dpt.DepthMeters;
					if (m_NMEA0183.Dpt.OffsetFromTransducerMeters != 999.) depth += m_NMEA0183.Dpt.OffsetFromTransducerMeters;
					SendSentenceToAllInstruments(OCPN_DBP_STC_DPT, toUsrDistance_Plugin(depth / 1852.0, g_iDashDepthUnit), getUsrDistanceUnit_Plugin(g_iDashDepthUnit));
				}
			}
		}
		// TODO: GBS - GPS Satellite fault detection
		else if (m_NMEA0183.LastSentenceIDReceived == _T("GGA")) {
			if (m_NMEA0183.Parse()) {
				if (m_NMEA0183.Gga.GPSQuality > 0) {
					if (mPriPosition >= 3) {
						mPriPosition = 3;
						double lat, lon;
						float llt = m_NMEA0183.Gga.Position.Latitude.Latitude;
						int lat_deg_int = (int)(llt / 100);
						float lat_deg = lat_deg_int;
						float lat_min = llt - (lat_deg * 100);
						lat = lat_deg + (lat_min / 60.);
						if (m_NMEA0183.Gga.Position.Latitude.Northing == South) lat = -lat;
						SendSentenceToAllInstruments(OCPN_DBP_STC_LAT, lat, _T("SDMM"));


						float lln = m_NMEA0183.Gga.Position.Longitude.Longitude;
						int lon_deg_int = (int)(lln / 100);
						float lon_deg = lon_deg_int;
						float lon_min = lln - (lon_deg * 100);
						lon = lon_deg + (lon_min / 60.);
						if (m_NMEA0183.Gga.Position.Longitude.Easting == West) lon = -lon;
						SendSentenceToAllInstruments(OCPN_DBP_STC_LON, lon, _T("SDMM"));

					}

					if (mPriDateTime >= 4) {
						// Not in use, we need the date too.
						//mPriDateTime = 4;
						//mUTCDateTime.ParseFormat( m_NMEA0183.Gga.UTCTime.c_str(), _T("%H%M%S") );
					}

					mSatsInView = m_NMEA0183.Gga.NumberOfSatellitesInUse;
				}
			}
		}

		else if (m_NMEA0183.LastSentenceIDReceived == _T("GLL")) {
			if (m_NMEA0183.Parse()) {
				if (m_NMEA0183.Gll.IsDataValid == NTrue) {
					if (mPriPosition >= 2) {
						mPriPosition = 2;
						double lat, lon;
						float llt = m_NMEA0183.Gll.Position.Latitude.Latitude;
						int lat_deg_int = (int)(llt / 100);
						float lat_deg = lat_deg_int;
						float lat_min = llt - (lat_deg * 100);
						lat = lat_deg + (lat_min / 60.);
						if (m_NMEA0183.Gll.Position.Latitude.Northing == South) lat = -lat;
						//mlat = lat;
						SendSentenceToAllInstruments(OCPN_DBP_STC_LAT, lat, _T("SDMM"));

						float lln = m_NMEA0183.Gll.Position.Longitude.Longitude;
						int lon_deg_int = (int)(lln / 100);
						float lon_deg = lon_deg_int;
						float lon_min = lln - (lon_deg * 100);
						lon = lon_deg + (lon_min / 60.);
						if (m_NMEA0183.Gll.Position.Longitude.Easting == West) lon = -lon;
						SendSentenceToAllInstruments(OCPN_DBP_STC_LON, lon, _T("SDMM"));
					}

					if (mPriDateTime >= 5) {
						// Not in use, we need the date too.
						//mPriDateTime = 5;
						//mUTCDateTime.ParseFormat( m_NMEA0183.Gll.UTCTime.c_str(), _T("%H%M%S") );
					}
				}
			}
		}

		else if (m_NMEA0183.LastSentenceIDReceived == _T("GSV")) {
			if (m_NMEA0183.Parse()) {
				mSatsInView = m_NMEA0183.Gsv.SatsInView;
				// m_NMEA0183.Gsv.NumberOfMessages;
				//SendSentenceToAllInstruments( OCPN_DBP_STC_SAT, m_NMEA0183.Gsv.SatsInView, _T("") );
				SendSatInfoToAllInstruments(m_NMEA0183.Gsv.SatsInView,
					m_NMEA0183.Gsv.MessageNumber, m_NMEA0183.Gsv.SatInfo);

				mGPS_Watchdog = gps_watchdog_timeout_ticks;
			}
		}

		else if (m_NMEA0183.LastSentenceIDReceived == _T("HDG")) {
			if (m_NMEA0183.Parse()) {
				if (mPriVar >= 2) {
					if (!wxIsNaN(m_NMEA0183.Hdg.MagneticVariationDegrees)){
						mPriVar = 2;
						if (m_NMEA0183.Hdg.MagneticVariationDirection == East)
							mVar = m_NMEA0183.Hdg.MagneticVariationDegrees;
						else if (m_NMEA0183.Hdg.MagneticVariationDirection == West)
							mVar = -m_NMEA0183.Hdg.MagneticVariationDegrees;
                        if (!wxIsNaN(mVar) )
						  SendSentenceToAllInstruments(OCPN_DBP_STC_HMV, mVar, _T("\u00B0"));
					}

				}
				if (mPriHeadingM >= 1) {
					mPriHeadingM = 1;
					mHdm = m_NMEA0183.Hdg.MagneticSensorHeadingDegrees;
					SendSentenceToAllInstruments(OCPN_DBP_STC_HDM, mHdm, _T("\u00B0"));
				}
				if (!wxIsNaN(m_NMEA0183.Hdg.MagneticSensorHeadingDegrees))
					mHDx_Watchdog = gps_watchdog_timeout_ticks;

				//      If Variation is available, no higher priority HDT is available,
				//      then calculate and propagate calculated HDT
				if (!wxIsNaN(m_NMEA0183.Hdg.MagneticSensorHeadingDegrees)) {
					if (!wxIsNaN(mVar) && (mPriHeadingT > 3)){
						mPriHeadingT = 4;
						calmHdt = mHdm + mVar;
						if (calmHdt < 0.0) {
							calmHdt = calmHdt + 360.0;
						}
						else if (calmHdt >= 360.0) {
							calmHdt = calmHdt - 360.0;
						}
						SendSentenceToAllInstruments(OCPN_DBP_STC_HDT, calmHdt, _T("\u00B0"));
						mHDT_Watchdog = gps_watchdog_timeout_ticks;
					}
				}
			}
		}

		else if (m_NMEA0183.LastSentenceIDReceived == _T("HDM")) {
			if (m_NMEA0183.Parse()) {
				if (mPriHeadingM >= 2) {
					mPriHeadingM = 2;
					mHdm = m_NMEA0183.Hdm.DegreesMagnetic;
					SendSentenceToAllInstruments(OCPN_DBP_STC_HDM, mHdm, _T("\u00B0M"));
				}
				if (!wxIsNaN(m_NMEA0183.Hdm.DegreesMagnetic))
					mHDx_Watchdog = gps_watchdog_timeout_ticks;

				//      If Variation is available, no higher priority HDT is available,
				//      then calculate and propagate calculated HDT
				if (!wxIsNaN(m_NMEA0183.Hdm.DegreesMagnetic)) {
					if (!wxIsNaN(mVar) && (mPriHeadingT > 2)){
						mPriHeadingT = 3;
						calmHdt = mHdm + mVar;
						if (calmHdt < 0.0) {
							calmHdt = calmHdt + 360.0;
						}
						else if (calmHdt >= 360.0) {
							calmHdt = calmHdt - 360.0;
						}
						SendSentenceToAllInstruments(OCPN_DBP_STC_HDT, calmHdt, _T("\u00B0"));
						mHDT_Watchdog = gps_watchdog_timeout_ticks;
					}
				}

			}
		}

		else if (m_NMEA0183.LastSentenceIDReceived == _T("HDT")) {
			if (m_NMEA0183.Parse()) {
				if (mPriHeadingT >= 1) {
					mPriHeadingT = 1;
					if (m_NMEA0183.Hdt.DegreesTrue < 999.) {
						SendSentenceToAllInstruments(OCPN_DBP_STC_HDT, m_NMEA0183.Hdt.DegreesTrue,
							_T("\u00B0T"));
					}
				}
				if (!wxIsNaN(m_NMEA0183.Hdt.DegreesTrue))
					mHDT_Watchdog = gps_watchdog_timeout_ticks;

			}
		}
		else if (m_NMEA0183.LastSentenceIDReceived == _T("MTA")) {  //Air temperature
			if (m_NMEA0183.Parse()) {
				/*
				double   m_NMEA0183.Mta.Temperature;
				wxString m_NMEA0183.Mta.UnitOfMeasurement;
				*/
				SendSentenceToAllInstruments(OCPN_DBP_STC_ATMP, m_NMEA0183.Mta.Temperature,
					m_NMEA0183.Mta.UnitOfMeasurement);
			}
		}
		else if (m_NMEA0183.LastSentenceIDReceived == _T("MDA")) {  //Barometric pressure
			if (m_NMEA0183.Parse()) {
				// TODO make posibilyti to select between Bar or InchHg
				/*

				double   m_NMEA0183.Mda.Pressure;

				wxString m_NMEA0183.Mda.UnitOfMeasurement;

				*/

				if (m_NMEA0183.Mda.Pressure > .8 && m_NMEA0183.Mda.Pressure < 1.1) {
					SendSentenceToAllInstruments(OCPN_DBP_STC_MDA, m_NMEA0183.Mda.Pressure * 1000,
						_T("hPa")); //Convert to hpa befor sending to instruments.
				}

			}

		}
		else if (m_NMEA0183.LastSentenceIDReceived == _T("MTW")) {
			if (m_NMEA0183.Parse()) {
				/*
				double   m_NMEA0183.Mtw.Temperature;
				wxString m_NMEA0183.Mtw.UnitOfMeasurement;
				*/
				SendSentenceToAllInstruments(OCPN_DBP_STC_TMP, m_NMEA0183.Mtw.Temperature,
					m_NMEA0183.Mtw.UnitOfMeasurement);
			}

		}
		else if (m_NMEA0183.LastSentenceIDReceived == _T("VLW")) {
			if (m_NMEA0183.Parse()) {
				/*
				double   m_NMEA0183.Vlw.TotalMileage;
				double   m_NMEA0183.Vlw.TripMileage;
				*/
				/* SendSentenceToAllInstruments( OCPN_DBP_STC_VLW1, toUsrDistance_Plugin( m_NMEA0183.Vlw.TripMileage, g_iDashDistanceUnit ),
				getUsrDistanceUnit_Plugin( g_iDashDistanceUnit ) );

				SendSentenceToAllInstruments( OCPN_DBP_STC_VLW2, toUsrDistance_Plugin( m_NMEA0183.Vlw.TotalMileage, g_iDashDistanceUnit ),
				getUsrDistanceUnit_Plugin( g_iDashDistanceUnit ) );*/
			}

		}
		// NMEA 0183 standard Wind Direction and Speed, with respect to north.
		else if (m_NMEA0183.LastSentenceIDReceived == _T("MWD")) {
			if (m_NMEA0183.Parse()) {
				// Option for True vs Magnetic
				wxString windunit;
				if (m_NMEA0183.Mwd.WindAngleTrue < 999.) { //if WindAngleTrue is available, use it ...
					SendSentenceToAllInstruments(OCPN_DBP_STC_TWD, m_NMEA0183.Mwd.WindAngleTrue,
						_T("\u00B0T"));
					mTWD_Watchdog = gps_watchdog_timeout_ticks;
				}
				else if (m_NMEA0183.Mwd.WindAngleMagnetic < 999.) { //otherwise try WindAngleMagnetic ...
					SendSentenceToAllInstruments(OCPN_DBP_STC_TWD, m_NMEA0183.Mwd.WindAngleMagnetic,
						_T("\u00B0M"));
					mTWD_Watchdog = gps_watchdog_timeout_ticks;
				}

				SendSentenceToAllInstruments(OCPN_DBP_STC_TWS, toUsrSpeed_Plugin(m_NMEA0183.Mwd.WindSpeedKnots, g_iDashWindSpeedUnit),
					getUsrSpeedUnit_Plugin(g_iDashWindSpeedUnit));
				mTWS_Watchdog = gps_watchdog_timeout_ticks;

				/*SendSentenceToAllInstruments( OCPN_DBP_STC_TWS2, toUsrSpeed_Plugin( m_NMEA0183.Mwd.WindSpeedKnots, g_iDashWindSpeedUnit ),
				getUsrSpeedUnit_Plugin( g_iDashWindSpeedUnit ) );*/
				//m_NMEA0183.Mwd.WindSpeedms
			}
		}
		// NMEA 0183 standard Wind Speed and Angle, in relation to the vessel's bow/centerline.
		else if (m_NMEA0183.LastSentenceIDReceived == _T("MWV")) {
			if (m_NMEA0183.Parse()) {
				if (m_NMEA0183.Mwv.IsDataValid == NTrue) {
					//MWV windspeed has different units. Form it to knots to fit "toUsrSpeed_Plugin()"
					double m_wSpeedFactor = 1.0; //knots ("N")
					if (m_NMEA0183.Mwv.WindSpeedUnits == _T("K")) m_wSpeedFactor = 0.53995; //km/h > knots
					if (m_NMEA0183.Mwv.WindSpeedUnits == _T("M")) m_wSpeedFactor = 1.94384; //m/s > knots

					if (m_NMEA0183.Mwv.Reference == _T("R")) // Relative (apparent wind)
					{
						if (mPriAWA >= 1) {
							mPriAWA = 1;
							wxString m_awaunit;
							double m_awaangle;
							if (m_NMEA0183.Mwv.WindAngle >180) {
								m_awaunit = _T("\u00B0L");
								m_awaangle = 180.0 - (m_NMEA0183.Mwv.WindAngle - 180.0);
							}
							else {
								m_awaunit = _T("\u00B0R");
								m_awaangle = m_NMEA0183.Mwv.WindAngle;
							}
							SendSentenceToAllInstruments(OCPN_DBP_STC_AWA,
								m_awaangle, m_awaunit);
							SendSentenceToAllInstruments(OCPN_DBP_STC_AWS,
								toUsrSpeed_Plugin(m_NMEA0183.Mwv.WindSpeed * m_wSpeedFactor, g_iDashWindSpeedUnit),
								getUsrSpeedUnit_Plugin(g_iDashWindSpeedUnit));
							mAWS_Watchdog = gps_watchdog_timeout_ticks;

						}
					}
					else if (m_NMEA0183.Mwv.Reference == _T("T")) // Theoretical (aka True)
					{
						if (mPriTWA >= 1) {
							mPriTWA = 1;
							wxString m_twaunit;
							double m_twaangle;
							if (m_NMEA0183.Mwv.WindAngle >180) {
								m_twaunit = _T("\u00B0L");
								m_twaangle = 180.0 - (m_NMEA0183.Mwv.WindAngle - 180.0);
							}
							else {
								m_twaunit = _T("\u00B0R");
								m_twaangle = m_NMEA0183.Mwv.WindAngle;
							}
							SendSentenceToAllInstruments(OCPN_DBP_STC_TWA,
								m_twaangle, m_twaunit);
							SendSentenceToAllInstruments(OCPN_DBP_STC_TWS,
								toUsrSpeed_Plugin(m_NMEA0183.Mwv.WindSpeed * m_wSpeedFactor, g_iDashWindSpeedUnit),
								getUsrSpeedUnit_Plugin(g_iDashWindSpeedUnit));
							mTWS_Watchdog = gps_watchdog_timeout_ticks;
						}
					}
				}
			}
		}
		else if (m_NMEA0183.LastSentenceIDReceived == _T("RMB")) { //TR, for compass ...
			if (m_NMEA0183.Parse()) {
				if (m_NMEA0183.Rmb.IsDataValid == NTrue) {
					//					// it's always degrees, so send the WP Name as "unit"
					SendSentenceToAllInstruments(OCPN_DBP_STC_BRG, m_NMEA0183.Rmb.BearingToDestinationDegreesTrue, m_NMEA0183.Rmb.To);
					SendSentenceToAllInstruments(OCPN_DBP_STC_DTW, m_NMEA0183.Rmb.RangeToDestinationNauticalMiles, _T("Nm"));
					//					SendSentenceToAllInstruments(OCPN_DBP_STC_DCV, m_NMEA0183.Rmb.DestinationClosingVelocityKnots, _T("Kn"));
					/*double lat, lon;
					float llt = m_NMEA0183.Rmb.DestinationPosition.Latitude.Latitude;
					int lat_deg_int = (int)(llt / 100);
					float lat_deg = lat_deg_int;
					float lat_min = llt - (lat_deg * 100);
					lat = lat_deg + (lat_min / 60.);
					if (m_NMEA0183.Rmb.DestinationPosition.Latitude.Northing == South) lat = -lat;
					//mlat = lat;
					SendSentenceToAllInstruments(OCPN_DBP_STC_RMBLAT, lat, _T("SDMM"));


					float lln = m_NMEA0183.Rmb.DestinationPosition.Longitude.Longitude;
					int lon_deg_int = (int)(lln / 100);
					float lon_deg = lon_deg_int;
					float lon_min = lln - (lon_deg * 100);
					lon = lon_deg + (lon_min / 60.);
					if (m_NMEA0183.Rmb.DestinationPosition.Longitude.Easting == West) lon = -lon;
					//mlong = lon;
					SendSentenceToAllInstruments(OCPN_DBP_STC_RMBLON, lon, _T("SDMM"));
					*/
				}
				else
					SendSentenceToAllInstruments(OCPN_DBP_STC_BRG, m_NMEA0183.Rmb.BearingToDestinationDegreesTrue, m_NMEA0183.ErrorMessage);
				if (!wxIsNaN(m_NMEA0183.Rmb.BearingToDestinationDegreesTrue))
					mBRG_Watchdog = gps_watchdog_timeout_ticks;

			}

		}

		else if (m_NMEA0183.LastSentenceIDReceived == _T("RMC")) {
			if (m_NMEA0183.Parse()) {
				if (m_NMEA0183.Rmc.IsDataValid == NTrue) {
					if (mPriPosition >= 4) {
						mPriPosition = 4;
						double lat, lon;
						float llt = m_NMEA0183.Rmc.Position.Latitude.Latitude;
						int lat_deg_int = (int)(llt / 100);
						float lat_deg = lat_deg_int;
						float lat_min = llt - (lat_deg * 100);
						lat = lat_deg + (lat_min / 60.);
						if (m_NMEA0183.Rmc.Position.Latitude.Northing == South) lat = -lat;
						//mlat = lat;
						SendSentenceToAllInstruments(OCPN_DBP_STC_LAT, lat, _T("SDMM"));

						float lln = m_NMEA0183.Rmc.Position.Longitude.Longitude;
						int lon_deg_int = (int)(lln / 100);
						float lon_deg = lon_deg_int;
						float lon_min = lln - (lon_deg * 100);
						lon = lon_deg + (lon_min / 60.);
						if (m_NMEA0183.Rmc.Position.Longitude.Easting == West) lon = -lon;
						//mlong = lon;
						SendSentenceToAllInstruments(OCPN_DBP_STC_LON, lon, _T("SDMM"));
					}

					if (mPriCOGSOG >= 3) {
						mPriCOGSOG = 3;
						if (m_NMEA0183.Rmc.SpeedOverGroundKnots < 999.) {
							//SendSentenceToAllInstruments( OCPN_DBP_STC_SOG,
							//        toUsrSpeed_Plugin( m_NMEA0183.Rmc.SpeedOverGroundKnots, g_iDashSpeedUnit ), getUsrSpeedUnit_Plugin( g_iDashSpeedUnit ) );
							SendSentenceToAllInstruments(OCPN_DBP_STC_SOG,
								toUsrSpeed_Plugin(mSOGFilter.filter(m_NMEA0183.Rmc.SpeedOverGroundKnots), g_iDashSpeedUnit), getUsrSpeedUnit_Plugin(g_iDashSpeedUnit));
						}
						else {
							//->SetData(_T("---"));
						}
						if (m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue < 999.) {
							//mCOG = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue;
							//                            SendSentenceToAllInstruments( OCPN_DBP_STC_COG,
							//                                    m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue, _T("\u00B0") );
							SendSentenceToAllInstruments(OCPN_DBP_STC_COG,
								mCOGFilter.filter(m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue), _T("\u00B0"));
						}
						else {
							//->SetData(_T("---"));
						}
						if (m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue < 999. && m_NMEA0183.Rmc.MagneticVariation < 999.) {
							double dMagneticCOG;
							if (m_NMEA0183.Rmc.MagneticVariationDirection == East) {
								//                                dMagneticCOG = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue - m_NMEA0183.Rmc.MagneticVariation;
								dMagneticCOG = mCOGFilter.get() - m_NMEA0183.Rmc.MagneticVariation;
								if (dMagneticCOG < 0.0) dMagneticCOG = 360.0 + dMagneticCOG;
							}
							else {
								//                                dMagneticCOG = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue + m_NMEA0183.Rmc.MagneticVariation;
								dMagneticCOG = mCOGFilter.get() + m_NMEA0183.Rmc.MagneticVariation;
								if (dMagneticCOG >= 360.0) dMagneticCOG = dMagneticCOG - 360.0;
							}
							//mCOG = dMagneticCOG;
							SendSentenceToAllInstruments(OCPN_DBP_STC_MCOG,
								dMagneticCOG, _T("\u00B0M"));
						}
						else {
							//->SetData(_T("---"));
						}
					}

					if (mPriVar >= 3) {
						if (!wxIsNaN(m_NMEA0183.Rmc.MagneticVariation)){
							mPriVar = 3;
							if (m_NMEA0183.Rmc.MagneticVariationDirection == East)
								mVar = m_NMEA0183.Rmc.MagneticVariation;
							else if (m_NMEA0183.Rmc.MagneticVariationDirection == West)
								mVar = -m_NMEA0183.Rmc.MagneticVariation;
							mVar_Watchdog = gps_watchdog_timeout_ticks;
                            if (!wxIsNaN(mVar) )
							  SendSentenceToAllInstruments(OCPN_DBP_STC_HMV, mVar, _T("\u00B0"));
						}
					}

					if (mPriDateTime >= 3) {
						mPriDateTime = 3;
						wxString dt = m_NMEA0183.Rmc.Date + m_NMEA0183.Rmc.UTCTime;
						mUTCDateTime.ParseFormat(dt.c_str(), _T("%d%m%y%H%M%S"));
					}
				}
			}
		}

		else if (m_NMEA0183.LastSentenceIDReceived == _T("RSA")) {
			if (m_NMEA0183.Parse()) {
				/*if( m_NMEA0183.Rsa.IsStarboardDataValid == NTrue ) {
				SendSentenceToAllInstruments(OCPN_DBP_STC_RSA, m_NMEA0183.Rsa.Starboard,
				_T("\u00B0") );
				} else if( m_NMEA0183.Rsa.IsPortDataValid == NTrue ) {
				SendSentenceToAllInstruments( OCPN_DBP_STC_RSA, -m_NMEA0183.Rsa.Port,
				_T("\u00B0") );
				}*/
			}
		}

		else if (m_NMEA0183.LastSentenceIDReceived == _T("VHW")) {
			if (m_NMEA0183.Parse()) {
				if (mPriHeadingT >= 2) {
					if (m_NMEA0183.Vhw.DegreesTrue < 999.) {
						mPriHeadingT = 2;
						//mHdt = m_NMEA0183.Vhw.DegreesTrue;
						SendSentenceToAllInstruments(OCPN_DBP_STC_HDT, m_NMEA0183.Vhw.DegreesTrue,
							_T("\u00B0T"));
					}
				}
				if (mPriHeadingM >= 3) {
					mPriHeadingM = 3;
					SendSentenceToAllInstruments(OCPN_DBP_STC_HDM, m_NMEA0183.Vhw.DegreesMagnetic,
						_T("\u00B0M"));
				}
				if (m_NMEA0183.Vhw.Knots < 999.) {
					SendSentenceToAllInstruments(OCPN_DBP_STC_STW, toUsrSpeed_Plugin(m_NMEA0183.Vhw.Knots, g_iDashSpeedUnit),
						getUsrSpeedUnit_Plugin(g_iDashSpeedUnit));
				}

				if (!wxIsNaN(m_NMEA0183.Vhw.DegreesMagnetic))
					mHDx_Watchdog = gps_watchdog_timeout_ticks;
				if (!wxIsNaN(m_NMEA0183.Vhw.DegreesTrue))
					mHDT_Watchdog = gps_watchdog_timeout_ticks;

			}
		}

		else if (m_NMEA0183.LastSentenceIDReceived == _T("VTG")) {
			if (m_NMEA0183.Parse()) {
				if (mPriCOGSOG >= 2) {
					mPriCOGSOG = 2;
					//    Special check for unintialized values, as opposed to zero values
					if (m_NMEA0183.Vtg.SpeedKnots < 999.) {
						//SendSentenceToAllInstruments( OCPN_DBP_STC_SOG, toUsrSpeed_Plugin( m_NMEA0183.Vtg.SpeedKnots, g_iDashSpeedUnit ),
						//        getUsrSpeedUnit_Plugin( g_iDashSpeedUnit ) );
						SendSentenceToAllInstruments(OCPN_DBP_STC_SOG, toUsrSpeed_Plugin(mSOGFilter.filter(m_NMEA0183.Vtg.SpeedKnots), g_iDashSpeedUnit),
							getUsrSpeedUnit_Plugin(g_iDashSpeedUnit));

					}
					else {
						//->SetData(_T("---"));
					}
					// Vtg.SpeedKilometersPerHour;
					if (m_NMEA0183.Vtg.TrackDegreesTrue < 999.) {
						//                        SendSentenceToAllInstruments( OCPN_DBP_STC_COG,
						//                                m_NMEA0183.Vtg.TrackDegreesTrue, _T("\u00B0") );
						SendSentenceToAllInstruments(OCPN_DBP_STC_COG,
							mCOGFilter.filter(m_NMEA0183.Vtg.TrackDegreesTrue), _T("\u00B0"));
					}
					else {
						//->SetData(_T("---"));
					}
				}

				/*
				m_NMEA0183.Vtg.TrackDegreesMagnetic;
				*/
			}
		}
		/* NMEA 0183 Relative (Apparent) Wind Speed and Angle. Wind angle in relation
		* to the vessel's heading, and wind speed measured relative to the moving vessel. */
		else if (m_NMEA0183.LastSentenceIDReceived == _T("VWR")) {
			if (m_NMEA0183.Parse()) {
				if (mPriAWA >= 2) {
					mPriAWA = 2;

					wxString awaunit;
					awaunit = m_NMEA0183.Vwr.DirectionOfWind == Left ? _T("\u00B0L") : _T("\u00B0R");
					SendSentenceToAllInstruments(OCPN_DBP_STC_AWA,
						m_NMEA0183.Vwr.WindDirectionMagnitude, awaunit);
					SendSentenceToAllInstruments(OCPN_DBP_STC_AWS, toUsrSpeed_Plugin(m_NMEA0183.Vwr.WindSpeedKnots, g_iDashWindSpeedUnit),
						getUsrSpeedUnit_Plugin(g_iDashWindSpeedUnit));
					mAWS_Watchdog = gps_watchdog_timeout_ticks;

					/*
					double m_NMEA0183.Vwr.WindSpeedms;
					double m_NMEA0183.Vwr.WindSpeedKmh;
					*/
				}
			}
		}
		/* NMEA 0183 True wind angle in relation to the vessel's heading, and true wind
		* speed referenced to the water. True wind is the vector sum of the Relative
		* (apparent) wind vector and the vessel's velocity vector relative to the water along
		* the heading line of the vessel. It represents the wind at the vessel if it were
		* stationary relative to the water and heading in the same direction. */
		else if (m_NMEA0183.LastSentenceIDReceived == _T("VWT")) {
			if (m_NMEA0183.Parse()) {
				if (mPriTWA >= 2) {
					mPriTWA = 2;
					wxString vwtunit;
					vwtunit = m_NMEA0183.Vwt.DirectionOfWind == Left ? _T("\u00B0L") : _T("\u00B0R");
					SendSentenceToAllInstruments(OCPN_DBP_STC_TWA,
						m_NMEA0183.Vwt.WindDirectionMagnitude, vwtunit);
					SendSentenceToAllInstruments(OCPN_DBP_STC_TWS, toUsrSpeed_Plugin(m_NMEA0183.Vwt.WindSpeedKnots, g_iDashWindSpeedUnit),
						getUsrSpeedUnit_Plugin(g_iDashWindSpeedUnit));
					mTWS_Watchdog = gps_watchdog_timeout_ticks;
					/*
					double           m_NMEA0183.Vwt.WindSpeedms;
					double           m_NMEA0183.Vwt.WindSpeedKmh;
					*/
				}
			}
		}
		else if (m_NMEA0183.LastSentenceIDReceived == _T("XDR")) {  //Transducer measurement
			if (m_NMEA0183.Parse())
			{
				wxString xdrunit;
				double xdrdata;
				for (int i = 0; i<m_NMEA0183.Xdr.TransducerCnt; i++){

					xdrdata = m_NMEA0183.Xdr.TransducerInfo[i].MeasurementData;
					// NKE style of XDR Airtemp
					if (m_NMEA0183.Xdr.TransducerInfo[i].TransducerName == _T("AirTemp")){
						SendSentenceToAllInstruments(OCPN_DBP_STC_ATMP, m_NMEA0183.Xdr.TransducerInfo[i].MeasurementData, m_NMEA0183.Xdr.TransducerInfo[i].UnitOfMeasurement);
					} //Nasa style air temp
					// NKE style of XDR Barometer
					if (m_NMEA0183.Xdr.TransducerInfo[i].TransducerName == _T("Barometer")){

						double data;
						if (m_NMEA0183.Xdr.TransducerInfo[i].UnitOfMeasurement == _T("B"))
							data = m_NMEA0183.Xdr.TransducerInfo[i].MeasurementData * 1000.;
						else
							data = m_NMEA0183.Xdr.TransducerInfo[i].MeasurementData;

						SendSentenceToAllInstruments(OCPN_DBP_STC_MDA, data, _T("hPa"));
					} //Nasa style air temp
					if (m_NMEA0183.Xdr.TransducerInfo[i].TransducerName == _T("ENV_OUTAIR_T") || m_NMEA0183.Xdr.TransducerInfo[i].TransducerName == _T("ENV_OUTSIDE_T")){
						SendSentenceToAllInstruments(OCPN_DBP_STC_ATMP, m_NMEA0183.Xdr.TransducerInfo[i].MeasurementData, m_NMEA0183.Xdr.TransducerInfo[i].UnitOfMeasurement);
					}
					// NKE style of XDR Pitch (=Bow up/down)
					if (m_NMEA0183.Xdr.TransducerInfo[i].TransducerName == _T("PTCH")) {
						if (m_NMEA0183.Xdr.TransducerInfo[i].MeasurementData > 0){
							xdrunit = _("\u00B0 Bow up");
						}
						else if (m_NMEA0183.Xdr.TransducerInfo[i].MeasurementData < 0) {
							xdrunit = _("\u00B0 Bow down");
							xdrdata *= -1;
						}
						else {
							xdrunit = _T("\u00B0");
						}
						SendSentenceToAllInstruments(OCPN_DBP_STC_PITCH, xdrdata, xdrunit);
					}
					// NKE style of XDR Heel
					if ((m_NMEA0183.Xdr.TransducerInfo[i].TransducerName == _T("ROLL")) ||
						(m_NMEA0183.Xdr.TransducerInfo[i].TransducerName == _T("Heel Angle"))){
						if (m_NMEA0183.Xdr.TransducerInfo[i].MeasurementData > 0)
							xdrunit = _T("\u00B0r");
						else if (m_NMEA0183.Xdr.TransducerInfo[i].MeasurementData < 0) {
							xdrunit = _T("\u00B0l");
						}
						else
							xdrunit = _T("\u00B0");
						SendSentenceToAllInstruments(OCPN_DBP_STC_HEEL, xdrdata, xdrunit);

					} //Nasa style water temp
					if (m_NMEA0183.Xdr.TransducerInfo[i].TransducerName == _T("ENV_WATER_T")){
						SendSentenceToAllInstruments(OCPN_DBP_STC_TMP, m_NMEA0183.Xdr.TransducerInfo[i].MeasurementData, m_NMEA0183.Xdr.TransducerInfo[i].UnitOfMeasurement);
					}
				}

			}
		}
		else if (m_NMEA0183.LastSentenceIDReceived == _T("ZDA")) {
			if (m_NMEA0183.Parse()) {
				if (mPriDateTime >= 2) {
					mPriDateTime = 2;

					/*
					wxString m_NMEA0183.Zda.UTCTime;
					int      m_NMEA0183.Zda.Day;
					int      m_NMEA0183.Zda.Month;
					int      m_NMEA0183.Zda.Year;
					int      m_NMEA0183.Zda.LocalHourDeviation;
					int      m_NMEA0183.Zda.LocalMinutesDeviation;
					*/
					wxString dt;
					dt.Printf(_T("%4d%02d%02d"), m_NMEA0183.Zda.Year, m_NMEA0183.Zda.Month,
						m_NMEA0183.Zda.Day);
					dt.Append(m_NMEA0183.Zda.UTCTime);
					mUTCDateTime.ParseFormat(dt.c_str(), _T("%Y%m%d%H%M%S"));
				}
			}
		}
	}
	//      Process an AIVDO message
	else if (sentence.Mid(1, 5).IsSameAs(_T("AIVDO"))) {
		PlugIn_Position_Fix_Ex gpd;
		if (DecodeSingleVDOMessage(sentence, &gpd, &m_VDO_accumulator)) {

			if (!wxIsNaN(gpd.Lat))
				SendSentenceToAllInstruments(OCPN_DBP_STC_LAT, gpd.Lat, _T("SDMM"));

			if (!wxIsNaN(gpd.Lon))
				SendSentenceToAllInstruments(OCPN_DBP_STC_LON, gpd.Lon, _T("SDMM"));

			//            SendSentenceToAllInstruments( OCPN_DBP_STC_SOG, toUsrSpeed_Plugin( gpd.Sog, g_iDashSpeedUnit ), getUsrSpeedUnit_Plugin( g_iDashSpeedUnit ) );
			//            SendSentenceToAllInstruments( OCPN_DBP_STC_COG, gpd.Cog, _T("\u00B0") );
			SendSentenceToAllInstruments(OCPN_DBP_STC_SOG, toUsrSpeed_Plugin(mSOGFilter.filter(gpd.Sog), g_iDashSpeedUnit), getUsrSpeedUnit_Plugin(g_iDashSpeedUnit));
			SendSentenceToAllInstruments(OCPN_DBP_STC_COG, mCOGFilter.filter(gpd.Cog), _T("\u00B0"));
			if (!wxIsNaN(gpd.Hdt)) {
				SendSentenceToAllInstruments(OCPN_DBP_STC_HDT, gpd.Hdt, _T("\u00B0T"));
				mHDT_Watchdog = gps_watchdog_timeout_ticks;
			}
		}
	}
}

void tactics_pi::SetPositionFix(PlugIn_Position_Fix &pfix)
{
	if (mPriPosition >= 1) {
		mPriPosition = 1;
		SendSentenceToAllInstruments(OCPN_DBP_STC_LAT, pfix.Lat, _T("SDMM"));
		SendSentenceToAllInstruments(OCPN_DBP_STC_LON, pfix.Lon, _T("SDMM"));
	}
	if (mPriCOGSOG >= 1) {
		double dMagneticCOG;
		mPriCOGSOG = 1;
		//SendSentenceToAllInstruments( OCPN_DBP_STC_SOG, toUsrSpeed_Plugin( pfix.Sog, g_iDashSpeedUnit ), getUsrSpeedUnit_Plugin( g_iDashSpeedUnit ) );
		//SendSentenceToAllInstruments( OCPN_DBP_STC_COG, pfix.Cog, _T("\u00B0") );
		//dMagneticCOG = pfix.Cog - pfix.Var;
		SendSentenceToAllInstruments(OCPN_DBP_STC_SOG, toUsrSpeed_Plugin(mSOGFilter.filter(pfix.Sog), g_iDashSpeedUnit), getUsrSpeedUnit_Plugin(g_iDashSpeedUnit));
		SendSentenceToAllInstruments(OCPN_DBP_STC_COG, mCOGFilter.filter(pfix.Cog), _T("\u00B0"));
		dMagneticCOG = mCOGFilter.get() - pfix.Var;
		if (dMagneticCOG < 0.0) dMagneticCOG = 360.0 + dMagneticCOG;
		if (dMagneticCOG >= 360.0) dMagneticCOG = dMagneticCOG - 360.0;
		SendSentenceToAllInstruments(OCPN_DBP_STC_MCOG, dMagneticCOG, _T("\u00B0M"));
	}
	if (mPriVar >= 1) {
		if (!wxIsNaN(pfix.Var)){
			mPriVar = 1;
			mVar = pfix.Var;
			mVar_Watchdog = gps_watchdog_timeout_ticks;

			SendSentenceToAllInstruments(OCPN_DBP_STC_HMV, pfix.Var, _T("\u00B0"));
		}
	}
	if (mPriDateTime >= 6) { //We prefer the GPS datetime
		mPriDateTime = 6;
		mUTCDateTime.Set(pfix.FixTime);
		mUTCDateTime = mUTCDateTime.ToUTC();
	}
	mSatsInView = pfix.nSats;
	//    SendSentenceToAllInstruments( OCPN_DBP_STC_SAT, mSatsInView, _T("") );

}

void tactics_pi::SetCursorLatLon(double lat, double lon)
{
	g_dcur_lat = lat; //TR
	g_dcur_lon = lon;

	//SendSentenceToAllInstruments( OCPN_DBP_STC_PLA, lat, _T("SDMM") );
	//SendSentenceToAllInstruments( OCPN_DBP_STC_PLO, lon, _T("SDMM") );
}

void tactics_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{
	if (message_id == _T("WMM_VARIATION_BOAT"))
	{

		// construct the JSON root object
		wxJSONValue  root;
		// construct a JSON parser
		wxJSONReader reader;

		// now read the JSON text and store it in the 'root' structure
		// check for errors before retreiving values...
		int numErrors = reader.Parse(message_body, &root);
		if (numErrors > 0)  {
			//              const wxArrayString& errors = reader.GetErrors();
			return;
		}

		// get the DECL value from the JSON message
		wxString decl = root[_T("Decl")].AsString();
		double decl_val;
		decl.ToDouble(&decl_val);


		if (mPriVar >= 4) {
			mPriVar = 4;
			mVar = decl_val;
			mVar_Watchdog = gps_watchdog_timeout_ticks;
			SendSentenceToAllInstruments(OCPN_DBP_STC_HMV, mVar, _T("\u00B0"));
		}
	}
}

int tactics_pi::GetToolbarToolCount(void)
{
	return 1;
}

void tactics_pi::ShowPreferencesDialog(wxWindow* parent)
{
	TacticsPreferencesDialog *dialog = new TacticsPreferencesDialog(parent, wxID_ANY,
		m_ArrayOfTacticsWindow);

	if (dialog->ShowModal() == wxID_OK) {
		delete g_pFontTitle;
		g_pFontTitle = new wxFont(dialog->m_pFontPickerTitle->GetSelectedFont());
		delete g_pFontData;
		g_pFontData = new wxFont(dialog->m_pFontPickerData->GetSelectedFont());
		delete g_pFontLabel;
		g_pFontLabel = new wxFont(dialog->m_pFontPickerLabel->GetSelectedFont());
		delete g_pFontSmall;
		g_pFontSmall = new wxFont(dialog->m_pFontPickerSmall->GetSelectedFont());

		// OnClose should handle that for us normally but it doesn't seems to do so
		// We must save changes first
		dialog->SaveTacticsConfig();
		m_ArrayOfTacticsWindow.Clear();
		m_ArrayOfTacticsWindow = dialog->m_Config;

		ApplyConfig();
		SaveConfig();
		SetToolbarItemState(m_toolbar_item_id, GetTacticsWindowShownCount() != 0);
	}
	dialog->Destroy();
}

void tactics_pi::SetColorScheme(PI_ColorScheme cs)
{
	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindow *tactics_window = m_ArrayOfTacticsWindow.Item(i)->m_pTacticsWindow;
		if (tactics_window) tactics_window->SetColorScheme(cs);
	}
}

int tactics_pi::GetTacticsWindowShownCount()
{
	int cnt = 0;

	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindow *tactics_window = m_ArrayOfTacticsWindow.Item(i)->m_pTacticsWindow;
		if (tactics_window) {
			wxAuiPaneInfo &pane = m_pauimgr->GetPane(tactics_window);
			if (pane.IsOk() && pane.IsShown()) cnt++;
		}
	}
	return cnt;
}

void tactics_pi::OnPaneClose(wxAuiManagerEvent& event)
{
	// if name is unique, we should use it
	TacticsWindow *tactics_window = (TacticsWindow *)event.pane->window;
	int cnt = 0;
	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindowContainer *cont = m_ArrayOfTacticsWindow.Item(i);
		TacticsWindow *d_w = cont->m_pTacticsWindow;
		if (d_w) {
			// we must not count this one because it is being closed
			if (tactics_window != d_w) {
				wxAuiPaneInfo &pane = m_pauimgr->GetPane(d_w);
				if (pane.IsOk() && pane.IsShown()) cnt++;
			}
			else {
				cont->m_bIsVisible = false;
			}
		}
	}
	SetToolbarItemState(m_toolbar_item_id, cnt != 0);

	event.Skip();
}

void tactics_pi::OnToolbarToolCallback(int id)
{
	int cnt = GetTacticsWindowShownCount();

	bool b_anyviz = false;
	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindowContainer *cont = m_ArrayOfTacticsWindow.Item(i);
		if (cont->m_bIsVisible) {
			b_anyviz = true;
			break;
		}
	}

	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindowContainer *cont = m_ArrayOfTacticsWindow.Item(i);
		TacticsWindow *tactics_window = cont->m_pTacticsWindow;
		if (tactics_window) {
			wxAuiPaneInfo &pane = m_pauimgr->GetPane(tactics_window);
			if (pane.IsOk()) {
				bool b_reset_pos = false;

#ifdef __WXMSW__
				//  Support MultiMonitor setups which an allow negative window positions.
				//  If the requested window title bar does not intersect any installed monitor,
				//  then default to simple primary monitor positioning.
				RECT frame_title_rect;
				frame_title_rect.left = pane.floating_pos.x;
				frame_title_rect.top = pane.floating_pos.y;
				frame_title_rect.right = pane.floating_pos.x + pane.floating_size.x;
				frame_title_rect.bottom = pane.floating_pos.y + 30;

				if (NULL == MonitorFromRect(&frame_title_rect, MONITOR_DEFAULTTONULL)) b_reset_pos =
					true;
#else

				//    Make sure drag bar (title bar) of window intersects wxClient Area of screen, with a little slop...
				wxRect window_title_rect;// conservative estimate
				window_title_rect.x = pane.floating_pos.x;
				window_title_rect.y = pane.floating_pos.y;
				window_title_rect.width = pane.floating_size.x;
				window_title_rect.height = 30;

				wxRect ClientRect = wxGetClientDisplayRect();
				ClientRect.Deflate(60, 60);// Prevent the new window from being too close to the edge
				if (!ClientRect.Intersects(window_title_rect))
					b_reset_pos = true;

#endif

				if (b_reset_pos) pane.FloatingPosition(50, 50);

				if (cnt == 0)
					if (b_anyviz)
						pane.Show(cont->m_bIsVisible);
					else {
						cont->m_bIsVisible = cont->m_bPersVisible;
						pane.Show(cont->m_bIsVisible);
					}
				else
					pane.Show(false);
			}

			//  This patch fixes a bug in wxAUIManager
			//  FS#548
			// Dropping a DashBoard Window right on top on the (supposedly fixed) chart bar window
			// causes a resize of the chart bar, and the Tactics window assumes some of its properties
			// The Tactics window is no longer grabbable...
			// Workaround:  detect this case, and force the pane to be on a different Row.
			// so that the display is corrected by toggling the tactics off and back on.
			if ((pane.dock_direction == wxAUI_DOCK_BOTTOM) && pane.IsDocked()) pane.Row(2);
		}
	}
	// Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
	// to actual status to ensure right status upon toolbar rebuild
	SetToolbarItemState(m_toolbar_item_id, GetTacticsWindowShownCount() != 0/*cnt==0*/);
	m_pauimgr->Update();

}

void tactics_pi::UpdateAuiStatus(void)
{
	//    This method is called after the PlugIn is initialized
	//    and the frame has done its initial layout, possibly from a saved wxAuiManager "Perspective"
	//    It is a chance for the PlugIn to syncronize itself internally with the state of any Panes that
	//    were added to the frame in the PlugIn ctor.

	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindowContainer *cont = m_ArrayOfTacticsWindow.Item(i);
		wxAuiPaneInfo &pane = m_pauimgr->GetPane(cont->m_pTacticsWindow);
		// Initialize visible state as perspective is loaded now
		cont->m_bIsVisible = (pane.IsOk() && pane.IsShown());
	}

	//    We use this callback here to keep the context menu selection in sync with the window state

	SetToolbarItemState(m_toolbar_item_id, GetTacticsWindowShownCount() != 0);
}

bool tactics_pi::LoadConfig(void)
{
	wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

	if (pConf) {

		pConf->SetPath(_T("/PlugIns/Tactics/Performance"));
		pConf->Read(_T("PolarFile"), &g_path_to_PolarFile, _T("NULL"));
		pConf->Read(_T("BoatLeewayFactor"), &g_dLeewayFactor, 10);
		pConf->Read(_T("fixedLeeway"), &g_dfixedLeeway, 30);
		pConf->Read(_T("UseHeelSensor"), &g_bUseHeelSensor, true);
		pConf->Read(_T("UseFixedLeeway"), &g_bUseFixedLeeway, false);
		pConf->Read(_T("UseManHeelInput"), &g_bManHeelInput, false);
		pConf->Read(_T("Heel_5kn_45Degree"), &g_dheel[1][1], 5);
		pConf->Read(_T("Heel_5kn_90Degree"), &g_dheel[1][2], 8);
		pConf->Read(_T("Heel_5kn_135Degree"), &g_dheel[1][3], 5);
		pConf->Read(_T("Heel_10kn_45Degree"), &g_dheel[2][1], 8);
		pConf->Read(_T("Heel_10kn_90Degree"), &g_dheel[2][2], 10);
		pConf->Read(_T("Heel_10kn_135Degree"), &g_dheel[2][3], 11);
		pConf->Read(_T("Heel_15kn_45Degree"), &g_dheel[3][1], 25);
		pConf->Read(_T("Heel_15kn_90Degree"), &g_dheel[3][2], 20);
		pConf->Read(_T("Heel_15kn_135Degree"), &g_dheel[3][3], 13);
		pConf->Read(_T("Heel_20kn_45Degree"), &g_dheel[4][1], 20);
		pConf->Read(_T("Heel_20kn_90Degree"), &g_dheel[4][2], 16);
		pConf->Read(_T("Heel_20kn_135Degree"), &g_dheel[4][3], 15);
		pConf->Read(_T("Heel_25kn_45Degree"), &g_dheel[5][1], 25);
		pConf->Read(_T("Heel_25kn_90Degree"), &g_dheel[5][2], 20);
		pConf->Read(_T("Heel_25kn_135Degree"), &g_dheel[5][3], 20);
		pConf->Read(_T("UseManHeelInput"), &g_bManHeelInput, false);
		pConf->Read(_T("CorrectSTWwithLeeway"), &g_bCorrectSTWwithLeeway, false);  //if true, STW is corrected with Leeway (in case Leeway is available)
		pConf->Read(_T("CorrectAWwithHeel"), &g_bCorrectAWwithHeel, false);    //if true, AWS/AWA are corrected with Heel-Angle
		pConf->Read(_T("ForceTrueWindCalculation"), &g_bForceTrueWindCalculation, false);    //if true, NMEA Data for TWS,TWA,TWD is not used, but the plugin calculated data is used
		pConf->Read(_T("ShowWindbarbOnChart"), &g_bShowWindbarbOnChart, false);
		m_bShowWindbarbOnChart = g_bShowWindbarbOnChart;
		pConf->Read(_T("ShowPolarOnChart"), &g_bShowPolarOnChart, false);
		m_bShowPolarOnChart = g_bShowPolarOnChart;
		pConf->Read(_T("UseSOGforTWCalc"), &g_bUseSOGforTWCalc, false);
		pConf->Read(_T("ExpPolarSpeed"), &g_bExpPerfData01, false);
		pConf->Read(_T("ExpCourseOtherTack"), &g_bExpPerfData02, false);
		pConf->Read(_T("ExpTargetVMG"), &g_bExpPerfData03, false);
		pConf->Read(_T("ExpVMG_CMG_Diff_Gain"), &g_bExpPerfData04, false);
		pConf->Read(_T("ExpCurrent"), &g_bExpPerfData05, false);
		pConf->SetPath(_T("/PlugIns/Tactics"));

		wxString version;
		pConf->Read(_T("Version"), &version, wxEmptyString);
		wxString config;
		pConf->Read(_T("FontTitle"), &config, wxEmptyString);
		if (!config.IsEmpty()) g_pFontTitle->SetNativeFontInfo(config);
		pConf->Read(_T("FontData"), &config, wxEmptyString);
		if (!config.IsEmpty()) g_pFontData->SetNativeFontInfo(config);
		pConf->Read(_T("FontLabel"), &config, wxEmptyString);
		if (!config.IsEmpty()) g_pFontLabel->SetNativeFontInfo(config);
		pConf->Read(_T("FontSmall"), &config, wxEmptyString);
		if (!config.IsEmpty()) g_pFontSmall->SetNativeFontInfo(config);

		pConf->Read(_T("SpeedometerMax"), &g_iDashSpeedMax, 12);
		pConf->Read(_T("COGDamp"), &g_iDashCOGDamp, 0);
		pConf->Read(_T("SpeedUnit"), &g_iDashSpeedUnit, 0);
		pConf->Read(_T("SOGDamp"), &g_iDashSOGDamp, 0);
		pConf->Read(_T("DepthUnit"), &g_iDashDepthUnit, 3);
		g_iDashDepthUnit = wxMax(g_iDashDepthUnit, 3);

		pConf->Read(_T("DistanceUnit"), &g_iDashDistanceUnit, 0);
		pConf->Read(_T("WindSpeedUnit"), &g_iDashWindSpeedUnit, 0);
		pConf->Read(_T("CurrentDampingFactor"), &g_dalpha_currdir, 0.008);
		pConf->Read(_T("LaylineLenghtonChart"), &g_dLaylineLengthonChart, 10.0);
		pConf->Read(_T("MinLaylineWidth"), &g_iMinLaylineWidth, 4);
		pConf->Read(_T("MaxLaylineWidth"), &g_iMaxLaylineWidth, 30);
		pConf->Read(_T("LaylineWidthDampingFactor"), &g_dalphaDeltCoG, 0.25);
		pConf->Read(_T("ShowCurrentOnChart"), &g_bDisplayCurrentOnChart, false);
		pConf->Read(_T("CMGSynonym"), &g_sCMGSynonym, _T("CMG"));
		pConf->Read(_T("VMGSynonym"), &g_sVMGSynonym, _T("VMG"));

		m_bDisplayCurrentOnChart = g_bDisplayCurrentOnChart;
		int d_cnt;
		pConf->Read(_T("TacticsCount"), &d_cnt, -1);
		// TODO: Memory leak? We should destroy everything first
		m_ArrayOfTacticsWindow.Clear();
		if (version.IsEmpty() && d_cnt == -1) {
			m_config_version = 1;
			// Let's load version 1 or default settings.
			int i_cnt;
			pConf->Read(_T("InstrumentCount"), &i_cnt, -1);
			wxArrayInt ar;
			if (i_cnt != -1) {
				for (int i = 0; i < i_cnt; i++) {
					int id;
					pConf->Read(wxString::Format(_T("Instrument%d"), i + 1), &id, -1);
					if (id != -1) ar.Add(id);
				}
			}
			else {
				// This is the default instrument list
				ar.Add(ID_DBP_I_POS);
				ar.Add(ID_DBP_D_COG);
				//ar.Add( ID_DBP_D_GPS );
			}

			m_ArrayOfTacticsWindow.Add(
				new TacticsWindowContainer(NULL, GetUUID(), _("Tactics"), _T("V"), ar));
		}
		else {
			// Version 2
			m_config_version = 2;
			bool b_onePersisted = false;
			for (int i = 0; i < d_cnt; i++) {
				pConf->SetPath(wxString::Format(_T("/PlugIns/Tactics/Tactics%d"), i + 1));
				wxString name;
				pConf->Read(_T("Name"), &name, GetUUID());
				wxString caption;
				pConf->Read(_T("Caption"), &caption, _("Tactics"));
				wxString orient;
				pConf->Read(_T("Orientation"), &orient, _T("V"));
				int i_cnt;
				pConf->Read(_T("InstrumentCount"), &i_cnt, -1);
				bool b_persist;
				pConf->Read(_T("Persistence"), &b_persist, 1);

				wxArrayInt ar;
				if (i_cnt != -1) {
					for (int i = 0; i < i_cnt; i++) {
						int id;
						pConf->Read(wxString::Format(_T("Instrument%d"), i + 1), &id, -1);
						if (id != -1) ar.Add(id);
					}
				}
				else {
					// This is the default instrument list
					ar.Add(ID_DBP_D_BRG);
					ar.Add(ID_DBP_I_CURRSPD);
					ar.Add(ID_DBP_I_CURRDIR);
				}

				// TODO: Do not add if GetCount == 0
				TacticsWindowContainer *cont = new TacticsWindowContainer(NULL, name, caption, orient, ar);
				cont->m_bPersVisible = b_persist;

				if (b_persist)
					b_onePersisted = true;

				m_ArrayOfTacticsWindow.Add(cont);

			}

			// Make sure at least one dashboard is scheduled to be visible
			if (m_ArrayOfTacticsWindow.Count() && !b_onePersisted){
				TacticsWindowContainer *cont = m_ArrayOfTacticsWindow.Item(0);
				if (cont)
					cont->m_bPersVisible = true;
			}

		}

		return true;
	}
	else
		return false;
}

bool tactics_pi::SaveConfig(void)
{
	wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

	if (pConf) {
		pConf->SetPath(_T("/PlugIns/Tactics"));
		pConf->Write(_T("Version"), _T("2"));
		pConf->Write(_T("FontTitle"), g_pFontTitle->GetNativeFontInfoDesc());
		pConf->Write(_T("FontData"), g_pFontData->GetNativeFontInfoDesc());
		pConf->Write(_T("FontLabel"), g_pFontLabel->GetNativeFontInfoDesc());
		pConf->Write(_T("FontSmall"), g_pFontSmall->GetNativeFontInfoDesc());

		pConf->Write(_T("SpeedometerMax"), g_iDashSpeedMax);
		pConf->Write(_T("COGDamp"), g_iDashCOGDamp);
		pConf->Write(_T("SpeedUnit"), g_iDashSpeedUnit);
		pConf->Write(_T("SOGDamp"), g_iDashSOGDamp);
		pConf->Write(_T("DepthUnit"), g_iDashDepthUnit);
		pConf->Write(_T("DistanceUnit"), g_iDashDistanceUnit);
		pConf->Write(_T("WindSpeedUnit"), g_iDashWindSpeedUnit);
		pConf->Write(_T("TacticsCount"), (int)m_ArrayOfTacticsWindow.GetCount());
		pConf->Write(_T("CurrentDampingFactor"), g_dalpha_currdir);
		pConf->Write(_T("LaylineLenghtonChart"), g_dLaylineLengthonChart);
		pConf->Write(_T("MinLaylineWidth"), g_iMinLaylineWidth);
		pConf->Write(_T("MaxLaylineWidth"), g_iMaxLaylineWidth);
		pConf->Write(_T("LaylineWidthDampingFactor"), g_dalphaDeltCoG);
		pConf->Write(_T("ShowCurrentOnChart"), g_bDisplayCurrentOnChart);
		pConf->Write(_T("CMGSynonym"), g_sCMGSynonym);
		pConf->Write(_T("VMGSynonym"), g_sVMGSynonym);
		pConf->SetPath(_T("/PlugIns/Tactics/Performance"));
		pConf->Write(_T("PolarFile"), g_path_to_PolarFile);
		pConf->Write(_T("BoatLeewayFactor"), g_dLeewayFactor);
		pConf->Write(_T("fixedLeeway"), g_dfixedLeeway);
		pConf->Write(_T("UseHeelSensor"), g_bUseHeelSensor);
		pConf->Write(_T("UseFixedLeeway"), g_bUseFixedLeeway);
		pConf->Write(_T("UseManHeelInput"), g_bManHeelInput);
		pConf->Write(_T("CorrectSTWwithLeeway"), g_bCorrectSTWwithLeeway);
		pConf->Write(_T("CorrectAWwithHeel"), g_bCorrectAWwithHeel);
		pConf->Write(_T("ForceTrueWindCalculation"), g_bForceTrueWindCalculation);
		pConf->Write(_T("UseSOGforTWCalc"), g_bUseSOGforTWCalc);
		pConf->Write(_T("ShowWindbarbOnChart"), g_bShowWindbarbOnChart);
		pConf->Write(_T("ShowPolarOnChart"), g_bShowPolarOnChart);
		pConf->Write(_T("Heel_5kn_45Degree"), g_dheel[1][1]);
		pConf->Write(_T("Heel_5kn_90Degree"), g_dheel[1][2]);
		pConf->Write(_T("Heel_5kn_135Degree"), g_dheel[1][3]);
		pConf->Write(_T("Heel_10kn_45Degree"), g_dheel[2][1]);
		pConf->Write(_T("Heel_10kn_90Degree"), g_dheel[2][2]);
		pConf->Write(_T("Heel_10kn_135Degree"), g_dheel[2][3]);
		pConf->Write(_T("Heel_15kn_45Degree"), g_dheel[3][1]);
		pConf->Write(_T("Heel_15kn_90Degree"), g_dheel[3][2]);
		pConf->Write(_T("Heel_15kn_135Degree"), g_dheel[3][3]);
		pConf->Write(_T("Heel_20kn_45Degree"), g_dheel[4][1]);
		pConf->Write(_T("Heel_20kn_90Degree"), g_dheel[4][2]);
		pConf->Write(_T("Heel_20kn_135Degree"), g_dheel[4][3]);
		pConf->Write(_T("Heel_25kn_45Degree"), g_dheel[5][1]);
		pConf->Write(_T("Heel_25kn_90Degree"), g_dheel[5][2]);
		pConf->Write(_T("Heel_25kn_135Degree"), g_dheel[5][3]);
		pConf->Write(_T("ExpPolarSpeed"), g_bExpPerfData01);
		pConf->Write(_T("ExpCourseOtherTack"), g_bExpPerfData02);
		pConf->Write(_T("ExpTargetVMG"), g_bExpPerfData03);
		pConf->Write(_T("ExpVMG_CMG_Diff_Gain"), g_bExpPerfData04);
		pConf->Write(_T("ExpCurrent"), g_bExpPerfData05);

		for (unsigned int i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
			TacticsWindowContainer *cont = m_ArrayOfTacticsWindow.Item(i);
			pConf->SetPath(wxString::Format(_T("/PlugIns/Tactics/Tactics%d"), i + 1));
			pConf->Write(_T("Name"), cont->m_sName);
			pConf->Write(_T("Caption"), cont->m_sCaption);
			pConf->Write(_T("Orientation"), cont->m_sOrientation);
			pConf->Write(_T("Persistence"), cont->m_bPersVisible);

			pConf->Write(_T("InstrumentCount"), (int)cont->m_aInstrumentList.GetCount());
			for (unsigned int j = 0; j < cont->m_aInstrumentList.GetCount(); j++)
				pConf->Write(wxString::Format(_T("Instrument%d"), j + 1),
				cont->m_aInstrumentList.Item(j));
		}

		return true;
	}
	else
		return false;
}

void tactics_pi::ApplyConfig(void)
{
	// Reverse order to handle deletes
	for (size_t i = m_ArrayOfTacticsWindow.GetCount(); i > 0; i--) {
		TacticsWindowContainer *cont = m_ArrayOfTacticsWindow.Item(i - 1);
		int orient = (cont->m_sOrientation == _T("V") ? wxVERTICAL : wxHORIZONTAL);
		if (cont->m_bIsDeleted) {
			if (cont->m_pTacticsWindow) {
				m_pauimgr->DetachPane(cont->m_pTacticsWindow);
				cont->m_pTacticsWindow->Close();
				cont->m_pTacticsWindow->Destroy();
				cont->m_pTacticsWindow = NULL;
			}
			m_ArrayOfTacticsWindow.Remove(cont);
			delete cont;

		}
		else if (!cont->m_pTacticsWindow) {
			// A new tactics is created
			cont->m_pTacticsWindow = new TacticsWindow(GetOCPNCanvasWindow(), wxID_ANY,
				m_pauimgr, this, orient, cont);
			cont->m_pTacticsWindow->SetInstrumentList(cont->m_aInstrumentList);
			bool vertical = orient == wxVERTICAL;
			wxSize sz = cont->m_pTacticsWindow->GetMinSize();
			// Mac has a little trouble with initial Layout() sizing...
#ifdef __WXOSX__
			if (sz.x == 0)
				sz.IncTo(wxSize(160, 388));
#endif
			m_pauimgr->AddPane(cont->m_pTacticsWindow,
				wxAuiPaneInfo().Name(cont->m_sName).Caption(cont->m_sCaption).CaptionVisible(true).TopDockable(
				!vertical).BottomDockable(!vertical).LeftDockable(vertical).RightDockable(vertical).MinSize(
				sz).BestSize(sz).FloatingSize(sz).FloatingPosition(100, 100).Float().Show(cont->m_bIsVisible));
		}
		else {
			wxAuiPaneInfo& pane = m_pauimgr->GetPane(cont->m_pTacticsWindow);
			pane.Caption(cont->m_sCaption).Show(cont->m_bIsVisible);
			if (!cont->m_pTacticsWindow->isInstrumentListEqual(cont->m_aInstrumentList)) {
				cont->m_pTacticsWindow->SetInstrumentList(cont->m_aInstrumentList);
				wxSize sz = cont->m_pTacticsWindow->GetMinSize();
				pane.MinSize(sz).BestSize(sz).FloatingSize(sz);
			}
			if (cont->m_pTacticsWindow->GetSizerOrientation() != orient) {
				cont->m_pTacticsWindow->ChangePaneOrientation(orient, false);
			}
		}
	}
	m_pauimgr->Update();
	mSOGFilter.setFC(g_iDashSOGDamp ? 1.0 / (2.0*g_iDashSOGDamp) : 0.0);
	mCOGFilter.setFC(g_iDashCOGDamp ? 1.0 / (2.0*g_iDashCOGDamp) : 0.0);
	mCOGFilter.setType(IIRFILTER_TYPE_DEG);
}

void tactics_pi::PopulateContextMenu(wxMenu* menu)
{
	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindowContainer *cont = m_ArrayOfTacticsWindow.Item(i);
		wxMenuItem* item = menu->AppendCheckItem(i + 1, cont->m_sCaption);
		item->Check(cont->m_bIsVisible);
	}
}

void tactics_pi::ShowTactics(size_t id, bool visible)
{
	if (id < m_ArrayOfTacticsWindow.GetCount()) {
		TacticsWindowContainer *cont = m_ArrayOfTacticsWindow.Item(id);
		m_pauimgr->GetPane(cont->m_pTacticsWindow).Show(visible);
		cont->m_bIsVisible = visible;
		cont->m_bPersVisible = visible;
		m_pauimgr->Update();
	}
}

/* TacticsPreferencesDialog
*
*/

TacticsPreferencesDialog::TacticsPreferencesDialog(wxWindow *parent, wxWindowID id,
	wxArrayOfTactics config) :
	wxDialog(parent, id, _("Tactics preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxRESIZE_BORDER)
	//wxDEFAULT_DIALOG_STYLE )
{
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(TacticsPreferencesDialog::OnCloseDialog),
		NULL, this);

	m_pconfig = GetOCPNConfigObject();
	// Copy original config
	m_Config = wxArrayOfTactics(config);
	//      Build Tactics Page for Toolbox
	int border_size = 2;

	wxBoxSizer* itemBoxSizerMainPanel = new wxBoxSizer(wxVERTICAL);
	SetSizer(itemBoxSizerMainPanel);

	wxNotebook *itemNotebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxNB_TOP);
	itemBoxSizerMainPanel->Add(itemNotebook, 1,
		wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL | wxEXPAND, border_size);
	wxPanel *itemPanelNotebook01 = new wxPanel(itemNotebook, wxID_ANY, wxDefaultPosition,
		wxDefaultSize, wxTAB_TRAVERSAL);

	wxFlexGridSizer *itemFlexGridSizer01 = new wxFlexGridSizer(2);
	itemFlexGridSizer01->AddGrowableCol(1);
	itemPanelNotebook01->SetSizer(itemFlexGridSizer01);
	itemNotebook->AddPage(itemPanelNotebook01, _("Tactics"));

	wxBoxSizer *itemBoxSizer01 = new wxBoxSizer(wxVERTICAL);
	itemFlexGridSizer01->Add(itemBoxSizer01, 1, wxEXPAND | wxTOP | wxLEFT, border_size);

	wxImageList *imglist1 = new wxImageList(32, 32, true, 1);
	imglist1->Add(*_img_tactics_pi);

	m_pListCtrlTacticss = new wxListCtrl(itemPanelNotebook01, wxID_ANY, wxDefaultPosition,
		wxSize(50, 200), wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL);
	m_pListCtrlTacticss->AssignImageList(imglist1, wxIMAGE_LIST_SMALL);
	m_pListCtrlTacticss->InsertColumn(0, _T(""));
	m_pListCtrlTacticss->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED,
		wxListEventHandler(TacticsPreferencesDialog::OnTacticsSelected), NULL, this);
	m_pListCtrlTacticss->Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED,
		wxListEventHandler(TacticsPreferencesDialog::OnTacticsSelected), NULL, this);
	itemBoxSizer01->Add(m_pListCtrlTacticss, 1, wxEXPAND, 0);

	wxBoxSizer *itemBoxSizer02 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer01->Add(itemBoxSizer02);

	m_pButtonAddTactics = new wxBitmapButton(itemPanelNotebook01, wxID_ANY, *_img_plus,
		wxDefaultPosition, wxDefaultSize);
	itemBoxSizer02->Add(m_pButtonAddTactics, 0, wxALIGN_CENTER, 2);
	m_pButtonAddTactics->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(TacticsPreferencesDialog::OnTacticsAdd), NULL, this);
	m_pButtonDeleteTactics = new wxBitmapButton(itemPanelNotebook01, wxID_ANY, *_img_minus,
		wxDefaultPosition, wxDefaultSize);
	itemBoxSizer02->Add(m_pButtonDeleteTactics, 0, wxALIGN_CENTER, 2);
	m_pButtonDeleteTactics->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(TacticsPreferencesDialog::OnTacticsDelete), NULL, this);

	m_pPanelTactics = new wxPanel(itemPanelNotebook01, wxID_ANY, wxDefaultPosition,
		wxDefaultSize, wxBORDER_SUNKEN);
	itemFlexGridSizer01->Add(m_pPanelTactics, 1, wxEXPAND | wxTOP | wxRIGHT, border_size);

	wxBoxSizer* itemBoxSizer03 = new wxBoxSizer(wxVERTICAL);
	m_pPanelTactics->SetSizer(itemBoxSizer03);

	wxStaticBox* itemStaticBox02 = new wxStaticBox(m_pPanelTactics, wxID_ANY, _("Tactics"));
	wxStaticBoxSizer* itemStaticBoxSizer02 = new wxStaticBoxSizer(itemStaticBox02, wxHORIZONTAL);
	itemBoxSizer03->Add(itemStaticBoxSizer02, 0, wxEXPAND | wxALL, border_size);
	wxFlexGridSizer *itemFlexGridSizer = new wxFlexGridSizer(2);
	itemFlexGridSizer->AddGrowableCol(1);
	itemStaticBoxSizer02->Add(itemFlexGridSizer, 1, wxEXPAND | wxALL, 0);

	m_pCheckBoxIsVisible = new wxCheckBox(m_pPanelTactics, wxID_ANY, _("show this tactics"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer->Add(m_pCheckBoxIsVisible, 0, wxEXPAND | wxALL, border_size);
	wxStaticText *itemDummy01 = new wxStaticText(m_pPanelTactics, wxID_ANY, _T(""));
	itemFlexGridSizer->Add(itemDummy01, 0, wxEXPAND | wxALL, border_size);

	wxStaticText* itemStaticText01 = new wxStaticText(m_pPanelTactics, wxID_ANY, _("Caption:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer->Add(itemStaticText01, 0, wxEXPAND | wxALL, border_size);
	m_pTextCtrlCaption = new wxTextCtrl(m_pPanelTactics, wxID_ANY, _T(""), wxDefaultPosition,
		wxDefaultSize);
	itemFlexGridSizer->Add(m_pTextCtrlCaption, 0, wxEXPAND | wxALL, border_size);

	wxStaticText* itemStaticText02 = new wxStaticText(m_pPanelTactics, wxID_ANY,
		_("Orientation:"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer->Add(itemStaticText02, 0, wxEXPAND | wxALL, border_size);
	m_pChoiceOrientation = new wxChoice(m_pPanelTactics, wxID_ANY, wxDefaultPosition,
		wxSize(120, -1));
	m_pChoiceOrientation->Append(_("Vertical"));
	m_pChoiceOrientation->Append(_("Horizontal"));
	itemFlexGridSizer->Add(m_pChoiceOrientation, 0, wxALIGN_RIGHT | wxALL, border_size);

	wxImageList *imglist = new wxImageList(20, 20, true, 2);
	imglist->Add(*_img_instrument);
	imglist->Add(*_img_dial);

	wxStaticBox* itemStaticBox03 = new wxStaticBox(m_pPanelTactics, wxID_ANY, _("Instruments"));
	wxStaticBoxSizer* itemStaticBoxSizer03 = new wxStaticBoxSizer(itemStaticBox03, wxHORIZONTAL);
	itemBoxSizer03->Add(itemStaticBoxSizer03, 1, wxEXPAND | wxALL, border_size);

	m_pListCtrlInstruments = new wxListCtrl(m_pPanelTactics, wxID_ANY, wxDefaultPosition,
		wxSize(-1, 200), wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL);
	itemStaticBoxSizer03->Add(m_pListCtrlInstruments, 1, wxEXPAND | wxALL, border_size);
	m_pListCtrlInstruments->AssignImageList(imglist, wxIMAGE_LIST_SMALL);
	m_pListCtrlInstruments->InsertColumn(0, _("Instruments"));
	m_pListCtrlInstruments->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED,
		wxListEventHandler(TacticsPreferencesDialog::OnInstrumentSelected), NULL, this);
	m_pListCtrlInstruments->Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED,
		wxListEventHandler(TacticsPreferencesDialog::OnInstrumentSelected), NULL, this);

	wxBoxSizer* itemBoxSizer04 = new wxBoxSizer(wxVERTICAL);
	itemStaticBoxSizer03->Add(itemBoxSizer04, 0, wxALIGN_TOP | wxALL, border_size);
	m_pButtonAdd = new wxButton(m_pPanelTactics, wxID_ANY, _("Add"), wxDefaultPosition,
		wxSize(20, -1));
	itemBoxSizer04->Add(m_pButtonAdd, 0, wxEXPAND | wxALL, border_size);
	m_pButtonAdd->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(TacticsPreferencesDialog::OnInstrumentAdd), NULL, this);

	/* TODO  Instrument Properties
	m_pButtonEdit = new wxButton( m_pPanelTactics, wxID_ANY, _("Edit"), wxDefaultPosition,
	wxDefaultSize );
	itemBoxSizer04->Add( m_pButtonEdit, 0, wxEXPAND | wxALL, border_size );
	m_pButtonEdit->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
	wxCommandEventHandler(TacticsPreferencesDialog::OnInstrumentEdit), NULL, this );
	*/
	m_pButtonDelete = new wxButton(m_pPanelTactics, wxID_ANY, _("Delete"), wxDefaultPosition,
		wxSize(20, -1));
	itemBoxSizer04->Add(m_pButtonDelete, 0, wxEXPAND | wxALL, border_size);
	m_pButtonDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(TacticsPreferencesDialog::OnInstrumentDelete), NULL, this);
	itemBoxSizer04->AddSpacer(10);
	m_pButtonUp = new wxButton(m_pPanelTactics, wxID_ANY, _("Up"), wxDefaultPosition,
		wxDefaultSize);
	itemBoxSizer04->Add(m_pButtonUp, 0, wxEXPAND | wxALL, border_size);
	m_pButtonUp->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(TacticsPreferencesDialog::OnInstrumentUp), NULL, this);
	m_pButtonDown = new wxButton(m_pPanelTactics, wxID_ANY, _("Down"), wxDefaultPosition,
		wxDefaultSize);
	itemBoxSizer04->Add(m_pButtonDown, 0, wxEXPAND | wxALL, border_size);
	m_pButtonDown->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(TacticsPreferencesDialog::OnInstrumentDown), NULL, this);

	wxPanel *itemPanelNotebook02 = new wxPanel(itemNotebook, wxID_ANY, wxDefaultPosition,
		wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* itemBoxSizer05 = new wxBoxSizer(wxVERTICAL);
	itemPanelNotebook02->SetSizer(itemBoxSizer05);
	itemNotebook->AddPage(itemPanelNotebook02, _("Appearance"));

	wxStaticBox* itemStaticBox01 = new wxStaticBox(itemPanelNotebook02, wxID_ANY, _("Fonts"));
	wxStaticBoxSizer* itemStaticBoxSizer01 = new wxStaticBoxSizer(itemStaticBox01, wxHORIZONTAL);
	itemBoxSizer05->Add(itemStaticBoxSizer01, 0, wxEXPAND | wxALL, border_size);
	wxFlexGridSizer *itemFlexGridSizer03 = new wxFlexGridSizer(2);
	itemFlexGridSizer03->AddGrowableCol(1);
	itemStaticBoxSizer01->Add(itemFlexGridSizer03, 1, wxEXPAND | wxALL, 0);
	wxStaticText* itemStaticText04 = new wxStaticText(itemPanelNotebook02, wxID_ANY, _("Title:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer03->Add(itemStaticText04, 0, wxEXPAND | wxALL, border_size);
	m_pFontPickerTitle = new wxFontPickerCtrl(itemPanelNotebook02, wxID_ANY, *g_pFontTitle,
		wxDefaultPosition, wxDefaultSize);
	itemFlexGridSizer03->Add(m_pFontPickerTitle, 0, wxALIGN_RIGHT | wxALL, 0);
	wxStaticText* itemStaticText05 = new wxStaticText(itemPanelNotebook02, wxID_ANY, _("Data:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer03->Add(itemStaticText05, 0, wxEXPAND | wxALL, border_size);
	m_pFontPickerData = new wxFontPickerCtrl(itemPanelNotebook02, wxID_ANY, *g_pFontData,
		wxDefaultPosition, wxDefaultSize);
	itemFlexGridSizer03->Add(m_pFontPickerData, 0, wxALIGN_RIGHT | wxALL, 0);
	wxStaticText* itemStaticText06 = new wxStaticText(itemPanelNotebook02, wxID_ANY, _("Label:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer03->Add(itemStaticText06, 0, wxEXPAND | wxALL, border_size);
	m_pFontPickerLabel = new wxFontPickerCtrl(itemPanelNotebook02, wxID_ANY, *g_pFontLabel,
		wxDefaultPosition, wxDefaultSize);
	itemFlexGridSizer03->Add(m_pFontPickerLabel, 0, wxALIGN_RIGHT | wxALL, 0);
	wxStaticText* itemStaticText07 = new wxStaticText(itemPanelNotebook02, wxID_ANY, _("Small:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer03->Add(itemStaticText07, 0, wxEXPAND | wxALL, border_size);
	m_pFontPickerSmall = new wxFontPickerCtrl(itemPanelNotebook02, wxID_ANY, *g_pFontSmall,
		wxDefaultPosition, wxDefaultSize);
	itemFlexGridSizer03->Add(m_pFontPickerSmall, 0, wxALIGN_RIGHT | wxALL, 0);
	//      wxColourPickerCtrl

	wxStaticBox* itemStaticBox04 = new wxStaticBox(itemPanelNotebook02, wxID_ANY, _("Units, Ranges, Formats"));
	wxStaticBoxSizer* itemStaticBoxSizer04 = new wxStaticBoxSizer(itemStaticBox04, wxHORIZONTAL);
	itemBoxSizer05->Add(itemStaticBoxSizer04, 0, wxEXPAND | wxALL, border_size);
	wxFlexGridSizer *itemFlexGridSizer04 = new wxFlexGridSizer(2);
	itemFlexGridSizer04->AddGrowableCol(1);
	itemStaticBoxSizer04->Add(itemFlexGridSizer04, 1, wxEXPAND | wxALL, 0);
	wxStaticText* itemStaticText08 = new wxStaticText(itemPanelNotebook02, wxID_ANY, _("Speedometer max value:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer04->Add(itemStaticText08, 0, wxEXPAND | wxALL, border_size);
	m_pSpinSpeedMax = new wxSpinCtrl(itemPanelNotebook02, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 10, 100, g_iDashSpeedMax);
	itemFlexGridSizer04->Add(m_pSpinSpeedMax, 0, wxALIGN_RIGHT | wxALL, 0);
	//iir filter for sog, cog
	wxStaticText* itemStaticText10 = new wxStaticText(itemPanelNotebook02, wxID_ANY, _("Speed Over Ground Damping Factor:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer04->Add(itemStaticText10, 0, wxEXPAND | wxALL, border_size);
	m_pSpinSOGDamp = new wxSpinCtrl(itemPanelNotebook02, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, g_iDashSOGDamp);
	itemFlexGridSizer04->Add(m_pSpinSOGDamp, 0, wxALIGN_RIGHT | wxALL, 0);

	wxStaticText* itemStaticText11 = new wxStaticText(itemPanelNotebook02, wxID_ANY, _("COG Damping Factor:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer04->Add(itemStaticText11, 0, wxEXPAND | wxALL, border_size);
	m_pSpinCOGDamp = new wxSpinCtrl(itemPanelNotebook02, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, g_iDashCOGDamp);
	itemFlexGridSizer04->Add(m_pSpinCOGDamp, 0, wxALIGN_RIGHT | wxALL, 0);
	//iir filter end
	wxStaticText* itemStaticText09 = new wxStaticText(itemPanelNotebook02, wxID_ANY, _("Boat speed units:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer04->Add(itemStaticText09, 0, wxEXPAND | wxALL, border_size);
	wxString m_SpeedUnitChoices[] = { _("Honor OpenCPN settings"), _("Kts"), _("mph"), _("km/h"), _("m/s") };
	int m_SpeedUnitNChoices = sizeof(m_SpeedUnitChoices) / sizeof(wxString);
	m_pChoiceSpeedUnit = new wxChoice(itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_SpeedUnitNChoices, m_SpeedUnitChoices, 0);
	m_pChoiceSpeedUnit->SetSelection(g_iDashSpeedUnit + 1);
	itemFlexGridSizer04->Add(m_pChoiceSpeedUnit, 0, wxALIGN_RIGHT | wxALL, 0);

	wxStaticText* itemStaticTextDepthU = new wxStaticText(itemPanelNotebook02, wxID_ANY, _("Depth units:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer04->Add(itemStaticTextDepthU, 0, wxEXPAND | wxALL, border_size);
	wxString m_DepthUnitChoices[] = { _("Meters"), _("Feet"), _("Fathoms"), _("Centimeters"), _("Inches") };
	int m_DepthUnitNChoices = sizeof(m_DepthUnitChoices) / sizeof(wxString);
	m_pChoiceDepthUnit = new wxChoice(itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DepthUnitNChoices, m_DepthUnitChoices, 0);
	m_pChoiceDepthUnit->SetSelection(g_iDashDepthUnit - 3);
	itemFlexGridSizer04->Add(m_pChoiceDepthUnit, 0, wxALIGN_RIGHT | wxALL, 0);

	wxStaticText* itemStaticText0b = new wxStaticText(itemPanelNotebook02, wxID_ANY, _("Distance units:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer04->Add(itemStaticText0b, 0, wxEXPAND | wxALL, border_size);
	wxString m_DistanceUnitChoices[] = { _("Honor OpenCPN settings"), _("Nautical miles"), _("Statute miles"), _("Kilometers"), _("Meters") };
	int m_DistanceUnitNChoices = sizeof(m_DistanceUnitChoices) / sizeof(wxString);
	m_pChoiceDistanceUnit = new wxChoice(itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_DistanceUnitNChoices, m_DistanceUnitChoices, 0);
	m_pChoiceDistanceUnit->SetSelection(g_iDashDistanceUnit + 1);
	itemFlexGridSizer04->Add(m_pChoiceDistanceUnit, 0, wxALIGN_RIGHT | wxALL, 0);

	wxStaticText* itemStaticText0a = new wxStaticText(itemPanelNotebook02, wxID_ANY, _("Wind speed units:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer04->Add(itemStaticText0a, 0, wxEXPAND | wxALL, border_size);
	wxString m_WSpeedUnitChoices[] = { _("Kts"), _("mph"), _("km/h"), _("m/s") };
	int m_WSpeedUnitNChoices = sizeof(m_WSpeedUnitChoices) / sizeof(wxString);
	m_pChoiceWindSpeedUnit = new wxChoice(itemPanelNotebook02, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_WSpeedUnitNChoices, m_WSpeedUnitChoices, 0);
	m_pChoiceWindSpeedUnit->SetSelection(g_iDashWindSpeedUnit);
	itemFlexGridSizer04->Add(m_pChoiceWindSpeedUnit, 0, wxALIGN_RIGHT | wxALL, 0);
	//****************************************************************************************************
	// wxPanel *itemPanelNotebook03 = new wxPanel(itemNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxVSCROLL);
	wxScrolledWindow *itemPanelNotebook03 = new wxScrolledWindow(itemNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxVSCROLL);

	int scrollRate = 5;
#ifdef __OCPN__ANDROID__
	scrollRate = 1;
#endif
	itemPanelNotebook03->SetScrollRate(0, scrollRate);
	//itemNotebook->Layout();

	wxBoxSizer* itemBoxSizer06 = new wxBoxSizer(wxVERTICAL);
	itemPanelNotebook03->SetSizer(itemBoxSizer06);
	itemNotebook->AddPage(itemPanelNotebook03, _("Performance Parameters"));
	//****************************************************************************************************
	wxStaticBox* itemStaticBox05 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Laylines"));
	wxStaticBoxSizer* itemStaticBoxSizer05 = new wxStaticBoxSizer(itemStaticBox05, wxHORIZONTAL);
	itemBoxSizer06->Add(itemStaticBoxSizer05, 0, wxEXPAND | wxALL, border_size);

	wxFlexGridSizer *itemFlexGridSizer05 = new wxFlexGridSizer(2);
	itemFlexGridSizer05->AddGrowableCol(1);
	itemStaticBoxSizer05->Add(itemFlexGridSizer05, 1, wxEXPAND | wxALL, 0);
	wxString s;
	//--------------------
	wxStaticText* itemStaticText20 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Layline width damping factor [0.025-1]:  "),
		wxDefaultPosition, wxDefaultSize, 0);
	itemStaticText20->SetToolTip(_("The width of the boat laylines is based on the yawing of the boat (vertical axis), i.e. your COG changes.\nThe idea is to display the COG range where you're sailing to.\n Low values mean high damping."));
	itemFlexGridSizer05->Add(itemStaticText20, 0, wxEXPAND | wxALL, border_size);
	m_alphaDeltCoG = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0.025, 1, g_dalphaDeltCoG, 0.001);
	itemFlexGridSizer05->Add(m_alphaDeltCoG, 0, wxALIGN_LEFT, 0);
	m_alphaDeltCoG->SetValue(g_dalphaDeltCoG);
	m_alphaDeltCoG->SetToolTip(_("Width of the boat laylines is based on the yawing of the boat (vertical axis), i.e. your COG changes.\nThe idea is to display the range where you're sailing to.\n Low values mean high damping."));

	//--------------------
	wxStaticText* itemStaticText19 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Layline length on Chart [nm]:  "),
		wxDefaultPosition, wxDefaultSize, 0);
	itemStaticText19->SetToolTip(_("Length of the boat laylines in [nm]"));

	itemFlexGridSizer05->Add(itemStaticText19, 0, wxEXPAND | wxALL, border_size);
	m_pLaylineLength = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0.0, 20.0, g_dLaylineLengthonChart, 0.1);
	itemFlexGridSizer05->Add(m_pLaylineLength, 0, wxALIGN_LEFT | wxALL, 0);
	m_pLaylineLength->SetValue(g_dLaylineLengthonChart);
	m_pLaylineLength->SetToolTip(_("Length of the boat laylines in [nm]"));
	//--------------------
	wxStaticText* itemStaticText21 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Min. Layline Width [\u00B0]:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemStaticText21->SetToolTip(_("Min. width of boat laylines in degrees."));
	itemFlexGridSizer05->Add(itemStaticText21, 0, wxEXPAND | wxALL, border_size);
	m_minLayLineWidth = new wxSpinCtrl(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 20, g_iMinLaylineWidth);
	m_minLayLineWidth->SetToolTip(_("Min. width of boat laylines in degrees."));
	itemFlexGridSizer05->Add(m_minLayLineWidth, 0, wxALIGN_LEFT | wxALL, 0);

	//--------------------
	wxStaticText* itemStaticText22 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Max. Layline Width [\u00B0]:"),
		wxDefaultPosition, wxDefaultSize, 0);
	itemStaticText22->SetToolTip(_("Max. width of boat laylines in degrees."));
	itemFlexGridSizer05->Add(itemStaticText22, 0, wxEXPAND | wxALL, border_size);
	m_maxLayLineWidth = new wxSpinCtrl(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 30, g_iMaxLaylineWidth);
	m_maxLayLineWidth->SetToolTip(_("Max. width of boat laylines in degrees."));
	itemFlexGridSizer05->Add(m_maxLayLineWidth, 0, wxALIGN_LEFT | wxALL, 0);
	//****************************************************************************************************
	wxStaticBox* itemStaticBox06 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Leeway"));
	wxStaticBoxSizer* itemStaticBoxSizer06 = new wxStaticBoxSizer(itemStaticBox06, wxHORIZONTAL);
	itemBoxSizer06->Add(itemStaticBoxSizer06, 0, wxEXPAND | wxALL, border_size);
	wxFlexGridSizer *itemFlexGridSizer06 = new wxFlexGridSizer(2);
	itemFlexGridSizer06->AddGrowableCol(1);
	itemStaticBoxSizer06->Add(itemFlexGridSizer06, 1, wxEXPAND | wxALL, 0);


	//--------------------
	wxStaticText* itemStaticText23a = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Boat's Leeway factor [0-20]:  "), wxDefaultPosition, wxDefaultSize, 0);
	itemStaticText23a->SetToolTip(_("Leeway='Drift' of boat due to heel/wind influence\nLow values mean high performance of hull\nLeeway = (LeewayFactor * Heel) / STW\u00B2;")); //²
	itemFlexGridSizer06->Add(itemStaticText23a, 0, wxEXPAND | wxALL, border_size);
	m_LeewayFactor = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 20, g_dLeewayFactor, 0.01);
	m_LeewayFactor->SetToolTip(_("Leeway='Drift' of boat due to heel/wind influence\nLow values mean high performance of hull\nLeeway = (LeewayFactor * Heel) / STW\u00B2;"));

	itemFlexGridSizer06->Add(m_LeewayFactor, 0, wxALIGN_LEFT | wxALL, 0);
	m_LeewayFactor->SetValue(g_dLeewayFactor);
	//--------------------
	m_ButtonUseHeelSensor = new wxRadioButton(itemPanelNotebook03, wxID_ANY, _("Use Heel Sensor"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	itemFlexGridSizer06->Add(m_ButtonUseHeelSensor, 0, wxALL, 5);
	m_ButtonUseHeelSensor->SetValue(g_bUseHeelSensor);
	m_ButtonUseHeelSensor->SetToolTip(_("Use the internal heel sensor if available\nImportant for the correct calculation of the surface current."));
	wxStaticText* itemStaticText23b = new wxStaticText(itemPanelNotebook03, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer06->Add(itemStaticText23b, 0, wxEXPAND | wxALL, border_size);
	//--------------------
	m_ButtonFixedLeeway = new wxRadioButton(itemPanelNotebook03, wxID_ANY, _("fixed/max Leeway [\u00B0]:"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer06->Add(m_ButtonFixedLeeway, 0, wxALL, 5);
	m_ButtonFixedLeeway->SetValue(g_bUseFixedLeeway);
	m_ButtonFixedLeeway->SetToolTip(_("Dual purpose !\nIf Radiobutton is NOT set, then it's used to limit Leeway to a max value.\n If Radiobutton is set, then it fixes Leeway to this constant value."));

	m_ButtonFixedLeeway->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(TacticsPreferencesDialog::OnManualHeelUpdate), NULL, this);

	m_fixedLeeway = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 30, g_dfixedLeeway, 0.01);
	itemFlexGridSizer06->Add(m_fixedLeeway, 0, wxALIGN_LEFT, 0);
	m_fixedLeeway->SetValue(g_dfixedLeeway);
	//--------------------
	m_ButtonHeelInput = new wxRadioButton(itemPanelNotebook03, wxID_ANY, _("manual Heel input:"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer06->Add(m_ButtonHeelInput, 0, wxALL, 5);
	m_ButtonHeelInput->SetValue(g_bManHeelInput);
	m_ButtonHeelInput->SetToolTip(_("If no heel sensor is available, you can create a manual 'heel polar' here.\nJust read/enter the data from a mechanical heel sensor (e.g. on compass).\nUse True Wind Speed & Angle only !\nTake care: motoring w/o sails&heel will show wrong current data !!!"));

	m_ButtonHeelInput->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(TacticsPreferencesDialog::OnManualHeelUpdate), NULL, this);

	wxStaticText* itemStaticText23c = new wxStaticText(itemPanelNotebook03, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer06->Add(itemStaticText23c, 0, wxEXPAND | wxALL, border_size);
	//****************************************************************************************************
	wxStaticBox* itemStaticBox07 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Heel"));
	wxStaticBoxSizer* itemStaticBoxSizer07 = new wxStaticBoxSizer(itemStaticBox07, wxHORIZONTAL);
	itemBoxSizer06->Add(itemStaticBoxSizer07, 0, wxEXPAND | wxALL, border_size);
	wxFlexGridSizer *itemFlexGridSizer07 = new wxFlexGridSizer(4);
	itemStaticBoxSizer07->Add(itemFlexGridSizer07, 1, wxEXPAND | wxALL, 0);

	//--------------------
	wxStaticText* itemStaticText23T0 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("TWS/TWA "), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer07->Add(itemStaticText23T0, 0, wxEXPAND | wxALL, border_size);
	wxStaticText* itemStaticText23T1 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _(" 45\u00B0"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer07->Add(itemStaticText23T1, 0, wxALIGN_CENTER | wxALL, border_size);
	wxStaticText* itemStaticText23T2 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _(" 90\u00B0"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer07->Add(itemStaticText23T2, 0, wxALIGN_CENTER | wxALL, border_size);
	wxStaticText* itemStaticText23T3 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("135\u00B0"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer07->Add(itemStaticText23T3, 0, wxALIGN_CENTER | wxALL, border_size);
	//--------------------
	wxStaticText* itemStaticText23ws5 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("5 kn"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer07->Add(itemStaticText23ws5, 0, wxEXPAND | wxALL, border_size);
	m_heel5_45 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[1][1], 0.1);
	itemFlexGridSizer07->Add(m_heel5_45, 0, wxALIGN_LEFT, 0);
	m_heel5_45->SetValue(g_dheel[1][1]);
	//--------------------
	m_heel5_90 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[1][2], 0.1);
	itemFlexGridSizer07->Add(m_heel5_90, 0, wxALIGN_LEFT, 0);
	m_heel5_90->SetValue(g_dheel[1][2]);
	//--------------------
	m_heel5_135 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[1][3], 0.1);
	itemFlexGridSizer07->Add(m_heel5_135, 0, wxALIGN_LEFT, 0);
	m_heel5_135->SetValue(g_dheel[1][3]);
	//--------------------
	wxStaticText* itemStaticText23ws10 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("10 kn"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer07->Add(itemStaticText23ws10, 0, wxEXPAND | wxALL, border_size);
	m_heel10_45 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[2][1], 0.1);
	itemFlexGridSizer07->Add(m_heel10_45, 0, wxALIGN_LEFT, 0);
	m_heel10_45->SetValue(g_dheel[2][1]);
	//--------------------
	m_heel10_90 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[2][2], 0.1);
	itemFlexGridSizer07->Add(m_heel10_90, 0, wxALIGN_LEFT, 0);
	m_heel10_90->SetValue(g_dheel[2][2]);
	//--------------------
	m_heel10_135 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[2][3], 0.1);
	itemFlexGridSizer07->Add(m_heel10_135, 0, wxALIGN_LEFT, 0);
	m_heel10_135->SetValue(g_dheel[2][3]);
	//--------------------
	wxStaticText* itemStaticText23ws15 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("15 kn"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer07->Add(itemStaticText23ws15, 0, wxEXPAND | wxALL, border_size);

	m_heel15_45 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[3][1], 0.1);
	itemFlexGridSizer07->Add(m_heel15_45, 0, wxALIGN_LEFT, 0);
	m_heel15_45->SetValue(g_dheel[3][1]);
	//--------------------
	m_heel15_90 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[3][2], 0.1);
	itemFlexGridSizer07->Add(m_heel15_90, 0, wxALIGN_LEFT, 0);
	m_heel15_90->SetValue(g_dheel[3][2]);
	//--------------------
	m_heel15_135 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[3][3], 0.1);
	itemFlexGridSizer07->Add(m_heel15_135, 0, wxALIGN_LEFT, 0);
	m_heel15_135->SetValue(g_dheel[3][3]);
	//--------------------
	wxStaticText* itemStaticText23ws20 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("20 kn"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer07->Add(itemStaticText23ws20, 0, wxEXPAND | wxALL, border_size);
	m_heel20_45 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[4][1], 0.1);
	itemFlexGridSizer07->Add(m_heel20_45, 0, wxALIGN_LEFT, 0);
	m_heel20_45->SetValue(g_dheel[4][1]);
	//--------------------
	m_heel20_90 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[4][2], 0.1);
	itemFlexGridSizer07->Add(m_heel20_90, 0, wxALIGN_LEFT, 0);
	m_heel20_90->SetValue(g_dheel[4][2]);
	//--------------------
	m_heel20_135 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[4][3], 0.1);
	itemFlexGridSizer07->Add(m_heel20_135, 0, wxALIGN_LEFT, 0);
	m_heel20_135->SetValue(g_dheel[4][3]);
	//--------------------
	wxStaticText* itemStaticText23ws25 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("25 kn"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer07->Add(itemStaticText23ws25, 0, wxEXPAND | wxALL, border_size);
	m_heel25_45 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[5][1], 0.1);
	itemFlexGridSizer07->Add(m_heel25_45, 0, wxALIGN_LEFT, 0);
	m_heel25_45->SetValue(g_dheel[5][1]);
	//--------------------
	m_heel25_90 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[5][2], 0.1);
	itemFlexGridSizer07->Add(m_heel25_90, 0, wxALIGN_LEFT, 0);
	m_heel25_90->SetValue(g_dheel[5][2]);
	//--------------------
	m_heel25_135 = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 60, g_dheel[5][3], 0.1);
	itemFlexGridSizer07->Add(m_heel25_135, 0, wxALIGN_LEFT, 0);
	m_heel25_135->SetValue(g_dheel[5][3]);

	//****************************************************************************************************
	wxStaticBox* itemStaticBox08 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Current"));
	wxStaticBoxSizer* itemStaticBoxSizer08 = new wxStaticBoxSizer(itemStaticBox08, wxHORIZONTAL);
	itemBoxSizer06->Add(itemStaticBoxSizer08, 0, wxEXPAND | wxALL, border_size);
	wxFlexGridSizer *itemFlexGridSizer08 = new wxFlexGridSizer(2);
	itemFlexGridSizer08->AddGrowableCol(1);
	itemStaticBoxSizer08->Add(itemFlexGridSizer08, 1, wxEXPAND | wxALL, 0);

	//--------------------
	//
	/*
	wxStaticText* itemStaticText24 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Current damping factor [0.001-0.4]:  "),wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer08->Add(itemStaticText24, 0, wxEXPAND | wxALL, border_size);
	m_AlphaCurrDir = new wxSpinCtrlDouble(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS | wxSP_WRAP, 0.001, 0.4, g_dalpha_currdir, 0.001);
	itemFlexGridSizer08->Add(m_AlphaCurrDir, 0, wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 0);
	m_AlphaCurrDir->SetValue(g_dalpha_currdir);*/
	wxStaticText* itemStaticText24 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Current damping factor [1-400]:  "), wxDefaultPosition, wxDefaultSize, 0);
	itemStaticText24->SetToolTip(_("Stabilizes the surface current 'arrow' in the chart overlay, bearing compass and also the numerical instruments\nLow values mean high damping"));
	itemFlexGridSizer08->Add(itemStaticText24, 0, wxEXPAND | wxALL, border_size);
	m_AlphaCurrDir = new wxSpinCtrl(itemPanelNotebook03, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS | wxSP_WRAP, 1, 400, g_dalpha_currdir * 1000);
	itemFlexGridSizer08->Add(m_AlphaCurrDir, 0, wxALIGN_LEFT, 0);
	m_AlphaCurrDir->SetValue(g_dalpha_currdir * 1000);
	m_AlphaCurrDir->SetToolTip(_("Stabilizes the surface current 'arrow' in the chart overlay, bearing compass and also the numerical instruments\nLow values mean high damping"));
	/*    int ialphaCurrDir = g_dalpha_currdir*2000;wxBOTTOM | wxEXPAND | wxLEFT | wxRIGHT | wxTOP
	m_AlphaCurrDir = new wxSlider(itemPanelNotebook03, wxID_ANY, ialphaCurrDir, 2, 800, wxDefaultPosition, wxSize(200, 20));
	itemFlexGridSizer08->Add(m_AlphaCurrDir, 1, wxALL | wxEXPAND, 2);
	m_AlphaCurrDir->SetValue(ialphaCurrDir);
	m_AlphaCurrDir->Connect(wxEVT_COMMAND_SLIDER_UPDATED, wxCommandEventHandler(TacticsPreferencesDialog::OnAlphaCurrDirSliderUpdated), NULL, this);
	*/
	//--------------------
	m_CurrentOnChart = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Display Current on Chart (OpenGL)"));
	itemFlexGridSizer08->Add(m_CurrentOnChart, 0, wxEXPAND, 5);
	m_CurrentOnChart->SetValue(g_bDisplayCurrentOnChart);
	m_CurrentOnChart->SetToolTip(_("The default on program startup"));
	//****************************************************************************************************
	wxStaticBox* itemStaticBox10 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("True Wind"));
	wxStaticBoxSizer* itemStaticBoxSizer10 = new wxStaticBoxSizer(itemStaticBox10, wxHORIZONTAL);
	itemBoxSizer06->Add(itemStaticBoxSizer10, 0, wxEXPAND | wxALL, border_size);
	wxFlexGridSizer *itemFlexGridSizer10 = new wxFlexGridSizer(2);
	itemFlexGridSizer10->AddGrowableCol(1);
	itemStaticBoxSizer10->Add(itemFlexGridSizer10, 1, wxEXPAND | wxALL, 0);

	//--------------------
	m_CorrectSTWwithLeeway = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Correct STW with Leeway"));
	itemFlexGridSizer10->Add(m_CorrectSTWwithLeeway, 0, wxEXPAND, 5);
	m_CorrectSTWwithLeeway->SetValue(g_bCorrectSTWwithLeeway);
	m_CorrectSTWwithLeeway->SetToolTip(_("Apply a correction to your log speed throughout the plugin based on the calculated Leeway (via the heel sensor).\nOnly makes sense with a real heel sensor.\nMake sure your instruments do not already apply this correction !"));
	//--------------------
	m_CorrectAWwithHeel = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Correct AWS/AWA with Heel"));
	itemFlexGridSizer10->Add(m_CorrectAWwithHeel, 0, wxEXPAND, 5);
	m_CorrectAWwithHeel->SetValue(g_bCorrectAWwithHeel);
	m_CorrectAWwithHeel->SetToolTip(_("Use with care, this is normally done by the instruments themselves as soon as you have an integrated, original equipment heel sensor"));

	m_CorrectAWwithHeel->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(TacticsPreferencesDialog::OnAWSAWACorrectionUpdated), NULL, this);
	//--------------------
	m_ForceTrueWindCalculation = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Force True Wind Calculation"));
	itemFlexGridSizer10->Add(m_ForceTrueWindCalculation, 0, wxEXPAND, 5);
	m_ForceTrueWindCalculation->SetValue(g_bForceTrueWindCalculation);
	m_ForceTrueWindCalculation->SetToolTip(_("Internally calculates True Wind data (TWS,TWA,TWD) and uses it within the whole plugin even if there is True Wind data available via NMEA."));

	//--------------------
	m_UseSOGforTWCalc = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Use SOG instead of STW for True Wind Calc."));
	itemFlexGridSizer10->Add(m_UseSOGforTWCalc, 0, wxEXPAND, 5);
	m_UseSOGforTWCalc->SetValue(g_bUseSOGforTWCalc);
	m_UseSOGforTWCalc->SetToolTip(_("Recommended. As True Wind blows over the earth surface, we should calc. it with Speed Over Ground.\nEliminates the influence of currents."));

	m_ShowWindbarbOnChart = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Show Wind Barb on Chart (OpenGL)"));
	itemFlexGridSizer10->Add(m_ShowWindbarbOnChart, 0, wxEXPAND, 5);
	m_ShowWindbarbOnChart->SetValue(g_bShowWindbarbOnChart);
	m_ShowWindbarbOnChart->SetToolTip(_("The default on program startup"));

	//****************************************************************************************************
	wxStaticBox* itemStaticBox09 = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Polar"));
	wxStaticBoxSizer* itemStaticBoxSizer09 = new wxStaticBoxSizer(itemStaticBox09, wxHORIZONTAL);
	itemBoxSizer06->Add(itemStaticBoxSizer09, 0, wxEXPAND | wxALL, border_size);
	wxFlexGridSizer *itemFlexGridSizer09 = new wxFlexGridSizer(2);
	itemFlexGridSizer09->AddGrowableCol(1);
	itemStaticBoxSizer09->Add(itemFlexGridSizer09, 1, wxEXPAND | wxALL, 0);

	wxStaticText* itemStaticText30 = new wxStaticText(itemPanelNotebook03, wxID_ANY, _("Polar :"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer09->Add(itemStaticText30, 0, wxEXPAND | wxALL, border_size);

	m_pTextCtrlPolar = new wxTextCtrl(itemPanelNotebook03, wxID_ANY, g_path_to_PolarFile, wxDefaultPosition, wxDefaultSize);
	itemFlexGridSizer09->Add(m_pTextCtrlPolar, 0, wxALIGN_LEFT | wxEXPAND | wxALL, border_size);

	m_buttonLoadPolar = new wxButton(itemPanelNotebook03, wxID_ANY, _("Load"), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizer09->Add(m_buttonLoadPolar, 0, wxALIGN_LEFT | wxALL, 5);
	m_buttonLoadPolar->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TacticsPreferencesDialog::SelectPolarFile), NULL, this);

	m_ShowPolarOnChart = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Show Polar on Chart (OpenGL)"));
	itemFlexGridSizer09->Add(m_ShowPolarOnChart, 0, wxEXPAND, 5);
	m_ShowPolarOnChart->SetValue(g_bShowPolarOnChart);
	m_ShowPolarOnChart->SetToolTip(_("The default on program startup"));
	//****************************************************************************************************
	wxStaticBox* itemStaticBoxExpData = new wxStaticBox(itemPanelNotebook03, wxID_ANY, _("Export NMEA Performance Data"));
	wxStaticBoxSizer* itemStaticBoxSizerExpData = new wxStaticBoxSizer(itemStaticBoxExpData, wxHORIZONTAL);
	itemBoxSizer06->Add(itemStaticBoxSizerExpData, 0, wxEXPAND | wxALL, border_size);
	wxFlexGridSizer *itemFlexGridSizerExpData = new wxFlexGridSizer(2);
	itemFlexGridSizerExpData->AddGrowableCol(1);
	itemStaticBoxSizerExpData->Add(itemFlexGridSizerExpData, 1, wxEXPAND | wxALL, 0);
	//-------------------- Radiobutton(s) for different instrument systems -----------
	m_ButtonExpNKE = new wxRadioButton(itemPanelNotebook03, wxID_ANY, _("NKE format ($PNKEP)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	itemFlexGridSizerExpData->Add(m_ButtonExpNKE, 0, wxALL, 5);
	m_ButtonExpNKE->SetValue(true); // fixed value for now
	m_ButtonExpNKE->SetToolTip(_("Currently only set up for NKE instruments. Exports a predefined set of up to 5 NMEA records which are 'known' by NKE instruments and can be displayed there.\nRead the manual how to set up the interface connection !"));

	wxStaticText* itemStaticTextDummy = new wxStaticText(itemPanelNotebook03, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize, 0);
	itemFlexGridSizerExpData->Add(itemStaticTextDummy, 0, wxEXPAND | wxALL, border_size);
	//--------------------
	//--------------------
	m_ExpPerfData01 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Target Polar Speed"));
	itemFlexGridSizerExpData->Add(m_ExpPerfData01, 0, wxEXPAND, 5);
	m_ExpPerfData01->SetValue(g_bExpPerfData01);
	//--------------------
	m_ExpPerfData02 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("CoG on other Tack"));
	itemFlexGridSizerExpData->Add(m_ExpPerfData02, 0, wxEXPAND, 5);
	m_ExpPerfData02->SetValue(g_bExpPerfData02);
	//--------------------
	//    m_ExpPerfData03 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Target-VMG angle + Perf. %"));
	m_ExpPerfData03 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Target-") + g_sVMGSynonym + _(" angle + Perf. %"));
	itemFlexGridSizerExpData->Add(m_ExpPerfData03, 0, wxEXPAND, 5);
	m_ExpPerfData03->SetValue(g_bExpPerfData03);
	//--------------------
	//    m_ExpPerfData04 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Diff. angle to Target-VMG/-CMG + corresp. gain"));
	m_ExpPerfData04 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Diff. angle to Target-") + g_sVMGSynonym + _T("/-") + g_sCMGSynonym + _(" + corresp.gain"));
	itemFlexGridSizerExpData->Add(m_ExpPerfData04, 0, wxEXPAND, 5);
	m_ExpPerfData04->SetValue(g_bExpPerfData04);
	//--------------------
	m_ExpPerfData05 = new wxCheckBox(itemPanelNotebook03, wxID_ANY, _("Current Direction + Speed"));
	itemFlexGridSizerExpData->Add(m_ExpPerfData05, 0, wxEXPAND, 5);
	m_ExpPerfData05->SetValue(g_bExpPerfData05);
	//--------------------

	//****************************************************************************************************
	//    m_buttonPrefsApply = new wxButton(itemPanelNotebook03, wxID_ANY, _("Apply"), wxDefaultPosition, wxDefaultSize, 0);
	//    itemFlexGridSizer09->Add(m_buttonPrefsApply, 0, wxALIGN_RIGHT | wxALL, 5);
	//    m_buttonPrefsApply->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TacticsPreferencesDialog::ApplyPrefs), NULL, this);

	wxStdDialogButtonSizer* DialogButtonSizer = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
	//wxStdDialogButtonSizer *m_sdbSizerBtns = new wxStdDialogButtonSizer();
	//wxButton *m_sdbSizerBtnsOK = new wxButton(this, wxID_OK);
	//m_sdbSizerBtns->AddButton(m_sdbSizerBtnsOK);
	//m_sdbSizerBtns->Realize();

	m_buttonPrefsApply = new wxButton(this, wxID_APPLY);
	DialogButtonSizer->AddButton(m_buttonPrefsApply);
	//DialogButtonSizer->SetAffirmativeButton(m_buttonPrefsApply);
	m_buttonPrefsApply->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TacticsPreferencesDialog::ApplyPrefs), NULL, this);
	DialogButtonSizer->Realize();
	//m_buttonPrefOK = new wxButton(DialogButtonSizer, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxOK);
	//m_buttonPrefOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(tactics_pi::SaveConfig), NULL, this);
	//m_buttonPrefOK = DialogButtonSizer->GetAffirmativeButton();

	itemBoxSizerMainPanel->Add(DialogButtonSizer, 0, wxALIGN_RIGHT | wxALL, 5);
	//  m_buttonPrefOK = new wxButton;
	//  m_buttonPrefOK = DialogButtonSizer->GetAffirmativeButton();
	//    m_buttonPrefOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(tactics_pi::SaveConfig), NULL, this);
	//wxButton *GetAffirmativeButton() const { return m_buttonAffirmative; }
	/*   m_buttonPrefsApply = new wxButton(itemBoxSizerMainPanel, wxID_ANY, _("Apply"), wxDefaultPosition, wxDefaultSize, 0);
	DialogButtonSizer->AddButton(m_buttonPrefsApply);
	m_buttonPrefsApply->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TacticsPreferencesDialog::SelectPolarFile), NULL, this);
	DialogButtonSizer->Realize();*/
	curSel = -1;
	for (size_t i = 0; i < m_Config.GetCount(); i++) {
		m_pListCtrlTacticss->InsertItem(i, 0);
		// Using data to store m_Config index for managing deletes
		m_pListCtrlTacticss->SetItemData(i, i);
	}
	m_pListCtrlTacticss->SetColumnWidth(0, wxLIST_AUTOSIZE);
	//itemPanelNotebook03->SetSize(itemBoxSizer06->GetSize());
	UpdateTacticsButtonsState();
	UpdateButtonsState();
	SetMinSize(wxSize(450, -1));
	Fit();
}
void TacticsPreferencesDialog::OnPrefScroll(wxCommandEvent& event){
	m_AlphaCurrDir->Refresh();
	//itemPanelNotebook03->Refresh();
}
void TacticsPreferencesDialog::OnAWSAWACorrectionUpdated(wxCommandEvent& event)
{ // check if heel is available
	//...
	if (!m_ButtonUseHeelSensor->GetValue()){
		wxMessageBox(_("This option makes only sense with a real heel sensor."));
		m_CorrectAWwithHeel->SetValue(false);
	}
	else{
		//display warning
		if (m_CorrectAWwithHeel->IsChecked()){
			wxMessageBox(_("Make sure your instruments do not internally correct AWS / AWA with heel.\nThis may result in wrong wind data."));
		}
	}
}
void TacticsPreferencesDialog::OnManualHeelUpdate(wxCommandEvent& event)
{
	if (m_ButtonFixedLeeway->GetValue() || m_ButtonHeelInput->GetValue()){
		if (m_CorrectAWwithHeel->IsChecked()){
			wxMessageBox(_("This will also disable the AWA/AWS correction."));
			m_CorrectAWwithHeel->SetValue(false);
		}
	}
}
void TacticsPreferencesDialog::OnAlphaCurrDirSliderUpdated(wxCommandEvent& event){
	//g_dalpha_currdir = (double) m_AlphaCurrDir->GetValue() / 2000.0;
}
void TacticsPreferencesDialog::OnCloseDialog(wxCloseEvent& event)
{
	SaveTacticsConfig();
	event.Skip();
}
void TacticsPreferencesDialog::SelectPolarFile(wxCommandEvent& event)
{
	wxFileDialog fdlg(GetOCPNCanvasWindow(), _("Select a Polar-File"), _T(""));
	if (fdlg.ShowModal() == wxID_CANCEL) return;
	g_path_to_PolarFile = fdlg.GetPath();
	BoatPolar->loadPolar(g_path_to_PolarFile);
	if (m_pTextCtrlPolar)  m_pTextCtrlPolar->SetValue(g_path_to_PolarFile);
	wxFileConfig *pConf = (wxFileConfig *)m_pconfig;
	if (pConf) {
		pConf->SetPath(_T("/PlugIns/Tactics/Performance"));
		pConf->Write(_T("PolarFile"), g_path_to_PolarFile);
	}
}
void TacticsPreferencesDialog::ApplyPrefs(wxCommandEvent& event)
{
	//wxLogMessage("Apply");
	SaveTacticsConfig();
}
void TacticsPreferencesDialog::SaveTacticsConfig()
{
	g_iDashSpeedMax = m_pSpinSpeedMax->GetValue();
	g_iDashCOGDamp = m_pSpinCOGDamp->GetValue();
	g_iDashSOGDamp = m_pSpinSOGDamp->GetValue();
	g_iDashSpeedUnit = m_pChoiceSpeedUnit->GetSelection() - 1;
	g_iDashDepthUnit = m_pChoiceDepthUnit->GetSelection() + 3;
	g_iDashDistanceUnit = m_pChoiceDistanceUnit->GetSelection() - 1;
	g_iDashWindSpeedUnit = m_pChoiceWindSpeedUnit->GetSelection();
	g_dLeewayFactor = m_LeewayFactor->GetValue();
	g_dfixedLeeway = m_fixedLeeway->GetValue();

	g_dalpha_currdir = (double)m_AlphaCurrDir->GetValue() / 1000.0;
	//    g_dalpha_currdir = m_AlphaCurrDir->GetValue();
	g_dalphaDeltCoG = m_alphaDeltCoG->GetValue();
	g_dLaylineLengthonChart = m_pLaylineLength->GetValue();
	g_iMinLaylineWidth = m_minLayLineWidth->GetValue();
	g_iMaxLaylineWidth = m_maxLayLineWidth->GetValue();
	g_bDisplayCurrentOnChart = m_CurrentOnChart->GetValue();
	g_dheel[1][1] = m_heel5_45->GetValue();
	g_dheel[1][2] = m_heel5_90->GetValue();
	g_dheel[1][3] = m_heel5_135->GetValue();
	g_dheel[2][1] = m_heel10_45->GetValue();
	g_dheel[2][2] = m_heel10_90->GetValue();
	g_dheel[2][3] = m_heel10_135->GetValue();
	g_dheel[3][1] = m_heel15_45->GetValue();
	g_dheel[3][2] = m_heel15_90->GetValue();
	g_dheel[3][3] = m_heel15_135->GetValue();
	g_dheel[4][1] = m_heel20_45->GetValue();
	g_dheel[4][2] = m_heel20_90->GetValue();
	g_dheel[4][3] = m_heel20_135->GetValue();
	g_dheel[5][1] = m_heel25_45->GetValue();
	g_dheel[5][2] = m_heel25_90->GetValue();
	g_dheel[5][3] = m_heel25_135->GetValue();

	g_bUseHeelSensor = m_ButtonUseHeelSensor->GetValue();
	g_bUseFixedLeeway = m_ButtonFixedLeeway->GetValue();
	g_bManHeelInput = m_ButtonHeelInput->GetValue();
	g_path_to_PolarFile = m_pTextCtrlPolar->GetValue();
	g_bCorrectSTWwithLeeway = m_CorrectSTWwithLeeway->GetValue();
	g_bCorrectAWwithHeel = m_CorrectAWwithHeel->GetValue();
	g_bForceTrueWindCalculation = m_ForceTrueWindCalculation->GetValue();
	g_bUseSOGforTWCalc = m_UseSOGforTWCalc->GetValue();
	g_bShowWindbarbOnChart = m_ShowWindbarbOnChart->GetValue();
	g_bShowPolarOnChart = m_ShowPolarOnChart->GetValue();
	g_bExpPerfData01 = m_ExpPerfData01->GetValue();
	g_bExpPerfData02 = m_ExpPerfData02->GetValue();
	g_bExpPerfData03 = m_ExpPerfData03->GetValue();
	g_bExpPerfData04 = m_ExpPerfData04->GetValue();
	g_bExpPerfData05 = m_ExpPerfData05->GetValue();
	if (curSel != -1) {
		TacticsWindowContainer *cont = m_Config.Item(curSel);
		cont->m_bIsVisible = m_pCheckBoxIsVisible->IsChecked();
		cont->m_sCaption = m_pTextCtrlCaption->GetValue();
		cont->m_sOrientation = m_pChoiceOrientation->GetSelection() == 0 ? _T("V") : _T("H");
		cont->m_aInstrumentList.Clear();
		for (int i = 0; i < m_pListCtrlInstruments->GetItemCount(); i++)
			cont->m_aInstrumentList.Add((int)m_pListCtrlInstruments->GetItemData(i));
	}
}

void TacticsPreferencesDialog::OnTacticsSelected(wxListEvent& event)
{
	// save changes
	SaveTacticsConfig();
	UpdateTacticsButtonsState();
}

void TacticsPreferencesDialog::UpdateTacticsButtonsState()
{
	long item = -1;
	item = m_pListCtrlTacticss->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	bool enable = (item != -1);

	//  Disable the Tactics Delete button if the parent(Tactics) of this dialog is selected.
	bool delete_enable = enable;
	if (item != -1) {
		int sel = m_pListCtrlTacticss->GetItemData(item);
		TacticsWindowContainer *cont = m_Config.Item(sel);
		TacticsWindow *dash_sel = cont->m_pTacticsWindow;
		if (dash_sel == GetParent())
			delete_enable = false;
	}
	m_pButtonDeleteTactics->Enable(delete_enable);

	m_pPanelTactics->Enable(enable);

	if (item != -1) {
		curSel = m_pListCtrlTacticss->GetItemData(item);
		TacticsWindowContainer *cont = m_Config.Item(curSel);
		m_pCheckBoxIsVisible->SetValue(cont->m_bIsVisible);
		m_pTextCtrlCaption->SetValue(cont->m_sCaption);
		m_pChoiceOrientation->SetSelection(cont->m_sOrientation == _T("V") ? 0 : 1);
		m_pListCtrlInstruments->DeleteAllItems();
		for (size_t i = 0; i < cont->m_aInstrumentList.GetCount(); i++) {
			wxListItem item;
			getListItemForInstrument(item, cont->m_aInstrumentList.Item(i));
			item.SetId(m_pListCtrlInstruments->GetItemCount());
			m_pListCtrlInstruments->InsertItem(item);
		}

		m_pListCtrlInstruments->SetColumnWidth(0, wxLIST_AUTOSIZE);
	}
	else {
		curSel = -1;
		m_pCheckBoxIsVisible->SetValue(false);
		m_pTextCtrlCaption->SetValue(_T(""));
		m_pChoiceOrientation->SetSelection(0);
		m_pListCtrlInstruments->DeleteAllItems();
	}
	//      UpdateButtonsState();
}

void TacticsPreferencesDialog::OnTacticsAdd(wxCommandEvent& event)
{
	int idx = m_pListCtrlTacticss->GetItemCount();
	m_pListCtrlTacticss->InsertItem(idx, 0);
	// Data is index in m_Config
	m_pListCtrlTacticss->SetItemData(idx, m_Config.GetCount());
	wxArrayInt ar;
	TacticsWindowContainer *dwc = new TacticsWindowContainer(NULL, GetUUID(), _("Tactics"), _T("V"), ar);
	dwc->m_bIsVisible = true;
	m_Config.Add(dwc);
}

void TacticsPreferencesDialog::OnTacticsDelete(wxCommandEvent& event)
{
	long itemID = -1;
	itemID = m_pListCtrlTacticss->GetNextItem(itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	int idx = m_pListCtrlTacticss->GetItemData(itemID);
	m_pListCtrlTacticss->DeleteItem(itemID);
	m_Config.Item(idx)->m_bIsDeleted = true;
	UpdateTacticsButtonsState();
}

void TacticsPreferencesDialog::OnInstrumentSelected(wxListEvent& event)
{
	UpdateButtonsState();
}

void TacticsPreferencesDialog::UpdateButtonsState()
{
	long item = -1;
	item = m_pListCtrlInstruments->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	bool enable = (item != -1);

	m_pButtonDelete->Enable(enable);
	//    m_pButtonEdit->Enable( false ); // TODO: Properties
	m_pButtonUp->Enable(item > 0);
	m_pButtonDown->Enable(item != -1 && item < m_pListCtrlInstruments->GetItemCount() - 1);
}

void TacticsPreferencesDialog::OnInstrumentAdd(wxCommandEvent& event)
{
	AddInstrumentDlg pdlg((wxWindow *)event.GetEventObject(), wxID_ANY);

	if (pdlg.ShowModal() == wxID_OK) {
		wxListItem item;
		getListItemForInstrument(item, pdlg.GetInstrumentAdded());
		item.SetId(m_pListCtrlInstruments->GetItemCount());
		m_pListCtrlInstruments->InsertItem(item);
		m_pListCtrlInstruments->SetColumnWidth(0, wxLIST_AUTOSIZE);
		UpdateButtonsState();
	}
}

void TacticsPreferencesDialog::OnInstrumentDelete(wxCommandEvent& event)
{
	long itemID = -1;
	itemID = m_pListCtrlInstruments->GetNextItem(itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	m_pListCtrlInstruments->DeleteItem(itemID);
	UpdateButtonsState();
}

void TacticsPreferencesDialog::OnInstrumentEdit(wxCommandEvent& event)
{
	// TODO: Instument options
}

void TacticsPreferencesDialog::OnInstrumentUp(wxCommandEvent& event)
{
	long itemID = -1;
	itemID = m_pListCtrlInstruments->GetNextItem(itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	wxListItem item;
	item.SetId(itemID);
	item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE | wxLIST_MASK_DATA);
	m_pListCtrlInstruments->GetItem(item);
	item.SetId(itemID - 1);
	m_pListCtrlInstruments->DeleteItem(itemID);
	m_pListCtrlInstruments->InsertItem(item);
	m_pListCtrlInstruments->SetItemState(itemID - 1, wxLIST_STATE_SELECTED,
		wxLIST_STATE_SELECTED);
	UpdateButtonsState();
}

void TacticsPreferencesDialog::OnInstrumentDown(wxCommandEvent& event)
{
	long itemID = -1;
	itemID = m_pListCtrlInstruments->GetNextItem(itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	wxListItem item;
	item.SetId(itemID);
	item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE | wxLIST_MASK_DATA);
	m_pListCtrlInstruments->GetItem(item);
	item.SetId(itemID + 1);
	m_pListCtrlInstruments->DeleteItem(itemID);
	m_pListCtrlInstruments->InsertItem(item);
	m_pListCtrlInstruments->SetItemState(itemID + 1, wxLIST_STATE_SELECTED,
		wxLIST_STATE_SELECTED);
	UpdateButtonsState();
}

//----------------------------------------------------------------
//
//    Add Instrument Dialog Implementation
//
//----------------------------------------------------------------

AddInstrumentDlg::AddInstrumentDlg(wxWindow *pparent, wxWindowID id) :
wxDialog(pparent, id, _("Add instrument"), wxDefaultPosition, wxDefaultSize,
wxDEFAULT_DIALOG_STYLE)
{
	wxBoxSizer* itemBoxSizer01 = new wxBoxSizer(wxVERTICAL);
	SetSizer(itemBoxSizer01);
	wxStaticText* itemStaticText01 = new wxStaticText(this, wxID_ANY,
		_("Select instrument to add:"), wxDefaultPosition, wxDefaultSize, 0);
	itemBoxSizer01->Add(itemStaticText01, 0, wxEXPAND | wxALL, 5);

	wxImageList *imglist = new wxImageList(20, 20, true, 2);
	imglist->Add(*_img_instrument);
	imglist->Add(*_img_dial);

	m_pListCtrlInstruments = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(250, 180),
		wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING);
	itemBoxSizer01->Add(m_pListCtrlInstruments, 0, wxEXPAND | wxALL, 5);
	m_pListCtrlInstruments->AssignImageList(imglist, wxIMAGE_LIST_SMALL);
	m_pListCtrlInstruments->InsertColumn(0, _("Instruments"));
	wxStdDialogButtonSizer* DialogButtonSizer = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
	itemBoxSizer01->Add(DialogButtonSizer, 0, wxALIGN_RIGHT | wxALL, 5);

	for (unsigned int i = ID_DBP_I_POS; i < ID_DBP_LAST_ENTRY; i++) { //do not reference an instrument, but the last dummy entry in the list
		wxListItem item;
		if (IsObsolete(i)) continue;
		getListItemForInstrument(item, i);
		item.SetId(i);
		m_pListCtrlInstruments->InsertItem(item);
	}

	m_pListCtrlInstruments->SetColumnWidth(0, wxLIST_AUTOSIZE);
	m_pListCtrlInstruments->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	Fit();
}

unsigned int AddInstrumentDlg::GetInstrumentAdded()
{
	long itemID = -1;
	itemID = m_pListCtrlInstruments->GetNextItem(itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	return (int)m_pListCtrlInstruments->GetItemData(itemID);
}

//----------------------------------------------------------------
//
//    Tactics Window Implementation
//
//----------------------------------------------------------------

// wxWS_EX_VALIDATE_RECURSIVELY required to push events to parents
TacticsWindow::TacticsWindow(wxWindow *pparent, wxWindowID id, wxAuiManager *auimgr,
	tactics_pi* plugin, int orient, TacticsWindowContainer* mycont) :
	wxWindow(pparent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE,
	_T("Tactics"))
{
	m_pauimgr = auimgr;
	m_plugin = plugin;
	m_Container = mycont;

	//wx2.9      itemBoxSizer = new wxWrapSizer( orient );
	itemBoxSizer = new wxBoxSizer(orient);
	SetSizer(itemBoxSizer);
	Connect(wxEVT_SIZE, wxSizeEventHandler(TacticsWindow::OnSize), NULL, this);
	Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(TacticsWindow::OnContextMenu), NULL,
		this);
	Connect(wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(TacticsWindow::OnContextMenuSelect), NULL, this);
}

TacticsWindow::~TacticsWindow()
{
	for (size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++) {
		TacticsInstrumentContainer *pdic = m_ArrayOfInstrument.Item(i);
		delete pdic;
	}
}

void TacticsWindow::OnSize(wxSizeEvent& event)
{
	event.Skip();
	for (unsigned int i = 0; i<m_ArrayOfInstrument.size(); i++) {
		TacticsInstrument* inst = m_ArrayOfInstrument.Item(i)->m_pInstrument;
		inst->SetMinSize(inst->GetSize(itemBoxSizer->GetOrientation(), GetClientSize()));
	}
	Layout();
	Refresh();
}

void TacticsWindow::OnContextMenu(wxContextMenuEvent& event)
{
	wxMenu* contextMenu = new wxMenu();

	wxMenuItem* btnVertical = contextMenu->AppendRadioItem(ID_DASH_VERTICAL, _("Vertical"));
	btnVertical->Check(itemBoxSizer->GetOrientation() == wxVERTICAL);
	wxMenuItem* btnHorizontal = contextMenu->AppendRadioItem(ID_DASH_HORIZONTAL, _("Horizontal"));
	btnHorizontal->Check(itemBoxSizer->GetOrientation() == wxHORIZONTAL);
	contextMenu->AppendSeparator();

	m_plugin->PopulateContextMenu(contextMenu);

	contextMenu->AppendSeparator();
	contextMenu->Append(ID_DASH_PREFS, _("Preferences..."));

	wxMenuItem* btnShowLaylines = contextMenu->AppendCheckItem(ID_DASH_LAYLINE, _("Show Laylines"));//TR
	btnShowLaylines->Check(m_plugin->GetLaylineVisibility(this));

	wxMenuItem* btnShowCurrent = contextMenu->AppendCheckItem(ID_DASH_CURRENT, _("Show Current"));//TR
	btnShowCurrent->Check(m_plugin->GetCurrentVisibility(this));

	wxMenuItem* btnShowWindbarb = contextMenu->AppendCheckItem(ID_DASH_WINDBARB, _("Show Windbarb"));//TR
	btnShowWindbarb->Check(m_plugin->GetWindbarbVisibility(this));

	wxMenuItem* btnShowPolar = contextMenu->AppendCheckItem(ID_DASH_POLAR, _("Show Polar"));//TR
	btnShowPolar->Check(m_plugin->GetPolarVisibility(this));

	PopupMenu(contextMenu);
	delete contextMenu;
}

void TacticsWindow::OnContextMenuSelect(wxCommandEvent& event)
{
	if (event.GetId() < ID_DASH_PREFS) { // Toggle tactics visibility
		m_plugin->ShowTactics(event.GetId() - 1, event.IsChecked());
		if (m_plugin)
			SetToolbarItemState(m_plugin->GetToolbarItemId(), m_plugin->GetTacticsWindowShownCount() != 0);
	}

	switch (event.GetId()){
	case ID_DASH_PREFS: {
		m_plugin->ShowPreferencesDialog(this);
		return; // Does it's own save.
	}
	case ID_DASH_VERTICAL: {
		ChangePaneOrientation(wxVERTICAL, true);
		m_Container->m_sOrientation = _T("V");
		break;
	}
	case ID_DASH_HORIZONTAL: {
		ChangePaneOrientation(wxHORIZONTAL, true);
		m_Container->m_sOrientation = _T("H");
		break;
	}
	case ID_DASH_LAYLINE: {//TR
		m_plugin->ToggleLaylineRender(this);
		return; // Does it's own save.
	}
	case ID_DASH_CURRENT: {//TR
		m_plugin->ToggleCurrentRender(this);
		return; // Does it's own save.
	}
	case ID_DASH_POLAR: {//TR
		m_plugin->TogglePolarRender(this);
		return; // Does it's own save.
	}
	case ID_DASH_WINDBARB: {//TR
		m_plugin->ToggleWindbarbRender(this);
		return; // Does it's own save.
	}

	}
	m_plugin->SaveConfig();
}

void TacticsWindow::SetColorScheme(PI_ColorScheme cs)
{
	DimeWindow(this);
	Refresh(false);
}

void TacticsWindow::ChangePaneOrientation(int orient, bool updateAUImgr)
{
	m_pauimgr->DetachPane(this);
	SetSizerOrientation(orient);
	bool vertical = orient == wxVERTICAL;
	//wxSize sz = GetSize( orient, wxDefaultSize );
	wxSize sz = GetMinSize();
	// We must change Name to reset AUI perpective
	m_Container->m_sName = GetUUID();
	m_pauimgr->AddPane(this, wxAuiPaneInfo().Name(m_Container->m_sName).Caption(
		m_Container->m_sCaption).CaptionVisible(true).TopDockable(!vertical).BottomDockable(
		!vertical).LeftDockable(vertical).RightDockable(vertical).MinSize(sz).BestSize(
		sz).FloatingSize(sz).FloatingPosition(100, 100).Float().Show(m_Container->m_bIsVisible));
	if (updateAUImgr) m_pauimgr->Update();
}

void TacticsWindow::SetSizerOrientation(int orient)
{
	itemBoxSizer->SetOrientation(orient);
	/* We must reset all MinSize to ensure we start with new default */
	wxWindowListNode* node = GetChildren().GetFirst();
	while (node) {
		node->GetData()->SetMinSize(wxDefaultSize);
		node = node->GetNext();
	}
	SetMinSize(wxDefaultSize);
	Fit();
	SetMinSize(itemBoxSizer->GetMinSize());
}

int TacticsWindow::GetSizerOrientation()
{
	return itemBoxSizer->GetOrientation();
}

bool isArrayIntEqual(const wxArrayInt& l1, const wxArrayOfInstrument &l2)
{
	if (l1.GetCount() != l2.GetCount()) return false;

	for (size_t i = 0; i < l1.GetCount(); i++)
		if (l1.Item(i) != l2.Item(i)->m_ID) return false;

	return true;
}

bool TacticsWindow::isInstrumentListEqual(const wxArrayInt& list)
{
	return isArrayIntEqual(list, m_ArrayOfInstrument);
}

void TacticsWindow::SetInstrumentList(wxArrayInt list)
{
	/* options
	ID_DBP_D_SOG: config max value, show STW optional
	ID_DBP_D_COG:  +SOG +HDG? +BRG?
	ID_DBP_D_AWS: config max value. Two arrows for AWS+TWS?
	ID_DBP_D_VMG: config max value
	ID_DBP_I_DPT: config unit (meter, feet, fathoms)
	ID_DBP_D_DPT: show temp optional
	// compass: use COG or HDG
	// velocity range
	// rudder range

	*/
	m_ArrayOfInstrument.Clear();
	itemBoxSizer->Clear(true);
	for (size_t i = 0; i < list.GetCount(); i++) {
		int id = list.Item(i);
		TacticsInstrument *instrument = NULL;
		switch (id){
		case ID_DBP_I_POS:
			instrument = new TacticsInstrument_Position(this, wxID_ANY,
				getInstrumentCaption(id));
			break;
		case ID_DBP_I_SOG:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_SOG, _T("%5.2f"));
			break;
		case ID_DBP_D_SOG:
			instrument = new TacticsInstrument_Speedometer(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_SOG, 0, g_iDashSpeedMax);
			((TacticsInstrument_Dial *)instrument)->SetOptionLabel(g_iDashSpeedMax / 20 + 1,
				DIAL_LABEL_HORIZONTAL);
			//(TacticsInstrument_Dial *)instrument->SetOptionMarker(0.1, DIAL_MARKER_SIMPLE, 5);
			((TacticsInstrument_Dial *)instrument)->SetOptionMarker(0.5,
				DIAL_MARKER_SIMPLE, 2);
			((TacticsInstrument_Dial *)instrument)->SetOptionExtraValue(
				OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT);
			break;
		case ID_DBP_I_COG:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_COG, _T("%.0f"));
			break;
		case ID_DBP_M_COG:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_MCOG, _T("%.0f"));
			break;
		case ID_DBP_D_COG:
			instrument = new TacticsInstrument_Compass(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_COG);
			((TacticsInstrument_Dial *)instrument)->SetOptionMarker(5,
				DIAL_MARKER_SIMPLE, 2);
			((TacticsInstrument_Dial *)instrument)->SetOptionLabel(30,
				DIAL_LABEL_ROTATED);
			((TacticsInstrument_Dial *)instrument)->SetOptionExtraValue(
				OCPN_DBP_STC_SOG, _T("SOG\n%.2f"), DIAL_POSITION_BOTTOMLEFT);
			break;
		case ID_DBP_D_HDT:
			instrument = new TacticsInstrument_Compass(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_HDT);
			((TacticsInstrument_Dial *)instrument)->SetOptionMarker(5,
				DIAL_MARKER_SIMPLE, 2);
			((TacticsInstrument_Dial *)instrument)->SetOptionLabel(30,
				DIAL_LABEL_ROTATED);
			((TacticsInstrument_Dial *)instrument)->SetOptionExtraValue(
				OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT);
			break;
		case ID_DBP_I_STW:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_STW, _T("%.2f"));
			break;
		case ID_DBP_I_HDT: //true heading
			// TODO: Option True or Magnetic
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_HDT, _T("%.0f"));
			break;
		case ID_DBP_I_HDM:  //magnetic heading
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_HDM, _T("%.0f"));
			break;
		case ID_DBP_D_AW:
		case ID_DBP_D_AWA:
			instrument = new TacticsInstrument_Wind(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_AWA);
			((TacticsInstrument_Dial *)instrument)->SetOptionMainValue(_T("%.0f"),
				DIAL_POSITION_BOTTOMLEFT);
			((TacticsInstrument_Dial *)instrument)->SetOptionExtraValue(
				OCPN_DBP_STC_AWS, _T("%.1f"), DIAL_POSITION_INSIDE);
			break;
		case ID_DBP_I_AWS:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_AWS, _T("%.2f"));
			break;
		case ID_DBP_D_AWS:
			instrument = new TacticsInstrument_Speedometer(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_AWS, 0, 45);
			((TacticsInstrument_Dial *)instrument)->SetOptionLabel(5,
				DIAL_LABEL_HORIZONTAL);
			((TacticsInstrument_Dial *)instrument)->SetOptionMarker(1,
				DIAL_MARKER_SIMPLE, 5);
			((TacticsInstrument_Dial *)instrument)->SetOptionMainValue(_T("A %.2f"),
				DIAL_POSITION_BOTTOMLEFT);
			((TacticsInstrument_Dial *)instrument)->SetOptionExtraValue(
				OCPN_DBP_STC_TWS, _T("T %.1f"), DIAL_POSITION_BOTTOMRIGHT);
			break;
		case ID_DBP_D_TW: //True Wind angle +-180° on boat axis
			instrument = new TacticsInstrument_TrueWindAngle(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_TWA);
			((TacticsInstrument_Dial *)instrument)->SetOptionMainValue(_T("%.0f"),
				DIAL_POSITION_BOTTOMLEFT);
			((TacticsInstrument_Dial *)instrument)->SetOptionExtraValue(
				OCPN_DBP_STC_TWS, _T("%.1f"), DIAL_POSITION_INSIDE);
			break;
		case ID_DBP_D_AWA_TWA: //App/True Wind angle +-180° on boat axis
			instrument = new TacticsInstrument_AppTrueWindAngle(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_AWA | OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWD);
			((TacticsInstrument_Dial *)instrument)->SetOptionMainValue(_T("%.0f"),
				DIAL_POSITION_NONE);
			((TacticsInstrument_Dial *)instrument)->SetOptionExtraValue(
				OCPN_DBP_STC_TWS | OCPN_DBP_STC_AWS, _T("%.1f"), DIAL_POSITION_NONE);
			break;
		case ID_DBP_D_TWD: //True Wind direction
			instrument = new TacticsInstrument_WindCompass(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_TWD);
			((TacticsInstrument_Dial *)instrument)->SetOptionMainValue(_T("%.0f"),
				DIAL_POSITION_BOTTOMLEFT);
			((TacticsInstrument_Dial *)instrument)->SetOptionExtraValue(
				OCPN_DBP_STC_TWS, _T("%.1f"), DIAL_POSITION_INSIDE);
			//TR			        OCPN_DBP_STC_TWS2, _T("%.1f"), DIAL_POSITION_INSIDE );
			break;
		case ID_DBP_I_DPT:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_DPT, _T("%5.1f"));
			break;
		case ID_DBP_D_DPT:
			instrument = new TacticsInstrument_Depth(this, wxID_ANY,
				getInstrumentCaption(id));
			break;
		case ID_DBP_I_TMP: //water temperature
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_TMP, _T("%2.1f"));
			break;
		case ID_DBP_I_MDA: //barometric pressure
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_MDA, _T("%5.1f"));
			break;
		case ID_DBP_D_MDA: //barometric pressure
			instrument = new TacticsInstrument_Speedometer(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_MDA, 930, 1080);
			((TacticsInstrument_Dial *)instrument)->SetOptionLabel(10,
				DIAL_LABEL_HORIZONTAL);
			((TacticsInstrument_Dial *)instrument)->SetOptionMarker(5,
				DIAL_MARKER_SIMPLE, 1);
			((TacticsInstrument_Dial *)instrument)->SetOptionMainValue(_T("%5.1f"),
				DIAL_POSITION_INSIDE);
			break;
		case ID_DBP_I_ATMP: //air temperature
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_ATMP, _T("%2.1f"));
			break;
			/*case ID_DBP_I_VLW1: // Trip Log
			instrument = new TacticsInstrument_Single( this, wxID_ANY,
			getInstrumentCaption( id ), OCPN_DBP_STC_VLW1, _T("%2.1f") );
			break;
			case ID_DBP_I_VLW2: // Sum Log
			instrument = new TacticsInstrument_Single( this, wxID_ANY,
			getInstrumentCaption( id ), OCPN_DBP_STC_VLW2, _T("%2.1f") );
			break;*/
		case ID_DBP_I_TWA: //true wind angle
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_TWA, _T("%5.0f"));
			break;
		case ID_DBP_I_TWD: //true wind direction
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_TWD, _T("%5.0f"));
			break;
		case ID_DBP_I_TWS: // true wind speed
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_TWS, _T("%2.2f"));
			break;
		case ID_DBP_I_AWA: //apparent wind angle
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_AWA, _T("%5.0f"));
			break;
		case ID_DBP_I_VMG:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_VMG, _T("%5.2f"));
			break;
		case ID_DBP_D_VMG:
			instrument = new TacticsInstrument_Speedometer(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_VMG, 0, g_iDashSpeedMax);
			((TacticsInstrument_Dial *)instrument)->SetOptionLabel(1,
				DIAL_LABEL_HORIZONTAL);
			((TacticsInstrument_Dial *)instrument)->SetOptionMarker(0.5,
				DIAL_MARKER_SIMPLE, 2);
			((TacticsInstrument_Dial *)instrument)->SetOptionExtraValue(
				OCPN_DBP_STC_SOG, _T("SOG\n%.2f"), DIAL_POSITION_BOTTOMLEFT);
			break;
			//case ID_DBP_I_RSA:
			//instrument = new TacticsInstrument_Single( this, wxID_ANY,
			//        getInstrumentCaption( id ), OCPN_DBP_STC_RSA, _T("%5.0f") );
			//break;
			//case ID_DBP_D_RSA:
			//instrument = new TacticsInstrument_RudderAngle( this, wxID_ANY,
			//        getInstrumentCaption( id ) );
			//  break;
			//case ID_DBP_I_SAT:
			//instrument = new TacticsInstrument_Single( this, wxID_ANY,
			//        getInstrumentCaption( id ), OCPN_DBP_STC_SAT, _T("%5.0f") );
			//break;
			//case ID_DBP_D_GPS:
			//instrument = new TacticsInstrument_GPS( this, wxID_ANY,
			//        getInstrumentCaption( id ) );
			//  break;
			//case ID_DBP_I_PTR:
			// instrument = new TacticsInstrument_Position( this, wxID_ANY,
			//       getInstrumentCaption( id ), OCPN_DBP_STC_PLA, OCPN_DBP_STC_PLO );
			//break;
		case ID_DBP_I_CLK:
			instrument = new TacticsInstrument_Clock(this, wxID_ANY,
				getInstrumentCaption(id));
			break;
		case ID_DBP_I_SUN:
			instrument = new TacticsInstrument_Sun(this, wxID_ANY,
				getInstrumentCaption(id));
			break;
			/*case ID_DBP_D_MON:
			instrument = new TacticsInstrument_Moon( this, wxID_ANY,
			getInstrumentCaption( id ) );
			break;*/
		case ID_DBP_D_WDH:
			instrument = new TacticsInstrument_WindDirHistory(this, wxID_ANY,
				getInstrumentCaption(id));
			break;
		case ID_DBP_D_BPH:
			instrument = new TacticsInstrument_BaroHistory(this, wxID_ANY,
				getInstrumentCaption(id));
			break;
		case ID_DBP_I_FOS:
			instrument = new TacticsInstrument_FromOwnship(this, wxID_ANY,
				getInstrumentCaption(id));
			break;
		case ID_DBP_I_PITCH:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_PITCH, _T("%2.1f"));
			break;
		case ID_DBP_I_HEEL:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_HEEL, _T("%2.1f"));
			break;
		case ID_DBP_I_CURRDIR:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_CURRDIR, _T("%2.0f"));
			break;
		case ID_DBP_I_CURRSPD:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_CURRSPD, _T("%2.2f"));
			break;
		case ID_DBP_I_LEEWAY:
			instrument = new TacticsInstrument_Single(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_LEEWAY, _T("%2.1f"));
			break;
		case ID_DBP_D_BRG:  // Bearing Compass
			instrument = new TacticsInstrument_BearingCompass(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_COG | OCPN_DBP_STC_BRG | OCPN_DBP_STC_CURRDIR | OCPN_DBP_STC_CURRSPD
				| OCPN_DBP_STC_TWA | OCPN_DBP_STC_LEEWAY | OCPN_DBP_STC_HDT | OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON
				| OCPN_DBP_STC_STW | OCPN_DBP_STC_AWA | OCPN_DBP_STC_TWS | OCPN_DBP_STC_TWD);
			((TacticsInstrument_Dial *)instrument)->SetOptionMarker(5,
				DIAL_MARKER_SIMPLE, 2);
			((TacticsInstrument_Dial *)instrument)->SetOptionLabel(30,
				DIAL_LABEL_ROTATED);
			((TacticsInstrument_Dial *)instrument)->SetOptionExtraValue(
				OCPN_DBP_STC_DTW, _T("%.2f"), DIAL_POSITION_TOPLEFT);
			//				OCPN_DBP_STC_DTW | OCPN_DBP_STC_DCV, _T("%.2f"), DIAL_POSITION_TOPLEFT);
			break;
		case ID_DBP_D_POLCOMP: // Polar Compass
			instrument = new TacticsInstrument_PolarCompass(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_COG | OCPN_DBP_STC_BRG | OCPN_DBP_STC_CURRDIR | OCPN_DBP_STC_CURRSPD
				| OCPN_DBP_STC_TWA | OCPN_DBP_STC_LEEWAY | OCPN_DBP_STC_HDT | OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON
				| OCPN_DBP_STC_STW | OCPN_DBP_STC_AWA | OCPN_DBP_STC_TWS | OCPN_DBP_STC_TWD);
			((TacticsInstrument_Dial *)instrument)->SetOptionMarker(5,
				DIAL_MARKER_SIMPLE, 2);
			((TacticsInstrument_Dial *)instrument)->SetOptionLabel(30,
				DIAL_LABEL_ROTATED);
			((TacticsInstrument_Dial *)instrument)->SetOptionExtraValue(
				OCPN_DBP_STC_DTW, _T("%.2f"), DIAL_POSITION_TOPLEFT);
			//				OCPN_DBP_STC_DTW | OCPN_DBP_STC_DCV, _T("%.2f"), DIAL_POSITION_TOPLEFT);
			break;
		case ID_DBP_I_TWAMARK:
			instrument = new TacticsInstrument_PerformanceSingle(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_BRG | OCPN_DBP_STC_TWD | OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON, _T("%5.0f"));
			((TacticsInstrument_PerformanceSingle *)instrument)->SetDisplayType(TWAMARK);
			break;

		case ID_DBP_I_POLSPD:
			instrument = new TacticsInstrument_PerformanceSingle(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_STW | OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
			((TacticsInstrument_PerformanceSingle *)instrument)->SetDisplayType(POLARSPEED);
			break;
		case ID_DBP_I_POLVMG:
			instrument = new TacticsInstrument_PerformanceSingle(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_STW | OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
			((TacticsInstrument_PerformanceSingle *)instrument)->SetDisplayType(POLARVMG);
			break;
		case ID_DBP_I_POLTVMG:
			instrument = new TacticsInstrument_PerformanceSingle(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_STW | OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
			((TacticsInstrument_PerformanceSingle *)instrument)->SetDisplayType(POLARTARGETVMG);
			break;
		case ID_DBP_I_POLTVMGANGLE:
			instrument = new TacticsInstrument_PerformanceSingle(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_STW | OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS, _T("%.2f"));
			((TacticsInstrument_PerformanceSingle *)instrument)->SetDisplayType(POLARTARGETVMGANGLE);
			break;
		case ID_DBP_I_POLCMG:
			instrument = new TacticsInstrument_PerformanceSingle(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_STW | OCPN_DBP_STC_COG | OCPN_DBP_STC_SOG | OCPN_DBP_STC_BRG | OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON, _T("%.2f"));
			((TacticsInstrument_PerformanceSingle *)instrument)->SetDisplayType(POLARCMG);
			break;
		case ID_DBP_I_POLTCMG:
			instrument = new TacticsInstrument_PerformanceSingle(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_STW | OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS | OCPN_DBP_STC_HDT | OCPN_DBP_STC_BRG | OCPN_DBP_STC_TWD | OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON, _T("%.2f"));
			((TacticsInstrument_PerformanceSingle *)instrument)->SetDisplayType(POLARTARGETCMG);
			break;
		case ID_DBP_I_POLTCMGANGLE:
			instrument = new TacticsInstrument_PerformanceSingle(this, wxID_ANY,
				getInstrumentCaption(id), OCPN_DBP_STC_STW | OCPN_DBP_STC_TWA | OCPN_DBP_STC_TWS | OCPN_DBP_STC_HDT | OCPN_DBP_STC_BRG | OCPN_DBP_STC_TWD | OCPN_DBP_STC_LAT | OCPN_DBP_STC_LON, _T("%.2f"));
			((TacticsInstrument_PerformanceSingle *)instrument)->SetDisplayType(POLARTARGETCMGANGLE);
			break;
		case ID_DBP_D_POLPERF:
			instrument = new TacticsInstrument_PolarPerformance(this, wxID_ANY,
				getInstrumentCaption(id));
			break;
		case ID_DBP_D_AVGWIND:
			instrument = new TacticsInstrument_AvgWindDir(this, wxID_ANY,
				getInstrumentCaption(id));

		}
		if (instrument) {
			instrument->instrumentTypeId = id;
			m_ArrayOfInstrument.Add(
				new TacticsInstrumentContainer(id, instrument,
				instrument->GetCapacity()));
			itemBoxSizer->Add(instrument, 0, wxEXPAND, 0);
			if (itemBoxSizer->GetOrientation() == wxHORIZONTAL) {
				itemBoxSizer->AddSpacer(5);
			}
		}
	}
	Fit();
	Layout();
	SetMinSize(itemBoxSizer->GetMinSize());
}

void TacticsWindow::SendSentenceToAllInstruments(int st, double value, wxString unit)
{

	for (size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++) {
		if (m_ArrayOfInstrument.Item(i)->m_cap_flag & st) m_ArrayOfInstrument.Item(i)->m_pInstrument->SetData(
			st, value, unit);
	}
}

void TacticsWindow::SendSatInfoToAllInstruments(int cnt, int seq, SAT_INFO sats[4])
{
	/*for( size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++ ) {
	if( ( m_ArrayOfInstrument.Item( i )->m_cap_flag & OCPN_DBP_STC_GPS )
	&& m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf(
	CLASSINFO(TacticsInstrument_GPS)))
	((TacticsInstrument_GPS*)m_ArrayOfInstrument.Item(i)->m_pInstrument)->SetSatInfo(cnt, seq, sats);
	}*/
}


void TacticsWindow::SendUtcTimeToAllInstruments(wxDateTime value)
{
	for (size_t i = 0; i < m_ArrayOfInstrument.GetCount(); i++) {
		if ((m_ArrayOfInstrument.Item(i)->m_cap_flag & OCPN_DBP_STC_CLK)
			&& m_ArrayOfInstrument.Item(i)->m_pInstrument->IsKindOf(CLASSINFO(TacticsInstrument_Clock)))
			//                  || m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf( CLASSINFO( TacticsInstrument_Sun ) )
			//                  || m_ArrayOfInstrument.Item( i )->m_pInstrument->IsKindOf( CLASSINFO( TacticsInstrument_Moon ) ) ) )
			((TacticsInstrument_Clock*)m_ArrayOfInstrument.Item(i)->m_pInstrument)->SetUtcTime(value);
	}
}
/*********************************************************************************
set system variables
**********************************************************************************/
void tactics_pi::SetCalcVariables(int st, double value, wxString unit)
{
	switch (st) {
	case OCPN_DBP_STC_AWA:
		mAWA = value;
		mAWAUnit = unit;
		break;
	case OCPN_DBP_STC_AWS:
		mAWS = value;
		mAWSUnit = unit;
		break;
	case  OCPN_DBP_STC_TWA:
		if (g_bForceTrueWindCalculation && !wxIsNaN(m_calcTWA)){ //otherwise we distribute the original O TWA
			mTWA = m_calcTWA;
		}
		else
			mTWA = value;
		break;
	case  OCPN_DBP_STC_TWS:
		if (g_bForceTrueWindCalculation && !wxIsNaN(m_calcTWS)){ //otherwise we distribute the original O TWS
			mTWS = m_calcTWS;
		}
		else
			mTWS = value;
		break;
	case  OCPN_DBP_STC_TWD:
		if (g_bForceTrueWindCalculation && !wxIsNaN(m_calcTWD)){ //otherwise we distribute the original O TWD
			mTWD = m_calcTWD;
		}
		else
			mTWD = value;
		break;
	case OCPN_DBP_STC_STW:
		mStW = value;
		break;
	case OCPN_DBP_STC_HDT:
		mHdt = value;
		break;
	case OCPN_DBP_STC_HEEL:
		if (g_bUseHeelSensor){
			mheel = value;
			mHeelUnit = unit;
		}
		msensorheel = value; //TR TEMP for testing !
		break;
	case OCPN_DBP_STC_COG:
		mCOG = value;
		break;
	case OCPN_DBP_STC_SOG:
		mSOG = value;
		break;
	case OCPN_DBP_STC_LAT:
		mlat = value;
		break;
	case OCPN_DBP_STC_LON:
		mlon = value;
		break;
	case OCPN_DBP_STC_CURRDIR:
		m_CurrentDirection = value;
		break;
	case OCPN_DBP_STC_CURRSPD:
		m_ExpSmoothCurrSpd = value;
		break;
	case OCPN_DBP_STC_BRG:
		mBRG = value;
		break;
	default:
		break;
	}
	if (g_bManHeelInput){
		mHeelUnit = (mAWAUnit == _T("\u00B0L")) ? _T("\u00B0r") : _T("\u00B0l");
		g_dheel[0][0] = g_dheel[1][0] = g_dheel[2][0] = g_dheel[3][0] = g_dheel[4][0] = g_dheel[5][0] = g_dheel[0][1] = g_dheel[0][2] = g_dheel[0][3] = g_dheel[0][4] = 0.0;
		if (wxIsNaN(mTWS)) mTWS = 0;
		if (wxIsNaN(mTWA)) mTWA = 0;
		int twsmin = (int)(mTWS / 5);
		int twsmax = twsmin + 1;
		int twamin = (int)(mTWA / 45);
		int twamax = twamin + 1;
		double tws1 = twsmin*5.;
		double tws2 = twsmax * 5.;
		double twa1 = twamin * 45.;
		double twa2 = twamax * 45.;


		double twsfact = (mTWS - tws1) / (tws2 - tws1);
		double twafact = (mTWA - twa1) / (twa2 - twa1);
		double heel1 = g_dheel[twsmin][twamin] + twsfact*(g_dheel[twsmax][twamin] - g_dheel[twsmin][twamin]);
		double heel2 = g_dheel[twsmin][twamax] + twsfact*(g_dheel[twsmax][twamax] - g_dheel[twsmin][twamax]);

		mheel = heel1 + twafact*(heel2 - heel1);
		if (mHeelUnit == _T("\u00B0l")) mheel = -mheel;
	}
    if (!wxIsNaN(mLeeway)){
      if (mLeeway >= -90 && mLeeway <= 90)
        m_LeewayOK = true;
    }
}

/*********************************************************************************
calculates TWA,TWS and TWD in case it is not available on the System
**********************************************************************************/
void tactics_pi::CalculateTrueWind(int st, double value, wxString unit)
{
	double spdval;

	if (st == OCPN_DBP_STC_TWA || st == OCPN_DBP_STC_TWS || st == OCPN_DBP_STC_TWD) {
		m_bTrueWind_available = true;
	}

    if (st == OCPN_DBP_STC_AWS && !wxIsNaN(mStW) && !wxIsNaN(mSOG)){
      //  Calculate TWS (from AWS and StW/SOG)
      spdval = (g_bUseSOGforTWCalc) ? mSOG : mStW ;
      // only start calculating if we have a full set of data
      if ((!m_bTrueWind_available || g_bForceTrueWindCalculation) && mAWA >= 0 && mAWS>=0  && spdval >= 0 && mAWAUnit != _("") && !wxIsNaN(mHdt)) {
        //we have to do the calculation in knots
        double aws_kts = fromUsrSpeed_Plugin(mAWS, g_iDashWindSpeedUnit);
        spdval = fromUsrSpeed_Plugin(spdval, g_iDashSpeedUnit);

        mTWA = 0;
        mTWD = 0.;
        if (mAWA < 180.) {
          mTWA = 90. - (180. / M_PI*atan((aws_kts*cos(mAWA*M_PI / 180.) - spdval) / (aws_kts*sin(mAWA*M_PI / 180.))));
        }
        else if (mAWA > 180.) {
          mTWA = 360. - (90. - (180. / M_PI*atan((aws_kts*cos((180. - (mAWA - 180.))*M_PI / 180.) - spdval) / (aws_kts*sin((180. - (mAWA - 180.))*M_PI / 180.)))));
        }
        else {
          mTWA = 180.;
        }
        mTWS = sqrt(pow((aws_kts*cos(mAWA*M_PI / 180.)) - spdval, 2) + pow(aws_kts*sin(mAWA*M_PI / 180.), 2));
      /* ToDo: adding leeway needs to be reviewed, as the direction of the bow is still based in the magnetic compass,
               no matter if leeway or not ...
      if (!wxIsNaN(mLeeway) && g_bUseHeelSensor) { //correct TWD with Leeway if heel is available. Makes only sense with heel sensor
        mTWD = (mAWAUnit == _T("\u00B0R")) ? mHdt + mTWA + mLeeway : mHdt - mTWA + mLeeway;
        }
        else*/
          mTWD = (mAWAUnit == _T("\u00B0R")) ? mHdt + mTWA : mHdt - mTWA;
        //endif
        if (mTWD >= 360) mTWD -= 360;
        if (mTWD < 0) mTWD += 360;
        //convert mTWS back to user wind speed settings
        mTWS = toUsrSpeed_Plugin(mTWS, g_iDashWindSpeedUnit);
        m_calcTWS = mTWS;
        m_calcTWD = mTWD;
        m_calcTWA = mTWA;
        if (mAWSUnit == _("")) mAWSUnit = mAWAUnit;
        //distribute data to all instruments
        for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
          TacticsWindow *tactics_window = m_ArrayOfTacticsWindow.Item(i)->m_pTacticsWindow;
          if (tactics_window){
            tactics_window->SendSentenceToAllInstruments(OCPN_DBP_STC_TWA, mTWA, mAWAUnit);
            tactics_window->SendSentenceToAllInstruments(OCPN_DBP_STC_TWS, mTWS, mAWSUnit);
            mTWS_Watchdog = gps_watchdog_timeout_ticks;
            tactics_window->SendSentenceToAllInstruments(OCPN_DBP_STC_TWD, mTWD, _T("\u00B0T"));
            mTWD_Watchdog = gps_watchdog_timeout_ticks;
          }
        }
      }
      else{
//        m_calcTWS = mTWS = NAN;
//        m_calcTWD = mTWD = NAN;
        //m_calcTWA = mTWA = NAN;
        m_calcTWS = NAN;
        m_calcTWD = NAN;
        m_calcTWA = NAN;

      }
    }
}
/*********************************************************************************
Calculate Leeway from heel
**********************************************************************************/
void tactics_pi::CalculateLeeway(int st, double value, wxString unit)
{

	if (g_bUseFixedLeeway){
		mHeelUnit = (mAWAUnit == _T("\u00B0L")) ? _T("\u00B0r") : _T("\u00B0l");
		mLeeway = g_dfixedLeeway;
		if (wxIsNaN(mheel)) mheel = 0;

		if (mHeelUnit == _T("\u00B0l") && mLeeway > 0) mLeeway = -mLeeway;
		if (mHeelUnit == _T("\u00B0r") && mLeeway < 0) mLeeway = -mLeeway;
	}

	else {//g_bUseHeelSensor or g_bManHeelInput

		// only start calculating if we have a full set of data
		if (!wxIsNaN(mheel) && !wxIsNaN(mStW)) {
			double stw_kts = fromUsrSpeed_Plugin(mStW, g_iDashSpeedUnit);

			// calculate Leeway based on Heel
			if (mheel == 0)
				mLeeway = 0;
			else if (mStW == 0)
				mLeeway = g_dfixedLeeway;
			else
				mLeeway = (g_dLeewayFactor*mheel) / (stw_kts*stw_kts);
			if (mLeeway > g_dfixedLeeway) mLeeway = g_dfixedLeeway;
			if (mLeeway < -g_dfixedLeeway) mLeeway = -g_dfixedLeeway; //22.04TR : auf neg. Werte prüfen !!!
			mHeelUnit = (mheel < 0) ? _T("\u00B0l") : _T("\u00B0r");
		}
	}

	//distribute data to all instruments
	for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
		TacticsWindow *tactics_window = m_ArrayOfTacticsWindow.Item(i)->m_pTacticsWindow;
		if (tactics_window)	tactics_window->SendSentenceToAllInstruments(OCPN_DBP_STC_LEEWAY, mLeeway, mHeelUnit);
	}
}


/*********************************************************************************
Calculate Current with DES
using COG/SOG-Vector + HDT/STW-Vector
from the heel sensor we get the boat drift due to heel = Leeway
the whole drift angle btw. COG and HDT is a mixture of Leeway  + current
first we apply leeway to HDT, and get CRS = Course through water
The remaining diff btw. CRS & COG is current
based on actual position, we calculate lat/lon of the endpoints of both vectors  (COG/HDT = direction, SOG/STW = length)
then we take lat/lon from the endpoints of these vectors and calculate current between them with length (=speed) and direction
return the endpoint of SOG,COG

ToDo :
we're already calculating in "user speed data", but the exp. smoothing is done on the value itself.
On unit-change, the smoothed data values need to be converted from "old" to "new"
**********************************************************************************/
void tactics_pi::CalculateCurrent(int st, double value, wxString unit)
{

	//don't calculate on ALL incoming sentences ...
	if (st == OCPN_DBP_STC_HDT) {

		// ... and only start calculating if we have a full set of data
      if (!wxIsNaN(mheel) && m_LeewayOK && !wxIsNaN(mCOG) && !wxIsNaN(mSOG) && !wxIsNaN(mStW) && !wxIsNaN(mHdt) && !wxIsNaN(mlat) && !wxIsNaN(mlon) && !wxIsNaN(mLeeway)) {

			double COGlon, COGlat;
			//we have to do the calculation in knots ...
			double sog_kts, stw_kts;
			sog_kts = fromUsrSpeed_Plugin(mSOG, g_iDashSpeedUnit);
			stw_kts = fromUsrSpeed_Plugin(mStW, g_iDashSpeedUnit);
			//calculate endpoint of COG/SOG
			PositionBearingDistanceMercator_Plugin(mlat, mlon, mCOG, sog_kts, &COGlat, &COGlon);

			//------------------------------------ 
			//correct HDT with Leeway
			//------------------------------------

			//         ^ Hdt, STW
			// Wind   /
			// -->   /        Leeway
			//      /
			//     /----------> CRS, STW (stw_corr)
			//     \
			      //      \        Current
			//       \ COG,SOG
			//        V        
			// if wind is from port, heel & mLeeway will be positive (to starboard), adding degrees on the compass rose
			//  CRS = Hdt + Leeway
			//  if wind is from starboard, heel/mLeeway are negative (to port), mLeeway has to be substracted from Hdt
			//   As mLeeway is a signed double, so we can generally define : CRS = Hdt + mLeeway 
			double CourseThroughWater = mHdt + mLeeway;
			if (CourseThroughWater >= 360) CourseThroughWater -= 360;
			if (CourseThroughWater < 0) CourseThroughWater += 360;
			double CRSlat, CRSlon;
			//calculate endpoint of StW/KdW;
			double stw_corr;
			//correct only if not already done before via preference setting
			if (g_bCorrectSTWwithLeeway == true && g_bUseHeelSensor && !wxIsNaN(mLeeway) && !wxIsNaN(mheel)) //in this case STW is already corrected !!!
				stw_corr = stw_kts;
			else
				stw_corr = stw_kts / cos(mLeeway *M_PI / 180.0); //we have to correct StW for CRS as well.
			PositionBearingDistanceMercator_Plugin(mlat, mlon, CourseThroughWater, stw_corr, &CRSlat, &CRSlon);

			//calculate the Current vector with brg & speed from the 2 endpoints above
			double currdir = 0, currspd = 0;
			//currdir = local_bearing(StWlat, StWlon, COGlat, COGlon );
			//currspd = local_distance(COGlat, COGlon, StWlat, StWlon);
			DistanceBearingMercator_Plugin(COGlat, COGlon, CRSlat, CRSlon, &currdir, &currspd);
			// double exponential smoothing on currdir / currspd
			if (currspd < 0) currspd = 0;
			if (wxIsNaN(m_ExpSmoothCurrSpd))
				m_ExpSmoothCurrSpd = currspd;
			if (wxIsNaN(m_ExpSmoothCurrDir))
				m_ExpSmoothCurrDir = currdir;

			double currdir_tan = currdir;
			mExpSmoothCurrSpd->SetAlpha(alpha_currspd);
			m_ExpSmoothCurrSpd = mExpSmoothCurrSpd->GetSmoothVal(currspd);

			double rad = (90 - currdir_tan)*M_PI / 180.;
			mSinCurrDir->SetAlpha(g_dalpha_currdir);
			mCosCurrDir->SetAlpha(g_dalpha_currdir);
			m_ExpSmoothSinCurrDir = mSinCurrDir->GetSmoothVal(sin(rad));
			m_ExpSmoothCosCurrDir = mCosCurrDir->GetSmoothVal(cos(rad));
			m_CurrentDirection = (90. - (atan2(m_ExpSmoothSinCurrDir, m_ExpSmoothCosCurrDir)*180. / M_PI) + 360.);
			while (m_CurrentDirection >= 360) m_CurrentDirection -= 360;
			// temporary output of Currdir to file ...
			//str = wxString::Format(_T("%.2f;%.2f\n"), currdir, m_CurrentDirection);
			//out.WriteString(str);
		}
		else{
			m_CurrentDirection = NAN;
			m_ExpSmoothCurrSpd = NAN;
		}
		//distribute data to all instruments
		for (size_t i = 0; i < m_ArrayOfTacticsWindow.GetCount(); i++) {
			TacticsWindow *tactics_window = m_ArrayOfTacticsWindow.Item(i)->m_pTacticsWindow;
			if (tactics_window){
				tactics_window->SendSentenceToAllInstruments(OCPN_DBP_STC_CURRDIR, m_CurrentDirection, _T("\u00B0"));
				tactics_window->SendSentenceToAllInstruments(OCPN_DBP_STC_CURRSPD, toUsrSpeed_Plugin(m_ExpSmoothCurrSpd, g_iDashSpeedUnit), getUsrSpeedUnit_Plugin(g_iDashSpeedUnit));
			}
		}
		// }
	}
}


/*********************************************************************************
Set MARK Position
**********************************************************************************/
void tactics_pi::OnContextMenuItemCallback(int id)
{
	m_pMark = new PlugIn_Waypoint(g_dcur_lat, g_dcur_lon, _T("circle"), _T("Tactics temp. WP"), _T("TacticsWP"));
	g_dmark_lat = m_pMark->m_lat;
	g_dmark_lon = m_pMark->m_lon;
	AddSingleWaypoint(m_pMark, false);
	m_pMark->m_CreateTime = wxDateTime::Now();
}

/*********************************************************************************
Central routine to calculate the polar based performance data, independent of the
use of any instrument or setting
**********************************************************************************/
void tactics_pi::CalculatePerformanceData(void)
{
	if (wxIsNaN(mTWA) || wxIsNaN(mTWS)) {
		return;
	}

	mPolarTargetSpeed = BoatPolar->GetPolarSpeed(mTWA, mTWS);
	//transfer targetangle dependent on AWA, not TWA
	if (mAWA <= 90)
		tvmg = BoatPolar->Calc_TargetVMG(60, mTWS);
	else
		tvmg = BoatPolar->Calc_TargetVMG(120, mTWS);

	// get Target VMG Angle from Polar
	//tvmg = BoatPolar->Calc_TargetVMG(mTWA, mTWS);
    if (tvmg.TargetSpeed > 0 && !wxIsNaN(mStW)) {
		double VMG = BoatPolar->Calc_VMG(mTWA, mStW);
		mPercentTargetVMGupwind = mPercentTargetVMGdownwind = 0;
		if (mTWA < 90){
			mPercentTargetVMGupwind = fabs(VMG / tvmg.TargetSpeed * 100.);
		}
		if (mTWA > 90){
			mPercentTargetVMGdownwind = fabs(VMG / tvmg.TargetSpeed * 100.);
		}
		//mVMGGain = 100.0 - mStW/tvmg.TargetSpeed  * 100.;
		mVMGGain = 100.0 - VMG / tvmg.TargetSpeed  * 100.;
	}
	else
	{
		mPercentTargetVMGupwind = mPercentTargetVMGdownwind = 0;
		mVMGGain = 0;
	}
	if (tvmg.TargetAngle >= 0 && tvmg.TargetAngle < 360) {
		mVMGoptAngle = getSignedDegRange(mTWA, tvmg.TargetAngle);
	}
	else
		mVMGoptAngle = 0;

    if (mBRG >= 0 && !wxIsNaN(mHdt) && !wxIsNaN(mStW) && !wxIsNaN(mTWD)){
		tcmg = BoatPolar->Calc_TargetCMG(mTWS, mTWD, mBRG);
		double actcmg = BoatPolar->Calc_CMG(mHdt, mStW, mBRG);
		// mCMGGain = (tcmg.TargetSpeed >0) ? (100.0 - mStW / tcmg.TargetSpeed *100.) : 0.0;
		mCMGGain = (tcmg.TargetSpeed >0) ? (100.0 - actcmg / tcmg.TargetSpeed *100.) : 0.0;
		if (tcmg.TargetAngle >= 0 && tcmg.TargetAngle < 360) {
			mCMGoptAngle = getSignedDegRange(mTWA, tcmg.TargetAngle);
		}
		else
			mCMGoptAngle = 0;

	}
	CalculatePredictedCourse();
}
/*********************************************************************************
First shot of an export routine for the NMEA $PNKEP (NKE style) performance data
**********************************************************************************/
void tactics_pi::ExportPerformanceData(void)
{
	//PolarTargetSpeed
	if (g_bExpPerfData01 && !wxIsNaN(mPolarTargetSpeed)){
		createPNKEP_NMEA(1, mPolarTargetSpeed, mPolarTargetSpeed  * 1.852, 0, 0);
	}
	//todo : extract mPredictedCoG calculation from layline.calc and add to CalculatePerformanceData
	if (g_bExpPerfData02 && !wxIsNaN(mPredictedCoG)){
		createPNKEP_NMEA(2, mPredictedCoG, 0, 0, 0); // course (CoG) on other tack
	}
	//Target VMG angle, act. VMG % upwind, act. VMG % downwind
	if (g_bExpPerfData03 && !wxIsNaN(tvmg.TargetAngle) && tvmg.TargetSpeed > 0){
		createPNKEP_NMEA(3, tvmg.TargetAngle, mPercentTargetVMGupwind, mPercentTargetVMGdownwind, 0);
	}
	//Gain VMG de 0 à 999%, Angle pour optimiser le VMG de 0 à 359°,Gain CMG de 0 à 999%,Angle pour optimiser le CMG de 0 à 359°
	if (g_bExpPerfData04)
		createPNKEP_NMEA(4, mCMGoptAngle, mCMGGain, mVMGoptAngle, mVMGGain);
	//current direction, current speed kts, current speed in km/h,
	if (g_bExpPerfData05 && !wxIsNaN(m_CurrentDirection) && !wxIsNaN(m_ExpSmoothCurrSpd)){
		createPNKEP_NMEA(5, m_CurrentDirection, m_ExpSmoothCurrSpd, m_ExpSmoothCurrSpd  * 1.852, 0);
	}
}
/************************************************************************************
Calculate the checksum of the created NMEA record.
Taken from nmeaconverter_pi.
All credits to Pavel !
************************************************************************************/
wxString tactics_pi::ComputeChecksum(wxString sentence)
{
	unsigned char calculated_checksum = 0;
	for (wxString::const_iterator i = sentence.begin() + 1; i != sentence.end() && *i != '*'; ++i)
		calculated_checksum ^= static_cast<unsigned char> (*i);

	return(wxString::Format(wxT("%02X"), calculated_checksum));
}
/************************************************************************************
Put the created NMEA record ot O's NMEA data stream
Taken from nmeaconverter_pi.
All credits to Pavel !
************************************************************************************/
void tactics_pi::SendNMEASentence(wxString sentence)
{
	wxString Checksum = ComputeChecksum(sentence);
	sentence = sentence.Append(wxT("*"));
	sentence = sentence.Append(Checksum);
	sentence = sentence.Append(wxT("\r\n"));
	//wxLogMessage(sentence);
	PushNMEABuffer(sentence);
}
/************************************************************************************
Creation of the specific NKE style  performance data NMEA-records.
These records are visible in the NKE instruments !
You need to define an outbound interface and filter for $PNKEP
-----------------------
Speed and performance target (code PNKEP01)
By definition of NKE this is the theoretically best VMG (Target-VMG),
I implemented "Polar speed" here !, as this also works on crosswind courses
And Target-VMG % is available in $PNKEP03 and $PNKEP04
The Channel in the NKE instruments is called "Target Speed"
$PNKEP,01,x.x,N,x.x,K*hh<CR><LF>
|      \ target speed in km/h
\ target speed in knots
course on next tack (code PNKEP02)
$PNKEP,02,x.x*hh<CR><LF>
\ Cap sur bord Opposé/prochain bord de 0 à 359°
Opt. VMG angle and performance up and downwind   (code PNKEP03)
$PNKEP,03,x.x,x.x,x.x*hh<CR><LF>
|   |   \ performance downwind from 0 to 99%
|   \ performance upwind from 0 to 99%
\ opt. VMG angle  0 à 359°
Angles pour optimiser le CMG et VMG et gain correspondant (code PNKEP04)
$PNKEP,04,x.x,x.x,x.x,x.x*hh<CR><LF>
|   |   |   \ Gain VMG de 0 à 999%
|   |   \ Angle pour optimiser le VMG de 0 à 359°
|   \ Gain CMG de 0 à 999%
\ Angle pour optimiser le CMG de 0 à 359°
Direction and speed of sea current (code PNKEP05)
$PNKEP,05,x.x,x.x,N,x.x,K*hh<CR><LF>
|   |     \ current speed in km/h
|   \ current speed in knots
\ current direction from 0 à 359°
-------------------------------------
Found in the documentation of the normal heel sensor ...
I bet we could also upload acceleration and gyrometer data ;-)

Les mesures de ses capteurs (accéléromètre et gyromètre) sont aussi envoyées sur sa
sortie NMEA, à l’aide d’une trame propriétaire nke.
$PNKEP,06,-0.988,-0.096,12.0*5D
-------------------------------------

Detail of the NMEA Interface NKE/NMEA this single sentence frames has been
created for the purposes of the Topline bus with Adrena software compatibility:
$PNKEA,AA,N,FF,XXXXX,TTTTTTTTTT,UUUUUUU *hh<CR><LF>
* AA : Code corresponding to the channel data displayed, consisting of 2
alphanumeric characters. These 2 letters allow a view of the data on the old displays,
able to display simple dynamic channels.
* N : number display on the dynamic page order. 1 To 4 for the main channels and 5
to 8 to display alternating.
* FF : digital code corresponding to the display format.
* XXXXX raw digital data between 0 and 32767.
* TTTTTTTTTT: label of the title (10 characters)
* UUUUUUU: label of the units (7 characters)
Remarques :
* If one of the fields is empty the data will not be published
* Labels can contain uppercase letters and or lower case, but on non-graphical
displays (type TL25), they will be displayed in uppercase.
Example of sentences (the checksum is not calculated in these examples):
$PNKEA,TL,1,11,135,Tps Ligne,Min Sec*hh
$PNKEA,DL,5,40,111,Dist Ligne,Long.,*hh
$PNKEA,PV,2,34,84,Pcent VMG,Bord 1,*hh
$PNKEA,TB,3,11,201,Tps Bouee,Min Sec,*hh
$PNKEA,DB,7,40,60,Dist Bouee,Long,*hh
$PNKEA,TR,4,5,40,Ang Target,Pres,*hh
$PNKEA,TO,8,5,205,Ang Target,Portant,*hh
*************************************************************************************/
void tactics_pi::createPNKEP_NMEA(int sentence, double data1, double data2, double data3, double data4)
{
	wxString nmeastr = "";
	switch (sentence)
	{
	case 0:
		//strcpy(nmeastr, "$PNKEPA,");
		break;
	case 1:
		nmeastr = _T("$PNKEP,01,") + wxString::Format("%.2f,N,", data1) + wxString::Format("%.2f,K", data2);
		break;
	case 2:
		/*course on next tack(code PNKEP02)
		$PNKEP, 02, x.x*hh<CR><LF>
		\ Cap sur bord Opposé / prochain bord de 0 à 359°*/
		nmeastr = _T("$PNKEP,02,") + wxString::Format("%.1f", data1);
		break;
	case 3:
		/*    $PNKEP, 03, x.x, x.x, x.x*hh<CR><LF>
		|    |     \ performance downwind from 0 to 99 %
		|     \ performance upwind from 0 to 99 %
		\ opt.VMG angle  0 à 359°  */
		nmeastr = _T("$PNKEP,03,") + wxString::Format("%.1f,", data1) + wxString::Format("%.1f,", data2) + wxString::Format("%.1f", data3);
		break;
	case 4:
		/*Calculates the gain for VMG & CMG and stores it in the variables
		mVMGGain, mCMGGain,mVMGoptAngle,mCMGoptAngle
		Gain is the percentage btw. the current boat speed mStW value and Target-VMG/CMG
		Question : shouldn't we compare act.VMG with Target-VMG ? To be investigated ...
		$PNKEP, 04, x.x, x.x, x.x, x.x*hh<CR><LF>
		|    |    |    \ Gain VMG de 0 à 999 %
		|    |     \ Angle pour optimiser le VMG de 0 à 359°
		|    \ Gain CMG de 0 à 999 %
		\ Angle pour optimiser le CMG de 0 à 359°*/
		nmeastr = _T("$PNKEP,04,") + wxString::Format("%.1f,", data1) + wxString::Format("%.1f,", data2) + wxString::Format("%.1f,", data3) + wxString::Format("%.1f", data4);
		break;
	case 5:
		nmeastr = _T("$PNKEP,05,") + wxString::Format("%.1f,", data1) + wxString::Format("%.2f,N,", data2) + wxString::Format("%.2f,K", data3);
		break;
	default:
		nmeastr = _T("");
		break;
	}
	if (nmeastr != "")
		SendNMEASentence(nmeastr);
}

/************************************************************************************
Calculates the predicted course/speed on the other tack and stores it in the variables
mPredictedCoG, mPredictedSoG
************************************************************************************/
void tactics_pi::CalculatePredictedCourse(void)
{
	double predictedKdW; //==predicted Course Through Water
    if (!wxIsNaN(mStW) && !wxIsNaN(mHdt) && !wxIsNaN(mTWA) && !wxIsNaN(mlat) && !wxIsNaN(mlon) && !wxIsNaN(mLeeway) && !wxIsNaN(m_CurrentDirection) && !wxIsNaN(m_ExpSmoothCurrSpd)){
      //New: with BearingCompass in Head-Up mode = Hdt
      double Leeway = (mHeelUnit == _T("\u00B0L")) ? -mLeeway : mLeeway;
      //todo : assuming TWAunit = AWAunit ...
      if (mAWAUnit == _T("\u00B0L")){ //currently wind is from port, target is from starboard ...
        predictedKdW = mHdt - 2 * mTWA - Leeway;
      }
      else if (mAWAUnit == _T("\u00B0R")){ //so, currently wind from starboard
        predictedKdW = mHdt + 2 * mTWA - Leeway;
      }
      else {
        predictedKdW = (mTWA < 10) ? 180 : 0; // should never happen, but is this correct ???
      }
      if (predictedKdW >= 360) predictedKdW -= 360;
      if (predictedKdW < 0) predictedKdW += 360;
      double predictedLatHdt, predictedLonHdt, predictedLatCog, predictedLonCog;
      //double predictedCoG;
      //standard triangle calculation to get predicted CoG / SoG
      //get endpoint from boat-position by applying  KdW, StW
      PositionBearingDistanceMercator_Plugin(mlat, mlon, predictedKdW, mStW, &predictedLatHdt, &predictedLonHdt);
      //wxLogMessage(_T("Step1: m_lat=%f,m_lon=%f, predictedKdW=%f,m_StW=%f --> predictedLatHdt=%f,predictedLonHdt=%f\n"), m_lat, m_lon, predictedKdW, m_StW, predictedLatHdt, predictedLonHdt);
      //apply surface current with direction & speed to endpoint from above
      PositionBearingDistanceMercator_Plugin(predictedLatHdt, predictedLonHdt, m_CurrentDirection, m_ExpSmoothCurrSpd, &predictedLatCog, &predictedLonCog);
      //wxLogMessage(_T("Step2: predictedLatHdt=%f,predictedLonHdt=%f, m_CurrDir=%f,m_CurrSpeed=%f --> predictedLatCog=%f,predictedLonCog=%f\n"), predictedLatHdt, predictedLonHdt, m_CurrDir, m_CurrSpeed, predictedLatCog, predictedLonCog);
      //now get predicted CoG & SoG as difference between the 2 endpoints (coordinates) from above
      DistanceBearingMercator_Plugin(predictedLatCog, predictedLonCog, mlat, mlon, &mPredictedCoG, &mPredictedSoG);
    }
}
/************************************************************************************
Calculates the gain for VMG & CMG and stores it in the variables
mVMGGain, mCMGGain,mVMGoptAngle,mCMGoptAngle
Gain is the percentage btw. the current boat speed mStW value and Target-VMG/CMG
$PNKEP, 04, x.x, x.x, x.x, x.x*hh<CR><LF>
|    |    |    \ Gain VMG de 0 à 999 %
|    |     \ Angle pour optimiser le VMG de 0 à 359°
|    \ Gain CMG de 0 à 999 %
\ Angle pour optimiser le CMG de 0 à 359°

************************************************************************************/

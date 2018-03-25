/******************************************************************************
 * $Id: tactics_pi.h, v1.0 2016/06/27 tom_BigSpeedy Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Tactics Plugin
 * Author:   Thomas Rauch
 *   (Inspired by original work from Jean-Eudes Onfray)
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

#ifndef _TACTICSPI_H_
#define _TACTICSPI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#define     PLUGIN_VERSION_MAJOR    1
#define     PLUGIN_VERSION_MINOR    0
#define     PLUGIN_VERSION_PATCH    8

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    12

#include <wx/notebook.h>
#include <wx/fileconf.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/spinctrl.h>
#include <wx/aui/aui.h>
#include <wx/fontpicker.h>
#include <wx/glcanvas.h>
//wx2.9 #include <wx/wrapsizer.h>

#include "ocpn_plugin.h"
#include "nmea0183/nmea0183.h"
#include "instrument.h"
#include "speedometer.h"
#include "compass.h"
#include "wind.h"
#include "rudder_angle.h"
#include "gps.h"
#include "depth.h"
#include "clock.h"
#include "wind_history.h"
#include "baro_history.h"
#include "from_ownship.h"
#include "iirfilter.h"
#include "performance.h"
#include "bearingcompass.h"
#include "avg_wind.h"
#include "polarcompass.h"

class TacticsWindow;
class TacticsWindowContainer;
class TacticsInstrumentContainer;
class Polar;

#define TACTICS_TOOL_POSITION -1          // Request default positioning of toolbar tool

#define gps_watchdog_timeout_ticks  10
#define CURR_RECORD_COUNT 20
#define COGRANGE 60
class TacticsWindowContainer
{
      public:
            TacticsWindowContainer(TacticsWindow *tactics_window, wxString name, wxString caption, wxString orientation, wxArrayInt inst) {
                  m_pTacticsWindow = tactics_window; m_sName = name; m_sCaption = caption; m_sOrientation = orientation; m_aInstrumentList = inst; m_bIsVisible = false; m_bIsDeleted = false; }

            ~TacticsWindowContainer(){}
            TacticsWindow              *m_pTacticsWindow;
            bool                          m_bIsVisible;
            bool                          m_bIsDeleted;
            bool                          m_bPersVisible;  // Persists visibility, even when Dashboard tool is toggled off.
            wxString                      m_sName;
            wxString                      m_sCaption;
            wxString                      m_sOrientation;
            wxArrayInt                    m_aInstrumentList;
};

class TacticsInstrumentContainer
{
      public:
            TacticsInstrumentContainer(int id, TacticsInstrument *instrument, int capa){
                  m_ID = id; m_pInstrument = instrument; m_cap_flag = capa; }
            ~TacticsInstrumentContainer(){ delete m_pInstrument; }

            TacticsInstrument    *m_pInstrument;
            int                     m_ID;
            int                     m_cap_flag;
};

//    Dynamic arrays of pointers need explicit macros in wx261
#ifdef __WX261
WX_DEFINE_ARRAY_PTR(TacticsWindowContainer *, wxArrayOfTactics);
WX_DEFINE_ARRAY_PTR(TacticsInstrumentContainer *, wxArrayOfInstrument);
#else
WX_DEFINE_ARRAY(TacticsWindowContainer *, wxArrayOfTactics);
WX_DEFINE_ARRAY(TacticsInstrumentContainer *, wxArrayOfInstrument);
#endif

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------


class tactics_pi : public wxTimer, opencpn_plugin_112
{
public:
      tactics_pi(void *ppimgr);
      ~tactics_pi(void);

//    The required PlugIn Methods
      int Init(void);
      bool DeInit(void);

      void Notify();

      int GetAPIVersionMajor();
      int GetAPIVersionMinor();
      int GetPlugInVersionMajor();
      int GetPlugInVersionMinor();
      wxBitmap *GetPlugInBitmap();
      wxString GetCommonName();
      wxString GetShortDescription();
      wxString GetLongDescription();

//    The optional method overrides
      void SetNMEASentence(wxString &sentence);
      void SetPositionFix(PlugIn_Position_Fix &pfix);
      void SetCursorLatLon(double lat, double lon);
      int GetToolbarToolCount(void);
      void OnToolbarToolCallback(int id);
      void ShowPreferencesDialog( wxWindow* parent );
      void SetColorScheme(PI_ColorScheme cs);
      void OnPaneClose( wxAuiManagerEvent& event );
      void UpdateAuiStatus(void);
      bool SaveConfig(void);
      void PopulateContextMenu( wxMenu* menu );
      void ShowTactics( size_t id, bool visible );
      int GetToolbarItemId(){ return m_toolbar_item_id; }
      int GetTacticsWindowShownCount();
      void SetPluginMessage(wxString &message_id, wxString &message_body);
	  //TR
	  void CalculateCurrent(int st, double value, wxString unit);
	  void CalculateLeeway(int st, double value, wxString unit);
	  void CalculateTrueWind(int st, double value, wxString unit);
	  void CalculateLaylineDegreeRange(void);
      void CalculatePerformanceData(void);
      void CalculatePredictedCourse(void);
      void ExportPerformanceData(void);
      wxString ComputeChecksum(wxString sentence);
      void SendNMEASentence(wxString sentence);
      void createPNKEP_NMEA(int sentence, double data1, double data2, double data3, double data4);
	  void SetCalcVariables(int st, double value, wxString unit);
	  bool RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp);
	  bool RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
	  void DoRenderLaylineGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
	  void DoRenderCurrentGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
      void DrawWindBarb(wxPoint pp, PlugIn_ViewPort *vp);
      void DrawPolar(PlugIn_ViewPort *vp, wxPoint pp, double PolarAngle );
      //void DrawTargetAngle(PlugIn_ViewPort *vp, wxPoint pp, double PolarAngle, double TargetAngle, wxString color, double rad);
      void DrawTargetAngle(PlugIn_ViewPort *vp, wxPoint pp, double Angle, wxString color, int size, double rad);
      void ToggleLaylineRender(wxWindow* parent);
      void ToggleCurrentRender(wxWindow* parent);
      void TogglePolarRender(wxWindow* parent);
      void ToggleWindbarbRender(wxWindow* parent);
      bool GetLaylineVisibility(wxWindow* parent);
      bool GetWindbarbVisibility(wxWindow* parent);
      bool GetCurrentVisibility(wxWindow* parent);
      bool GetPolarVisibility(wxWindow* parent);
      void OnContextMenuItemCallback(int id);

private:
      bool LoadConfig(void);
      void ApplyConfig(void);
      void SendSentenceToAllInstruments(int st, double value, wxString unit);
      void SendSatInfoToAllInstruments(int cnt, int seq, SAT_INFO sats[4]);
      void SendUtcTimeToAllInstruments( wxDateTime value );

      wxFileConfig         *m_pconfig;
      wxAuiManager         *m_pauimgr;
      int                  m_toolbar_item_id;

      wxArrayOfTactics     m_ArrayOfTacticsWindow;
      int                  m_show_id;
      int                  m_hide_id;

      NMEA0183             m_NMEA0183;                 // Used to parse NMEA Sentences
      short                mPriPosition, mPriCOGSOG, mPriHeadingM, mPriHeadingT, mPriVar, mPriDateTime, mPriAWA, mPriTWA, mPriDepth;
      double               mVar;
      // FFU
      double               mSatsInView;
	  double               mHdm;
	  double               calmHdt;
      wxDateTime           mUTCDateTime;
      int                  m_config_version;
      wxString             m_VDO_accumulator;
      int                  mHDx_Watchdog;
      int                  mHDT_Watchdog;
      int                  mGPS_Watchdog;
      int                  mVar_Watchdog;
      int                  mBRG_Watchdog;
      int                  mTWD_Watchdog;
      int                  mTWS_Watchdog;
      int                  mAWS_Watchdog;

	  // TR : bearing compass + TWA/TWD calculation
	  wxMenu               *m_pmenu;
	  double               mHdt, mStW, mSOG, mCOG, mlat, mlon, mheel,msensorheel, mLeeway;
      double               m_calcTWS, m_calcTWA, m_calcTWD; //temp testing for Windbarb display
	  wxString             mHeelUnit, mAWAUnit, mAWSUnit;
	  double               mAWA, mAWS, mTWA, mTWD, mTWS;
      bool                 m_bTrueWind_available, m_bLaylinesIsVisible, m_bDisplayCurrentOnChart, m_bShowWindbarbOnChart, m_bShowPolarOnChart;
	  bool                 m_LeewayOK;
	  double               alpha_currspd, alpha_CogHdt;
	  double               m_ExpSmoothCurrSpd, m_ExpSmoothCurrDir,m_ExpSmoothSog;
	  double               m_ExpSmoothSinCurrDir, m_ExpSmoothCosCurrDir;
	  double               m_tempSmoothedLaylineCOG;
	  double			   m_ExpSmoothDiffCogHdt;
	  double               m_LaylineDegRange, m_COGRange[COGRANGE], m_ExpSmoothDegRange, m_alphaDeltaCog;
	  double               m_LaylineSmoothedCog, m_ExpSmoothSinCog, m_ExpSmoothCosCog, m_alphaLaylineCog;
      //Performance Variables
      double               mPolarTargetSpeed, mPredictedHdG, mPredictedCoG, mPredictedSoG, mPercentTargetVMGupwind, mPercentTargetVMGdownwind;
      TargetxMG tvmg,tcmg;
      double               mVMGGain, mCMGGain, mVMGoptAngle, mCMGoptAngle,mBRG;
	  wxDC            *m_pdc;
	  wxPoint         vpoints[3],tackpoints[3];
	  double          m_CurrentDirection;

	  DoubleExpSmooth *mSinCurrDir;
	  DoubleExpSmooth *mCosCurrDir;
	  ExpSmooth       *mExpSmoothCurrSpd;
	  DoubleExpSmooth *mExpSmoothSog;
	  ExpSmooth       *mExpSmSinCog;
	  ExpSmooth       *mExpSmCosCog;
	  ExpSmooth       *mExpSmDegRange;
	  ExpSmooth       *mExpSmDiffCogHdt;

      iirfilter            mSOGFilter;
      iirfilter            mCOGFilter;

//protected:
//      DECLARE_EVENT_TABLE();
};

class TacticsPreferencesDialog : public wxDialog
{
public:
      TacticsPreferencesDialog( wxWindow *pparent, wxWindowID id, wxArrayOfTactics config );
      ~TacticsPreferencesDialog() {}

      void OnCloseDialog(wxCloseEvent& event);
      void OnTacticsSelected(wxListEvent& event);
      void OnTacticsAdd(wxCommandEvent& event);
      void OnTacticsDelete(wxCommandEvent& event);
      void OnInstrumentSelected(wxListEvent& event);
      void OnInstrumentAdd(wxCommandEvent& event);
      void OnInstrumentEdit(wxCommandEvent& event);
      void OnInstrumentDelete(wxCommandEvent& event);
      void OnInstrumentUp(wxCommandEvent& event);
      void OnInstrumentDown(wxCommandEvent& event);
      void OnPrefScroll(wxCommandEvent& event);
	  void SelectPolarFile(wxCommandEvent& event);
      void OnAWSAWACorrectionUpdated(wxCommandEvent& event);
      void OnManualHeelUpdate(wxCommandEvent& event);
      void OnAlphaCurrDirSliderUpdated(wxCommandEvent& event);
      void ApplyPrefs(wxCommandEvent& event);
	  void SaveTacticsConfig();

      wxArrayOfTactics            m_Config;
      wxFontPickerCtrl             *m_pFontPickerTitle;
      wxFontPickerCtrl             *m_pFontPickerData;
      wxFontPickerCtrl             *m_pFontPickerLabel;
      wxFontPickerCtrl             *m_pFontPickerSmall;
      wxSpinCtrl                   *m_pSpinSpeedMax;
      wxSpinCtrl                   *m_pSpinCOGDamp;
      wxSpinCtrl                   *m_pSpinSOGDamp;
      wxChoice                     *m_pChoiceSpeedUnit;
      wxChoice                     *m_pChoiceDepthUnit;
      wxChoice                     *m_pChoiceDistanceUnit;
      wxChoice                     *m_pChoiceWindSpeedUnit;

      wxSpinCtrlDouble             *m_alphaDeltCoG; //TR
      wxSpinCtrl                   *m_minLayLineWidth;//TR
	  wxSpinCtrl                   *m_maxLayLineWidth;//TR
      wxSpinCtrlDouble             *m_LeewayFactor;//TR
      wxSpinCtrl           *m_AlphaCurrDir; //TR
      wxSpinCtrlDouble             *m_fixedLeeway;//TR
      //wxSlider                     *m_AlphaCurrDir; //TR
	  wxButton                     *m_buttonLoadPolar;//TR
      wxButton                     *m_buttonPrefsApply;//TR
       wxButton                     *m_buttonPrefOK;//TR
	  wxTextCtrl                   *m_pTextCtrlPolar; //TR
      wxSpinCtrlDouble             *m_pLaylineLength; //TR
      wxSpinCtrlDouble             *m_heel5_45;
      wxSpinCtrlDouble             *m_heel5_90;
      wxSpinCtrlDouble             *m_heel5_135;
      wxSpinCtrlDouble             *m_heel10_45;
      wxSpinCtrlDouble             *m_heel10_90;
      wxSpinCtrlDouble             *m_heel10_135;
      wxSpinCtrlDouble             *m_heel15_45;
      wxSpinCtrlDouble             *m_heel15_90;
      wxSpinCtrlDouble             *m_heel15_135;
      wxSpinCtrlDouble             *m_heel20_45;
      wxSpinCtrlDouble             *m_heel20_90;
      wxSpinCtrlDouble             *m_heel20_135;
      wxSpinCtrlDouble             *m_heel25_45;
      wxSpinCtrlDouble             *m_heel25_90;
      wxSpinCtrlDouble             *m_heel25_135;
	  wxTextCtrl                   *m_UseHeelSensor;
	  wxCheckBox                   *m_CurrentOnChart;
	  wxRadioButton                *m_ButtonLeewayFactor;
	  wxRadioButton                *m_ButtonFixedLeeway;
	  wxRadioButton                *m_ButtonHeelInput;
	  wxRadioButton                *m_ButtonUseHeelSensor;
      wxCheckBox                   *m_CorrectSTWwithLeeway;
      wxCheckBox                   *m_CorrectAWwithHeel;
      wxCheckBox                   *m_ForceTrueWindCalculation;
      wxCheckBox                   *m_UseSOGforTWCalc;
      wxCheckBox                   *m_ShowWindbarbOnChart;
      wxCheckBox                   *m_ShowPolarOnChart;
      wxRadioButton                *m_ButtonExpNKE;
      wxCheckBox                   *m_ExpPerfData01;
      wxCheckBox                   *m_ExpPerfData02;
      wxCheckBox                   *m_ExpPerfData03;
      wxCheckBox                   *m_ExpPerfData04;
      wxCheckBox                   *m_ExpPerfData05;
private:
      void UpdateTacticsButtonsState(void);
      void UpdateButtonsState(void);
      wxFileConfig     *m_pconfig;

      int                           curSel;
      wxListCtrl                   *m_pListCtrlTacticss;
      wxBitmapButton               *m_pButtonAddTactics;
      wxBitmapButton               *m_pButtonDeleteTactics;
      wxPanel                      *m_pPanelTactics;
      wxTextCtrl                   *m_pTextCtrlCaption;
      wxCheckBox                   *m_pCheckBoxIsVisible;
      wxChoice                     *m_pChoiceOrientation;
      wxListCtrl                   *m_pListCtrlInstruments;
      wxButton                     *m_pButtonAdd;
      wxButton                     *m_pButtonEdit;
      wxButton                     *m_pButtonDelete;
      wxButton                     *m_pButtonUp;
      wxButton                     *m_pButtonDown;
};

class AddInstrumentDlg : public wxDialog
{
public:
      AddInstrumentDlg(wxWindow *pparent, wxWindowID id);
      ~AddInstrumentDlg() {}

      unsigned int GetInstrumentAdded();

private:
      wxListCtrl*                   m_pListCtrlInstruments;
};

enum
{
      ID_DASHBOARD_WINDOW
};

enum
{
      ID_DASH_PREFS = 999,
      ID_DASH_VERTICAL,
      ID_DASH_HORIZONTAL,
	  ID_DASH_LAYLINE,
      ID_DASH_CURRENT,
      ID_DASH_POLAR,
      ID_DASH_WINDBARB
};

class TacticsWindow : public wxWindow
{
public:
    TacticsWindow( wxWindow *pparent, wxWindowID id, wxAuiManager *auimgr, tactics_pi* plugin,
             int orient, TacticsWindowContainer* mycont );
    ~TacticsWindow();

    void SetColorScheme( PI_ColorScheme cs );
    void SetSizerOrientation( int orient );
    int GetSizerOrientation();
    void OnSize( wxSizeEvent& evt );
    void OnContextMenu( wxContextMenuEvent& evt );
    void OnContextMenuSelect( wxCommandEvent& evt );
    bool isInstrumentListEqual( const wxArrayInt& list );
    void SetInstrumentList( wxArrayInt list );
    void SendSentenceToAllInstruments( int st, double value, wxString unit );
    void SendSatInfoToAllInstruments( int cnt, int seq, SAT_INFO sats[4] );
    void SendUtcTimeToAllInstruments( wxDateTime value );
    void ChangePaneOrientation( int orient, bool updateAUImgr );

/*TODO: OnKeyPress pass event to main window or disable focus*/

    TacticsWindowContainer* m_Container;

private:
      wxAuiManager         *m_pauimgr;
      tactics_pi*         m_plugin;

//wx2.9      wxWrapSizer*          itemBoxSizer;
      wxBoxSizer*          itemBoxSizer;
      wxArrayOfInstrument  m_ArrayOfInstrument;
};

#endif


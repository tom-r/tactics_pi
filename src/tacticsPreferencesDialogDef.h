///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bmpbuttn.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/fontpicker.h>
#include <wx/spinctrl.h>
#include <wx/radiobut.h>
#include <wx/scrolwin.h>
#include <wx/statline.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TacticsPreferencesDialogDef
///////////////////////////////////////////////////////////////////////////////
class TacticsPreferencesDialogDef : public wxDialog
{
	private:

	protected:
		wxNotebook* m_notebookPreferences;
		wxPanel* m_panelTactics;
		wxListCtrl* m_listCtrlTactics;
		wxBitmapButton* m_bpButtonAddTactics;
		wxBitmapButton* m_bpButtonDeleteTactics;
		wxCheckBox* m_checkBoxIsVisible;
		wxStaticText* m_staticText19;
		wxTextCtrl* m_textCtrlCaption;
		wxStaticText* m_staticText20;
		wxChoice* m_choiceOrientation;
		wxListCtrl* m_listCtrlInstruments;
		wxButton* m_buttonAdd;
		wxButton* m_buttonDelete;
		wxButton* m_buttonUp;
		wxButton* m_buttonDown;
		wxPanel* m_panelAppearance;
		wxStaticText* m_staticText21;
		wxFontPickerCtrl* m_fontPickerTitle;
		wxStaticText* m_staticText22;
		wxFontPickerCtrl* m_fontPickerData;
		wxStaticText* m_staticText23;
		wxFontPickerCtrl* m_fontPickerLabel;
		wxStaticText* m_staticText24;
		wxFontPickerCtrl* m_fontPicker4;
		wxStaticText* m_staticText25;
		wxSpinCtrl* m_spinCtrlSpeedMax;
		wxStaticText* m_staticText26;
		wxSpinCtrl* m_spinCtrl2;
		wxStaticText* m_staticText27;
		wxChoice* m_choiceSpeedUnit;
		wxStaticText* m_staticText28;
		wxChoice* m_choiceDepthUnit;
		wxStaticText* m_staticText29;
		wxChoice* m_choiceDistanceUnit;
		wxStaticText* m_staticText30;
		wxChoice* m_choiceWindSpeedUnit;
		wxScrolledWindow* m_scrolledWindowPerformanceParameters;
		wxStaticText* m_staticText31;
		wxSpinCtrlDouble* m_spinCtrlDoubleAlphaLaylineDampFactor;
		wxStaticText* m_staticText32;
		wxSpinCtrlDouble* m_spinCtrlDoubleAlphaDeltCoG;
		wxStaticText* m_staticText33;
		wxSpinCtrlDouble* m_spinCtrlDoubleLaylineLength;
		wxStaticText* m_staticText34;
		wxSpinCtrlDouble* m_spinCtrlDoubleMinLaylineWidth;
		wxStaticText* m_staticText35;
		wxSpinCtrlDouble* m_spinCtrlDoubleMaxLaylineWidth;
		wxStaticText* m_staticText37;
		wxSpinCtrlDouble* m_spinCtrlDouble;
		wxRadioButton* m_radioBtnUseHeelSensor;
		wxRadioButton* m_radioBtnFixedLeeway;
		wxSpinCtrlDouble* m_spinCtrlDoublefixedLeeway;
		wxRadioButton* m_radioBtnHeelnput;
		wxStaticText* m_staticText39;
		wxStaticText* m_staticText40;
		wxStaticText* m_staticText41;
		wxStaticText* m_staticText42;
		wxStaticText* m_staticText43;
		wxSpinCtrlDouble* m_spinCtrlDoubleHel5_45;
		wxSpinCtrlDouble* m_spinCtrlDoubleheel5_90;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel5_135;
		wxStaticText* m_staticText44;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel10_45;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel10_90;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel10_135;
		wxStaticText* m_staticText45;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel15_45;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel15_90;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel15_135;
		wxStaticText* m_staticText46;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel20_45;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel20_90;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel20_135;
		wxStaticText* m_staticText47;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel25_45;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel25_90;
		wxSpinCtrlDouble* m_spinCtrlDoubleHeel25_135;
		wxStaticText* m_staticText48;
		wxSpinCtrl* m_spinCtrlAlphaCurrentDir;
		wxCheckBox* m_checkBoxCurrentOnChart;
		wxCheckBox* m_checkBoxCorrectSTWwithLeeway;
		wxCheckBox* m_checkBoxCorrectAWwithHeel;
		wxCheckBox* m_checkBoxForceTrueWindCalculation;
		wxCheckBox* m_checkBoxUseSOGforTWCalc;
		wxCheckBox* m_checkBoxShowWindbarbOnChart;
		wxStaticText* m_staticText49;
		wxTextCtrl* m_textCtrlPolar;
		wxButton* m_buttonLoadPolar;
		wxCheckBox* m_checkBoxShowPolarOnChart;
		wxRadioButton* m_radioBtnExpNKE;
		wxCheckBox* m_checkBoxExpPerfData01;
		wxCheckBox* m_checkBoxExpPerfData02;
		wxCheckBox* m_checkBoxExpPerfData03;
		wxCheckBox* m_checkBoxExpPerfData04;
		wxCheckBox* m_checkBoxExpPerfData05;
		wxPanel* m_panelAbout;
		wxStaticText* m_staticTextName;
		wxStaticText* m_staticTextNameVal;
		wxStaticText* m_staticTextVersion;
		wxStaticText* m_staticTextMajor;
		wxStaticText* m_staticTextMajorVal;
		wxStaticText* m_staticTextMinor;
		wxStaticText* m_staticTextMinorVal;
		wxStaticText* m_staticTextPatch;
		wxStaticText* m_staticTextPatchVal;
		wxStaticText* m_staticTextOCPNVersion;
		wxStaticText* m_staticTextOCPNAPIVersionMajor;
		wxStaticText* m_staticTextOCPNAPIVersionMajorVal;
		wxStaticText* m_staticTextOCPNAPIVersionMinor;
		wxStaticText* m_staticTextOCPNAPIVersionMinorVal;
		wxStaticText* m_staticTextDate;
		wxStaticText* m_staticTextDateVal;
		wxStaticLine* m_staticline1;
		wxStaticText* m_staticTextOther;
		wxStaticText* m_staticTextOtherVal;
		wxButton* m_buttonApply;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnTacticsSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnTacticsAdd( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTacticsDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnInstrumentSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnInstrumentAdd( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnInstrumentDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnInstrumentUp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnInstrumentDown( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnManualHeelUpdate( wxCommandEvent& event ) { event.Skip(); }
		virtual void ApplyPrefs( wxCommandEvent& event ) { event.Skip(); }


	public:

		TacticsPreferencesDialogDef( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Tactics Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 669,1039 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );
		~TacticsPreferencesDialogDef();

};


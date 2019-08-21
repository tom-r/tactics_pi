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
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TackticsPreferencesDialogDef
///////////////////////////////////////////////////////////////////////////////
class TackticsPreferencesDialogDef : public wxDialog
{
	private:

	protected:
		wxNotebook* m_notebookPreferences;
		wxPanel* m_panelTactics;
		wxPanel* m_panelAppearance;
		wxPanel* m_panelPerformanceParameters;
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
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		wxButton* m_buttonApply;

		// Virtual event handlers, overide them in your derived class
		virtual void OnTacticsPreferencesOKClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTacticsPreferencesCancelClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTacticsPreferencesApplyClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		TackticsPreferencesDialogDef( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Tactics Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 532,155 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );
		~TackticsPreferencesDialogDef();

};


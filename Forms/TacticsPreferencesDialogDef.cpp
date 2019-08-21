///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "TacticsPreferencesDialogDef.h"

///////////////////////////////////////////////////////////////////////////

TackticsPreferencesDialogDef::TackticsPreferencesDialogDef( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_notebookPreferences = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panelTactics = new wxPanel( m_notebookPreferences, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebookPreferences->AddPage( m_panelTactics, _("Tactics"), false );
	m_panelAppearance = new wxPanel( m_notebookPreferences, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebookPreferences->AddPage( m_panelAppearance, _("Appearance"), false );
	m_panelPerformanceParameters = new wxPanel( m_notebookPreferences, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_notebookPreferences->AddPage( m_panelPerformanceParameters, _("Performance Parameters"), false );
	m_panelAbout = new wxPanel( m_notebookPreferences, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizerAboutLayout;
	fgSizerAboutLayout = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizerAboutLayout->SetFlexibleDirection( wxBOTH );
	fgSizerAboutLayout->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticTextName = new wxStaticText( m_panelAbout, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextName->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextName, 0, wxALL, 5 );

	m_staticTextNameVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextNameVal->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextNameVal, 0, wxALL, 5 );

	m_staticTextVersion = new wxStaticText( m_panelAbout, wxID_ANY, _("Version:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextVersion->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextVersion, 0, wxALL, 5 );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 6, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticTextMajor = new wxStaticText( m_panelAbout, wxID_ANY, _("Major:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMajor->Wrap( -1 );
	fgSizer2->Add( m_staticTextMajor, 0, wxALL, 5 );

	m_staticTextMajorVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMajorVal->Wrap( -1 );
	fgSizer2->Add( m_staticTextMajorVal, 0, wxALL, 5 );

	m_staticTextMinor = new wxStaticText( m_panelAbout, wxID_ANY, _("Minor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMinor->Wrap( -1 );
	fgSizer2->Add( m_staticTextMinor, 0, wxALL, 5 );

	m_staticTextMinorVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMinorVal->Wrap( -1 );
	fgSizer2->Add( m_staticTextMinorVal, 0, wxALL, 5 );

	m_staticTextPatch = new wxStaticText( m_panelAbout, wxID_ANY, _("Patch:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPatch->Wrap( -1 );
	fgSizer2->Add( m_staticTextPatch, 0, wxALL, 5 );

	m_staticTextPatchVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPatchVal->Wrap( -1 );
	fgSizer2->Add( m_staticTextPatchVal, 0, wxALL, 5 );


	fgSizerAboutLayout->Add( fgSizer2, 1, wxEXPAND, 5 );

	m_staticTextOCPNVersion = new wxStaticText( m_panelAbout, wxID_ANY, _("OCPN API Version:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOCPNVersion->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextOCPNVersion, 0, wxALL, 5 );

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticTextOCPNAPIVersionMajor = new wxStaticText( m_panelAbout, wxID_ANY, _("Major:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOCPNAPIVersionMajor->Wrap( -1 );
	fgSizer3->Add( m_staticTextOCPNAPIVersionMajor, 0, wxALL, 5 );

	m_staticTextOCPNAPIVersionMajorVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOCPNAPIVersionMajorVal->Wrap( -1 );
	fgSizer3->Add( m_staticTextOCPNAPIVersionMajorVal, 0, wxALL, 5 );

	m_staticTextOCPNAPIVersionMinor = new wxStaticText( m_panelAbout, wxID_ANY, _("Minor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOCPNAPIVersionMinor->Wrap( -1 );
	fgSizer3->Add( m_staticTextOCPNAPIVersionMinor, 0, wxALL, 5 );

	m_staticTextOCPNAPIVersionMinorVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOCPNAPIVersionMinorVal->Wrap( -1 );
	fgSizer3->Add( m_staticTextOCPNAPIVersionMinorVal, 0, wxALL, 5 );


	fgSizerAboutLayout->Add( fgSizer3, 1, wxEXPAND, 5 );

	m_staticTextDate = new wxStaticText( m_panelAbout, wxID_ANY, _("Date:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDate->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextDate, 0, wxALL, 5 );

	m_staticTextDateVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDateVal->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextDateVal, 0, wxALL, 5 );

	m_staticline1 = new wxStaticLine( m_panelAbout, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizerAboutLayout->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticTextOther = new wxStaticText( m_panelAbout, wxID_ANY, _("Other:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOther->Wrap( -1 );
	fgSizer4->Add( m_staticTextOther, 0, wxALL, 5 );

	m_staticTextOtherVal = new wxStaticText( m_panelAbout, wxID_ANY, _("Please report problems using FlySpray at:\nhttps://www.opencpn.org/flyspray/index.php?project=60&do=index&switch=1\n\nor\n\nat the OpenCPN forum:\nhttp://www.cruisersforum.com/forums/f134\n\nor\n\nRaise an issue in GIT at:\nhttps://github.com/rgleason/tacktics_pi/issues\n"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOtherVal->Wrap( -1 );
	fgSizer4->Add( m_staticTextOtherVal, 0, wxALL, 5 );


	fgSizerAboutLayout->Add( fgSizer4, 1, wxEXPAND, 5 );


	bSizer2->Add( fgSizerAboutLayout, 1, wxEXPAND, 5 );


	m_panelAbout->SetSizer( bSizer2 );
	m_panelAbout->Layout();
	bSizer2->Fit( m_panelAbout );
	m_notebookPreferences->AddPage( m_panelAbout, _("About"), true );

	bSizer1->Add( m_notebookPreferences, 1, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizerOKCancelApply;
	bSizerOKCancelApply = new wxBoxSizer( wxVERTICAL );

	m_buttonOK = new wxButton( this, wxID_ANY, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerOKCancelApply->Add( m_buttonOK, 0, wxALL, 5 );

	m_buttonCancel = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerOKCancelApply->Add( m_buttonCancel, 0, wxALL, 5 );

	m_buttonApply = new wxButton( this, wxID_ANY, _("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerOKCancelApply->Add( m_buttonApply, 0, wxALL, 5 );


	bSizer1->Add( bSizerOKCancelApply, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TackticsPreferencesDialogDef::OnTacticsPreferencesOKClick ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TackticsPreferencesDialogDef::OnTacticsPreferencesCancelClick ), NULL, this );
	m_buttonApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TackticsPreferencesDialogDef::OnTacticsPreferencesApplyClick ), NULL, this );
}

TackticsPreferencesDialogDef::~TackticsPreferencesDialogDef()
{
	// Disconnect Events
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TackticsPreferencesDialogDef::OnTacticsPreferencesOKClick ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TackticsPreferencesDialogDef::OnTacticsPreferencesCancelClick ), NULL, this );
	m_buttonApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TackticsPreferencesDialogDef::OnTacticsPreferencesApplyClick ), NULL, this );

}

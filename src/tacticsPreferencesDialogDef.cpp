///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "TacticsPreferencesDialogDef.h"

///////////////////////////////////////////////////////////////////////////

TacticsPreferencesDialogDef::TacticsPreferencesDialogDef( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer( 3, 0, 0, 0 );
	fgSizer13->AddGrowableCol( 1 );
	fgSizer13->AddGrowableRow( 1 );
	fgSizer13->SetFlexibleDirection( wxBOTH );
	fgSizer13->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );

	m_notebookPreferences = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panelTactics = new wxPanel( m_notebookPreferences, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_listCtrlTactics = new wxListCtrl( m_panelTactics, wxID_ANY, wxDefaultPosition, wxSize( 50,200 ), wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL );
	fgSizer6->Add( m_listCtrlTactics, 0, wxALL, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	m_bpButtonAddTactics = new wxBitmapButton( m_panelTactics, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	bSizer4->Add( m_bpButtonAddTactics, 0, wxALL, 5 );

	m_bpButtonDeleteTactics = new wxBitmapButton( m_panelTactics, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	bSizer4->Add( m_bpButtonDeleteTactics, 0, wxALL, 5 );


	fgSizer6->Add( bSizer4, 1, wxALIGN_CENTER|wxEXPAND, 5 );


	fgSizer5->Add( fgSizer6, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panelTactics, wxID_ANY, _("Tactics") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer7->AddGrowableCol( 1 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_checkBoxIsVisible = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("show this tactics"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer7->Add( m_checkBoxIsVisible, 0, wxALL, 5 );


	fgSizer7->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText19 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Caption:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	fgSizer7->Add( m_staticText19, 0, wxALL, 5 );

	m_textCtrlCaption = new wxTextCtrl( sbSizer1->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer7->Add( m_textCtrlCaption, 0, wxALIGN_LEFT|wxALL|wxEXPAND, 5 );

	m_staticText20 = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, _("Orientation:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	fgSizer7->Add( m_staticText20, 0, wxALL, 5 );

	wxString m_choiceOrientationChoices[] = { _("Vertical"), _("Horizontal") };
	int m_choiceOrientationNChoices = sizeof( m_choiceOrientationChoices ) / sizeof( wxString );
	m_choiceOrientation = new wxChoice( sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceOrientationNChoices, m_choiceOrientationChoices, 0 );
	m_choiceOrientation->SetSelection( 0 );
	fgSizer7->Add( m_choiceOrientation, 0, wxALIGN_RIGHT|wxALL, 5 );


	sbSizer1->Add( fgSizer7, 1, wxEXPAND, 5 );


	bSizer7->Add( sbSizer1, 1, wxEXPAND, 5 );


	fgSizer8->Add( bSizer7, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panelTactics, wxID_ANY, _("Instruments") ), wxHORIZONTAL );

	m_listCtrlInstruments = new wxListCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL );
	sbSizer2->Add( m_listCtrlInstruments, 0, wxALL, 5 );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	m_buttonAdd = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonAdd, 0, wxALL|wxEXPAND, 5 );

	m_buttonDelete = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonDelete, 0, wxALL|wxEXPAND, 5 );


	bSizer8->Add( 0, 0, 1, wxEXPAND, 5 );

	m_buttonUp = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, _("UP"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonUp, 0, wxALL, 5 );

	m_buttonDown = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, _("Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonDown, 0, wxALL, 5 );


	sbSizer2->Add( bSizer8, 1, wxEXPAND, 5 );


	bSizer6->Add( sbSizer2, 1, wxEXPAND, 5 );


	fgSizer8->Add( bSizer6, 1, wxEXPAND, 5 );


	bSizer5->Add( fgSizer8, 1, wxEXPAND, 5 );


	fgSizer5->Add( bSizer5, 1, wxEXPAND, 5 );


	m_panelTactics->SetSizer( fgSizer5 );
	m_panelTactics->Layout();
	fgSizer5->Fit( m_panelTactics );
	m_notebookPreferences->AddPage( m_panelTactics, _("Tactics"), false );
	m_panelAppearance = new wxPanel( m_notebookPreferences, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer10->AddGrowableCol( 0 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizerFonts;
	sbSizerFonts = new wxStaticBoxSizer( new wxStaticBox( m_panelAppearance, wxID_ANY, _("Fonts") ), wxHORIZONTAL );

	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer11->AddGrowableCol( 1 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText21 = new wxStaticText( sbSizerFonts->GetStaticBox(), wxID_ANY, _("Title:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	fgSizer11->Add( m_staticText21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_fontPickerTitle = new wxFontPickerCtrl( sbSizerFonts->GetStaticBox(), wxID_ANY, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE );
	m_fontPickerTitle->SetMaxPointSize( 100 );
	fgSizer11->Add( m_fontPickerTitle, 0, wxALIGN_RIGHT|wxALL, 0 );

	m_staticText22 = new wxStaticText( sbSizerFonts->GetStaticBox(), wxID_ANY, _("Data:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	fgSizer11->Add( m_staticText22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_fontPickerData = new wxFontPickerCtrl( sbSizerFonts->GetStaticBox(), wxID_ANY, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE );
	m_fontPickerData->SetMaxPointSize( 100 );
	fgSizer11->Add( m_fontPickerData, 0, wxALIGN_RIGHT|wxALL, 0 );

	m_staticText23 = new wxStaticText( sbSizerFonts->GetStaticBox(), wxID_ANY, _("Label:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer11->Add( m_staticText23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_fontPickerLabel = new wxFontPickerCtrl( sbSizerFonts->GetStaticBox(), wxID_ANY, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE );
	m_fontPickerLabel->SetMaxPointSize( 100 );
	fgSizer11->Add( m_fontPickerLabel, 0, wxALIGN_RIGHT|wxALL, 0 );

	m_staticText24 = new wxStaticText( sbSizerFonts->GetStaticBox(), wxID_ANY, _("Small:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	fgSizer11->Add( m_staticText24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_fontPicker4 = new wxFontPickerCtrl( sbSizerFonts->GetStaticBox(), wxID_ANY, wxNullFont, wxDefaultPosition, wxDefaultSize, wxFNTP_DEFAULT_STYLE );
	m_fontPicker4->SetMaxPointSize( 100 );
	fgSizer11->Add( m_fontPicker4, 0, wxALIGN_RIGHT|wxALL, 0 );


	sbSizerFonts->Add( fgSizer11, 1, wxEXPAND, 5 );


	bSizer10->Add( sbSizerFonts, 1, wxEXPAND, 5 );


	fgSizer10->Add( bSizer10, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_panelAppearance, wxID_ANY, _("Units, Ranges, Formats") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer12->AddGrowableCol( 0 );
	fgSizer12->SetFlexibleDirection( wxBOTH );
	fgSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText25 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("Speed Over Ground Damping Factor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer12->Add( m_staticText25, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_spinCtrlSpeedMax = new wxSpinCtrl( sbSizer4->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 10, 100, 12 );
	fgSizer12->Add( m_spinCtrlSpeedMax, 0, wxALIGN_RIGHT|wxALL, 0 );

	m_staticText26 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("COG Damping Factor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	fgSizer12->Add( m_staticText26, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_spinCtrl2 = new wxSpinCtrl( sbSizer4->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0 );
	fgSizer12->Add( m_spinCtrl2, 0, wxALIGN_RIGHT|wxALL, 0 );

	m_staticText27 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("Boat speed units:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	fgSizer12->Add( m_staticText27, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxString m_choiceSpeedUnitChoices[] = { _("Honor OpenCPN settings"), _("Kts"), _("mph"), _("km/h"), _("m/s") };
	int m_choiceSpeedUnitNChoices = sizeof( m_choiceSpeedUnitChoices ) / sizeof( wxString );
	m_choiceSpeedUnit = new wxChoice( sbSizer4->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceSpeedUnitNChoices, m_choiceSpeedUnitChoices, 0 );
	m_choiceSpeedUnit->SetSelection( 0 );
	fgSizer12->Add( m_choiceSpeedUnit, 0, wxALL, 0 );

	m_staticText28 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("Depth units:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText28->Wrap( -1 );
	fgSizer12->Add( m_staticText28, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxString m_choiceDepthUnitChoices[] = { _("Meters"), _("Feet"), _("Fathoms"), _("Centimeters"), _("Inches") };
	int m_choiceDepthUnitNChoices = sizeof( m_choiceDepthUnitChoices ) / sizeof( wxString );
	m_choiceDepthUnit = new wxChoice( sbSizer4->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDepthUnitNChoices, m_choiceDepthUnitChoices, 0 );
	m_choiceDepthUnit->SetSelection( 0 );
	fgSizer12->Add( m_choiceDepthUnit, 0, wxALIGN_RIGHT|wxALL, 0 );

	m_staticText29 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("Distance units:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	fgSizer12->Add( m_staticText29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxString m_choiceDistanceUnitChoices[] = { _("Honor OpenCPN settings"), _("Nautical miles"), _("Statute miles"), _("Kilometers"), _("Meters") };
	int m_choiceDistanceUnitNChoices = sizeof( m_choiceDistanceUnitChoices ) / sizeof( wxString );
	m_choiceDistanceUnit = new wxChoice( sbSizer4->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDistanceUnitNChoices, m_choiceDistanceUnitChoices, 0 );
	m_choiceDistanceUnit->SetSelection( 0 );
	fgSizer12->Add( m_choiceDistanceUnit, 0, wxALL|wxEXPAND, 0 );

	m_staticText30 = new wxStaticText( sbSizer4->GetStaticBox(), wxID_ANY, _("Wind speed untis:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText30->Wrap( -1 );
	fgSizer12->Add( m_staticText30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxString m_choiceWindSpeedUnitChoices[] = { _("Kts"), _("mph"), _("km/h"), _("m/s") };
	int m_choiceWindSpeedUnitNChoices = sizeof( m_choiceWindSpeedUnitChoices ) / sizeof( wxString );
	m_choiceWindSpeedUnit = new wxChoice( sbSizer4->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceWindSpeedUnitNChoices, m_choiceWindSpeedUnitChoices, 0 );
	m_choiceWindSpeedUnit->SetSelection( 0 );
	fgSizer12->Add( m_choiceWindSpeedUnit, 0, wxALIGN_RIGHT|wxALL, 0 );


	sbSizer4->Add( fgSizer12, 1, wxEXPAND, 5 );


	bSizer11->Add( sbSizer4, 1, wxEXPAND, 5 );


	fgSizer10->Add( bSizer11, 1, wxEXPAND, 5 );


	m_panelAppearance->SetSizer( fgSizer10 );
	m_panelAppearance->Layout();
	fgSizer10->Fit( m_panelAppearance );
	m_notebookPreferences->AddPage( m_panelAppearance, _("Appearance"), false );
	m_scrolledWindowPerformanceParameters = new wxScrolledWindow( m_notebookPreferences, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxALWAYS_SHOW_SB|wxVSCROLL );
	m_scrolledWindowPerformanceParameters->SetScrollRate( 5, 5 );
	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer14->AddGrowableCol( 0 );
	fgSizer14->SetFlexibleDirection( wxBOTH );
	fgSizer14->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindowPerformanceParameters, wxID_ANY, _("Laylines") ), wxHORIZONTAL );

	wxFlexGridSizer* fgSizer15;
	fgSizer15 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer15->AddGrowableCol( 1 );
	fgSizer15->SetFlexibleDirection( wxBOTH );
	fgSizer15->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText31 = new wxStaticText( sbSizer5->GetStaticBox(), wxID_ANY, _("Layline damping factor [0.025-1]:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	m_staticText31->SetToolTip( _("The layline damping factor determines how fast the  laylines react on your course changes, i.e. your COG changes.\n Low values mean high damping.") );

	fgSizer15->Add( m_staticText31, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_spinCtrlDoubleAlphaLaylineDampFactor = new wxSpinCtrlDouble( sbSizer5->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.025, 1, 0.025000, 1 );
	m_spinCtrlDoubleAlphaLaylineDampFactor->SetDigits( 0 );
	m_spinCtrlDoubleAlphaLaylineDampFactor->SetToolTip( _("The layline damping factor determines how fast the  laylines react on your course changes, i.e. your COG changes.\n Low values mean high damping.") );

	fgSizer15->Add( m_spinCtrlDoubleAlphaLaylineDampFactor, 0, wxALIGN_RIGHT|wxALL, 0 );

	m_staticText32 = new wxStaticText( sbSizer5->GetStaticBox(), wxID_ANY, _("Layline width damping factor [0.025-1]:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32->Wrap( -1 );
	m_staticText32->SetToolTip( _("The width of the boat laylines is based on the yawing of the boat (vertical axis), i.e. your COG changes.\nThe idea is to display the COG range where you're sailing to.\n Low values mean high damping.") );

	fgSizer15->Add( m_staticText32, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_spinCtrlDoubleAlphaDeltCoG = new wxSpinCtrlDouble( sbSizer5->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.025, 1, 0.25, 1 );
	m_spinCtrlDoubleAlphaDeltCoG->SetDigits( 3 );
	m_spinCtrlDoubleAlphaDeltCoG->SetToolTip( _("Width of the boat laylines is based on the yawing of the boat (vertical axis), i.e. your COG changes.\nThe idea is to display the range where you're sailing to.\n Low values mean high damping.") );

	fgSizer15->Add( m_spinCtrlDoubleAlphaDeltCoG, 0, wxALIGN_RIGHT|wxALL, 0 );

	m_staticText33 = new wxStaticText( sbSizer5->GetStaticBox(), wxID_ANY, _("Layline length on Chart [nm]:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33->Wrap( -1 );
	m_staticText33->SetToolTip( _("Length of the boat laylines in [nm]") );

	fgSizer15->Add( m_staticText33, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_spinCtrlDoubleLaylineLength = new wxSpinCtrlDouble( sbSizer5->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 20, 0.1, 1 );
	m_spinCtrlDoubleLaylineLength->SetDigits( 1 );
	m_spinCtrlDoubleLaylineLength->SetToolTip( _("Length of the boat laylines in [nm]") );

	fgSizer15->Add( m_spinCtrlDoubleLaylineLength, 0, wxALIGN_RIGHT|wxALL, 0 );

	m_staticText34 = new wxStaticText( sbSizer5->GetStaticBox(), wxID_ANY, _("Min. Layline Width [°]:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText34->Wrap( -1 );
	m_staticText34->SetToolTip( _("Min. width of boat laylines in degrees.") );

	fgSizer15->Add( m_staticText34, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_spinCtrlDoubleMinLaylineWidth = new wxSpinCtrlDouble( sbSizer5->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 20, 0, 1 );
	m_spinCtrlDoubleMinLaylineWidth->SetDigits( 0 );
	m_spinCtrlDoubleMinLaylineWidth->SetToolTip( _("Min. width of boat laylines in degrees.") );

	fgSizer15->Add( m_spinCtrlDoubleMinLaylineWidth, 0, wxALIGN_RIGHT|wxALL, 0 );

	m_staticText35 = new wxStaticText( sbSizer5->GetStaticBox(), wxID_ANY, _("Max. Layline Width [°]:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText35->Wrap( -1 );
	m_staticText35->SetToolTip( _("Max. width of boat laylines in degrees.") );

	fgSizer15->Add( m_staticText35, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_spinCtrlDoubleMaxLaylineWidth = new wxSpinCtrlDouble( sbSizer5->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 30, 0, 1 );
	m_spinCtrlDoubleMaxLaylineWidth->SetDigits( 0 );
	m_spinCtrlDoubleMaxLaylineWidth->SetToolTip( _("Max. width of boat laylines in degrees.") );

	fgSizer15->Add( m_spinCtrlDoubleMaxLaylineWidth, 0, wxALIGN_RIGHT|wxALL, 0 );


	sbSizer5->Add( fgSizer15, 1, wxEXPAND, 5 );


	bSizer13->Add( sbSizer5, 1, wxEXPAND, 5 );


	fgSizer14->Add( bSizer13, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindowPerformanceParameters, wxID_ANY, _("Leeway") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer17;
	fgSizer17 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer17->AddGrowableCol( 1 );
	fgSizer17->SetFlexibleDirection( wxBOTH );
	fgSizer17->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText37 = new wxStaticText( sbSizer6->GetStaticBox(), wxID_ANY, _("Boat's Leeway factor [0-20]:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText37->Wrap( -1 );
	m_staticText37->SetToolTip( _("Leeway='Drift' of boat due to heel/wind influence\nLow values mean high performance of hull\nLeeway = (LeewayFactor * Heel) / STW °;") );

	fgSizer17->Add( m_staticText37, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_spinCtrlDouble = new wxSpinCtrlDouble( sbSizer6->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 20, 10, 0.01 );
	m_spinCtrlDouble->SetDigits( 2 );
	m_spinCtrlDouble->SetToolTip( _("Leeway='Drift' of boat due to heel/wind influence\nLow values mean high performance of hull\nLeeway = (LeewayFactor * Heel) / STW °;") );

	fgSizer17->Add( m_spinCtrlDouble, 0, wxALIGN_RIGHT|wxALL, 0 );

	m_radioBtnUseHeelSensor = new wxRadioButton( sbSizer6->GetStaticBox(), wxID_ANY, _("Use Heel Sensor"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_radioBtnUseHeelSensor->SetToolTip( _("Use the internal heel sensor if available\nImportant for the correct calculation of the surface current.") );

	fgSizer17->Add( m_radioBtnUseHeelSensor, 0, wxALL, 0 );


	fgSizer17->Add( 0, 0, 1, wxEXPAND, 5 );

	m_radioBtnFixedLeeway = new wxRadioButton( sbSizer6->GetStaticBox(), wxID_ANY, _("fixed/max Leeway [°]"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtnFixedLeeway->SetToolTip( _("Dual purpose !\nIf Radiobutton is NOT set, then it's used to limit Leeway to a max value.\n If Radiobutton is set, then it fixes Leeway to this constant value.") );

	fgSizer17->Add( m_radioBtnFixedLeeway, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0 );

	m_spinCtrlDoublefixedLeeway = new wxSpinCtrlDouble( sbSizer6->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 30, 0, 1 );
	m_spinCtrlDoublefixedLeeway->SetDigits( 0 );
	fgSizer17->Add( m_spinCtrlDoublefixedLeeway, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0 );

	m_radioBtnHeelnput = new wxRadioButton( sbSizer6->GetStaticBox(), wxID_ANY, _("manual Heel input:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtnHeelnput->SetToolTip( _("If no heel sensor is available, you can create a manual 'heel polar' here.\nJust read/enter the data from a mechanical heel sensor (e.g. on compass).\nUse True Wind Speed & Angle only !\nTake care: motoring w/o sails&heel will show wrong current data !!!") );

	fgSizer17->Add( m_radioBtnHeelnput, 0, wxALL, 0 );


	sbSizer6->Add( fgSizer17, 1, wxEXPAND, 5 );


	bSizer14->Add( sbSizer6, 1, wxEXPAND, 5 );


	fgSizer14->Add( bSizer14, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindowPerformanceParameters, wxID_ANY, _("Heel") ), wxVERTICAL );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer18;
	fgSizer18 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer18->SetFlexibleDirection( wxBOTH );
	fgSizer18->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText39 = new wxStaticText( sbSizer7->GetStaticBox(), wxID_ANY, _("TWS/TWA"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText39->Wrap( -1 );
	fgSizer18->Add( m_staticText39, 0, wxALL, 5 );

	m_staticText40 = new wxStaticText( sbSizer7->GetStaticBox(), wxID_ANY, _(" 45°"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText40->SetLabelMarkup( _(" 45°") );
	m_staticText40->Wrap( -1 );
	fgSizer18->Add( m_staticText40, 0, wxALL, 5 );

	m_staticText41 = new wxStaticText( sbSizer7->GetStaticBox(), wxID_ANY, _(" 90°"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText41->Wrap( -1 );
	fgSizer18->Add( m_staticText41, 0, wxALL, 5 );

	m_staticText42 = new wxStaticText( sbSizer7->GetStaticBox(), wxID_ANY, _("135°"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText42->Wrap( -1 );
	fgSizer18->Add( m_staticText42, 0, wxALL, 5 );

	m_staticText43 = new wxStaticText( sbSizer7->GetStaticBox(), wxID_ANY, _("5 kn"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText43->Wrap( -1 );
	fgSizer18->Add( m_staticText43, 0, wxALL, 5 );

	m_spinCtrlDoubleHel5_45 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 5, 1 );
	m_spinCtrlDoubleHel5_45->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHel5_45, 0, wxALL, 0 );

	m_spinCtrlDoubleheel5_90 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 8, 1 );
	m_spinCtrlDoubleheel5_90->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleheel5_90, 0, wxALL, 0 );

	m_spinCtrlDoubleHeel5_135 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 5, 1 );
	m_spinCtrlDoubleHeel5_135->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel5_135, 0, wxALL, 0 );

	m_staticText44 = new wxStaticText( sbSizer7->GetStaticBox(), wxID_ANY, _("10 kn"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText44->Wrap( -1 );
	fgSizer18->Add( m_staticText44, 0, wxALL, 5 );

	m_spinCtrlDoubleHeel10_45 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 8, 1 );
	m_spinCtrlDoubleHeel10_45->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel10_45, 0, wxALL, 0 );

	m_spinCtrlDoubleHeel10_90 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 10, 1 );
	m_spinCtrlDoubleHeel10_90->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel10_90, 0, wxALL, 0 );

	m_spinCtrlDoubleHeel10_135 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 11, 1 );
	m_spinCtrlDoubleHeel10_135->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel10_135, 0, wxALL, 0 );

	m_staticText45 = new wxStaticText( sbSizer7->GetStaticBox(), wxID_ANY, _("15 kn"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText45->Wrap( -1 );
	fgSizer18->Add( m_staticText45, 0, wxALL, 5 );

	m_spinCtrlDoubleHeel15_45 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 25, 1 );
	m_spinCtrlDoubleHeel15_45->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel15_45, 0, wxALL, 0 );

	m_spinCtrlDoubleHeel15_90 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 20, 1 );
	m_spinCtrlDoubleHeel15_90->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel15_90, 0, wxALL, 0 );

	m_spinCtrlDoubleHeel15_135 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 13, 1 );
	m_spinCtrlDoubleHeel15_135->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel15_135, 0, wxALL, 0 );

	m_staticText46 = new wxStaticText( sbSizer7->GetStaticBox(), wxID_ANY, _("20 kn"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText46->Wrap( -1 );
	fgSizer18->Add( m_staticText46, 0, wxALL, 5 );

	m_spinCtrlDoubleHeel20_45 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 20, 1 );
	m_spinCtrlDoubleHeel20_45->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel20_45, 0, wxALL, 0 );

	m_spinCtrlDoubleHeel20_90 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 16, 1 );
	m_spinCtrlDoubleHeel20_90->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel20_90, 0, wxALL, 0 );

	m_spinCtrlDoubleHeel20_135 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 15, 1 );
	m_spinCtrlDoubleHeel20_135->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel20_135, 0, wxALL, 0 );

	m_staticText47 = new wxStaticText( sbSizer7->GetStaticBox(), wxID_ANY, _("25 kn"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText47->Wrap( -1 );
	fgSizer18->Add( m_staticText47, 0, wxALL, 5 );

	m_spinCtrlDoubleHeel25_45 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 25, 1 );
	m_spinCtrlDoubleHeel25_45->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel25_45, 0, wxALL, 0 );

	m_spinCtrlDoubleHeel25_90 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 20, 1 );
	m_spinCtrlDoubleHeel25_90->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel25_90, 0, wxALL, 0 );

	m_spinCtrlDoubleHeel25_135 = new wxSpinCtrlDouble( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 60, 20, 1 );
	m_spinCtrlDoubleHeel25_135->SetDigits( 1 );
	fgSizer18->Add( m_spinCtrlDoubleHeel25_135, 0, wxALL, 0 );


	bSizer17->Add( fgSizer18, 1, wxEXPAND, 5 );


	sbSizer7->Add( bSizer17, 1, wxEXPAND, 5 );


	bSizer16->Add( sbSizer7, 1, wxEXPAND, 5 );


	fgSizer14->Add( bSizer16, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindowPerformanceParameters, wxID_ANY, _("Current") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer20;
	fgSizer20 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer20->SetFlexibleDirection( wxBOTH );
	fgSizer20->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText48 = new wxStaticText( sbSizer8->GetStaticBox(), wxID_ANY, _("Current damping factor [1-400]:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText48->Wrap( -1 );
	m_staticText48->SetToolTip( _("Stabilizes the surface current 'arrow' in the chart overlay, bearing compass and also the numerical instruments\nLow values mean high damping") );

	fgSizer20->Add( m_staticText48, 0, wxALL, 5 );

	m_spinCtrlAlphaCurrentDir = new wxSpinCtrl( sbSizer8->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 400, 8 );
	m_spinCtrlAlphaCurrentDir->SetToolTip( _("Stabilizes the surface current 'arrow' in the chart overlay, bearing compass and also the numerical instruments\nLow values mean high damping") );

	fgSizer20->Add( m_spinCtrlAlphaCurrentDir, 0, wxALL, 0 );

	m_checkBoxCurrentOnChart = new wxCheckBox( sbSizer8->GetStaticBox(), wxID_ANY, _("Display Current on Chart (OpenGL)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxCurrentOnChart->SetToolTip( _("The default on program startup") );

	fgSizer20->Add( m_checkBoxCurrentOnChart, 0, wxALL, 5 );


	sbSizer8->Add( fgSizer20, 1, wxEXPAND, 5 );


	bSizer18->Add( sbSizer8, 1, wxEXPAND, 5 );


	fgSizer14->Add( bSizer18, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindowPerformanceParameters, wxID_ANY, _("True Wind") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_checkBoxCorrectSTWwithLeeway = new wxCheckBox( sbSizer9->GetStaticBox(), wxID_ANY, _("Correct STW with Leeway"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxCorrectSTWwithLeeway->SetToolTip( _("Apply a correction to your log speed throughout the plugin based on the calculated Leeway (via the heel sensor).\nOnly makes sense with a real heel sensor.\nMake sure your instruments do not already apply this correction !") );

	fgSizer21->Add( m_checkBoxCorrectSTWwithLeeway, 0, wxALL, 0 );

	m_checkBoxCorrectAWwithHeel = new wxCheckBox( sbSizer9->GetStaticBox(), wxID_ANY, _("Correct AWS/AWA with Heel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxCorrectAWwithHeel->SetToolTip( _("Use with care, this is normally done by the instruments themselves as soon as you have an integrated, original equipment heel sensor") );

	fgSizer21->Add( m_checkBoxCorrectAWwithHeel, 0, wxALL, 0 );

	m_checkBoxForceTrueWindCalculation = new wxCheckBox( sbSizer9->GetStaticBox(), wxID_ANY, _("Force True Wind Calculation"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxForceTrueWindCalculation->SetToolTip( _("Internally calculates True Wind data (TWS,TWA,TWD) and uses it within the whole plugin even if there is True Wind data available via NMEA.") );

	fgSizer21->Add( m_checkBoxForceTrueWindCalculation, 0, wxALL, 0 );

	m_checkBoxUseSOGforTWCalc = new wxCheckBox( sbSizer9->GetStaticBox(), wxID_ANY, _("Use SOG instead of STW for True Wind Calc."), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxUseSOGforTWCalc->SetToolTip( _("Recommended. As True Wind blows over the earth surface, we should calc. it with Speed Over Ground.\nEliminates the influence of currents.") );

	fgSizer21->Add( m_checkBoxUseSOGforTWCalc, 0, wxALL, 0 );

	m_checkBoxShowWindbarbOnChart = new wxCheckBox( sbSizer9->GetStaticBox(), wxID_ANY, _("Show Wind Barb on Chart (OpenGL)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxShowWindbarbOnChart->SetToolTip( _("The default on program startup") );

	fgSizer21->Add( m_checkBoxShowWindbarbOnChart, 0, wxALL, 0 );


	sbSizer9->Add( fgSizer21, 1, wxEXPAND, 5 );


	bSizer19->Add( sbSizer9, 1, wxEXPAND, 5 );


	fgSizer14->Add( bSizer19, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindowPerformanceParameters, wxID_ANY, _("Polar") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer22;
	fgSizer22 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer22->AddGrowableCol( 1 );
	fgSizer22->SetFlexibleDirection( wxBOTH );
	fgSizer22->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText49 = new wxStaticText( sbSizer10->GetStaticBox(), wxID_ANY, _("Polar:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText49->Wrap( -1 );
	fgSizer22->Add( m_staticText49, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_textCtrlPolar = new wxTextCtrl( sbSizer10->GetStaticBox(), wxID_ANY, _("NULL"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer22->Add( m_textCtrlPolar, 0, wxALL|wxEXPAND, 5 );

	m_buttonLoadPolar = new wxButton( sbSizer10->GetStaticBox(), wxID_ANY, _("Load"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer22->Add( m_buttonLoadPolar, 0, wxALL, 5 );

	m_checkBoxShowPolarOnChart = new wxCheckBox( sbSizer10->GetStaticBox(), wxID_ANY, _("Show Polar on Chart (OpenGL)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxShowPolarOnChart->SetValue(true);
	m_checkBoxShowPolarOnChart->SetToolTip( _("The default on program startup") );

	fgSizer22->Add( m_checkBoxShowPolarOnChart, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	sbSizer10->Add( fgSizer22, 1, wxEXPAND, 5 );


	bSizer20->Add( sbSizer10, 1, wxEXPAND, 5 );


	fgSizer14->Add( bSizer20, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer11;
	sbSizer11 = new wxStaticBoxSizer( new wxStaticBox( m_scrolledWindowPerformanceParameters, wxID_ANY, _("Export NMEA Performance Data") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer201;
	fgSizer201 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer201->SetFlexibleDirection( wxBOTH );
	fgSizer201->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_radioBtnExpNKE = new wxRadioButton( sbSizer11->GetStaticBox(), wxID_ANY, _("NKE format ($PNKEP)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtnExpNKE->SetToolTip( _("Currently only set up for NKE instruments. Exports a predefined set of up to 5 NMEA records which are 'known' by NKE instruments and can be displayed there.\nRead the manual how to set up the interface connection !") );

	fgSizer201->Add( m_radioBtnExpNKE, 0, wxALL, 5 );


	fgSizer201->Add( 0, 0, 1, wxEXPAND, 5 );

	m_checkBoxExpPerfData01 = new wxCheckBox( sbSizer11->GetStaticBox(), wxID_ANY, _("Target Polar Speed"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer201->Add( m_checkBoxExpPerfData01, 0, wxALL, 5 );

	m_checkBoxExpPerfData02 = new wxCheckBox( sbSizer11->GetStaticBox(), wxID_ANY, _("CoG on other Tack"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer201->Add( m_checkBoxExpPerfData02, 0, wxALL, 5 );

	m_checkBoxExpPerfData03 = new wxCheckBox( sbSizer11->GetStaticBox(), wxID_ANY, _("Target-"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer201->Add( m_checkBoxExpPerfData03, 0, wxALL, 5 );

	m_checkBoxExpPerfData04 = new wxCheckBox( sbSizer11->GetStaticBox(), wxID_ANY, _("Diff. angle to Target"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer201->Add( m_checkBoxExpPerfData04, 0, wxALL, 5 );

	m_checkBoxExpPerfData05 = new wxCheckBox( sbSizer11->GetStaticBox(), wxID_ANY, _("Current Direction + Speed"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer201->Add( m_checkBoxExpPerfData05, 0, wxALL, 5 );


	sbSizer11->Add( fgSizer201, 1, wxEXPAND, 5 );


	bSizer21->Add( sbSizer11, 1, wxEXPAND, 5 );


	fgSizer14->Add( bSizer21, 1, wxEXPAND, 5 );


	m_scrolledWindowPerformanceParameters->SetSizer( fgSizer14 );
	m_scrolledWindowPerformanceParameters->Layout();
	fgSizer14->Fit( m_scrolledWindowPerformanceParameters );
	m_notebookPreferences->AddPage( m_scrolledWindowPerformanceParameters, _("Performance Parameters"), true );
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
	m_notebookPreferences->AddPage( m_panelAbout, _("About"), false );

	bSizer12->Add( m_notebookPreferences, 1, wxEXPAND | wxALL, 5 );


	fgSizer13->Add( bSizer12, 1, 0, 5 );

	wxBoxSizer* bSizerOKCancelApply;
	bSizerOKCancelApply = new wxBoxSizer( wxHORIZONTAL );

	m_buttonApply = new wxButton( this, wxID_ANY, _("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerOKCancelApply->Add( m_buttonApply, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();

	bSizerOKCancelApply->Add( m_sdbSizer1, 1, wxEXPAND, 5 );


	fgSizer13->Add( bSizerOKCancelApply, 1, wxALIGN_RIGHT, 5 );


	bSizer1->Add( fgSizer13, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_listCtrlTactics->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( TacticsPreferencesDialogDef::OnTacticsSelected ), NULL, this );
	m_listCtrlTactics->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( TacticsPreferencesDialogDef::OnTacticsSelected ), NULL, this );
	m_bpButtonAddTactics->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnTacticsAdd ), NULL, this );
	m_bpButtonDeleteTactics->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnTacticsDelete ), NULL, this );
	m_listCtrlInstruments->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( TacticsPreferencesDialogDef::OnInstrumentSelected ), NULL, this );
	m_listCtrlInstruments->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( TacticsPreferencesDialogDef::OnInstrumentSelected ), NULL, this );
	m_buttonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnInstrumentAdd ), NULL, this );
	m_buttonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnInstrumentDelete ), NULL, this );
	m_buttonUp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnInstrumentUp ), NULL, this );
	m_buttonDown->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnInstrumentDown ), NULL, this );
	m_radioBtnFixedLeeway->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnManualHeelUpdate ), NULL, this );
	m_radioBtnHeelnput->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnManualHeelUpdate ), NULL, this );
	m_buttonApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::ApplyPrefs ), NULL, this );
}

TacticsPreferencesDialogDef::~TacticsPreferencesDialogDef()
{
	// Disconnect Events
	m_listCtrlTactics->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( TacticsPreferencesDialogDef::OnTacticsSelected ), NULL, this );
	m_listCtrlTactics->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( TacticsPreferencesDialogDef::OnTacticsSelected ), NULL, this );
	m_bpButtonAddTactics->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnTacticsAdd ), NULL, this );
	m_bpButtonDeleteTactics->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnTacticsDelete ), NULL, this );
	m_listCtrlInstruments->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( TacticsPreferencesDialogDef::OnInstrumentSelected ), NULL, this );
	m_listCtrlInstruments->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( TacticsPreferencesDialogDef::OnInstrumentSelected ), NULL, this );
	m_buttonAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnInstrumentAdd ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnInstrumentDelete ), NULL, this );
	m_buttonUp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnInstrumentUp ), NULL, this );
	m_buttonDown->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnInstrumentDown ), NULL, this );
	m_radioBtnFixedLeeway->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnManualHeelUpdate ), NULL, this );
	m_radioBtnHeelnput->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( TacticsPreferencesDialogDef::OnManualHeelUpdate ), NULL, this );
	m_buttonApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TacticsPreferencesDialogDef::ApplyPrefs ), NULL, this );

}

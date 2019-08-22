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

#include "TacticsPreferencesDialogImpl.h"
#include "version.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

TacticsPreferencesDialogImpl::TacticsPreferencesDialogImpl( wxWindow* parent )
:
TacticsPreferencesDialogDef( parent )
{
    wxDialog::SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);
    
    m_staticTextNameVal->SetLabel( wxT("Tactics Plugin") );
    m_staticTextMajorVal->SetLabel(wxString::Format(wxT("%i"), PLUGIN_VERSION_MAJOR ));
    m_staticTextMinorVal->SetLabel(wxString::Format(wxT("%i"), PLUGIN_VERSION_MINOR ));
    m_staticTextPatchVal->SetLabel( wxT(TOSTRING(PLUGIN_VERSION_PATCH)) );
    m_staticTextDateVal->SetLabel( wxT(TOSTRING(PLUGIN_VERSION_DATE)) );
    m_staticTextOCPNAPIVersionMajorVal->SetLabel( wxT(TOSTRING(OCPN_API_VERSION_MAJOR)) );
    m_staticTextOCPNAPIVersionMinorVal->SetLabel( wxT(TOSTRING(OCPN_API_VERSION_MINOR)) );
    
    Fit();
    
    SetMinSize( GetBestSize() );
    
}

void TacticsPreferencesDialogImpl::OnTacticsPreferencesOKClick( wxCommandEvent& event )
{
    //SaveChanges(); // write changes to globals and update config
    Show( false );
#ifdef __WXOSX__    
    EndModal(wxID_OK);
#endif
    // SetClientSize(m_defaultClientSize);  // only needed if you have dynamic sizing in the dialog
    
    //ResetGlobalLocale();
    event.Skip();
}

void TacticsPreferencesDialogImpl::OnTacticsPreferencesCancelClick( wxCommandEvent& event )
{
    Show( false );
    #ifdef __WXOSX__    
    EndModal(wxID_CANCEL);
    #endif
    // SetClientSize(m_defaultClientSize);  // only needed if you have dynamic sizing in the dialog
    
    //ResetGlobalLocale();
    event.Skip();
}

void TacticsPreferencesDialogImpl::OnTacticsPreferencesApplyClick( wxCommandEvent& event )
{
    //SaveChanges(); // write changes to globals and update config
    
    event.Skip();
}




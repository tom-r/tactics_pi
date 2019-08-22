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
#include "icons.h"
#include "version.h"
#include "instrument.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

extern tactics_pi *g_tactics_pi;
extern int g_iDashSpeedMax;
extern int g_iDashCOGDamp;
extern int g_iDashSpeedUnit;
extern int g_iDashSOGDamp;
extern int g_iDashDepthUnit;
extern int g_iDashDistanceUnit;  //0="Nautical miles", 1="Statute miles", 2="Kilometers", 3="Meters"
extern int g_iDashWindSpeedUnit; //0="Kts", 1="mph", 2="km/h", 3="m/s"
//TR
extern double g_dalphaDeltCoG;
extern double g_dalphaLaylinedDampFactor;
extern double g_dLeewayFactor;
extern double g_dfixedLeeway;
extern double g_dalpha_currdir;
extern int g_iMinLaylineWidth;
extern int g_iMaxLaylineWidth;
extern double g_dLaylineLengthonChart;
extern bool g_bDisplayCurrentOnChart;
extern wxString g_path_to_PolarFile;
extern double g_dheel[6][5];
extern bool g_bUseHeelSensor;
extern bool g_bUseFixedLeeway;
extern bool g_bManHeelInput;
extern bool g_bCorrectSTWwithLeeway;  //if true STW is corrected with Leeway (in case Leeway is available)
extern bool g_bCorrectAWwithHeel;    //if true, AWS/AWA will be corrected with Heel-Angle
extern bool g_bForceTrueWindCalculation;    //if true, NMEA Data for TWS,TWA,TWD is not used, but the plugin calculated data is used
extern bool g_bUseSOGforTWCalc; //if true, use SOG instead of STW to calculate TWS,TWA,TWD
extern bool g_bShowWindbarbOnChart;
extern bool g_bShowPolarOnChart;
extern bool g_bExpPerfData01;
extern bool g_bExpPerfData02;
extern bool g_bExpPerfData03;
extern bool g_bExpPerfData04;
extern bool g_bExpPerfData05;
extern wxString g_sCMGSynonym, g_sVMGSynonym;
extern wxString g_sDataExportSeparator;



TacticsPreferencesDialogImpl::TacticsPreferencesDialogImpl( wxWindow* parent )
:
TacticsPreferencesDialogDef( parent )
{
    wxDialog::SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);
    
    wxImageList *imglist1 = new wxImageList(32, 32, true, 1);
    imglist1->Add(*_img_tactics_pi);
    
    
    m_listCtrlTactics->AssignImageList(imglist1, wxIMAGE_LIST_SMALL);
    m_bpButtonAddTactics->SetBitmap(*_img_plus);
    m_bpButtonDeleteTactics->SetBitmap(*_img_minus);
    
    m_staticTextNameVal->SetLabel( wxT("Tactics Plugin") );
    m_staticTextMajorVal->SetLabel(wxString::Format(wxT("%i"), PLUGIN_VERSION_MAJOR ));
    m_staticTextMinorVal->SetLabel(wxString::Format(wxT("%i"), PLUGIN_VERSION_MINOR ));
    m_staticTextPatchVal->SetLabel( wxT(TOSTRING(PLUGIN_VERSION_PATCH)) );
    m_staticTextDateVal->SetLabel( wxT(TOSTRING(PLUGIN_VERSION_DATE)) );
    m_staticTextOCPNAPIVersionMajorVal->SetLabel( wxT(TOSTRING(OCPN_API_VERSION_MAJOR)) );
    m_staticTextOCPNAPIVersionMinorVal->SetLabel( wxT(TOSTRING(OCPN_API_VERSION_MINOR)) );
    
    curSel = -1;
    for (size_t i = 0; i < m_Config.GetCount(); i++) {
        m_listCtrlTactics->InsertItem(i, 0);
        // Using data to store m_Config index for managing deletes
        m_listCtrlTactics->SetItemData(i, i);
    }
    m_listCtrlTactics->SetColumnWidth(0, wxLIST_AUTOSIZE);
    //itemPanelNotebook03->SetSize(itemBoxSizer06->GetSize());
    UpdateTacticsButtonsState();
    UpdateButtonsState();
    
    Fit();
    
    SetMinSize( GetBestSize() );
    
}

void TacticsPreferencesDialogImpl::OnTacticsSelected( wxListEvent& event )
{
    // save changes
    SaveTacticsConfig();
    UpdateTacticsButtonsState();
}

void TacticsPreferencesDialogImpl::OnTacticsAdd( wxCommandEvent& event )
{
    int idx = m_listCtrlTactics->GetItemCount();
    m_listCtrlTactics->InsertItem(idx, 0);
    // Data is index in m_Config
    m_listCtrlTactics->SetItemData(idx, m_Config.GetCount());
    wxArrayInt ar;
    TacticsWindowContainer *dwc = new TacticsWindowContainer(NULL, GetUUID(), _("Tactics"), _T("V"), ar);
    dwc->m_bIsVisible = true;
    m_Config.Add(dwc);
}

void TacticsPreferencesDialogImpl::OnTacticsDelete( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_listCtrlTactics->GetNextItem(itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    int idx = m_listCtrlTactics->GetItemData(itemID);
    m_listCtrlTactics->DeleteItem(itemID);
    m_Config.Item(idx)->m_bIsDeleted = true;
    UpdateTacticsButtonsState();
}

void TacticsPreferencesDialogImpl::OnInstrumentSelected( wxListEvent& event )
{
    UpdateButtonsState();
}

void TacticsPreferencesDialogImpl::OnInstrumentAdd( wxCommandEvent& event )
{
    AddInstrumentDlg pdlg((wxWindow *)event.GetEventObject(), wxID_ANY);
    
    if (pdlg.ShowModal() == wxID_OK) {
        wxListItem item;
        getListItemForInstrument(item, pdlg.GetInstrumentAdded());
        item.SetId(m_listCtrlInstruments->GetItemCount());
        m_listCtrlInstruments->InsertItem(item);
        m_listCtrlInstruments->SetColumnWidth(0, wxLIST_AUTOSIZE);
        UpdateButtonsState();
    }
}

void TacticsPreferencesDialogImpl::OnInstrumentDelete( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_listCtrlInstruments->GetNextItem(itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    m_listCtrlInstruments->DeleteItem(itemID);
    UpdateButtonsState();
}

void TacticsPreferencesDialogImpl::OnInstrumentUp( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_listCtrlInstruments->GetNextItem(itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    wxListItem item;
    item.SetId(itemID);
    item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE | wxLIST_MASK_DATA);
    m_listCtrlInstruments->GetItem(item);
    item.SetId(itemID - 1);
    m_listCtrlInstruments->DeleteItem(itemID);
    m_listCtrlInstruments->InsertItem(item);
    m_listCtrlInstruments->SetItemState(itemID - 1, wxLIST_STATE_SELECTED,
                                         wxLIST_STATE_SELECTED);
    UpdateButtonsState();
}

void TacticsPreferencesDialogImpl::OnInstrumentDown( wxCommandEvent& event )
{
    long itemID = -1;
    itemID = m_listCtrlInstruments->GetNextItem(itemID, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    wxListItem item;
    item.SetId(itemID);
    item.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE | wxLIST_MASK_DATA);
    m_listCtrlInstruments->GetItem(item);
    item.SetId(itemID + 1);
    m_listCtrlInstruments->DeleteItem(itemID);
    m_listCtrlInstruments->InsertItem(item);
    m_listCtrlInstruments->SetItemState(itemID + 1, wxLIST_STATE_SELECTED,
                                         wxLIST_STATE_SELECTED);
    UpdateButtonsState();
}

void TacticsPreferencesDialogImpl::OnManualHeelUpdate( wxCommandEvent& event )
{
    
}

void TacticsPreferencesDialogImpl::ApplyPrefs( wxCommandEvent& event )
{
    //wxLogMessage("Apply");
    SaveTacticsConfig();
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

void TacticsPreferencesDialogImpl::SaveTacticsConfig()
{
    g_iDashSpeedMax = m_spinCtrlSpeedMax->GetValue();
    g_iDashCOGDamp = m_spinCtrlCOGDamp->GetValue();
    g_iDashSOGDamp = m_spinCtrlSOGDamp->GetValue();
    g_iDashSpeedUnit = m_choiceSpeedUnit->GetSelection() - 1;
    g_iDashDepthUnit = m_choiceDepthUnit->GetSelection() + 3;
    g_iDashDistanceUnit = m_choiceDistanceUnit->GetSelection() - 1;
    g_iDashWindSpeedUnit = m_choiceWindSpeedUnit->GetSelection();
    g_dLeewayFactor = m_spinCtrlDoubleLeewayFactor->GetValue();
    g_dfixedLeeway = m_radioBtnFixedLeeway->GetValue();
    
    g_dalpha_currdir = (double)m_spinCtrlAlphaCurrDir->GetValue() / 1000.0;
    //    g_dalpha_currdir = m_AlphaCurrDir->GetValue();
    g_dalphaDeltCoG = m_spinCtrlDoubleAlphaDeltCoG->GetValue();
    g_dalphaLaylinedDampFactor = m_spinCtrlDoubleAlphaLaylineDampFactor->GetValue();
    g_dLaylineLengthonChart = m_spinCtrlDoubleLaylineLength->GetValue();
    g_iMinLaylineWidth = m_spinCtrlDoubleMinLaylineWidth->GetValue();
    g_iMaxLaylineWidth = m_spinCtrlDoubleMaxLaylineWidth->GetValue();
    g_bDisplayCurrentOnChart = m_checkBoxCurrentOnChart->GetValue();
    g_dheel[1][1] = m_spinCtrlDoubleHeel5_45->GetValue();
    g_dheel[1][2] = m_spinCtrlDoubleHeel5_90->GetValue();
    g_dheel[1][3] = m_spinCtrlDoubleHeel5_135->GetValue();
    g_dheel[2][1] = m_spinCtrlDoubleHeel10_45->GetValue();
    g_dheel[2][2] = m_spinCtrlDoubleHeel10_90->GetValue();
    g_dheel[2][3] = m_spinCtrlDoubleHeel10_135->GetValue();
    g_dheel[3][1] = m_spinCtrlDoubleHeel15_45->GetValue();
    g_dheel[3][2] = m_spinCtrlDoubleHeel15_90->GetValue();
    g_dheel[3][3] = m_spinCtrlDoubleHeel15_135->GetValue();
    g_dheel[4][1] = m_spinCtrlDoubleHeel20_45->GetValue();
    g_dheel[4][2] = m_spinCtrlDoubleHeel20_90->GetValue();
    g_dheel[4][3] = m_spinCtrlDoubleHeel20_135->GetValue();
    g_dheel[5][1] = m_spinCtrlDoubleHeel25_45->GetValue();
    g_dheel[5][2] = m_spinCtrlDoubleHeel25_90->GetValue();
    g_dheel[5][3] = m_spinCtrlDoubleHeel25_135->GetValue();
    
    g_bUseHeelSensor = m_radioBtnUseHeelSensor->GetValue();
    g_bUseFixedLeeway = m_radioBtnFixedLeeway->GetValue();
    g_bManHeelInput = m_radioBtnHeelnput->GetValue();
    g_path_to_PolarFile = m_textCtrlPolar->GetValue();
    g_bCorrectSTWwithLeeway = m_checkBoxCorrectSTWwithLeeway->GetValue();
    g_bCorrectAWwithHeel = m_checkBoxCorrectAWwithHeel->GetValue();
    g_bForceTrueWindCalculation = m_checkBoxForceTrueWindCalculation->GetValue();
    g_bUseSOGforTWCalc = m_checkBoxUseSOGforTWCalc->GetValue();
    g_bShowWindbarbOnChart = m_checkBoxShowWindbarbOnChart->GetValue();
    g_bShowPolarOnChart = m_checkBoxShowPolarOnChart->GetValue();
    g_bExpPerfData01 = m_checkBoxExpPerfData01->GetValue();
    g_bExpPerfData02 = m_checkBoxExpPerfData02->GetValue();
    g_bExpPerfData03 = m_checkBoxExpPerfData03->GetValue();
    g_bExpPerfData04 = m_checkBoxExpPerfData04->GetValue();
    g_bExpPerfData05 = m_checkBoxExpPerfData05->GetValue();
    if (curSel != -1) {
        TacticsWindowContainer *cont = m_Config.Item(curSel);
        cont->m_bIsVisible = m_checkBoxIsVisible->IsChecked();
        cont->m_sCaption = m_textCtrlCaption->GetValue();
        cont->m_sOrientation = m_choiceOrientation->GetSelection() == 0 ? _T("V") : _T("H");
        cont->m_aInstrumentList.Clear();
        for (int i = 0; i < m_listCtrlInstruments->GetItemCount(); i++)
            cont->m_aInstrumentList.Add((int)m_listCtrlInstruments->GetItemData(i));
    }
}

void TacticsPreferencesDialogImpl::UpdateTacticsButtonsState()
{
    long item = -1;
    item = m_listCtrlTactics->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    bool enable = (item != -1);
    
    //  Disable the Tactics Delete button if the parent(Tactics) of this dialog is selected.
    bool delete_enable = enable;
    if (item != -1) {
        int sel = m_listCtrlTactics->GetItemData(item);
        TacticsWindowContainer *cont = m_Config.Item(sel);
        TacticsWindow *dash_sel = cont->m_pTacticsWindow;
        if (dash_sel == GetParent())
            delete_enable = false;
    }
    m_bpButtonDeleteTactics->Enable(delete_enable);
    
    m_panelTactics->Enable(enable);
    
    if (item != -1) {
        curSel = m_listCtrlTactics->GetItemData(item);
        TacticsWindowContainer *cont = m_Config.Item(curSel);
        m_checkBoxIsVisible->SetValue(cont->m_bIsVisible);
        m_textCtrlCaption->SetValue(cont->m_sCaption);
        m_choiceOrientation->SetSelection(cont->m_sOrientation == _T("V") ? 0 : 1);
        m_listCtrlInstruments->DeleteAllItems();
        for (size_t i = 0; i < cont->m_aInstrumentList.GetCount(); i++) {
            wxListItem item;
            getListItemForInstrument(item, cont->m_aInstrumentList.Item(i));
            item.SetId(m_listCtrlInstruments->GetItemCount());
            m_listCtrlInstruments->InsertItem(item);
        }
        
        m_listCtrlInstruments->SetColumnWidth(0, wxLIST_AUTOSIZE);
    }
    else {
        curSel = -1;
        m_checkBoxIsVisible->SetValue(false);
        m_textCtrlCaption->SetValue(_T(""));
        m_choiceOrientation->SetSelection(0);
        m_listCtrlInstruments->DeleteAllItems();
    }
    //      UpdateButtonsState();
}

void TacticsPreferencesDialogImpl::getListItemForInstrument(wxListItem &item, unsigned int id)
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

wxString TacticsPreferencesDialogImpl::getInstrumentCaption(unsigned int id)
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

void TacticsPreferencesDialogImpl::UpdateButtonsState()
{
    long item = -1;
    item = m_listCtrlInstruments->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    bool enable = (item != -1);
    
    m_buttonDelete->Enable(enable);
    //    m_pButtonEdit->Enable( false ); // TODO: Properties
    m_buttonUp->Enable(item > 0);
    m_buttonDown->Enable(item != -1 && item < m_listCtrlInstruments->GetItemCount() - 1);
}

/*  These two function were taken from gpxdocument.cpp */
int TacticsPreferencesDialogImpl::GetRandomNumber(int range_min, int range_max)
{
    long u = (long)wxRound(((double)rand() / ((double)(RAND_MAX)+1) * (range_max - range_min)) + range_min);
    return (int)u;
}


// RFC4122 version 4 compliant random UUIDs generator.
wxString TacticsPreferencesDialogImpl::GetUUID(void)
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



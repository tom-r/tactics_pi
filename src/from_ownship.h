/***************************************************************************
 * $Id: from_ownship.h
 *
 * Project:  OpenCPN
 * Purpose:  Tactics Plugin
 * Author:   Pavel Kalian
 *
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

#ifndef _FROM_OWNSHIP_H_
#define _FROM_OWNSHIP_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "instrument.h"

class TacticsInstrument_FromOwnship : public TacticsInstrument
{
public:
    TacticsInstrument_FromOwnship(wxWindow *pparent, wxWindowID id, wxString title, int cap_flag1=0, int cap_flag2=0,int cap_flag3=OCPN_DBP_STC_LAT,int cap_flag4=OCPN_DBP_STC_LON);
    ~TacticsInstrument_FromOwnship(){}

    void SetData(int st, double data, wxString unit);
    wxSize GetSize( int orient, wxSize hint );
      
protected:
    wxString          m_data1;
    wxString          m_data2;
    double            c_lat;
    double            c_lon;
    double            s_lat;
    double            s_lon;
    int               m_cap_flag1;
    int               m_cap_flag2;
    int               m_cap_flag3;
    int               m_cap_flag4;
    int               m_DataHeight;

    void Draw(myDC* dc);
};

#endif

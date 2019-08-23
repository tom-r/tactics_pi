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

include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers
#include <wx/textwrapper.h>
 
#include "TacticsUtils.h"

// International helper functions
void SetGlobalLocale( void )
{
#ifndef __WXMSW__
    if(g_iLocaleDepth == 0) { 
        g_ODlocale = new wxString(wxSetlocale(LC_NUMERIC, NULL));
#if wxCHECK_VERSION(3,0,0)        
        wxSetlocale(LC_NUMERIC, "");
#else
        setlocale(LC_NUMERIC, "");
#endif
    }
    g_iLocaleDepth++;
#endif
}

void ResetGlobalLocale( void )
{
#ifndef __WXMSW__
    g_iLocaleDepth--;
    if(g_iLocaleDepth < 0) 
        g_iLocaleDepth = 0;
    if(g_iLocaleDepth == 0 && g_ODlocale) {
#if wxCHECK_VERSION(3,0,0)        
        wxSetlocale(LC_NUMERIC, g_ODlocale->ToAscii());
#else
        setlocale(LC_NUMERIC, g_ODlocale->ToAscii());
#endif
        delete g_ODlocale;
        g_ODlocale = NULL;
    } 
#endif
}

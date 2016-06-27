/******************************************************************************
 * $Id: instrument.cpp, v1.0 2016/06/07 tom_BigSpeedy Exp $
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


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "instrument.h"
#include "wx28compat.h"

//----------------------------------------------------------------
//
//    Generic TacticsInstrument Implementation
//
//----------------------------------------------------------------

TacticsInstrument::TacticsInstrument(wxWindow *pparent, wxWindowID id, wxString title, int cap_flag)
      :wxControl(pparent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
      m_title = title;
      m_cap_flag = cap_flag;

      SetBackgroundStyle( wxBG_STYLE_CUSTOM );
      SetDrawSoloInPane(false);
      wxClientDC dc(this);
      int width;
      dc.GetTextExtent(m_title, &width, &m_TitleHeight, 0, 0, g_pFontTitle);

      Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(TacticsInstrument::OnEraseBackground));
      Connect(wxEVT_PAINT, wxPaintEventHandler(TacticsInstrument::OnPaint));
      
      //  On OSX, there is an orphan mouse event that comes from the automatic
      //  exEVT_CONTEXT_MENU synthesis on the main wxWindow mouse handler.
      //  The event goes to an instrument window (here) that may have been deleted by the
      //  preferences dialog.  Result is NULL deref.
      //  Solution:  Handle right-click here, and DO NOT skip()
      //  Strangely, this does not work for GTK...
      //  See: http://trac.wxwidgets.org/ticket/15417
      
#ifdef __WXOSX__
      Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(TacticsInstrument::MouseEvent), NULL, this);
#endif      
}

void TacticsInstrument::MouseEvent( wxMouseEvent &event )
{
    if ( event.GetEventType() == wxEVT_RIGHT_DOWN )
    {
       wxContextMenuEvent evtCtx(wxEVT_CONTEXT_MENU,
                                  this->GetId(),
                                  this->ClientToScreen(event.GetPosition()));
        evtCtx.SetEventObject(this);
        GetParent()->GetEventHandler()->AddPendingEvent(evtCtx );
    }
}

int TacticsInstrument::GetCapacity()
{
      return m_cap_flag;
}
void TacticsInstrument::SetDrawSoloInPane(bool value)
{
    m_drawSoloInPane = value;
}
void TacticsInstrument::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
        // intentionally empty
}

void TacticsInstrument::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxBufferedPaintDC pdc( this );
    if( !pdc.IsOk() ) {
        wxLogMessage( _T("TacticsInstrument::OnPaint() fatal: wxBufferedPaintDC.IsOk() false.") );
        return;
    }

    wxSize size = GetClientSize();
    if( size.x == 0 || size.y == 0 ) {
        wxLogMessage( _T("TacticsInstrument::OnPaint() fatal: Zero size DC.") );
        return;
    }

//    wxBitmap bm( size.x, size.y, 32 );
//#if !wxCHECK_VERSION(2,9,4)
//    bm.UseAlpha();
//#endif
//    wxMemoryDC mdc( bm );
#if wxUSE_GRAPHICS_CONTEXT
    wxGCDC dc( pdc );
#else
    wxDC &dc( pdc );
#endif

    wxColour cl;
    GetGlobalColor( _T("DASHB"), &cl );
    dc.SetBackground( cl );
    dc.Clear();

    Draw( &dc );

    if(!m_drawSoloInPane) {
//        mdc.SelectObject( wxNullBitmap );
//        pdc.DrawBitmap( bm, 0, 0, false );
//    }
//    else {

    //  Windows GCDC does a terrible job of rendering small texts
    //  Workaround by using plain old DC for title box if text size is too small
#ifdef __WXMSW__
        if( g_pFontTitle->GetPointSize() > 12 )
#endif
        {
            wxPen pen;
            pen.SetStyle( wxPENSTYLE_SOLID );
            GetGlobalColor( _T("DASHL"), &cl );
            pen.SetColour( cl );
            dc.SetPen( pen );
            dc.SetBrush( cl );
            dc.DrawRoundedRectangle( 0, 0, size.x, m_TitleHeight, 3 );

            dc.SetFont( *g_pFontTitle );
            GetGlobalColor( _T("DASHF"), &cl );
            dc.SetTextForeground( cl );
            dc.DrawText( m_title, 5, 0 );

            //mdc.SelectObject( wxNullBitmap );
            //pdc.DrawBitmap( bm, 0, 0, false );
        }

#ifdef __WXMSW__
        if( g_pFontTitle->GetPointSize() <= 12 ) {
            //mdc.SelectObject( wxNullBitmap );           // the instrument body
            //pdc.DrawBitmap( bm, 0, 0, false );

            //wxBitmap tbm( size.x, m_TitleHeight, -1 );
            //wxMemoryDC tdc( tbm );
            wxColour cl;
            GetGlobalColor( _T("DASHB"), &cl );
            pdc.SetBrush(cl);
            pdc.DrawRectangle(0, 0, size.x, m_TitleHeight);

            wxPen pen;
            pen.SetStyle( wxSOLID );
            GetGlobalColor( _T("DASHL"), &cl );
            pen.SetColour( cl );
            pdc.SetPen( pen );
            pdc.SetBrush( cl );
            pdc.DrawRoundedRectangle( 0, 0, size.x, m_TitleHeight, 3 );

            pdc.SetFont( *g_pFontTitle );
            GetGlobalColor( _T("DASHF"), &cl );
            pdc.SetTextForeground( cl );
            pdc.DrawText( m_title, 5, 0 );
//            tdc.SelectObject( wxNullBitmap );
//            pdc.DrawBitmap( tbm, 0, 0, false );
        }
#endif
    }
}

//----------------------------------------------------------------
//
//    TacticsInstrument_Simple Implementation
//
//----------------------------------------------------------------

TacticsInstrument_Single::TacticsInstrument_Single(wxWindow *pparent, wxWindowID id, wxString title, int cap_flag, wxString format)
      :TacticsInstrument(pparent, id, title, cap_flag)
{
      m_format = format;
      m_data = _T("---");
}

wxSize TacticsInstrument_Single::GetSize( int orient, wxSize hint )
{
      wxClientDC dc(this);
      int w;
      dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
      dc.GetTextExtent(_T("000"), &w, &m_DataHeight, 0, 0, g_pFontData);

      if( orient == wxHORIZONTAL ) {
          return wxSize( DefaultWidth, wxMax(hint.y, m_TitleHeight+m_DataHeight) );
      } else {
          return wxSize( wxMax(hint.x, DefaultWidth), m_TitleHeight+m_DataHeight );
      }
}

void TacticsInstrument_Single::Draw(wxGCDC* dc)
{
      wxColour cl;
#ifdef __WXMSW__
      wxBitmap tbm( dc->GetSize().x, m_DataHeight, -1 );
      wxMemoryDC tdc( tbm );
      wxColour c2;
      GetGlobalColor( _T("DASHB"), &c2 );
      tdc.SetBackground( c2 );
      tdc.Clear();

      tdc.SetFont(*g_pFontData );
      GetGlobalColor( _T("DASHF"), &cl );
      tdc.SetTextForeground( cl );

      tdc.DrawText(m_data, 10, 0);

      tdc.SelectObject( wxNullBitmap );

      dc->DrawBitmap(tbm, 0, m_TitleHeight, false);
#else
      dc->SetFont(*g_pFontData );
      GetGlobalColor( _T("DASHF"), &cl );
      dc->SetTextForeground( cl );

      dc->DrawText(m_data, 10, m_TitleHeight);

#endif

}

void TacticsInstrument_Single::SetData(int st, double data, wxString unit)
{
      if (m_cap_flag & st){
            if(!wxIsNaN(data)){
                if (unit == _T("C"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_T("C");
                else if (unit == _T("\u00B0"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN;
                else if (unit == _T("\u00B0T"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_(" true");
                else if (unit == _T("\u00B0M"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_(" mag");
                else if (unit == _T("\u00B0L"))
                  m_data = _T(">")+ wxString::Format(m_format, data)+DEGREE_SIGN;
                else if (unit == _T("\u00B0R"))
                  m_data = wxString::Format(m_format, data)+DEGREE_SIGN+_T("<");
                else if (unit == _T("\u00B0l")){  //22.04.TR
                  if (data < 0) data = -data;     //22.04.TR
                  m_data = +_T("<") + wxString::Format(m_format, data) + DEGREE_SIGN;//22.04.TR
                }
                else if (unit == _T("\u00B0r")){//22.04.TR
                  if (data < 0) data = -data;   //22.04.TR
                  m_data = wxString::Format(m_format, data) + DEGREE_SIGN + _T(">"); //22.04.TR
                }
                else if (unit == _T("N")) //Knots
                  m_data = wxString::Format(m_format, data)+_T(" Kts");
/* maybe in the future ...
                else if (unit == _T("M")) // m/s
                  m_data = wxString::Format(m_format, data)+_T(" m/s");
                else if (unit == _T("K")) // km/h
                  m_data = wxString::Format(m_format, data)+_T(" km/h");
 ... to be completed
 */
                else
                  m_data = wxString::Format(m_format, data)+_T(" ")+unit;
            }
            else
                m_data = _T("---");
      }
}

//----------------------------------------------------------------
//
//    TacticsInstrument_Position Implementation
//
//----------------------------------------------------------------

TacticsInstrument_Position::TacticsInstrument_Position(wxWindow *pparent, wxWindowID id, wxString title, int cap_flag1, int cap_flag2)
      :TacticsInstrument(pparent, id, title, cap_flag1 | cap_flag2)
{

      m_data1 = _T("---");
      m_data2 = _T("---");
      m_cap_flag1 = cap_flag1;
      m_cap_flag2 = cap_flag2;
}

wxSize TacticsInstrument_Position::GetSize( int orient, wxSize hint )
{
      wxClientDC dc(this);
      int w;
      dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
      dc.GetTextExtent(_T("000  00.0000 W"), &w, &m_DataHeight, 0, 0, g_pFontData);

      if( orient == wxHORIZONTAL ) {
          return wxSize( w+10, wxMax(hint.y, m_TitleHeight+m_DataHeight*2) );
      } else {
          return wxSize( wxMax(hint.x, w+10), m_TitleHeight+m_DataHeight*2 );
      }
}

void TacticsInstrument_Position::Draw(wxGCDC* dc)
{
      wxColour cl;

#ifdef __WXMSW__
      wxBitmap tbm( dc->GetSize().x, m_DataHeight * 2, -1 );
      wxMemoryDC tdc( tbm );
      wxColour c2;
      GetGlobalColor( _T("DASHB"), &c2 );
      tdc.SetBackground( c2 );
      tdc.Clear();

      tdc.SetFont(*g_pFontData );
      GetGlobalColor( _T("DASHF"), &cl );
      tdc.SetTextForeground( cl );

      tdc.DrawText(m_data1, 10, 0);
      tdc.DrawText(m_data2, 10, m_DataHeight);

      tdc.SelectObject( wxNullBitmap );

      dc->DrawBitmap(tbm, 0, m_TitleHeight, false);
#else
      dc->SetFont(*g_pFontData );
      GetGlobalColor( _T("DASHF"), &cl );
      dc->SetTextForeground( cl );

      dc->DrawText(m_data1, 10, m_TitleHeight);
      dc->DrawText(m_data2, 10, m_TitleHeight + m_DataHeight);
#endif


}

void TacticsInstrument_Position::SetData(int st, double data, wxString unit)
{
      if (st == m_cap_flag1)
      {
            m_data1 = toSDMM(1, data);
            m_data1[0] = ' ';
      }
      else if (st == m_cap_flag2)
      {
            m_data2 = toSDMM(2, data);
      }
      else return;
      Refresh();
}

/**************************************************************************/
/*          Some assorted utilities                                       */
/**************************************************************************/

wxString toSDMM ( int NEflag, double a )
{
      short neg = 0;
      int d;
      long m;

      if ( a < 0.0 )
      {
            a = -a;
            neg = 1;
      }
      d = ( int ) a;
      m = ( long ) ( ( a - ( double ) d ) * 60000.0 );

      if ( neg )
            d = -d;

      wxString s;

      if ( !NEflag )
            s.Printf ( _T ( "%d %02ld.%03ld'" ), d, m / 1000, m % 1000 );
      else
      {
            if ( NEflag == 1 )
            {
                  char c = 'N';

                  if ( neg )
                  {
                        d = -d;
                        c = 'S';
                  }

                  s.Printf ( _T ( "%03d %02ld.%03ld %c" ), d, m / 1000, ( m % 1000 ), c );
            }
            else if ( NEflag == 2 )
            {
                  char c = 'E';

                  if ( neg )
                  {
                        d = -d;
                        c = 'W';
                  }
                  s.Printf ( _T ( "%03d %02ld.%03ld %c" ), d, m / 1000, ( m % 1000 ), c );
            }
      }
      return s;
}

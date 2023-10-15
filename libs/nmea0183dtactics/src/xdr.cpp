/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  NMEA0183 Support Classes
 * Author:   Thomas Rauch
 *
 ***************************************************************************
 *   Copyright (C) 2010 by Samuel R. Blackburn, David S Register           *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 *
 *   S Blackburn's original source license:                                *
 *         "You can use it any way you like."                              *
 *   More recent (2010) license statement:                                 *
 *         "It is BSD license, do with it what you will"                   *
 */

#include "nmea0183.h"

//IMPLEMENT_DYNAMIC( XDR, RESPONSE )

XDR::XDR()
{
   Mnemonic = _T("XDR");
   TransducerCnt=0;
   TransducerInfo[0].Type = wxEmptyString;
   TransducerInfo[0].Data = 0.0;
   TransducerInfo[0].Unit = wxEmptyString;
   TransducerInfo[0].Name = wxEmptyString;
   Empty();
}

XDR::~XDR()
{
   Mnemonic.Empty();
   Empty();
}

void XDR::Empty( void )
{
//   ASSERT_VALID( this );
     TransducerCnt=0;
     for (int idx = 0; idx < MaxTransducerCnt; idx++) {
         TransducerInfo[idx].Type = wxEmptyString;
         TransducerInfo[idx].Data = 0.0;
         TransducerInfo[idx].Unit = wxEmptyString;
         TransducerInfo[idx].Name = wxEmptyString;
     }
}

bool XDR::Parse( const SENTENCE& sentence )
{
//   ASSERT_VALID( this );

   /*
   ** XDR - Transducer Measurement
   **
   **        1 2   3 4            n
   **        | |   | |            |
   ** $--XDR,a,x.x,a,c--c, ..... *hh<CR><LF>
   **
   ** Field Number:
   **  1) Transducer Type
   **  2) Measurement Data
   **  3) Unit of Measurement, Celcius
   **  4) Name of transducer
   **  ...
   **  n) Checksum
   ** There may be any number of quadruplets like this, each describing a sensor. The last field will be a checksum as usual.
   */

   /*
   ** First we check the checksum...
   */
   int cksumFieldNr = 0;
   TransducerCnt = 0;
   TransducerCnt=(int)sentence.GetNumberOfDataFields()/4;
   cksumFieldNr=sentence.GetNumberOfDataFields()+1;
   if (TransducerCnt == 0 || TransducerCnt > MaxTransducerCnt) {
      SetErrorMessage( _T("Invalid Field count" ));
      return( FALSE );
   }

   if ( sentence.IsChecksumBad( cksumFieldNr ) == NTrue ) {
      SetErrorMessage( _T("Invalid Checksum" ));
      return( FALSE );
   }
    for (int idx = 0; idx < TransducerCnt; idx++)
   {
         TransducerInfo[idx].Type = sentence.Field( idx*4+1 );
         TransducerInfo[idx].Data = sentence.Double( idx*4+2 );
         TransducerInfo[idx].Unit = sentence.Field( idx*4+3 );
         TransducerInfo[idx].Name = sentence.Field( idx*4+4 );
   }

   return( TRUE );
}

bool XDR::Write( SENTENCE& sentence )
{
//   ASSERT_VALID( this );

   /*
   ** Let the parent do its thing
   */

   RESPONSE::Write( sentence );

   sentence += TransducerCnt;
   for (int idx = 0; idx < TransducerCnt; idx++) {
         sentence += TransducerInfo[idx].Type;
         sentence += TransducerInfo[idx].Data;
         sentence += TransducerInfo[idx].Unit;
         sentence += TransducerInfo[idx].Name;
         //sentence.Finish();
   }

   sentence.Finish();

   return( TRUE );
}

const XDR& XDR::operator = ( const XDR &source )
{
  if ( this != &source ) {
    TransducerCnt = source.TransducerCnt;
    for (int idx = 0; idx < TransducerCnt; idx++) {
      TransducerInfo[idx].Type = source.TransducerInfo[idx].Type;
      TransducerInfo[idx].Data = source.TransducerInfo[idx].Data;
      TransducerInfo[idx].Unit = source.TransducerInfo[idx].Unit;
      TransducerInfo[idx].Name = source.TransducerInfo[idx].Name;
    }
  }
  return *this;
}

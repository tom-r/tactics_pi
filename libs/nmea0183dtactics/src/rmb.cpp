/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  NMEA0183 Support Classes
 * Author:   Samuel R. Blackburn, David S. Register
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

/*
** Author: Samuel R. Blackburn
** CI$: 76300,326
** Internet: sammy@sed.csc.com
**
** You can use it any way you like.
*/


RMB::RMB()
{
#ifdef _TACTICSPI_H_
   isVersion2dot3OrLater = false;
#endif // _TACTICSPI_H_
   Mnemonic = _T("RMB");
   Empty();
}

RMB::~RMB()
{
   Mnemonic.Empty();
   Empty();
}

void RMB::Empty( void )
{

   IsDataValid                     = Unknown0183;
   CrossTrackError                 = 0.0;
   DirectionToSteer                = LR_Unknown;
   To.Empty();
   From.Empty();
   DestinationPosition.Empty();
   RangeToDestinationNauticalMiles = 0.0;
   BearingToDestinationDegreesTrue = 0.0;
   DestinationClosingVelocityKnots = 0.0;
   IsArrivalCircleEntered          = Unknown0183;
}

bool RMB::Parse( const SENTENCE& sentence )
{

   /*
   ** RMB - Recommended Minimum Navigation Information
   **
   */
#ifdef _TACTICSPI_H_
   /*
   **                                                             14
   **        1 2   3 4    5    6       7 8        9 10  11  12  13|  15
   **        | |   | |    |    |       | |        | |   |   |   | |  |
   ** $--RMB,A,x.x,a,c--c,c--c,llll.ll,a,yyyyy.yy,a,x.x,x.x,x.x,A,m,*hh<CR><LF>
   ** cf. https://opencpn.org/wiki/dokuwiki/doku.php?id=opencpn:opencpn_user_manual:advanced_features:nmea_sentences#rmb_-_recommended_minimum_navigation_information
   */
#else
   /*
   **                                                             14
   **        1 2   3 4    5    6       7 8        9 10  11  12  13|
   **        | |   | |    |    |       | |        | |   |   |   | |
   ** $--RMB,A,x.x,a,c--c,c--c,llll.ll,a,yyyyy.yy,a,x.x,x.x,x.x,A*hh<CR><LF>
   */
#endif // _TACTICSPI_H_
   /*
   ** Field Number:
   **  1) Status, V = Navigation receiver warning
   **  2) Cross Track error - nautical miles
   **  3) Direction to Steer, Left or Right
   **  4) TO Waypoint ID
   **  5) FROM Waypoint ID
   **  6) Destination Waypoint Latitude
   **  7) N or S
   **  8) Destination Waypoint Longitude
   **  9) E or W
   ** 10) Range to destination in nautical miles
   ** 11) Bearing to destination in degrees True
   ** 12) Destination closing velocity in knots
   ** 13) Arrival Status, A = Arrival Circle Entered
   */
#ifdef _TACTICSPI_H_
   /*
   ** 14) FAA mode indicator (NMEA 2.3 and later)
   **     A = Autonomous mode
   **     D = Differential Mode
   **     E = Estimated (dead-reckoning) mode
   **     M = Manual Input Mode
   **     S = Simulated Mode
   **     N = Data Not Valid
   ** 15) Checksum
   */
#else
   /*
   ** 14) Checksum
   */
#endif // _TACTICSPI_H_

    /*
   ** First we check the checksum...
   */

#ifdef _TACTICSPI_H_
    NMEA0183_BOOLEAN check;
    wxString field14 = sentence.Field( 14 );
    isVersion2dot3OrLater = true;
    if ( field14.Length() > 1 ) {
        isVersion2dot3OrLater = false;
        check = sentence.IsChecksumBad( 14 );
    } // then this is Status A/V, must be checksum, hence < v2.3
    else {
        check = sentence.IsChecksumBad( 15 );
    } // this is a v2.3 or higher and field 14 is Status, chksum 15
#else
    NMEA0183_BOOLEAN check = sentence.IsChecksumBad( 14 );
#endif // _TACTICSPI_H_

    if ( check == NTrue )
    {
        SetErrorMessage( _T("Invalid Checksum") );
        return( FALSE );
    }

    /*
      if ( check == Unknown0183 )
      {
      SetErrorMessage( _T("Missing Checksum") );
      return( FALSE );
      }
    */

#ifdef _TACTICSPI_H_
    wxString signalValidity;
    if ( isVersion2dot3OrLater ) {
        signalValidity = sentence.Field( 14 );
        if ( signalValidity.CmpNoCase( "N" ) == 0 ) {
            IsDataValid = NFalse;
        } // the FAA signal field indicates non-valid signal
        else {
            IsDataValid = NTrue;
        } // else FAA signal indicates some valid signal mode
    } // then there is a FAA signal status field
    else {
        IsDataValid = sentence.Boolean( 1 );
    } // in older versions field 1 is used, NMEA boolean logic (A/V)
#else
    IsDataValid                     = sentence.Boolean( 1 );
#endif // _TACTICSPI_H_
    CrossTrackError                 = sentence.Double( 2 );
    DirectionToSteer                = sentence.LeftOrRight( 3 );
    From                            = sentence.Field( 4 );
    To                              = sentence.Field( 5 );
    DestinationPosition.Parse( 6, 7, 8, 9, sentence );
    RangeToDestinationNauticalMiles = sentence.Double( 10 );
    BearingToDestinationDegreesTrue = sentence.Double( 11 );
    DestinationClosingVelocityKnots = sentence.Double( 12 );
    IsArrivalCircleEntered          = sentence.Boolean( 13 );

    return( TRUE );
}

bool RMB::Write( SENTENCE& sentence )
{

    /*
    ** Let the parent do its thing
    */

    RESPONSE::Write( sentence );

    sentence += IsDataValid;
    sentence += CrossTrackError;
    if(DirectionToSteer == Left)
        sentence += _T("L");
    else
        sentence += _T("R");

    sentence += From;
    sentence += To;
    sentence += DestinationPosition;
    sentence += RangeToDestinationNauticalMiles;
    sentence += BearingToDestinationDegreesTrue;
    sentence += DestinationClosingVelocityKnots;
    sentence += IsArrivalCircleEntered;
#ifdef _TACTICSPI_H_
    if ( isVersion2dot3OrLater )
        sentence += FAA_ModeIndicator;
#endif // _TACTICSPI_H_

    sentence.Finish();

    //   NMEA0183_BOOLEAN check = sentence.IsChecksumBad( 14 );

    return( TRUE );
}

const RMB& RMB::operator = ( const RMB &source )
{
  if ( this != &source ) {
    IsDataValid                     = source.IsDataValid;
    CrossTrackError                 = source.CrossTrackError;
    DirectionToSteer                = source.DirectionToSteer;
    From                            = source.From;
    To                              = source.To;
    DestinationPosition             = source.DestinationPosition;
    RangeToDestinationNauticalMiles = source.RangeToDestinationNauticalMiles;
    BearingToDestinationDegreesTrue = source.BearingToDestinationDegreesTrue;
    DestinationClosingVelocityKnots = source.DestinationClosingVelocityKnots;
    IsArrivalCircleEntered          = source.IsArrivalCircleEntered;
#ifdef _TACTICSPI_H_
    FAA_ModeIndicator               = source.FAA_ModeIndicator;
    isVersion2dot3OrLater           = source.isVersion2dot3OrLater;
#endif // _TACTICSPI_H_
  }
  return *this;
}

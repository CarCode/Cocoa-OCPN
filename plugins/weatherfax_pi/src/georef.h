/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Georef utility
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
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

 ***************************************************************************
 *  Parts of this file were adapted from source code found in              *
 *  John F. Waers (jfwaers@csn.net) public domain program MacGPS45         *
 ***************************************************************************

 */

#ifndef PI
      #define PI        3.1415926535897931160E0      /* pi */
#endif

#define DEGREE    (PI/180.0)
#define RADIAN    (180.0/PI)

#define DATUM_INDEX_WGS84     100
#define DATUM_INDEX_UNKNOWN   -1


static const double WGS84_semimajor_axis_meters       = 6378137.0;           // WGS84 semimajor axis
static const double mercator_k0                       = 0.9996;
static const double WGSinvf                           = 298.257223563;       /* WGS84 1/f */

static void WFtoSM_ECC(double lat, double lon, double lat0, double lon0, double *x, double *y)
{
      const double f = 1.0 / WGSinvf;       // WGS84 ellipsoid flattening parameter
      const double e2 = 2 * f - f * f;      // eccentricity^2  .006700
      const double e = sqrt(e2);

      const double z = WGS84_semimajor_axis_meters * mercator_k0;

      *x = (lon - lon0) * DEGREE * z;

// y =.5 ln( (1 + sin t) / (1 - sin t) )
      const double s = sin(lat * DEGREE);
//      const double y3 = (.5 * log((1 + s) / (1 - s))) * z;

      const double s0 = sin(lat0 * DEGREE);
//      const double y30 = (.5 * log((1 + s0) / (1 - s0))) * z;
//      const double y4 = y3 - y30;

    //Add eccentricity terms

      const double falsen =  z *log(tan(PI/4 + lat0 * DEGREE / 2)*pow((1. - e * s0)/(1. + e * s0), e/2.));
      const double test =    z *log(tan(PI/4 + lat  * DEGREE / 2)*pow((1. - e * s )/(1. + e * s ), e/2.));
      *y = test - falsen;
}

static void WFDistanceBearingMercator(double lat0, double lon0, double lat1, double lon1, double *brg, double *dist)
{
      //    Calculate bearing by conversion to SM (Mercator) coordinates, then simple trigonometry

      double lon0x = lon0;
      double lon1x = lon1;

      //    Make lon points the same phase
      if((lon0x * lon1x) < 0.)
      {
          lon0x < 0.0 ? lon0x += 360.0 : lon1x += 360.0;
            //    Choose the shortest distance
            if(fabs(lon0x - lon1x) > 180.)
            {
                lon0x > lon1x ? lon0x -= 360.0 : lon1x -= 360.0;
            }

            //    Make always positive
            lon1x += 360.;
            lon0x += 360.;
      }

      //    Classic formula, which fails for due east/west courses....
      if(dist)
      {
          //    In the case of exactly east or west courses
          //    we must make an adjustment if we want true Mercator distances

          //    This idea comes from Thomas(Cagney)
          //    We simply require the dlat to be (slightly) non-zero, and carry on.
          //    MAS022210 for HamishB from 1e-4 && .001 to 1e-9 for better precision
          //    on small latitude diffs
          const double mlat0 = fabs(lat1 - lat0) < 1e-9 ? lat0 + 1e-9 : lat0;

          double east, north;
          WFtoSM_ECC(lat1, lon1x, mlat0, lon0x, &east, &north);
          const double C = atan2(east, north);
          if(cos(C))
          {
              const double dlat = (lat1 - mlat0) * 60.;              // in minutes
              *dist = (dlat /cos(C));
          }
          else
          {
              const double mlon0 = fabs(lon1 - lon0) < 1e-9 ? lon0 + 1e-9 : lon0;
              const double dlon = (lon1 - mlon0) * 60.;              // in minutes
              *dist = (dlon /sin(C));
          }
      }

      //    Calculate the bearing using the un-adjusted original latitudes and Mercator Sailing
      if(brg)
      {
          double east, north;
          WFtoSM_ECC(lat1, lon1x, lat0, lon0x, &east, &north);

          const double C = atan2(east, north);
          const double brgt = 180. + (C * 180. / PI);
          if (brgt < 0)
              *brg = brgt + 360.;
          else if (brgt > 360.)
              *brg = brgt - 360.;
          else
              *brg = brgt;
      }
}

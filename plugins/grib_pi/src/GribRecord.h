/**********************************************************************
zyGrib: meteorological GRIB file viewer
Copyright (C) 2008 - Jacques Zaninetti - http://www.zygrib.org

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/


/******************************************
Elément de base d'un fichier GRIB
******************************************/

#ifndef GRIBRECORD_H
#define GRIBRECORD_H

#include <iostream>
#include <cmath>

#include "zuFile.h"

#define DEBUG_INFO    false
#define DEBUG_ERROR   true
#define debug(format, ...)  {if(DEBUG_INFO)  {fprintf(stderr,format,__VA_ARGS__);fprintf(stderr,"\n");}}
#define erreur(format, ...) {if(DEBUG_ERROR) {fprintf(stderr,"ERROR: ");fprintf(stderr,format,__VA_ARGS__);fprintf(stderr,"\n");}}

#define zuint  unsigned int
#define zuchar unsigned char

#define GRIB_NOTDEF -999999999

//--------------------------------------------------------
// dataTypes	Cf function translateDataType()
//--------------------------------------------------------
#define GRB_PRESSURE        2   /* Pa     */
#define GRB_GEOPOT_HGT      7   /* gpm    */
#define GRB_TEMP           11   /* K      */
#define GRB_TPOT           13   /* K      */
#define GRB_TMAX           15   /* K      */
#define GRB_TMIN           16   /* K      */
#define GRB_DEWPOINT       17   /* K      */
#define GRB_WIND_VX        33   /* m/s    */
#define GRB_WIND_VY        34   /* m/s    */
#define GRB_UOGRD          49   /*"u-component of current", "m/s" */
#define GRB_VOGRD          50   /*"v-component of current", "m/s" */

#define GRB_HUMID_SPEC     51   /* kg/kg  */
#define GRB_HUMID_REL      52   /* %      */
#define GRB_PRECIP_RATE    59   /* l/m2/s */
#define GRB_PRECIP_TOT     61   /* l/m2   */
#define GRB_SNOW_DEPTH     66   /* m      */
#define GRB_CLOUD_TOT      71   /* %      */
#define GRB_HTSGW         100   /* m      */
#define GRB_WTMP           80   /* "Water Temperature", "K" */

#define GRB_WVDIR         101
#define GRB_WVHGT         102
#define GRB_WVPER         103
#define GRB_SWDIR         104
#define GRB_SWELL         105
#define GRB_SWPER         106
#define GRB_DIRPW         107
#define GRB_PERPW         108
#define GRB_DIRSW         109
#define GRB_PERSW         110

#define GRB_CRAIN         140   /* "Categorical rain", "yes=1;no=0" */
#define GRB_FRZRAIN_CATEG 141   /* 1=yes 0=no */
#define GRB_SNOW_CATEG    143   /* 1=yes 0=no */
#define GRB_CAPE 		  157   /* J/kg   */

#define GRB_TSEC          171   /* "Seconds prior to initial reference time (defined in bytes 18-20)" */
#define GRB_WIND_GUST     180   /* m/s "wind gust */
#define GRB_USCT          190   /* Scatterometer estimated U Wind, NCEP Center 7  */
#define GRB_VSCT          191   /* Scatterometer estimated V Wind, NCEP Center 7  */

#define GRB_WIND_XY2D      250   /* private : GRB_WIND_VX+GRB_WIND_VX */
#define GRB_DIFF_TEMPDEW   251   /* private : GRB_TEMP-GRB_DEWPOINT */

//--------------------------------------------------------
// Levels types (altitude reference)
//--------------------------------------------------------
#define LV_GND_SURF    1
#define LV_ISOTHERM0   4
#define LV_ISOBARIC  100
#define LV_MSL       102
#define LV_ABOV_GND  105
#define LV_SIGMA     107
#define LV_ATMOS_ALL 200
//---------------------------------------------------------
enum DataCenterModel {
    NOAA_GFS,
    NOAA_NCEP_WW3,
    NOAA_NCEP_SST,
    NOAA_RTOFS,
    FNMOC_WW3_GLB,
    FNMOC_WW3_MED,
    NORWAY_METNO,
    OTHER_DATA_CENTER
};

//----------------------------------------------
class GribCode
{
	public:
		static zuint makeCode (zuchar dataType, zuchar levelType, zuint levelValue) {
			return ((levelValue&0xFFFF)<<16)+((levelType&0xFF)<<8)+dataType;
		}
		static zuchar getDataType (zuint code) {
			return code&0xFF;
		}
		static zuchar getLevelType (zuint code) {
			return (code>>8)&0xFF;
		}
		static zuint getLevelValue (zuint code) {
			return (code>>16)&0xFFFF;
		}
};

//----------------------------------------------
class GribRecord
{
    public:
        GribRecord(ZUFILE* file, int id_);
        GribRecord(const GribRecord &rec);
        GribRecord(const GribRecord &rec1, const GribRecord &rec2, double d);
        static GribRecord *MagnitudeRecord(const GribRecord &rec1, const GribRecord &rec2);
        ~GribRecord();

        bool  isOk()  const   {return ok;};
        bool  isDataKnown()  const   {return knownData;};
        bool  isEof() const   {return eof;};
        bool  isDuplicated()  const   {return IsDuplicated;};                                          
        //-----------------------------------------
        zuchar  getDataType() const         { return dataType; }
        void    setDataType(const zuchar t);

        zuchar  getLevelType() const   { return levelType; }
        zuint   getLevelValue() const  { return levelValue; }
        zuint   getDataCenterModel() const { return dataCenterModel; }
        //-----------------------------------------
        void    translateDataType();  // adapte les codes des différents centres météo
        //-----------------------------------------

        zuchar   getIdCenter() const  { return idCenter; }
        zuchar   getIdModel() const   { return idModel; }
        zuchar   getIdGrid() const    { return idGrid; }

        //-----------------------------------------
        std::string getKey() const  { return dataKey; }
        static std::string makeKey(int dataType,int levelType,int levelValue);

        //-----------------------------------------
        int    getPeriodP1() const  { return periodP1; }
        int    getPeriodP2() const  { return periodP2; }

        // Number of points in the grid
        int    getNi() const     { return Ni; }
        int    getNj() const     { return Nj; }
        double  getDi() const    { return Di; }
        double  getDj() const    { return Dj; }

        // Value at one point of the grid
        double getValue(int i, int j) const  { return ok ? data[j*Ni+i] : GRIB_NOTDEF;}

        void setValue(zuint i, zuint j, double v)
                        { if (i<Ni && j<Nj)
                              data[j*Ni+i] = v; }

        // Value for one point interpolated
        double  getInterpolatedValue(double px, double py, bool numericalInterpolation=true) const;

        // coordiantes of grid point
        inline double  getX(int i) const   { return ok ? Lo1+i*Di : GRIB_NOTDEF;}
        inline double  getY(int j) const   { return ok ? La1+j*Dj : GRIB_NOTDEF;}

        double  getLatMin() const   { return latMin;}
        double  getLonMin() const   { return lonMin;}
        double  getLatMax() const   { return latMax;}
        double  getLonMax() const   { return lonMax;}

        // Is a point within the extent of the grid?
        inline bool   isPointInMap(double x, double y) const;
        inline bool   isXInMap(double x) const;
        inline bool   isYInMap(double y) const;
        // Is there a value at a particular grid point ?
        inline bool   hasValue(int i, int j) const;

        // Reference date Date (file creation date)
        time_t getRecordRefDate () const         { return refDate; }
        const char* getStrRecordRefDate () const { return strRefDate; }

        // Date courante des prévisions
        time_t getRecordCurrentDate () const     { return curDate; }
        const char* getStrRecordCurDate () const { return strCurDate; }
        void  setRecordCurrentDate (time_t t);
    private:
        int    id;    // unique identifiant
        bool   ok;    // valid?
        bool   knownData;     // type de donnée connu
        bool   waveData;
        bool   IsDuplicated;
        bool   eof;
        std::string dataKey;
        char   strRefDate [32];
        char   strCurDate [32];
        int    dataCenterModel;
        //---------------------------------------------
        // SECTION 0: THE INDICATOR SECTION (IS)
        //---------------------------------------------
        zuint  fileOffset0;
        zuint  seekStart, totalSize;
        zuchar editionNumber;
        bool   b_len_add_8;

        // SECTION 1: THE PRODUCT DEFINITION SECTION (PDS)
        zuint  fileOffset1;
        zuint  sectionSize1;
        zuchar tableVersion;
        zuchar data1[28];
        zuchar idCenter;
        zuchar idModel;
        zuchar idGrid;
        zuchar dataType;      // octet 9 = parameters and units
        zuchar levelType;
        zuint  levelValue;
        bool   hasGDS;
        bool   hasBMS;
        zuint  refyear, refmonth, refday, refhour, refminute;
        zuchar periodP1, periodP2;
        zuchar timeRange;
        zuint  periodsec;     // period in seconds
        time_t refDate;      // C reference date
        time_t curDate;      // C current date
        double  decimalFactorD;
        // SECTION 2: THE GRID DESCRIPTION SECTION (GDS)
        zuint  fileOffset2;
        zuint  sectionSize2;
        zuchar NV, PV;
        zuchar gridType;
        zuint  Ni, Nj;
        double La1, Lo1, La2, Lo2;
		double latMin, lonMin, latMax, lonMax;
        double Di, Dj;
        zuchar resolFlags, scanFlags;
        bool  hasDiDj;
        bool  isEarthSpheric;
        bool  isUeastVnorth;
        bool  isScanIpositive;
        bool  isScanJpositive;
        bool  isAdjacentI;
        // SECTION 3: BIT MAP SECTION (BMS)
        zuint  fileOffset3;
        zuint  sectionSize3;
        zuchar *BMSbits;
        // SECTION 4: BINARY DATA SECTION (BDS)
        zuint  fileOffset4;
        zuint  sectionSize4;
        zuchar unusedBitsEndBDS;
        bool  isGridData;          // not spherical harmonics
        bool  isSimplePacking;
        bool  isFloatValues;
        int   scaleFactorE;
        double scaleFactorEpow2;
        double refValue;
        zuint  nbBitsInPack;
        double  *data;
        // SECTION 5: END SECTION (ES)

        //---------------------------------------------
        // Data Access
        //---------------------------------------------
        bool readGribSection0_IS (ZUFILE* file, bool b_skip_initial_GRIB);
        bool readGribSection1_PDS(ZUFILE* file);
        bool readGribSection2_GDS(ZUFILE* file);
        bool readGribSection3_BMS(ZUFILE* file);
        bool readGribSection4_BDS(ZUFILE* file);
        bool readGribSection5_ES (ZUFILE* file);

        //---------------------------------------------
        // Utility functions
        //---------------------------------------------
        zuchar readChar(ZUFILE* file);
        int    readSignedInt3(ZUFILE* file);
        int    readSignedInt2(ZUFILE* file);
        zuint  readInt2(ZUFILE* file);
        zuint  readInt3(ZUFILE* file);
        double readFloat4(ZUFILE* file);

        zuint  readPackedBits(zuchar *buf, zuint first, zuint nbBits);
        zuint  makeInt3(zuchar a, zuchar b, zuchar c);
        zuint  makeInt2(zuchar b, zuchar c);

        time_t makeDate(zuint year,zuint month,zuint day,zuint hour,zuint min,zuint sec);
        zuint  periodSeconds(zuchar unit, zuchar P1, zuchar P2, zuchar range);
        void   multiplyAllData(double k);

        void   print();
};

//==========================================================================
inline bool   GribRecord::hasValue(int i, int j) const
{
    // is data present in BMS ?
    if (!ok) {
        return false;
    }
    if (!hasBMS) {
        return true;
    }
    int bit;
    if (isAdjacentI) {
        bit = j*Ni + i;
    }
    else {
        bit = i*Nj + j;
    }
    zuchar c = BMSbits[bit/8];
    zuchar m = (zuchar)128 >> (bit % 8);
    return (m & c) != 0;
}

//-----------------------------------------------------------------
inline bool GribRecord::isPointInMap(double x, double y) const
{
    return isXInMap(x) && isYInMap(y);
/*    if (Dj < 0)
        return x>=Lo1 && y<=La1 && x<=Lo1+(Ni-1)*Di && y>=La1+(Nj-1)*Dj;
    else
        return x>=Lo1 && y>=La1 && x<=Lo1+(Ni-1)*Di && y<=La1+(Nj-1)*Dj;*/
}
//-----------------------------------------------------------------
inline bool GribRecord::isXInMap(double x) const
{
//    return x>=Lo1 && x<=Lo1+(Ni-1)*Di;
//printf ("%f %f %f\n", Lo1, Lo2, x);
    if (Di > 0) {
        double maxLo = Lo2;
        if(Lo2+Di >= 360) /* grib that covers the whole world */
            maxLo += Di;
        return x>=Lo1 && x<=maxLo;
    } else {
        double maxLo = Lo1;
        if(Lo2+Di >= 360) /* grib that covers the whole world */
            maxLo += Di;
        return x>=Lo2 && x<=maxLo;
    }
}
//-----------------------------------------------------------------
inline bool GribRecord::isYInMap(double y) const
{
    if (Dj < 0)
        return y<=La1 && y>=La2;
    else
        return y>=La1 && y<=La2;
}

#endif




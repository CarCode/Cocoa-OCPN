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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers



#include "GribReader.h"
#include <cassert>

//-------------------------------------------------------------------------------
GribReader::GribReader()
{
    ok = false;
	dewpointDataStatus = NO_DATA_IN_FILE;
}
//-------------------------------------------------------------------------------
GribReader::GribReader(const wxString fname)
{
    ok = false;
	dewpointDataStatus = NO_DATA_IN_FILE;
    if (fname != _T("")) {
        openFile(fname);
    }
    else {
        clean_all_vectors();
    }
}
//-------------------------------------------------------------------------------
GribReader::~GribReader()
{
    clean_all_vectors();
    if (file != NULL)
          zu_close(file);
    free(file);

}
//-------------------------------------------------------------------------------
void GribReader::clean_all_vectors()
{
	std::map < std::string, std::vector<GribRecord *>* >::iterator it;
	for (it=mapGribRecords.begin(); it!=mapGribRecords.end(); it++) {
		std::vector<GribRecord *> *ls = (*it).second;
		clean_vector( *ls );
		delete ls;
	}
	mapGribRecords.clear();
}
//-------------------------------------------------------------------------------
void GribReader::clean_vector(std::vector<GribRecord *> &ls)
{
    std::vector<GribRecord *>::iterator it;
    for (it=ls.begin(); it!=ls.end(); it++) {
        delete *it;
        *it = NULL;
    }
    ls.clear();
}

//---------------------------------------------------------------------------------
void GribReader::storeRecordInMap(GribRecord *rec)
{
	std::map <std::string, std::vector<GribRecord *>* >::iterator it;
	it = mapGribRecords.find(rec->getKey());
	if (it == mapGribRecords.end())
	{
		mapGribRecords[rec->getKey()] = new std::vector<GribRecord *>;
		assert(mapGribRecords[rec->getKey()]);
	}
	mapGribRecords[rec->getKey()]->push_back(rec);
}

//---------------------------------------------------------------------------------
void GribReader::readAllGribRecords()
{
    //--------------------------------------------------------
    // Lecture de l'ensemble des GribRecord du fichier
    // et stockage dans les listes appropriées.
    //--------------------------------------------------------
    GribRecord *rec;
    int id = 0;
    time_t firstdate = -1;
    bool b_EOF;

    do {
        id ++;
        rec = new GribRecord(file, id);
        assert(rec);
        if (rec->isOk())
        {
              b_EOF = rec->isEof();

        	if (rec->isDataKnown())
        	{
				ok = true;   // au moins 1 record ok

				if (firstdate== -1)
					firstdate = rec->getRecordCurrentDate();

/*
				if (//-----------------------------------------
						(rec->getDataType()==GRB_PRESSURE
							&& rec->getLevelType()==LV_MSL && rec->getLevelValue()==0)
					//-----------------------------------------
					|| ( (rec->getDataType()==GRB_TMIN || rec->getDataType()==GRB_TMAX)
							&& rec->getLevelType()==LV_ABOV_GND && rec->getLevelValue()==2)
					//-----------------------------------------
					|| (rec->getDataType()==GRB_TEMP
							&& rec->getLevelType()==LV_ABOV_GND && rec->getLevelValue()==2)
					|| (rec->getDataType()==GRB_TEMP
							&& rec->getLevelType()==LV_ISOBARIC
							&& (   rec->getLevelValue()==850
								|| rec->getLevelValue()==700
								|| rec->getLevelValue()==500
								|| rec->getLevelValue()==300 ) )
					//-----------------------------------------
					|| ( (rec->getDataType()==GRB_WIND_VX || rec->getDataType()==GRB_WIND_VY)
							&& rec->getLevelType()==LV_ABOV_GND
							&& rec->getLevelValue()==10)
					|| ( (rec->getDataType()==GRB_WIND_VX || rec->getDataType()==GRB_WIND_VY)
							&& rec->getLevelType()==LV_ISOBARIC
							&& (   rec->getLevelValue()==850
								|| rec->getLevelValue()==700
								|| rec->getLevelValue()==500
								|| rec->getLevelValue()==300 ) )
					//-----------------------------------------
					|| (rec->getDataType()==GRB_HUMID_SPEC
							&& rec->getLevelType()==LV_ISOBARIC
							&& (   rec->getLevelValue()==850
								|| rec->getLevelValue()==700
								|| rec->getLevelValue()==500
								|| rec->getLevelValue()==300 ) )
					//-----------------------------------------
					|| (rec->getDataType()==GRB_GEOPOT_HGT
							&& rec->getLevelType()==LV_ISOTHERM0 && rec->getLevelValue()==0)
					|| (rec->getDataType()==GRB_GEOPOT_HGT
							&& rec->getLevelType()==LV_ISOBARIC
							&& (   rec->getLevelValue()==850
								|| rec->getLevelValue()==700
								|| rec->getLevelValue()==500
								|| rec->getLevelValue()==300 ) )
					//-----------------------------------------
					|| (rec->getDataType()==GRB_PRECIP_TOT
							&& rec->getLevelType()==LV_GND_SURF && rec->getLevelValue()==0)
					//-----------------------------------------
					|| (rec->getDataType()==GRB_PRECIP_RATE
							&& rec->getLevelType()==LV_GND_SURF && rec->getLevelValue()==0)
					//-----------------------------------------
					|| (rec->getDataType()==GRB_SNOW_DEPTH
							&& rec->getLevelType()==LV_GND_SURF && rec->getLevelValue()==0)
					//-----------------------------------------
					|| (rec->getDataType()==GRB_SNOW_CATEG
							&& rec->getLevelType()==LV_GND_SURF && rec->getLevelValue()==0)
					//-----------------------------------------
					|| (rec->getDataType()==GRB_FRZRAIN_CATEG
							&& rec->getLevelType()==LV_GND_SURF && rec->getLevelValue()==0)
					//-----------------------------------------
					|| (rec->getDataType()==GRB_CLOUD_TOT
							&& rec->getLevelType()==LV_ATMOS_ALL && rec->getLevelValue()==0)
					//-----------------------------------------
					|| (rec->getDataType()==GRB_HUMID_REL
							&& rec->getLevelType()==LV_ABOV_GND && rec->getLevelValue()==2)
					//-----------------------------------------
					|| (rec->getDataType()==GRB_TPOT
							&& rec->getLevelType()==LV_SIGMA && rec->getLevelValue()==9950)
					//-----------------------------------------
					|| (rec->getDataType()==GRB_CAPE
							&& rec->getLevelType()==LV_GND_SURF && rec->getLevelValue()==0)

				)


*/

                        if (//-----------------------------------------
                            (rec->getDataType()==GRB_PRESSURE
                            && rec->getLevelType()==LV_MSL && rec->getLevelValue()==0)

                            || ( (rec->getDataType()==GRB_WIND_VX || rec->getDataType()==GRB_WIND_VY)
                            && rec->getLevelType()==LV_ABOV_GND
                            && rec->getLevelValue()==10)

                           )

                        {
                              storeRecordInMap(rec);

                        }
                        else if( (rec->getDataType()==GRB_WIND_GUST
                            && rec->getLevelType()==LV_GND_SURF && rec->getLevelValue()==0) )
                            storeRecordInMap(rec);

                        else if( (rec->getDataType()==GRB_WIND_VX || rec->getDataType()==GRB_WIND_VY)
                                  && rec->getLevelType()==LV_GND_SURF)
                              storeRecordInMap(rec);

                        else if( rec->getDataType()==GRB_TEMP	                 //Air temperature at 2m
                            && rec->getLevelType()==LV_ABOV_GND && rec->getLevelValue()==2 )
                            storeRecordInMap(rec);

                        else if(rec->getDataType()==GRB_PRECIP_TOT               //total rainfall
                            && rec->getLevelType()==LV_GND_SURF && rec->getLevelValue()==0 )
                            storeRecordInMap(rec);

                        else if(rec->getDataType()==GRB_CLOUD_TOT                //cloud cover
                            && rec->getLevelType()==LV_ATMOS_ALL && rec->getLevelValue()==0 )                          
                            storeRecordInMap(rec);

                        else if( rec->getDataType() == GRB_HTSGW )               // Significant Wave Height
                              storeRecordInMap(rec);

                        else if( rec->getDataType() == GRB_WVDIR )               // Wind Wave Direction
                              storeRecordInMap(rec);

                        else if( rec->getDataType() == GRB_USCT )                // Scatterometer U Wind Estimate
                              storeRecordInMap(rec);

                        else if( rec->getDataType() == GRB_VSCT )                // Scatterometer V Wind Estimate
                              storeRecordInMap(rec);

                        else if( rec->getDataType() == GRB_CRAIN )               // Catagorical Rain  1/0
                              storeRecordInMap(rec);

                        //else if ((rec->getDataType()==GRB_TEMP) && (rec->getLevelType()==LV_GND_SURF) && (rec->getLevelValue()==0))
                             // storeRecordInMap(rec);                              // gfs SEATMP

                        else if ((rec->getDataType()==GRB_WTMP) && (rec->getLevelType()==LV_GND_SURF) && (rec->getLevelValue()==0))
                              storeRecordInMap(rec);                             // rtofs Water Temp + translated gfs Water Temp  

                        else if( (rec->getDataType()==GRB_UOGRD || rec->getDataType()==GRB_VOGRD))          // rtofs model sea current current
                              storeRecordInMap(rec);

                        else
                        {
/*
                              fprintf(stderr,
                                      "GribReader: unknown record type: dataType=%d levelType=%d levelValue=%d idCenter==%d && idModel==%d && idGrid==%d\n",
                                      rec->getDataType(), rec->getLevelType(), rec->getLevelValue(),
                              rec->getIdCenter(), rec->getIdModel(), rec->getIdGrid()
                              );
                              */
                              delete rec;
                        }
                  }
        }
        else {    // ! rec-isOk
            delete rec;
            rec = NULL;
        }
    } while (rec != NULL &&  !b_EOF);
}


//---------------------------------------------------------------------------------
void  GribReader::copyFirstCumulativeRecord (int dataType,int levelType,int levelValue)
{
	time_t dateref = getRefDate();
	GribRecord *rec = getGribRecord(dataType, levelType, levelValue, dateref);
	if (rec == NULL)
	{
		rec = getFirstGribRecord(dataType, levelType, levelValue);
		if (rec != NULL)
		{
			GribRecord *r2 = new GribRecord(*rec);
			if (r2 != NULL) {
				r2->setRecordCurrentDate (dateref);    // 1er enregistrement factice
				storeRecordInMap(r2);
			}
		}

	}
}
/*
//---------------------------------------------------------------------------------
void  GribReader::removeFirstCumulativeRecord (int dataType,int levelType,int levelValue)
{
	time_t dateref = getRefDate();
	GribRecord *rec = getFirstGribRecord(dataType, levelType, levelValue);

	if (rec!=NULL  &&  rec->getRecordCurrentDate() == dateref)
	{
		std::vector<GribRecord *> *liste = getListOfGribRecords(dataType, levelType, levelValue);
		if (liste != NULL) {
			std::vector<GribRecord *>::iterator it;
			for (it=liste->begin(); it!=liste->end() && (*it)!=rec; it++)
			{
			}
			if ((*it) == rec) {
				liste->erase(it);
			}
		}
	}
}
*/
void  GribReader::copyMissingWaveRecords (int dataType, int levelType, int levelValue)
{
	std::set<time_t>  setdates = getListDates();
	std::set<time_t>::iterator itd, itd2;
	for (itd=setdates.begin(); itd!=setdates.end(); itd++) {
		time_t date = *itd;
		GribRecord *rec = getGribRecord( dataType, levelType, levelValue,date );
		if ( rec == NULL ) {
			itd2 = itd;
			itd2 ++;	// next date
			if (itd2 != setdates.end()) {
				time_t date2 = *itd2;
				GribRecord *rec2 = getGribRecord( dataType, levelType, levelValue, date2 );
				if (rec2 && rec2->isOk() ) {
					// create a copied record from date2
					GribRecord *r2 = new GribRecord (*rec2);
					if (r2 != NULL) {
						r2->setRecordCurrentDate (date);
						storeRecordInMap (r2);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------
void  GribReader::copyFirstCumulativeRecord()
{
    copyFirstCumulativeRecord(GRB_CLOUD_TOT,   LV_ATMOS_ALL, 0);
	copyFirstCumulativeRecord(GRB_PRECIP_TOT,  LV_GND_SURF, 0);
}
/*
//---------------------------------------------------------------------------------
void  GribReader::removeFirstCumulativeRecord()
{
	removeFirstCumulativeRecord(GRB_TMIN, LV_ABOV_GND, 2);
	removeFirstCumulativeRecord(GRB_TMAX, LV_ABOV_GND, 2);
    removeFirstCumulativeRecord(GRB_CLOUD_TOT,   LV_ATMOS_ALL, 0);
	removeFirstCumulativeRecord(GRB_PRECIP_TOT,  LV_GND_SURF, 0);
	removeFirstCumulativeRecord(GRB_PRECIP_RATE, LV_GND_SURF, 0);
	removeFirstCumulativeRecord(GRB_SNOW_CATEG,  LV_GND_SURF, 0);
	removeFirstCumulativeRecord(GRB_FRZRAIN_CATEG, LV_GND_SURF, 0);
}
*/
void  GribReader::copyMissingWaveRecords ()
{
	copyMissingWaveRecords (GRB_HTSGW, LV_GND_SURF, 0);
	copyMissingWaveRecords (GRB_WVDIR, LV_GND_SURF,0);
}

//---------------------------------------------------------------------------------
void GribReader::readGribFileContent()
{
    fileSize = zu_filesize(file);
    readAllGribRecords();

    createListDates();
//    hoursBetweenRecords = computeHoursBeetweenGribRecords();

	//-----------------------------------------------------
	// Are dewpoint data in file ?
	// If no, compute it with Magnus-Tetens formula, if possible.
	//-----------------------------------------------------
	dewpointDataStatus = DATA_IN_FILE;
	if (getNumberOfGribRecords(GRB_DEWPOINT, LV_ABOV_GND, 2) == 0)
	{
		dewpointDataStatus = NO_DATA_IN_FILE;
		if (  getNumberOfGribRecords(GRB_HUMID_REL, LV_ABOV_GND, 2) > 0
		   && getNumberOfGribRecords(GRB_TEMP, LV_ABOV_GND, 2) > 0)
		{
			dewpointDataStatus = COMPUTED_DATA;
			std::set<time_t>::iterator iter;
			for (iter=setAllDates.begin(); iter!=setAllDates.end(); iter++)
			{
				time_t date = *iter;
				GribRecord *recModel = getGribRecord(GRB_TEMP,LV_ABOV_GND,2,date);
				if (recModel != NULL)
				{
					// Crée un GribRecord avec les dewpoints calculés
					GribRecord *recDewpoint = new GribRecord(*recModel);
					if (recDewpoint != NULL)
					{
						recDewpoint->setDataType(GRB_DEWPOINT);
						for (zuint i=0; i<(zuint)recModel->getNi(); i++)
							for (zuint j=0; j<(zuint)recModel->getNj(); j++)
							{
								double x = recModel->getX(i);
								double y = recModel->getY(j);
								double dp = computeDewPoint(x, y, date);
								recDewpoint->setValue(i, j, dp);
							}
						storeRecordInMap(recDewpoint);
					}
				}
			}
		}
	}
	//-----------------------------------------------------
}

//---------------------------------------------------
int GribReader::getDewpointDataStatus(int /*levelType*/,int /*levelValue*/)
{

	return dewpointDataStatus;
}

//---------------------------------------------------
int GribReader::getTotalNumberOfGribRecords() {
	int nb=0;
	std::map < std::string, std::vector<GribRecord *>* >::iterator it;
	for (it=mapGribRecords.begin(); it!=mapGribRecords.end(); it++)
	{
		nb += (*it).second->size();
	}
	return nb;
}

//---------------------------------------------------
std::vector<GribRecord *> * GribReader::getFirstNonEmptyList()
{
    std::vector<GribRecord *> *ls = NULL;
	std::map < std::string, std::vector<GribRecord *>* >::iterator it;
	for (it=mapGribRecords.begin(); ls==NULL && it!=mapGribRecords.end(); it++)
	{
		if ((*it).second->size()>0)
			ls = (*it).second;
	}
	return ls;
}

//---------------------------------------------------
int GribReader::getNumberOfGribRecords(int dataType,int levelType,int levelValue)
{
	std::vector<GribRecord *> *liste = getListOfGribRecords(dataType,levelType,levelValue);
	if (liste != NULL)
		return liste->size();
	else
		return 0;
}

//---------------------------------------------------------------------
std::vector<GribRecord *> * GribReader::getListOfGribRecords(int dataType,int levelType,int levelValue)
{
	std::string key = GribRecord::makeKey(dataType,levelType,levelValue);
	if (mapGribRecords.find(key) != mapGribRecords.end())
		return mapGribRecords[key];
	else
		return NULL;
}
//---------------------------------------------------------------------------
double  GribReader::getTimeInterpolatedValue(int dataType,int levelType,int levelValue, double px, double py, time_t date)
{
	GribRecord *before, *after;
	findGribsAroundDate (dataType,levelType,levelValue, date, &before, &after);
	return get2GribsInterpolatedValueByDate(px, py, date, before, after);
}

//------------------------------------------------------------------
void GribReader::findGribsAroundDate (int dataType,int levelType,int levelValue, time_t date,
							GribRecord **before, GribRecord **after)
{
	// Cherche les GribRecord qui encadrent la date
	std::vector<GribRecord *> *ls = getListOfGribRecords(dataType,levelType,levelValue);
	*before = NULL;
	*after  = NULL;
	zuint nb = ls->size();
	for (zuint i=0; i<nb && *before==NULL && *after==NULL; i++)
	{
		GribRecord *rec = (*ls)[i];
		if (rec->getRecordCurrentDate() == date) {
			*before = rec;
			*after = rec;
		}
		else if (rec->getRecordCurrentDate() < date) {
			*before = rec;
		}
		else if (rec->getRecordCurrentDate() > date  &&  *before != NULL) {
			*after = rec;
		}
	}
}

//------------------------------------------------------------------
double 	GribReader::get2GribsInterpolatedValueByDate (
				double px, double py, time_t date,
				GribRecord *before, GribRecord *after)
{
	double val = GRIB_NOTDEF;
	if (before!=NULL && after!=NULL) {
		if (before == after) {
			val = before->getInterpolatedValue(px, py);
		}
		else {
			time_t t1 = before->getRecordCurrentDate();
			time_t t2 = after->getRecordCurrentDate();
			if (t1 == t2) {
				val = before->getInterpolatedValue(px, py);
			}
			else {
				double v1 = before->getInterpolatedValue(px, py);
				double v2 = after->getInterpolatedValue(px, py);
				if (v1!=GRIB_NOTDEF && v2!=GRIB_NOTDEF) {
					double k  = fabs( (double)(date-t1)/(t2-t1) );
					val = (1.0-k)*v1 + k*v2;
				}
			}
		}
	}
	return val;
}

//---------------------------------------------------
// Rectangle de la zone couverte par les données
bool GribReader::getZoneExtension(double *x0,double *y0, double *x1,double *y1)
{
    std::vector<GribRecord *> *ls = getFirstNonEmptyList();
    if (ls != NULL) {
        GribRecord *rec = ls->at(0);
        if (rec != NULL) {
            *x0 = rec->getX(0);
            *y0 = rec->getY(0);
            *x1 = rec->getX( rec->getNi()-1 );
            *y1 = rec->getY( rec->getNj()-1 );
            if (*x0 > *x1) {
				double tmp = *x0;
				*x0 = *x1;
				*x1 = tmp;
            }
            if (*y0 > *y1) {
				double tmp = *y0;
				*y0 = *y1;
				*y1 = tmp;
            }
        }
        return true;
    }
    else {
        return false;
    }
}
//---------------------------------------------------
// Premier GribRecord trouvé (pour récupérer la grille)
GribRecord * GribReader::getFirstGribRecord()
{
    std::vector<GribRecord *> *ls = getFirstNonEmptyList();
    if (ls != NULL) {
        return ls->at(0);
    }
    else {
        return NULL;
    }
}
//---------------------------------------------------
// Premier GribRecord (par date) pour un type donné
GribRecord * GribReader::getFirstGribRecord(int dataType,int levelType,int levelValue)
{
	std::set<time_t>::iterator it;
	GribRecord *rec = NULL;
	for (it=setAllDates.begin(); rec==NULL && it!=setAllDates.end(); it++)
	{
		time_t date = *it;
		rec = getGribRecord(dataType,levelType,levelValue, date);
	}
	return rec;
}
//---------------------------------------------------
// Délai en heures entre 2 records
// On suppose qu'il est fixe pour tout le fichier !!!
double GribReader::computeHoursBeetweenGribRecords()
{
	double res = 1;
    std::vector<GribRecord *> *ls = getFirstNonEmptyList();
    if (ls != NULL) {
        time_t t0 = (*ls)[0]->getRecordCurrentDate();
        time_t t1 = (*ls)[1]->getRecordCurrentDate();
        res = fabs((double)(t1-t0)) / 3600.0;
        if (res < 1)
        	res = 1;
    }
    return res;
}
//---------------------------------------------------
GribRecord * GribReader::getGribRecord(int dataType,int levelType,int levelValue, time_t date)
{
    std::vector<GribRecord *> *ls = getListOfGribRecords(dataType,levelType,levelValue);
    if (ls != NULL) {
        // Cherche le premier enregistrement à la bonne date
        GribRecord *res = NULL;
        zuint nb = ls->size();
        for (zuint i=0; i<nb && res==NULL; i++) {
            if ((*ls)[i]->getRecordCurrentDate() == date)
                res = (*ls)[i];
        }
        return res;
    }
    else {
        return NULL;
    }
}

//-------------------------------------------------------
// Génère la liste des dates pour lesquelles des prévisions existent
void GribReader::createListDates()
{   // Le set assure l'ordre et l'unicité des dates
    setAllDates.clear();
	std::map < std::string, std::vector<GribRecord *>* >::iterator it;
	for (it=mapGribRecords.begin(); it!=mapGribRecords.end(); it++)
	{
		std::vector<GribRecord *> *ls = (*it).second;
		for (zuint i=0; i<ls->size(); i++) {
			setAllDates.insert( ls->at(i)->getRecordCurrentDate() );
		}
	}
}

//-------------------------------------------------------
double GribReader::computeDewPoint(double lon, double lat, time_t now)
{
	double diewpoint = GRIB_NOTDEF;

	GribRecord *recTempDiew =  getGribRecord(GRB_DEWPOINT,LV_ABOV_GND,2,now);
	if (recTempDiew != NULL)
	{
		// GRIB file contains diew point data
		diewpoint = recTempDiew->getInterpolatedValue(lon, lat);
	}
	else
	{
		// Compute diew point with Magnus-Tetens formula
		GribRecord *recTemp =  getGribRecord(GRB_TEMP,LV_ABOV_GND,2,now);
		GribRecord *recHumid = getGribRecord(GRB_HUMID_REL,LV_ABOV_GND,2,now);
		if (recTemp && recHumid)
		{
			double temp = recTemp->getInterpolatedValue(lon, lat);
			double humid = recHumid->getInterpolatedValue(lon, lat);
			if (temp != GRIB_NOTDEF && humid != GRIB_NOTDEF)
			{
				double a = 17.27;
				double b = 237.7;
				double t  = temp-273.15;
				double rh = humid;
				//if ( t>0 && t<60 && rh>0.01)
				{
					double alpha = a*t/(b+t)+log(rh/100.0);
					diewpoint = b*alpha/(a-alpha);
					diewpoint += 273.15;
				}
			}
		}
	}
	return diewpoint;
}


//-------------------------------------------------------------------------------
// Lecture complète d'un fichier GRIB
//-------------------------------------------------------------------------------
void GribReader::openFile(const wxString fname)
{
    debug("Open file: %s", (const char *)fname.mb_str());
    fileName = fname;
    ok = false;
    clean_all_vectors();
    //--------------------------------------------------------
    // Open the file
    //--------------------------------------------------------
    file = zu_open((const char *)fname.mb_str(), "rb", ZU_COMPRESS_AUTO);
    if (file == NULL) {
        erreur("Can't open file: %s", (const char *)fname.mb_str());
        return;
    }
    readGribFileContent();

    // Look for compressed files with alternate extensions
 	if (! ok) {
    	if (file != NULL)
			zu_close(file);
    	file = zu_open((const char *)fname.mb_str(), "rb", ZU_COMPRESS_BZIP);
    	if (file != NULL)
    		readGribFileContent();
    }
 	if (! ok) {
    	if (file != NULL)
			zu_close(file);
      file = zu_open((const char *)fname.mb_str(), "rb", ZU_COMPRESS_GZIP);
    	if (file != NULL)
    		readGribFileContent();
    }
 	if (! ok) {
    	if (file != NULL)
			zu_close(file);
      file = zu_open((const char *)fname.mb_str(), "rb", ZU_COMPRESS_NONE);
    	if (file != NULL)
    		readGribFileContent();
    }
}


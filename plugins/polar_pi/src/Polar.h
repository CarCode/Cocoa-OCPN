#ifndef _POLAR_H_
#define _POLAR_H_

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/dynarray.h>
#include <map>

#include "nmea0183.h"

WX_DEFINE_ARRAY_DOUBLE(int, ArrayOfDouble);

#define WINDDIR 40

class PolarDialog;
class polar_pi;
class FilterDlg;
class Options;
//class NMEA0183;

class Polar
{
	enum PolColors {
	AQUAMARINE,
	BLUE,
	CYAN,
	YELLOW,
	YELLOWGREEN,
	GREEN,
	BROWN,
	RED,
	VIOLETRED,
	VIOLET
	};

public:
	Polar(PolarDialog* parent);
	~Polar(void);

	struct pol
	{
		double	wdir[WINDDIR];
		int		count[WINDDIR];
		int		scount[WINDDIR];
		std::multimap<int,double> winddir;
		double wdirMax[WINDDIR];
		double wdirMin[WINDDIR];
		double wdirTotal[WINDDIR];
	}windsp[10];

	wxDC*			dc;
	wxColor			windColor[10];
	polar_pi*		plugin;
	Options*		opt;

	wxArrayString	sails;
	bool			filterSails;
	double			knots;
	int				degrees;

	int				mode;
	double			sog;
	double			windAngle;
	double			windSpeed;
	wxString		windReference;
	double			gpsSpeed;
	double			speedoSpeed;
    int				timeout;
	bool			nmea;
	bool			engineRunning;
	wxDateTime		dtRPM;

	std::multimap<int,double>::iterator it;

	void Render();
	void createDiagram(wxDC& dc);
	void createPolar();
	void createSpeedBullets();
	void createSpeedBulletsMax();
	void save();
	void showDlg();
	void source(int sel);
	void setMode(int sel);
	void setValue(wxString s, int row, int col);
	void setSentence(wxString sentence);
	void reset();
	bool insert();
	void setEngineStatus(wxString str);
	void loadPolar();


private:
	PolarDialog*	dlg;
	FilterDlg*		filterDlg;

	NMEA0183       	m_NMEA0183;

	wxString		logbookDataPath;
	wxSize			center;
	int				radius;
	int				rSpeed[24];
	double			dist;

	double toRad(int angle);
	void   loadVDR();
//	void calculateData();
};
#endif
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __FILTERDLG_H__
#define __FILTERDLG_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/panel.h>
#include <wx/string.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////////
/// Class FilterDlg
///////////////////////////////////////////////////////////////////////////////
class FilterDlg : public wxDialog 
{
	private:
		wxStaticText* m_staticTextUnitKnots;
		wxStaticText* m_staticTextUnitHeight;
		wxFlexGridSizer* fgSizer50;
		wxPanel* m_panel33;
		wxButton* m_buttonSailsReset;
		wxStaticText* m_staticTextKnot;

		PolarDialog*	dlg;
		Polar*			polar;
		Options*		opt;

	protected:
		wxNotebook* m_notebook6;
		wxPanel* m_panel4;
		wxStaticText* m_staticText152;
		wxPanel* m_panel34;
		wxStaticText* m_staticText153;
		wxStaticText* m_staticText154;
		wxStdDialogButtonSizer* m_sdbSizer11;
		wxButton* m_sdbSizer11OK;
		wxButton* m_sdbSizer11Cancel;
		
		// Virtual event handlers, overide them in your derived class
		void OnOKButtonClick( wxCommandEvent& event );
		
	
	public:
		wxBoxSizer* bSizer54;
		wxBoxSizer* bSizer52;
		wxBoxSizer* bSizer53;
		wxCheckBox* m_checkBoxMax;
		wxCheckBox* m_checkBoxAverage;
		wxCheckBox* m_checkBoxRangePercent;
		wxChoice* m_choice6;
		bool filterSails;
		wxCheckBox* checkboxSails[14];
		wxChoice* checkboxWaveFrom[14];
		wxChoice* checkboxWaveTo[14];
		wxArrayString m_choiceChoicesFrom[10];
		wxArrayString m_choiceChoicesTo[10];

		void init();
		
		FilterDlg( PolarDialog* parent, Polar* polar,wxWindowID id = wxID_ANY, const wxString& title = _("Polar Filter"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 451,470 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~FilterDlg();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CollectDlg
///////////////////////////////////////////////////////////////////////////////
class CollectDlg : public wxDialog
{
	private:
	
	protected:
		wxStaticText* m_staticText9;
	
	public:		
		CollectDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 338,74 ), long style = 0 ); 
		~CollectDlg();

		wxStaticText* m_staticTextFile;
		wxGauge* m_gauge1;	
};

#endif 

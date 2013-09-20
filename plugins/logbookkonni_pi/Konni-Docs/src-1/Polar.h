#ifndef _POLAR_H_
#define _POLAR_H_

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/dynarray.h>

WX_DEFINE_ARRAY_DOUBLE(int, ArrayOfDouble);

#define WINDDIR 40

class LogbookDialog;
class PolarDlg;

class Polar
{
public:
	Polar(LogbookDialog* parent);
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

	wxDC* dc;

	wxArrayString	sails;
	bool			filterSails;
	double			knots;
	int				degrees;

	std::multimap<int,double>::iterator it;

	void Render();
	void createDiagram(wxDC& dc);
	void createPolar();
	void createSpeedBullets();
	void save();
	void showDlg();
	void source(int sel);

private:
	LogbookDialog*  dlg;
	Options*		opt;
	PolarDlg*		polarDlg;

	wxSize center;
	int radius;
	int rSpeed[24];
	double dist;

	double toRad(int angle);
//	void calculateData();
};

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __POLARDLG_H__
#define __POLARDLG_H__

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
/// Class PolarDlg
///////////////////////////////////////////////////////////////////////////////
class PolarDlg : public wxDialog 
{
	private:
		wxStaticText* m_staticTextUnitKnots;
		wxStaticText* m_staticTextUnitHeight;
		wxFlexGridSizer* fgSizer50;
		wxPanel* m_panel33;
		wxButton* m_buttonSailsReset;

		LogbookDialog*	dlg;
		Polar*			polar;
		Options*		opt;
	
	protected:
		wxNotebook* m_notebook6;
		wxStaticText* m_staticText152;
		wxPanel* m_panel34;
		wxStaticText* m_staticText153;
		wxStaticText* m_staticText147;
		wxStaticText* m_staticTextKnot;
		wxStaticLine* m_staticline46;
		wxStaticText* m_staticText148;
		wxChoice* m_choice40;
		wxStaticText* m_staticText149;
		wxChoice* m_choice41;
		wxStdDialogButtonSizer* m_sdbSizer11;
		wxButton* m_sdbSizer11OK;
		wxButton* m_sdbSizer11Cancel;
		wxBoxSizer* bSizer51;
		
		// Virtual event handlers, overide them in your derived class
		void OnOKButtonClick( wxCommandEvent& event );
		
	
	public:
		wxBoxSizer* bSizer54;
		wxBoxSizer* bSizer52;
		wxBoxSizer* bSizer53;
		bool filterSails;
		wxCheckBox* checkboxSails[14];
		wxChoice* checkboxWaveFrom[14];
		wxChoice* checkboxWaveTo[14];
		wxArrayString m_choiceChoicesFrom[10];
		wxArrayString m_choiceChoicesTo[10];
		void init();
		
		PolarDlg( LogbookDialog* parent, Polar* polar,wxWindowID id = wxID_ANY, const wxString& title = _("Polar Filter"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 451,470 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~PolarDlg();
	
};

#endif //__POLARDLG_H__


#endif
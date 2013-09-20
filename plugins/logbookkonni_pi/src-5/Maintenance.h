#pragma once
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/textfile.h> 
#include <wx/grid.h>
#include <wx/arrstr.h>
#include <wx/calctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

#include "Export.h"

class LogbookDialog;
class Options;

class Maintenance : public Export
{
public:
	enum fieldsRepairs{RPRIORITY,RTEXT};
	enum fieldsBuyParts{PPRIORITY,PCATEGORY,TITLE,PARTS,DATE,AT};
	enum fieldsService{PRIORITY,TEXT,IF,WARN,URGENT,START,ACTIVE};
public:
	Maintenance(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT);
	~Maintenance(void);

	void addLine();
	void addLineRepairs();
	void addLineBuyParts();
	void setRowDone(int row);
	void setRepairDone(int row);
	void cellCollChanged(int col, int row);
    void cellSelected(int col, int row);
    void checkService(int row);
    void checkRepairs();
    void checkBuyParts();
	void loadData();
	void update();
	void updateRepairs();
	void updateBuyParts();
	void buyParts(int i);
	void setLayoutLocation();
	wxString toODT(int tab,wxString path,wxString layout,int mode);
	void viewODT(int tab,wxString path,wxString layout,int mode);
	void viewHTML(int tab,wxString path,wxString layout,int mode);
	wxString toHTML(int tab,wxString path,wxString layout,int mode);
	void showDateDialog( int row, int col, wxGrid* grid);
	void deleteFindItRow(wxString category,wxString plugin);
	wxString getDateString(wxString s);

	wxGrid*		grid;
	wxGrid*		buyparts;
	wxGrid*		repairs;
	int			lastRow;
	int			lastRowBuyParts;
	int			lastRowRepairs;
	int			selectedCol;
	int			selectedRow;
	int			selectedRowRepairs;
	int			selectedColRepairs;
	int			selectedRowBuyParts;
	int			selectedColBuyParts;
	wxString	layout_locn;
	wxString	layout_locnService;
	wxString	layout_locnRepairs;
	wxString	layout_locnBuyParts;

	wxString		m_choices[12];
	int				m_choicesCount;
	wxString		m_YesNo[2];
	bool			modified;
	bool			modifiedR;	
	bool			modifiedB;
private:
	LogbookDialog*	dialog;
	Options*		opt;

	void setRowBackground(int row, wxColour &c);
	void setRowBackgroundRepairs(int row, wxColour &c);
	void setRowBackgroundBuyParts(int row, wxColour &c);
	void setAlignmentService();
	void setAlignmentRepairs();
	void setAlignmentBuyParts();
	void setBuyPartsPriority(wxGrid *grid ,int row, int col, int text);
	wxString readLayoutHTML(wxString path,wxString layout);
	wxString setPlaceHolders(int mode, wxGrid *grid, int row, wxString middleODT);
	wxString setPlaceHoldersService(int mode, wxGrid *grid, int row, wxString middleODT);
	wxString setPlaceHoldersRepairs(int mode, wxGrid *grid, int row, wxString middleODT);
	wxString setPlaceHoldersBuyParts(int mode, wxGrid *grid, int row, wxString middleODT);
	wxString replaceLabels(wxString s, wxGrid* grid);
	wxString replaceNewLine(int mode, wxString str);
	int		 getSelection(wxString s);

	wxColour green;
	wxColour red;
	wxColour redlight;
	wxColour yellow;
	wxColour yellowlight;
	wxColour white;
	
	wxString		layout;
	wxString		ODTLayout;

	wxString		m_Priority[6];

	wxString		data_locn;
	wxString		data_locnRepairs;
	wxString		data_locnBuyParts;
#ifdef __WXOSX__
//
#else
	bool			format; // 0 = ODT 1 = HTML
#endif
};

///////////////////////////////////////////////////////////////////////////////
/// Class DateDialog
///////////////////////////////////////////////////////////////////////////////
class DateDialog : public wxDialog 
{
	private:
	
	protected:
		wxStdDialogButtonSizer* m_sdbSizer6;
		wxButton* m_sdbSizer6OK;
		wxButton* m_sdbSizer6Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCalenderSelChanged( wxCalendarEvent& event ) { event.Skip(); }
		
	
	public:
		wxCalendarCtrl* m_calendar2;
		
		DateDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Select a date"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 221,198 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~DateDialog();
	
};

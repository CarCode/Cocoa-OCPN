#pragma once
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/textfile.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 
#include <wx/zipstrm.h> 

class LogbookDialog;

class Boat
{
public:
	Boat(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT);
	~Boat(void);

	void saveData();
	void loadData();
	void toHTML(wxString path,wxString layout, bool mode);
	wxString toODT(wxString path,wxString layout, bool mode);
	void toCSV(wxString path);
	void toXML(wxString path);
	void toODS(wxString path);
	void backup(wxString path);
	void viewHTML(wxString path,wxString layout,bool mode);
	void viewODT(wxString path,wxString layout,bool mode);
	wxString readLayoutFileODT(wxString layout);
	void addEquip();
	void cellChanged(int row, int col);
	void deleteRow(int row);
	void setLayoutLocation(wxString loc);

	wxString		layout_locn;
	wxString		layout;
	wxString		ODTLayout;

	bool			modified;
private:
	LogbookDialog*	parent; 
	wxTextFile*		boatFile;
	wxTextFile*		equipFile;
	wxTextFile*		boatLayoutFile;
	wxFile*			boatHTMLFile;
	wxFile*			boatODTFile;
	wxString		data_locn;
	wxString		equip_locn;
	wxString		lay;
	wxString		layoutODT;

	wxWindowList	ctrl;		// all wxTextCtrl's in Window;
	wxWindowList    ctrlStaticText; // all wxStaticText in Window


	void createFiles(wxString data, wxString lay);
	void createTextCtrlConnections();
	void createStaticTextList();
	void saveCSV(wxTextFile* file, bool mode);
	void saveXML(wxTextFile* file, bool mode);
	void saveODS( wxString path, bool mode );
	wxString repeatArea(wxString html);
	wxString repeatAreaODT(wxString odt);

 //   DECLARE_EVENT_TABLE()

};

//#pragma once
#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

#include "boat.h"
#include "LogbookDialog.h"
#include "logbook_pi.h"
#include "Export.h"
#include "Options.h"

#include <wx/textctrl.h>
#include <wx/filefn.h>
#include <wx/dir.h> 
#include <wx/event.h>
#include <wx/object.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 
#include <wx/zipstrm.h> 
#include <memory>
using namespace std;


Boat::Boat(LogbookDialog* d, wxString data, wxString lay, wxString layoutODT)
{
	parent = d;
	modified = false;
	this->layout = lay;
	this->ODTLayout = layoutODT;

	createFiles(data,lay);
	createTextCtrlConnections();
	createStaticTextList();
}

Boat::~Boat(void)
{
	saveData();
	for(unsigned int i = 0; i < ctrl.GetCount(); i++)
	{
		if(ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			ctrl[i]->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, 
				wxCommandEventHandler(LogbookDialog::boatNameOnTextEnter),NULL,parent );
		}
	}

}

void Boat::setLayoutLocation(wxString loc)
{
	bool radio = parent->m_radioBtnHTMLBoat->GetValue();
	if(radio)
		layout_locn = layout;
	else
		layout_locn = ODTLayout;
	wxString boatLay = layout_locn;

	layout_locn.Append(_T("boat"));
	parent->appendOSDirSlash(&layout_locn);

	parent->loadLayoutChoice(LogbookDialog::BOAT,layout_locn,parent->boatChoice,parent->logbookPlugIn->opt->layoutPrefix[LogbookDialog::BOAT]);
	if(radio)
		parent->boatChoice->SetSelection(parent->logbookPlugIn->opt->boatGridLayoutChoice);
	else
		parent->boatChoice->SetSelection(parent->logbookPlugIn->opt->boatGridLayoutChoiceODT);
}

void Boat::createFiles(wxString data, wxString lay)
{
	data_locn = data;
	data_locn.Append(_T("boat.txt"));
	boatFile = new wxTextFile(data_locn);
	wxFileName wxHomeFiledir = data_locn ;

	if(true != wxHomeFiledir.FileExists())
	{				
		boatFile->Create();
	}

	equip_locn = data;
	equip_locn.Append(_T("equipment.txt"));
	equipFile = new wxTextFile(equip_locn);
	wxHomeFiledir = equip_locn ;

	if(true != wxHomeFiledir.FileExists())
	{				
		equipFile->Create();
	}

	setLayoutLocation(lay);
}

void Boat::createTextCtrlConnections()
{
	wxWindowList l = parent->m_panel72->GetWindowChildren();

	for(unsigned int i = 0; i < l.GetCount(); i++)
	{
		if(l[i]->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			l[i]->Connect( wxEVT_COMMAND_TEXT_UPDATED, 
				wxCommandEventHandler(LogbookDialog::boatNameOnTextEnter),NULL,parent);		
			ctrl.Append(l[i]);
		}
	}
}

void Boat::createStaticTextList()
{
	wxWindowList l = parent->m_panel72->GetWindowChildren();

	for(unsigned int i = 0; i < l.GetCount(); i++)
	{
		if(l[i]->IsKindOf(CLASSINFO(wxStaticText)))
			ctrlStaticText.Append(l[i]);
	}
}

void Boat::loadData()
{
	wxString line;
	
	boatFile->Open();

	if(boatFile->GetLineCount() > 0) 
	{
		line = boatFile->GetLine(0);
		if(line.Contains(_T("#1.2#")))
			line = boatFile->GetLine(1);

		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY );
		
		int c = 0;

		while ( tkz.HasMoreTokens() )
		{
			wxString s;
			s += tkz.GetNextToken().RemoveLast();
			s = parent->restoreDangerChar(s);
			if(c == 18)
				s = parent->maintenance->getDateString(s);
			wxTextCtrl* t = wxDynamicCast(ctrl[c++], wxTextCtrl);
			t->ChangeValue(s);	
		}
	}
	boatFile->Close();

	equipFile->Open();
	if(equipFile->GetLineCount() <= 0) { equipFile->Close(); return; }
	if(parent->m_gridEquipment->GetNumberRows() > 0)
		parent->m_gridEquipment->DeleteRows(0,parent->m_gridEquipment->GetNumberRows()-1);

	for(unsigned int i = 0; i <  equipFile->GetLineCount(); i++)
	{
		line = 	equipFile->GetLine(i);

		parent->m_gridEquipment->AppendRows();

		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY );
		int c = 0;

		while ( tkz.HasMoreTokens() )
		{
			wxString s;
			s = tkz.GetNextToken().RemoveLast();
			s = parent->restoreDangerChar(s);
			parent->m_gridEquipment->SetCellValue(i,c++,s);
		}
	}
	equipFile->Close();
}

void Boat::saveData()
{
	if(!modified) return;
	modified = false;

	wxString t,s;

	boatFile->Open();
	boatFile->Clear();

	for(unsigned int i = 0; i < ctrl.GetCount(); i++)
	{
		if(ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			wxTextCtrl* te = wxDynamicCast(ctrl[i], wxTextCtrl);
			wxString temp = te->GetValue();
			temp = parent->replaceDangerChar(temp);	
			if(i == 18 && (!temp.IsEmpty() && temp.GetChar(0) != ' '))
			{
				wxDateTime dt;
				parent->myParseDate(temp,dt);
				t += wxString::Format(_T("%i/%i/%i \t"),dt.GetMonth(), dt.GetDay(), dt.GetYear());
			}
			else
				t += temp+wxT(" \t");
		}
	}
	t.RemoveLast();

	boatFile->AddLine(t);
	boatFile->Write();
	boatFile->Close();

	equipFile->Open();
	equipFile->Clear();

	int count = parent->m_gridEquipment->GetNumberRows();
	for(int r = 0; r < count; r++)
	{
		s = _T("");
		for(int i= 0; i < parent->m_gridEquipment->GetNumberCols(); i++)
		{
			s += parent->replaceDangerChar(
						 parent->m_gridEquipment->GetCellValue(r,i))
				+wxT(" \t");
		}
		equipFile->AddLine(s);
	}

	equipFile->Write();
	equipFile->Close();
}

wxString Boat::readLayoutFileODT(wxString layout)
{
	wxString odt = _T("");

	wxString filename = layout_locn + layout + _T(".odt");

	if(wxFileExists(filename))
	{
//#ifdef __WXOSX__
        auto_ptr<wxZipEntry> entry;
        static const wxString fn = _T("content.xml");
        wxString name = wxZipEntry::GetInternalName(fn);
        wxFFileInputStream in(filename);
        wxZipInputStream zip(in);
        do
        {
            entry.reset(zip.GetNextEntry());
        }
        while (entry.get() != NULL && entry->GetInternalName() != name);
        if (entry.get() != NULL)
        {
            wxTextInputStream txt(zip,_T("\n"),wxConvUTF8);
            while(!zip.Eof())
	            odt += txt.ReadLine();
        }
//#else
	/*	static const wxString fn = _T("content.xml");
		wxFileInputStream in(filename);
		wxZipInputStream zip(in);
		wxTextInputStream txt(zip);
		while(!zip.Eof())
			odt += txt.ReadLine();*/
//#endif
	}
	return odt;
}

void Boat::viewODT(wxString path,wxString layout,bool mode)
{
	if(parent->logbookPlugIn->opt->filterLayout)
        layout.Prepend(parent->logbookPlugIn->opt->layoutPrefix[LogbookDialog::BOAT]);

    toODT(path, layout, mode);
	if(layout != _T(""))
	{
	    wxString fn = data_locn;
	    fn.Replace(_T("txt"),_T("odt"));
		parent->startApplication(fn,_T(".odt"));
	}
}

wxString Boat::toODT(wxString path,wxString layout,bool mode)
{
	wxString s, odt;

	if(layout == _T(""))
	{
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK | wxICON_EXCLAMATION);
		return _T("");
	}

	saveData();

	odt = readLayoutFileODT(layout);

	for(unsigned int i = 0; i < ctrl.GetCount(); i++)
	{
		if(ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			wxTextCtrl* te = wxDynamicCast(ctrl[i], wxTextCtrl);

			switch(i)
			{
			case 0: 
				odt.Replace(wxT("#BOATNAME#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LBOATNAME#"),Export::replaceNewLine(true,parent->bname->GetLabel(),true));
				break;
			case 1: 
				odt.Replace(wxT("#HOMEPORT#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LHOMEPORT#"),Export::replaceNewLine(true,parent->m_staticText114->GetLabel(),true));
				break;
			case 2: 
				odt.Replace(wxT("#CALLSIGN#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LCALLSIGN#"),Export::replaceNewLine(true,parent->m_staticText115->GetLabel(),true));
				break;
			case 3: 
				odt.Replace(wxT("#HIN#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LHIN#"),Export::replaceNewLine(true,parent->m_staticText116->GetLabel(),true));
				break;
			case 4: 
				odt.Replace(wxT("#SAILNO#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LSAILNO#"),Export::replaceNewLine(true,parent->m_staticText117->GetLabel(),true));
				break;
			case 6: 
				odt.Replace(wxT("#REGISTRATION#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LREGISTRATION#"),Export::replaceNewLine(true,parent->m_staticText118->GetLabel(),true));
				break;
			case 5: 
				odt.Replace(wxT("#INSURANCE#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LINSURANCE#"),Export::replaceNewLine(true,parent->m_staticText119->GetLabel(),true));
				break;
			case 7: 
				odt.Replace(wxT("#POLICY#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LPOLICY#"),Export::replaceNewLine(true,parent->m_staticText120->GetLabel(),true));
				break;
			case 8: 
				odt.Replace(wxT("#MMSI#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LMMSI#"),Export::replaceNewLine(true,parent->m_staticText53->GetLabel(),true));
				break;
			case 9: 
				odt.Replace(wxT("#ONAME#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LONAME#"),Export::replaceNewLine(true,parent->m_staticText90->GetLabel(),true));
				break;
			case 10: 
				odt.Replace(wxT("#OFIRST#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LOFIRST#"),Export::replaceNewLine(true,parent->m_staticText91->GetLabel(),true));
				break;
			case 11: 
				odt.Replace(wxT("#TELEPHONE#"),Export::replaceNewLine(true,te->GetValue(), false));
				odt.Replace(wxT("#LTELEPHONE#"),Export::replaceNewLine(true,parent->m_staticText95->GetLabel(),true));
				break;
			case 12: 
				odt.Replace(wxT("#STREET#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LSTREET#"),Export::replaceNewLine(true,parent->m_staticText92->GetLabel(),true));
				break;
			case 13: 
				odt.Replace(wxT("#ZIP#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LZIP#"),Export::replaceNewLine(true,parent->m_staticText93->GetLabel(),true));
				break;
			case 14: 
				odt.Replace(wxT("#TOWN#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LTOWN#"),Export::replaceNewLine(true,parent->m_staticText94->GetLabel(),true));
				break;
			case 15: 
				odt.Replace(wxT("#TYPE#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LTYPE#"),Export::replaceNewLine(true,parent->m_staticText128->GetLabel(),true));
				break;
			case 16: 
				odt.Replace(wxT("#BUILDER#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LBUILDER#"),Export::replaceNewLine(true,parent->m_staticText125->GetLabel(),true));
				break;
			case 17: 
				odt.Replace(wxT("#HULL#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LHULL#"),Export::replaceNewLine(true,parent->m_staticText124->GetLabel(),true));
				break;
			case 18: 
				odt.Replace(wxT("#LAUNCHED#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LLAUNCHED#"),Export::replaceNewLine(true,parent->m_staticText126->GetLabel(),true));
				break;
			case 19:
				odt.Replace(wxT("#YARDNO#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LYARDNO#"),Export::replaceNewLine(true,parent->m_staticText127->GetLabel(),true));
				break;
			case 20:
				odt.Replace(wxT("#DESIGNER#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LDESIGNER#"),Export::replaceNewLine(true,parent->m_staticText123->GetLabel(),true));
				break;
			case 21: 
				odt.Replace(wxT("#CONSTRUCT#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LCONSTRUCT#"),Export::replaceNewLine(true,parent->m_staticText129->GetLabel(),true));
				break;
			case 22: 
				odt.Replace(wxT("#LOA#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LLOA#"),Export::replaceNewLine(true,parent->m_staticText106->GetLabel(),true));
				break;
			case 23:
				odt.Replace(wxT("#LOD#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LLOD#"),Export::replaceNewLine(true,parent->m_staticText107->GetLabel(),true));
				break;
			case 24:
				odt.Replace(wxT("#LWL#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LLWL#"),Export::replaceNewLine(true,parent->m_staticText108->GetLabel(),true));
				break;
			case 25: 
				odt.Replace(wxT("#BEAM#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LBEAM#"),Export::replaceNewLine(true,parent->m_staticText109->GetLabel(),true));
				break;
			case 26: 
				odt.Replace(wxT("#DRAFT#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LDRAFT#"),Export::replaceNewLine(true,parent->m_staticText110->GetLabel(),true));
				break;
			case 27: 
				odt.Replace(wxT("#LUSER1#"),Export::replaceNewLine(true,parent->UserLabel1->GetValue(),true));
				break;
			case 28:
				odt.Replace(wxT("#USER1#"),Export::replaceNewLine(true,te->GetValue(),false));
				break;
			case 29: 
				odt.Replace(wxT("#LUSER2#"),Export::replaceNewLine(true,parent->UserLabel2->GetValue(),true));
				break;
			case 30:
				odt.Replace(wxT("#USER2#"),Export::replaceNewLine(true,te->GetValue(),false));
				break;
			case 31: 
				odt.Replace(wxT("#LUSER3#"),Export::replaceNewLine(true,parent->UserLabel3->GetValue(),true));
				break;
			case 32:
				odt.Replace(wxT("#USER3#"),Export::replaceNewLine(true,te->GetValue(),false));
				break;
			case 33: 
				odt.Replace(wxT("#DISPL#"),Export::replaceNewLine(true,te->GetValue(),false));
				odt.Replace(wxT("#LDISPL#"),Export::replaceNewLine(true,parent->m_staticText122->GetLabel(),true));
				break;
			}
		}
	}

	odt.Replace(_T("#LOWNER#"),Export::replaceNewLine(true,parent->sbSizer6->GetStaticBox()->GetLabel(),true));
	if(odt.Contains(wxT("{{")))
		odt = repeatAreaODT(odt);
	
	wxString fn = data_locn;
	fn.Replace(_T("txt"),_T("odt"));
	if(mode == false)
	{
		fn.Replace(wxT("txt"),wxT("odt"));
	}
	else
		fn = path;

	if(::wxFileExists(fn))
		::wxRemoveFile(fn);

    auto_ptr<wxFFileInputStream> in(new wxFFileInputStream(layout_locn + layout + _T(".odt")));
    wxTempFileOutputStream out(fn);

    wxZipInputStream inzip(*in);
    wxZipOutputStream outzip(out);
	wxTextOutputStream odtFile(outzip);
    auto_ptr<wxZipEntry> entry;

    outzip.CopyArchiveMetaData(inzip);

    while (entry.reset(inzip.GetNextEntry()), entry.get() != NULL)
        if (!entry->GetName().Matches(_T("content.xml")))
            if (!outzip.CopyEntry(entry.release(), inzip))
                break;

    in.reset();

	outzip.PutNextEntry(_T("content.xml"));

	odtFile << odt;

	inzip.Eof() && outzip.Close() && out.Commit();
	return fn;
}

wxString Boat::repeatAreaODT(wxString odt)
{
	wxString topODT;
	wxString bottomODT;
	wxString middleODT;

	wxString seperatorTop = wxT("{{");
	wxString seperatorBottom = wxT("}}");

	int indexTop = odt.First(seperatorTop)+seperatorTop.Len();
	int indexBottom = odt.First(seperatorBottom)+seperatorBottom.Len();

	topODT = odt.substr(0,indexTop);
	middleODT = odt.substr(indexTop+9);
	indexTop = topODT.find_last_of('<');
	topODT = odt.substr(0,indexTop);
	bottomODT = odt.substr(indexBottom,odt.Len()-1);
	indexBottom = bottomODT.find_first_of('>')+1;
	bottomODT = bottomODT.substr(indexBottom,odt.Len()-1);

	middleODT.Replace(bottomODT,_T(""));
	indexBottom = middleODT.First(seperatorBottom)+seperatorBottom.Len();
	middleODT = middleODT.substr(0,indexBottom-1);
	indexBottom = middleODT.find_last_of('<');
	middleODT = middleODT.substr(0,indexBottom);

	topODT.Replace(_T("#LEQUIP#"),parent->sbSizer12->GetStaticBox()->GetLabel());
	topODT.Replace(_T("#LETYPE#"),parent->m_gridEquipment->GetColLabelValue(0));
	topODT.Replace(_T("#LDISCRIPTION#"),parent->m_gridEquipment->GetColLabelValue(1));
	topODT.Replace(_T("#LSERIAL#"),parent->m_gridEquipment->GetColLabelValue(2));
	topODT.Replace(_T("#LREMARKS#"),parent->m_gridEquipment->GetColLabelValue(3));

	wxString newMiddleODT = _T("");
	for(int i = 0; i < parent->m_gridEquipment->GetNumberRows(); i++)
	{
		wxString temp = middleODT;
		for(int c = 0; c < parent->m_gridEquipment->GetNumberCols(); c++)
		{
		wxString text = parent->m_gridEquipment->GetCellValue(i,c);

		switch(c)
		{
			case 0: 		
				temp.Replace(_T("#ETYPE#"),text);
				break;
			case 1: 		
				temp.Replace(_T("#DISCRIPTION#"),text);
				break;
			case 2: 		
				temp.Replace(_T("#SERIAL#"),text);
				break;
			case 3: 		
				temp.Replace(_T("#REMARKS#"),text);
				break;
		}
		}
		newMiddleODT += temp;
	}

	odt = topODT + newMiddleODT + bottomODT;
	return odt;
}

void Boat::toHTML(wxString path, wxString layout, bool mode)
{
	wxString s, html;

	if(layout == _T(""))
	{
		wxMessageBox(_("Sorry, no Layout installed"),_("Information"),wxOK | wxICON_EXCLAMATION);
		return;
	}

	saveData();

	boatLayoutFile = new wxTextFile(layout_locn+layout+wxT(".html"));
	boatLayoutFile->Open();

	wxString fn = data_locn;
	fn.Replace(_T("txt"),_T("html"));
	if(mode == false)
		boatHTMLFile = new wxFile(fn,wxFile::write);
	else
		boatHTMLFile = new wxFile(path,wxFile::write);

	for(unsigned int i = 0; i < boatLayoutFile->GetLineCount(); i++)
		html += boatLayoutFile->GetLine(i);
	boatLayoutFile->Close();

	for(unsigned int i = 0; i < ctrl.GetCount(); i++)
	{
		if(ctrl[i]->IsKindOf(CLASSINFO(wxTextCtrl)))
		{
			wxTextCtrl* te = wxDynamicCast(ctrl[i], wxTextCtrl);

			switch(i)
			{
			case 0: 
				html.Replace(wxT("#BOATNAME#"),te->GetValue());
				html.Replace(wxT("#LBOATNAME#"),parent->bname->GetLabel());
				break;
			case 1: 
				html.Replace(wxT("#HOMEPORT#"),te->GetValue());
				html.Replace(wxT("#LHOMEPORT#"),parent->m_staticText114->GetLabel());
				break;
			case 2: 
				html.Replace(wxT("#CALLSIGN#"),te->GetValue());
				html.Replace(wxT("#LCALLSIGN#"),parent->m_staticText115->GetLabel());
				break;
			case 3: 
				html.Replace(wxT("#HIN#"),te->GetValue());
				html.Replace(wxT("#LHIN#"),parent->m_staticText116->GetLabel());
				break;
			case 4: 
				html.Replace(wxT("#SAILNO#"),te->GetValue());
				html.Replace(wxT("#LSAILNO#"),parent->m_staticText117->GetLabel());
				break;
			case 6: 
				html.Replace(wxT("#REGISTRATION#"),te->GetValue());
				html.Replace(wxT("#LREGISTRATION#"),parent->m_staticText118->GetLabel());
				break;
			case 5: 
				html.Replace(wxT("#INSURANCE#"),te->GetValue());
				html.Replace(wxT("#LINSURANCE#"),parent->m_staticText119->GetLabel());
				break;
			case 7: 
				html.Replace(wxT("#POLICY#"),te->GetValue());
				html.Replace(wxT("#LPOLICY#"),parent->m_staticText120->GetLabel());
				break;
			case 8: 
				html.Replace(wxT("#MMSI#"),te->GetValue());
				html.Replace(wxT("#LMMSI#"),parent->m_staticText53->GetLabel());
				break;
			case 9: 
				html.Replace(wxT("#ONAME#"),te->GetValue());
				html.Replace(wxT("#LONAME#"),parent->m_staticText90->GetLabel());
				break;
			case 10: 
				html.Replace(wxT("#OFIRST#"),te->GetValue());
				html.Replace(wxT("#LOFIRST#"),parent->m_staticText91->GetLabel());
				break;
			case 11: 
				html.Replace(wxT("#TELEPHONE#"),te->GetValue());
				html.Replace(wxT("#LTELEPHONE#"),parent->m_staticText95->GetLabel());
				break;
			case 12: 
				html.Replace(wxT("#STREET#"),te->GetValue());
				html.Replace(wxT("#LSTREET#"),parent->m_staticText92->GetLabel());
				break;
			case 13: 
				html.Replace(wxT("#ZIP#"),te->GetValue());
				html.Replace(wxT("#LZIP#"),parent->m_staticText93->GetLabel());
				break;
			case 14: 
				html.Replace(wxT("#TOWN#"),te->GetValue());
				html.Replace(wxT("#LTOWN#"),parent->m_staticText94->GetLabel());
				break;
			case 15: 
				html.Replace(wxT("#TYPE#"),te->GetValue());
				html.Replace(wxT("#LTYPE#"),parent->m_staticText128->GetLabel());
				break;
			case 16: 
				html.Replace(wxT("#BUILDER#"),te->GetValue());
				html.Replace(wxT("#LBUILDER#"),parent->m_staticText125->GetLabel());
				break;
			case 17: 
				html.Replace(wxT("#HULL#"),te->GetValue());
				html.Replace(wxT("#LHULL#"),parent->m_staticText124->GetLabel());
				break;
			case 18: 
				html.Replace(wxT("#LAUNCHED#"),te->GetValue());
				html.Replace(wxT("#LLAUNCHED#"),parent->m_staticText126->GetLabel());
				break;
			case 19:
				html.Replace(wxT("#YARDNO#"),te->GetValue());
				html.Replace(wxT("#LYARDNO#"),parent->m_staticText127->GetLabel());
				break;
			case 20:
				html.Replace(wxT("#DESIGNER#"),te->GetValue());
				html.Replace(wxT("#LDESIGNER#"),parent->m_staticText123->GetLabel());
				break;
			case 21: 
				html.Replace(wxT("#CONSTRUCT#"),te->GetValue());
				html.Replace(wxT("#LCONSTRUCT#"),parent->m_staticText129->GetLabel());
				break;
			case 22: 
				html.Replace(wxT("#LOA#"),te->GetValue());
				html.Replace(wxT("#LLOA#"),parent->m_staticText106->GetLabel());
				break;
			case 23:
				html.Replace(wxT("#LOD#"),te->GetValue());
				html.Replace(wxT("#LLOD#"),parent->m_staticText107->GetLabel());
				break;
			case 24:
				html.Replace(wxT("#LWL#"),te->GetValue());				
				html.Replace(wxT("#LLWL#"),parent->m_staticText108->GetLabel());
				break;
			case 25: 
				html.Replace(wxT("#BEAM#"),te->GetValue());				
				html.Replace(wxT("#LBEAM#"),parent->m_staticText109->GetLabel());
				break;
			case 26: 
				html.Replace(wxT("#DRAFT#"),te->GetValue());
				html.Replace(wxT("#LDRAFT#"),parent->m_staticText110->GetLabel());
				break;
			case 27: 
				html.Replace(wxT("#LUSER1#"),parent->UserLabel1->GetValue());
				break;
			case 28:
				html.Replace(wxT("#USER1#"),te->GetValue());
				break;
			case 29: 
				html.Replace(wxT("#LUSER2#"),parent->UserLabel2->GetValue());
				break;
			case 30:
				html.Replace(wxT("#USER2#"),te->GetValue());
				break;
			case 31: 
				html.Replace(wxT("#LUSER3#"),parent->UserLabel3->GetValue());
				break;
			case 32:
				html.Replace(wxT("#USER3#"),te->GetValue());
				break;
			case 33: 
				html.Replace(wxT("#DISPL#"),te->GetValue());				
				html.Replace(wxT("#LDISPL#"),parent->m_staticText122->GetLabel());
				break; 
			}
		}
	}

	html.Replace(_T("#LOWNER#"),parent->sbSizer6->GetStaticBox()->GetLabel());
	html.Replace(_T("#LDATE#"),parent->m_gridGlobal->GetColLabelValue(1));
	html.Replace(_T("#LEQUIP#"),parent->sbSizer12->GetStaticBox()->GetLabel());

	if(html.Contains(wxT("<!--Repeat -->")))
		html = repeatArea(html);
#ifdef __WXOSX__
       wxString str(html.wx_str(), wxConvUTF8);
#else
	wxString str(html);	
#endif
	boatHTMLFile->Write(str);
	boatHTMLFile->Close();
}

wxString Boat::repeatArea(wxString html)
{
	wxString topHTML;
	wxString bottomHTML;
	wxString middleHTML;

	wxString seperatorTop = wxT("<!--Repeat -->");
	wxString seperatorBottom = wxT("<!--Repeat End -->");

	int indexTop = html.First(seperatorTop)+seperatorTop.Len();
	int indexBottom = html.First(seperatorBottom)+seperatorBottom.Len();

	topHTML = html.substr(0,indexTop);
	bottomHTML = html.substr(indexBottom,html.Len()-1);
	middleHTML = html.substr(indexTop,indexBottom-indexTop);

	topHTML.Replace(_T("#LETYPE#"),parent->m_gridEquipment->GetColLabelValue(0));
	topHTML.Replace(_T("#LDISCRIPTION#"),parent->m_gridEquipment->GetColLabelValue(1));
	topHTML.Replace(_T("#LSERIAL#"),parent->m_gridEquipment->GetColLabelValue(2));
	topHTML.Replace(_T("#LREMARKS#"),parent->m_gridEquipment->GetColLabelValue(3));

	wxString newMiddleHTML = _T("");
	for(int i = 0; i < parent->m_gridEquipment->GetNumberRows(); i++)
	{
		wxString temp = middleHTML;
		for(int c = 0; c < parent->m_gridEquipment->GetNumberCols(); c++)
		{
		wxString text = parent->m_gridEquipment->GetCellValue(i,c);

		switch(c)
		{
			case 0: 		
				temp.Replace(_T("#ETYPE#"),text);
				break;
			case 1: 		
				temp.Replace(_T("#DISCRIPTION#"),text);
				break;
			case 2: 		
				temp.Replace(_T("#SERIAL#"),text);
				break;
			case 3: 		
				temp.Replace(_T("#REMARKS#"),text);
				break;
		}
		}
		newMiddleHTML += temp;
	}

	html = topHTML + newMiddleHTML + bottomHTML;
	return html;
}

void Boat::viewHTML(wxString path, wxString layout, bool mode)
{
#ifndef __WXOSX__
	if(parent->logbookPlugIn->opt->filterLayout)
        layout.Prepend(parent->logbookPlugIn->opt->layoutPrefix[LogbookDialog::BOAT]);
#endif
    toHTML(path, layout, mode);
	if(layout != _T(""))
	{
	    wxString fn = data_locn;
	    fn.Replace(_T("txt"),_T("html"));
	    parent->startBrowser(fn);
	}
}

void Boat::addEquip()
{
	wxString s;

	modified = true;

	parent->m_gridEquipment->AppendRows();
	parent->m_gridEquipment->SetCellValue(parent->m_gridEquipment->GetNumberRows()-1,
										  parent->m_gridEquipment->GetNumberCols()-1,_T(" "));
	parent->m_gridEquipment->MakeCellVisible(parent->m_gridEquipment->GetNumberRows()-1,0);
	
	equipFile->Open();

	for(int i = 0; i < parent->m_gridEquipment->GetNumberCols(); i++)
	{
		s += _T(",");	
	}
	
	equipFile->AddLine(s);
	equipFile->Write();
	equipFile->Close();
}
void Boat::cellChanged(int row, int col)
{
	modified = true;

	if(parent->m_gridEquipment->GetCellValue(
		row,parent->m_gridEquipment->GetNumberCols()-1) == _T(""))
			parent->m_gridEquipment->SetCellValue(
				row,parent->m_gridEquipment->GetNumberCols()-1,_T(" "));
}

void Boat::deleteRow(int row)
{
	int answer = wxMessageBox(wxString::Format(_("Delete Row Nr. %i ?"),row+1), _("Confirm"),
                              wxYES_NO | wxCANCEL | wxICON_QUESTION, parent);
	if (answer == wxYES)
		parent->m_gridEquipment->DeleteRows(row);
}

void Boat::toCSV(wxString savePath)
{
	wxString line;
	wxString s;

	saveData();

	wxTextFile* csvFile = new wxTextFile(savePath);
	wxString path = savePath;
	path.Replace(wxT("Boat"),wxT("Equipment"));
	wxTextFile* csvEquipFile = new wxTextFile(path);

	if(csvFile->Exists())
	{
		::wxRemoveFile(savePath);
		::wxRemoveFile(path);
		csvFile->Create();
		equipFile->Create();
	}

	saveCSV(csvFile,true);
	saveCSV(csvEquipFile,false);

}

void Boat::saveCSV(wxTextFile* file, bool mode)
{
	wxString line, s;
	int col = 0;

	saveData();

	((mode)?boatFile:equipFile)->Open();

	for(unsigned int i = 0; i < ((mode)?boatFile:equipFile)->GetLineCount(); i++)
	{
		line = ((mode)?boatFile:equipFile)->GetLine(i);
		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY );
		col = 0;

		while ( tkz.HasMoreTokens() )
		{
			if(mode == true)
			{
				if( col != 27 && col != 29 && col != 31) 
				{
					s += wxT("\"")+parent->restoreDangerChar(tkz.GetNextToken().RemoveLast())+_T("\",");
				}
				else
					tkz.GetNextToken();
			}
			else
				s += wxT("\"")+parent->restoreDangerChar(tkz.GetNextToken().RemoveLast())+_T("\",");
			col++;
		}
		s.RemoveLast();
		file->AddLine(s);
		s=_T("");
	}

	file->Write();
	file->Close();
	((mode)?boatFile:equipFile)->Close();

}

void Boat::toXML(wxString savePath)
{
	wxString line;
	wxString s;

	wxTextFile* xmlFile = new wxTextFile(savePath);
	wxString path = savePath;
	path.Replace(wxT("Boat"),wxT("Equipment"));
	wxTextFile* xmlEquipFile = new wxTextFile(path);

	if(xmlFile->Exists())
	{
		::wxRemoveFile(savePath);
		::wxRemoveFile(path);
		xmlFile->Create();
		xmlEquipFile->Create();
	}

	saveData();
	saveXML(xmlFile,true);
	saveXML(xmlEquipFile,false);

}

void Boat::saveXML(wxTextFile* xmlFile, bool mode)
{
	wxTextFile* file;
	wxString temp;

	wxString line, s;

	if(mode)
		file = boatFile;
	else 
		file = equipFile;

	file->Open();

	xmlFile->AddLine(parent->xmlHead);

	for(unsigned int i = 0; i < file->GetLineCount(); i++)
	{
		int col = 0;
		line = file->GetLine(i);
		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY );
		s = wxString::Format(_T("<Row ss:Height=\"%u\">"),parent->m_gridGlobal->GetRowHeight(i));

		while ( tkz.HasMoreTokens() )
		{ 
			if(col != 27 && col != 29 && col != 31)
			{
				s += _T("<Cell>");
				s += _T("<Data ss:Type=\"String\">#DATA#</Data>");
				temp = parent->restoreDangerChar(tkz.GetNextToken().RemoveLast());
				temp.Replace(_T("\n"),_T("&#10;"));
				temp.Replace(_T("&"),_T("&amp;"));
				temp.Replace(_T("\""),_T("&quot;"));
				temp.Replace(_T("<"),_T("&lt;"));
				temp.Replace(_T(">"),_T("&gt;"));
				temp.Replace(_T("'"),_T("&apos;"));
				s.Replace(_T("#DATA#"),temp);
				s += _T("</Cell>");
			}
			else tkz.GetNextToken();
			col++;
		}
		s += _T("</Row>");
		xmlFile->AddLine(s);
	}

	xmlFile->AddLine(parent->xmlEnd);
	xmlFile->Write();
	file->Close();
	xmlFile->Close();
}

void Boat::backup(wxString path)
{
	wxFileName fn(path);
	wxString z = fn.GetName();

	saveData();

	wxCopyFile(data_locn,path);
	path.Replace(z,_T("equipment"));
	wxCopyFile(equip_locn,path);
}

void Boat::toODS(wxString path)
{
	wxString s = _T("");
	wxString line;
	wxString temp;

	saveData();

	saveODS(path,false);

	wxFileName fn(path);
	wxString sf = fn.GetName();
	path.Replace(sf,_T("equipment"));

	saveODS(path, true);
}

void Boat::saveODS( wxString path, bool mode )
{	
	wxString temp, col, tableName;
	int x = 0;

	if(mode)
	{
		temp = equip_locn;
		col = _T("table:number-columns-repeated=\"4\"");
		tableName = _T("Equipment");
		x = parent->m_gridEquipment->GetNumberCols();
	}
	else
	{
		temp = data_locn;
		col = _T("table:number-columns-repeated=\"31\""); 
		tableName = _T("Boat");
		x = ctrlStaticText.GetCount();
	}


	wxFileInputStream input( temp );
	wxTextInputStream* stream = new wxTextInputStream (input);

	wxFFileOutputStream out(path);
	wxZipOutputStream zip(out);
	wxTextOutputStream txt(zip);
	wxString sep(wxFileName::GetPathSeparator());

	temp = parent->content;
	temp.Replace(_T("table:number-columns-repeated=\"33\""),col);
	temp.Replace(_T("Logbook"),tableName);
	zip.PutNextEntry(wxT("content.xml"));
	txt << temp;

	txt << _T("<table:table-row table:style-name=\"ro2\">");
	for(int i = 0; i < x; i++)
	{
		txt << _T("<table:table-cell office:value-type=\"string\">");
		txt << _T("<text:p>");
		if(!mode)
		{
			if(i == 27)
			{
				txt << parent->UserLabel1->GetValue();
				txt << _T("</text:p>");
				txt << _T("</table:table-cell>");

				txt << _T("<table:table-cell office:value-type=\"string\">");
				txt << _T("<text:p>");
				txt << parent->UserLabel2->GetValue();
				txt << _T("</text:p>");
				txt << _T("</table:table-cell>");

				txt << _T("<table:table-cell office:value-type=\"string\">");
				txt << _T("<text:p>");
				txt << parent->UserLabel3->GetValue();
				txt << _T("</text:p>");
				txt << _T("</table:table-cell>");

				txt << _T("<table:table-cell office:value-type=\"string\">");
				txt << _T("<text:p>");
				wxStaticText* t = wxDynamicCast(ctrlStaticText[i], wxStaticText);
				txt << t->GetLabel();
			}
			else
			{
				wxStaticText* t = wxDynamicCast(ctrlStaticText[i], wxStaticText);
				txt << t->GetLabel();
			}
		}
		else
			txt << parent->m_gridEquipment->GetColLabelValue(i);
		txt << _T("</text:p>");
		txt << _T("</table:table-cell>");
	}
	txt << _T("</table:table-row>");

//	bool empty = false;
	long emptyCol = 0;

    wxString line;
	while(!input.Eof())
	{
		line = stream->ReadLine(); // for #1.2#
		int col = 0;
		if(input.Eof()) break;
		txt << _T("<table:table-row table:style-name=\"ro2\">");
		wxStringTokenizer tkz(line, _T("\t"),wxTOKEN_RET_EMPTY );

		while ( tkz.HasMoreTokens() )
		{ 
			wxString s = parent->restoreDangerChar(tkz.GetNextToken().RemoveLast());
			if(s == _T(""))
			{
				txt <<  _T("<table:table-cell />");
				//empty = true;
				emptyCol++;
				col++;
				continue;
			}

			if(col != 27 && col != 29 && col != 31 )
				txt << _T("<table:table-cell office:value-type=\"string\">");
			else
			{ 
			  col++; 
			  continue; 
			}

			wxStringTokenizer str(s, _T("\n"));
			while(str.HasMoreTokens())
			{
				wxString e = str.GetNextToken();
				e.Replace(_T("&"),_T("&amp;"));
				e.Replace(_T("\""),_T("&quot;"));
				e.Replace(_T("<"),_T("&lt;"));
				e.Replace(_T(">"),_T("&gt;"));
				e.Replace(_T("'"),_T("&apos;"));
				txt << _T("<text:p>");
				txt << e;
				txt << _T("</text:p>");
			}
			txt << _T("</table:table-cell>");
			col++;
		}
		txt << _T("</table:table-row>");;

	}
#ifdef __WXOSX__
    delete stream;
#endif
	txt << parent->contentEnd;

	zip.PutNextEntry(wxT("mimetype"));
	txt << wxT("application/vnd.oasis.opendocument.spreadsheet");

	zip.PutNextEntry(wxT("styles.xml"));
	txt << parent->styles;

	zip.PutNextEntry(wxT("meta.xml"));
	txt << parent->meta;

	zip.PutNextEntry(wxT("META-INF") + sep + wxT("manifest.xml"));
	txt << parent->manifest;

	zip.PutNextEntry(wxT("Thumbnails") + sep);

	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("floater"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("menubar"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("popupmenu"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("progressbar"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("statusbar"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("toolbar"));
	zip.PutNextEntry(wxT("Configurations2") + sep + wxT("images") + sep + wxT("Bitmaps"));

	zip.Close();
	out.Close();
}


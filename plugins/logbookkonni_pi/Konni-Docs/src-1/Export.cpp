#ifndef WX_PRECOMP
      #include <wx/wx.h>
#endif

#include "Export.h"
#include "LogbookDialog.h"
#include "boat.h"

#include <wx/filename.h> 
#include <wx/mimetype.h>
#include <wx/wfstream.h> 
#include <wx/txtstrm.h> 
#include <wx/zipstrm.h> 
#include <wx/generic/gridctrl.h>

#include <memory>
using namespace std;

Export::Export(LogbookDialog *dialog)
{
	this->dialog = dialog;
}

Export::~Export(void)
{
}

wxString Export::readLayoutODT(wxString path,wxString layout)
{
	wxString odt = _T("");


	wxString filename = path + layout + _T(".odt");

	if(wxFileExists(filename))
	{
		static const wxString fn = _T("content.xml");
		wxZipInputStream zip(filename,fn);
		wxTextInputStream txt(zip);
		while(!zip.Eof())
			odt += txt.ReadLine();
	}
	return odt;
}

bool Export::cutInPartsODT(wxString odt, wxString* top, wxString* header, 
						wxString* middle, wxString* bottom)
{
	int indexTop;
	int indexBottom;
	wxString seperatorTop        = wxT("[[");
	wxString seperatorBottom     = wxT("]]");

	if(odt.Contains(seperatorTop))
	{
		indexTop    = odt.Find(seperatorTop);
		indexBottom = odt.Find(seperatorBottom);
		*top				= odt.substr(0,indexTop);
		*top				= (*top).substr(0,(*top).find_last_of('<'));
		*bottom			= odt.substr(indexBottom+1);
		*bottom			= (*bottom).substr((*bottom).find_first_of('>')+1);
		*middle			= odt.substr(indexTop+11);
		*middle			= (*middle).substr(0,(*middle).Find(seperatorBottom));
		*middle			= (*middle).substr(0,(*middle).find_last_of('<'));
		return _T("OK");
	}
	else
		return _T("");

}

bool Export::writeToHTML(wxTextFile* logFile, wxGrid* grid, wxString filenameOut,wxString filenameIn, 
		wxString top,wxString header,wxString middle,wxString bottom, int mode)
{
	wxFileInputStream input( filenameIn );
	
	wxFileOutputStream output( filenameOut);
	wxTextOutputStream htmlFile(output);

	top.Replace(wxT("#TYPE#"),dialog->boatType->GetValue());
	top.Replace(wxT("#BOATNAME#"),dialog->boatName->GetValue());
	top.Replace(wxT("#HOMEPORT#"),dialog->homeport->GetValue());
	top.Replace(wxT("#CALLSIGN#"),dialog->callsign->GetValue());
	top.Replace(wxT("#REGISTRATION#"),dialog->registration->GetValue());

	htmlFile << top;

	wxString newMiddle = middle;
	for(int row = 0; row < grid->GetNumberRows(); row++)
		{
		  newMiddle = setPlaceHolders(mode,grid, row, middle);
		  htmlFile << newMiddle;
		}
	
	htmlFile << bottom;

	output.Close();
	return true;
}

wxString Export::readLayoutHTML(wxString path,wxString layoutFileName)
{
	wxString html, path1;

	path1 = path + layoutFileName + wxT(".html");;
	wxTextFile layout(path1);
	
	layout.Open();

	for(unsigned int i = 0; i < layout.GetLineCount(); i++)
	{
		html += layout.GetLine(i)+_T("\n");
	}

	layout.Close();

	return html;
}

bool Export::cutInPartsHTML(wxString html, wxString* top, wxString* header, wxString* middle, wxString* bottom)
{
	wxString seperatorTop = _T("<!-- Repeat -->");
	wxString seperatorBottom = _T("<!-- Repeat End -->");

	int indexTop = html.Find(seperatorTop);
	indexTop += seperatorTop.Len();
	int indexBottom = html.Find(seperatorBottom);
	indexBottom += seperatorBottom.Len();

	*top = html.substr(0,indexTop);
	*bottom = html.substr(indexBottom);
	*middle = html.substr(indexTop,(indexBottom-indexTop)-seperatorBottom.Len());

	return true;
}

wxTextFile* Export::setFiles(wxString savePath, wxString *path, int mode)
{
	if(mode == 0 )
	{	(*path).Replace(wxT("txt"),wxT("html")); }
	else if(mode == 1)
	{	(*path).Replace(wxT("txt"),wxT("odt")); }
	else 
	{	(*path) = savePath; }

	if(::wxFileExists(*path))
		::wxRemoveFile(*path);

	wxTextFile *logFile = new wxTextFile(*path);
	return logFile;
}

bool Export::writeToODT(wxTextFile* logFile,wxGrid* grid, wxString filenameOut,wxString filenameIn, wxString top,wxString header,
				wxString middle,wxString bottom, int mode)
{
	auto_ptr<wxFFileInputStream> in(new wxFFileInputStream(filenameIn));
    wxTempFileOutputStream out(filenameOut);

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

    odtFile << top;

	wxString newMiddle;
	for(int row = 0; row < grid->GetNumberRows(); row++)
		{
		  newMiddle = setPlaceHolders(mode,grid, row, middle);
		  odtFile << newMiddle;
		}

	odtFile << bottom;

	inzip.Eof() && outzip.Close() && out.Commit();
	logFile->Close();

	return true;
}


wxString Export::replaceNewLine(int mode, wxString str, bool label)
{
	str.Replace(wxT("&"),wxT("&amp;"));
	str.Replace(wxT("<"),wxT("&lt;"));
	str.Replace(wxT(">"),wxT("&gt;"));
	str.Replace(wxT("'"),wxT("&apos;"));
	str.Replace(wxT("\""),wxT("&quot;"));
//	str.Replace(wxT("°"),wxT("&deg;"));

	if(mode == 0)
		{// HTML
		  str.Replace(wxT("\n"),wxT("<br>"));
		} 
	else
		{// ODT
		  if(!label)
		  {
		  str.Replace(wxT("\n"),wxT("<text:line-break/>"));
		  str.Replace(wxT("\xD\xA"),wxT("<text:line-break/>"));
		  }
		  else
		  {
		  str.Replace(wxT("\n"),wxT(" "));
		  str.Replace(wxT("\xD\xA"),wxT(" "));
		  }
		}

	return str;
}

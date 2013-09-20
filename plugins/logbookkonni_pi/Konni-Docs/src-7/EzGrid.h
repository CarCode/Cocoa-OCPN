#ifndef _EZGRID_H_INCLUDED_
#define _EZGRID_H_INCLUDED_

#include "wx/grid.h"


// EzGrid Provides extra functionality to wxGrid
class EzGrid : public wxGrid
{
public:
	EzGrid(wxWindow *parent,
		wxWindowID id,	const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, long style = wxWANTS_CHARS,
		const wxString& name = wxPanelNameStr);
	~EzGrid();
	void PreviousState();
	void SetNumberRows(int nRows);
	void RecalcRowLabel();
	void UpdateNumRows();
	void ClearSelection();
	void RevertSel()
	{
		if (m_selTemp) 
		{
			wxASSERT(m_selection == NULL);
			m_selection = m_selTemp;
			m_selTemp = NULL;
		}
	}
protected:
	wxSize DoGetBestSize() const;
	DECLARE_EVENT_TABLE()
	void OnCellLeftClick(wxGridEvent& event);
#ifdef __WXGTK__
    void OnMouseWheel( wxMouseEvent& event );
#endif
	wxGridSelection *m_selTemp;
};



#endif // _EZGRID_H_INCLUDED_

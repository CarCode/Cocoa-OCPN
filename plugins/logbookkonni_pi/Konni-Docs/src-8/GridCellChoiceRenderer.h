#ifndef _GRIDCELLCHOICERENDERER_H_INCLUDED_
#define _GRIDCELLCHOICERENDERER_H_INCLUDED_

#include "wx/grid.h"
#include "wx/laywin.h"

class wxGridCellChoiceRenderer : public wxGridCellStringRenderer
{
public:
	wxGridCellChoiceRenderer(wxLayoutAlignment border = wxLAYOUT_NONE) : 
		m_border(border) {}
		~wxGridCellChoiceRenderer(void){}
	void Draw(wxGrid& grid,
		wxGridCellAttr& attr,
		wxDC& dc,
		const wxRect& rect,
		int row, int col,
		bool isSelected);
	wxGridCellRenderer *Clone() const
		{ return new wxGridCellChoiceRenderer; }
private:
	wxLayoutAlignment m_border;
};


#endif // _GRIDCELLCHOICERENDERER_H_INCLUDED_

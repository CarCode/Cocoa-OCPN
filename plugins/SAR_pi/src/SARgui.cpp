///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SARgui.h"

///////////////////////////////////////////////////////////////////////////

DlgDef::DlgDef( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	bSframe = new wxBoxSizer( wxVERTICAL );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	wx_panel_dome = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9111;
	bSizer9111 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1511 = new wxStaticText( wx_panel_dome, wxID_ANY, _("Parallel Search"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1511->Wrap( -1 );
	m_staticText1511->SetFont( wxFont( 20, 70, 90, 92, false, wxEmptyString ) );
	
	bSizer9111->Add( m_staticText1511, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer1311;
	bSizer1311 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer1421;
	bSizer1421 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3221 = new wxStaticText( wx_panel_dome, wxID_ANY, _("Approach course     "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3221->Wrap( -1 );
	bSizer1421->Add( m_staticText3221, 0, wxALL, 5 );
	
	m_Approach_PS = new wxTextCtrl( wx_panel_dome, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Approach_PS->SetMaxLength( 0 ); 
	bSizer1421->Add( m_Approach_PS, 0, wxALL, 5 );
	
	m_staticText3321 = new wxStaticText( wx_panel_dome, wxID_ANY, _("Degrees"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3321->Wrap( -1 );
	bSizer1421->Add( m_staticText3321, 0, wxALL, 5 );
	
	
	bSizer1311->Add( bSizer1421, 0, 0, 5 );
	
	wxBoxSizer* bSizer14112;
	bSizer14112 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText32112 = new wxStaticText( wx_panel_dome, wxID_ANY, _("Leg Distance              "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32112->Wrap( -1 );
	bSizer14112->Add( m_staticText32112, 0, wxALL, 5 );
	
	m_dx_PS = new wxTextCtrl( wx_panel_dome, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_dx_PS->SetMaxLength( 0 ); 
	bSizer14112->Add( m_dx_PS, 0, wxALL, 5 );
	
	m_staticText33112 = new wxStaticText( wx_panel_dome, wxID_ANY, _("NM"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33112->Wrap( -1 );
	bSizer14112->Add( m_staticText33112, 0, wxALL, 5 );
	
	
	bSizer1311->Add( bSizer14112, 0, 0, 5 );
	
	wxBoxSizer* bSizer141111;
	bSizer141111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText321111 = new wxStaticText( wx_panel_dome, wxID_ANY, _("Leg Spacing                "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText321111->Wrap( -1 );
	bSizer141111->Add( m_staticText321111, 0, wxALL, 5 );
	
	m_dy_PS = new wxTextCtrl( wx_panel_dome, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_dy_PS->SetMaxLength( 0 ); 
	bSizer141111->Add( m_dy_PS, 0, wxALL, 5 );
	
	m_staticText331111 = new wxStaticText( wx_panel_dome, wxID_ANY, _("NM"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText331111->Wrap( -1 );
	bSizer141111->Add( m_staticText331111, 0, wxALL, 5 );
	
	
	bSizer1311->Add( bSizer141111, 0, 0, 5 );
	
	wxBoxSizer* bSizer1411111;
	bSizer1411111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3211111 = new wxStaticText( wx_panel_dome, wxID_ANY, _("Number of Legs       "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3211111->Wrap( -1 );
	bSizer1411111->Add( m_staticText3211111, 0, wxALL, 5 );
	
	m_n_PS = new wxTextCtrl( wx_panel_dome, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_n_PS->SetMaxLength( 0 ); 
	bSizer1411111->Add( m_n_PS, 0, wxALL, 5 );
	
	m_staticText3311111 = new wxStaticText( wx_panel_dome, wxID_ANY, _("n"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3311111->Wrap( -1 );
	bSizer1411111->Add( m_staticText3311111, 0, wxALL, 5 );
	
	
	bSizer1311->Add( bSizer1411111, 0, 0, 5 );
	
	wxBoxSizer* bSizer14111112;
	bSizer14111112 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText32111112 = new wxStaticText( wx_panel_dome, wxID_ANY, _("Search Speed             "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32111112->Wrap( -1 );
	bSizer14111112->Add( m_staticText32111112, 0, wxALL, 5 );
	
	m_Speed_PS = new wxTextCtrl( wx_panel_dome, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Speed_PS->SetMaxLength( 0 ); 
	bSizer14111112->Add( m_Speed_PS, 0, wxALL, 5 );
	
	m_staticText33111111 = new wxStaticText( wx_panel_dome, wxID_ANY, _("kts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33111111->Wrap( -1 );
	bSizer14111112->Add( m_staticText33111111, 0, wxALL, 5 );
	
	
	bSizer1311->Add( bSizer14111112, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer14111111;
	bSizer14111111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText32111111 = new wxStaticText( wx_panel_dome, wxID_ANY, _("For Ship Number      "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32111111->Wrap( -1 );
	bSizer14111111->Add( m_staticText32111111, 0, wxALL, 5 );
	
	wxString m_NshipChoices[] = { _("1"), _("2") };
	int m_NshipNChoices = sizeof( m_NshipChoices ) / sizeof( wxString );
	m_Nship = new wxChoice( wx_panel_dome, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_NshipNChoices, m_NshipChoices, 0 );
	m_Nship->SetSelection( 0 );
	bSizer14111111->Add( m_Nship, 0, wxALL, 5 );
	
	
	bSizer1311->Add( bSizer14111111, 0, 0, 5 );
	
	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button311 = new wxButton( wx_panel_dome, wxID_ANY, _("Calculate"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer71->Add( m_button311, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_button3111 = new wxButton( wx_panel_dome, wxID_ANY, _("Generate &GPX"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer71->Add( m_button3111, 0, wxALL, 5 );
	
	
	bSizer1311->Add( bSizer71, 1, wxALIGN_CENTER, 5 );
	
	
	bSizer9111->Add( bSizer1311, 0, 0, 5 );
	
	
	wx_panel_dome->SetSizer( bSizer9111 );
	wx_panel_dome->Layout();
	bSizer9111->Fit( wx_panel_dome );
	bSizer3->Add( wx_panel_dome, 0, wxALL, 5 );
	
	m_bitmap_trackln = new wxStaticBitmap( m_panel1, wxID_ANY, wxBitmap( wxT("bitmaps/trackln.jpg"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_bitmap_trackln, 0, wxALL, 5 );
	
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_notebook1->AddPage( m_panel1, _("SAR: Trackline"), false );
	m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	m_panel2->SetMinSize( wxSize( -1,335 ) );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_panel7 = new wxPanel( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer911;
	bSizer911 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText151 = new wxStaticText( m_panel7, wxID_ANY, _("Expanding Square\n         Search"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText151->Wrap( -1 );
	m_staticText151->SetFont( wxFont( 20, 70, 90, 92, false, wxEmptyString ) );
	
	bSizer911->Add( m_staticText151, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer131;
	bSizer131 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer142;
	bSizer142 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText322 = new wxStaticText( m_panel7, wxID_ANY, _("Approach course     "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText322->Wrap( -1 );
	bSizer142->Add( m_staticText322, 0, wxALL, 5 );
	
	m_Approach_ES = new wxTextCtrl( m_panel7, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Approach_ES->SetMaxLength( 0 ); 
	bSizer142->Add( m_Approach_ES, 0, wxALL, 5 );
	
	m_staticText332 = new wxStaticText( m_panel7, wxID_ANY, _("Degrees"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText332->Wrap( -1 );
	bSizer142->Add( m_staticText332, 0, wxALL, 5 );
	
	
	bSizer131->Add( bSizer142, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer1411;
	bSizer1411 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3211 = new wxStaticText( m_panel7, wxID_ANY, _("Leg Distance               "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3211->Wrap( -1 );
	bSizer1411->Add( m_staticText3211, 0, wxALL, 5 );
	
	m_dx_ES = new wxTextCtrl( m_panel7, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_dx_ES->SetMaxLength( 0 ); 
	bSizer1411->Add( m_dx_ES, 0, wxALL, 5 );
	
	m_staticText3311 = new wxStaticText( m_panel7, wxID_ANY, _("NM"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3311->Wrap( -1 );
	bSizer1411->Add( m_staticText3311, 0, wxALL, 5 );
	
	
	bSizer131->Add( bSizer1411, 0, 0, 5 );
	
	wxBoxSizer* bSizer14111;
	bSizer14111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText32111 = new wxStaticText( m_panel7, wxID_ANY, _("Number of Squares"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32111->Wrap( -1 );
	bSizer14111->Add( m_staticText32111, 0, wxALL, 5 );
	
	m_n_ES = new wxTextCtrl( m_panel7, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_n_ES->SetMaxLength( 0 ); 
	bSizer14111->Add( m_n_ES, 0, wxALL, 5 );
	
	m_staticText33111 = new wxStaticText( m_panel7, wxID_ANY, _("n"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33111->Wrap( -1 );
	bSizer14111->Add( m_staticText33111, 0, wxALL, 5 );
	
	
	bSizer131->Add( bSizer14111, 0, 0, 5 );
	
	wxBoxSizer* bSizer141111121;
	bSizer141111121 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText321111121 = new wxStaticText( m_panel7, wxID_ANY, _("Search Speed             "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText321111121->Wrap( -1 );
	bSizer141111121->Add( m_staticText321111121, 0, wxALL, 5 );
	
	m_Speed_ES = new wxTextCtrl( m_panel7, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Speed_ES->SetMaxLength( 0 ); 
	bSizer141111121->Add( m_Speed_ES, 0, wxALL, 5 );
	
	m_staticText331111111 = new wxStaticText( m_panel7, wxID_ANY, _("kts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText331111111->Wrap( -1 );
	bSizer141111121->Add( m_staticText331111111, 0, wxALL, 5 );
	
	
	bSizer131->Add( bSizer141111121, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer711;
	bSizer711 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button3112 = new wxButton( m_panel7, wxID_ANY, _("Calculate"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer711->Add( m_button3112, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_button31111 = new wxButton( m_panel7, wxID_ANY, _("Generate &GPX"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer711->Add( m_button31111, 0, wxALL, 5 );
	
	
	bSizer131->Add( bSizer711, 1, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	bSizer911->Add( bSizer131, 0, 0, 5 );
	
	
	m_panel7->SetSizer( bSizer911 );
	m_panel7->Layout();
	bSizer911->Fit( m_panel7 );
	bSizer4->Add( m_panel7, 0, wxALL, 5 );
	
	m_bitmap_exp_sq = new wxStaticBitmap( m_panel2, wxID_ANY, wxBitmap( wxT("bitmaps/exp_sq.jpg"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	m_bitmap_exp_sq->SetMinSize( wxSize( 290,300 ) );
	
	bSizer4->Add( m_bitmap_exp_sq, 0, wxALL, 5 );
	
	
	m_panel2->SetSizer( bSizer4 );
	m_panel2->Layout();
	bSizer4->Fit( m_panel2 );
	m_notebook1->AddPage( m_panel2, _("SAR: Expanding Square"), false );
	m_panel3 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_panel8 = new wxPanel( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText15 = new wxStaticText( m_panel8, wxID_ANY, _("Sector Search"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	m_staticText15->SetFont( wxFont( 20, 70, 90, 92, false, wxEmptyString ) );
	
	bSizer91->Add( m_staticText15, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText32 = new wxStaticText( m_panel8, wxID_ANY, _("Approach course"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32->Wrap( -1 );
	bSizer14->Add( m_staticText32, 0, wxALL, 5 );
	
	m_Approach_SS = new wxTextCtrl( m_panel8, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Approach_SS->SetMaxLength( 0 ); 
	bSizer14->Add( m_Approach_SS, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText33 = new wxStaticText( m_panel8, wxID_ANY, _("Degrees"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33->Wrap( -1 );
	bSizer14->Add( m_staticText33, 0, wxALL, 5 );
	
	
	bSizer13->Add( bSizer14, 0, 0, 5 );
	
	wxBoxSizer* bSizer141;
	bSizer141 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText321 = new wxStaticText( m_panel8, wxID_ANY, _("Leg Distance          "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText321->Wrap( -1 );
	bSizer141->Add( m_staticText321, 0, wxALL, 5 );
	
	m_dx_SS = new wxTextCtrl( m_panel8, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_dx_SS->SetMaxLength( 0 ); 
	bSizer141->Add( m_dx_SS, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText331 = new wxStaticText( m_panel8, wxID_ANY, _("NM"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText331->Wrap( -1 );
	bSizer141->Add( m_staticText331, 0, wxALL, 5 );
	
	
	bSizer13->Add( bSizer141, 0, 0, 5 );
	
	wxBoxSizer* bSizer141111122;
	bSizer141111122 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText321111122 = new wxStaticText( m_panel8, wxID_ANY, _("Search Speed        "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText321111122->Wrap( -1 );
	bSizer141111122->Add( m_staticText321111122, 0, wxALL, 5 );
	
	m_Speed_SS = new wxTextCtrl( m_panel8, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Speed_SS->SetMaxLength( 0 ); 
	bSizer141111122->Add( m_Speed_SS, 0, wxALL, 5 );
	
	m_staticText331111112 = new wxStaticText( m_panel8, wxID_ANY, _("kts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText331111112->Wrap( -1 );
	bSizer141111122->Add( m_staticText331111112, 0, wxALL, 5 );
	
	
	bSizer13->Add( bSizer141111122, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer141111111;
	bSizer141111111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText321111111 = new wxStaticText( m_panel8, wxID_ANY, _("Number of cycles"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText321111111->Wrap( -1 );
	bSizer141111111->Add( m_staticText321111111, 0, wxALL, 5 );
	
	wxString m_NcyclesChoices[] = { _("1"), _("2") };
	int m_NcyclesNChoices = sizeof( m_NcyclesChoices ) / sizeof( wxString );
	m_Ncycles = new wxChoice( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_NcyclesNChoices, m_NcyclesChoices, 0 );
	m_Ncycles->SetSelection( 0 );
	bSizer141111111->Add( m_Ncycles, 0, wxALL, 5 );
	
	
	bSizer13->Add( bSizer141111111, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7111;
	bSizer7111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button31121 = new wxButton( m_panel8, wxID_ANY, _("Calculate"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7111->Add( m_button31121, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_button311111 = new wxButton( m_panel8, wxID_ANY, _("Generate &GPX"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7111->Add( m_button311111, 0, wxALL, 5 );
	
	
	bSizer13->Add( bSizer7111, 1, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	bSizer91->Add( bSizer13, 0, 0, 5 );
	
	
	m_panel8->SetSizer( bSizer91 );
	m_panel8->Layout();
	bSizer91->Fit( m_panel8 );
	bSizer5->Add( m_panel8, 0, wxALL, 5 );
	
	m_bitmap_sector = new wxStaticBitmap( m_panel3, wxID_ANY, wxBitmap( wxT("bitmaps/sector.jpg"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	m_bitmap_sector->SetMinSize( wxSize( 280,300 ) );
	
	bSizer5->Add( m_bitmap_sector, 0, wxALL, 5 );
	
	
	m_panel3->SetSizer( bSizer5 );
	m_panel3->Layout();
	bSizer5->Fit( m_panel3 );
	m_notebook1->AddPage( m_panel3, _("SAR: Sector Search"), true );
	m_panek4 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panek4->SetMinSize( wxSize( 300,200 ) );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_panel10 = new wxPanel( m_panek4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer912;
	bSizer912 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText152 = new wxStaticText( m_panel10, wxID_ANY, _("Oil Rig Search"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText152->Wrap( -1 );
	m_staticText152->SetFont( wxFont( 20, 70, 90, 92, false, wxEmptyString ) );
	
	bSizer912->Add( m_staticText152, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer132;
	bSizer132 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer143;
	bSizer143 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText323 = new wxStaticText( m_panel10, wxID_ANY, _("Approach course"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText323->Wrap( -1 );
	bSizer143->Add( m_staticText323, 0, wxALL, 5 );
	
	m_Approach_OR = new wxTextCtrl( m_panel10, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Approach_OR->SetMaxLength( 0 ); 
	bSizer143->Add( m_Approach_OR, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText333 = new wxStaticText( m_panel10, wxID_ANY, _("Degrees"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText333->Wrap( -1 );
	bSizer143->Add( m_staticText333, 0, wxALL, 5 );
	
	
	bSizer132->Add( bSizer143, 1, wxEXPAND|wxALIGN_RIGHT, 5 );
	
	wxBoxSizer* bSizer1412;
	bSizer1412 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3212 = new wxStaticText( m_panel10, wxID_ANY, _("Leg Distance          "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3212->Wrap( -1 );
	bSizer1412->Add( m_staticText3212, 0, wxALL, 5 );
	
	m_dx_OR = new wxTextCtrl( m_panel10, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_dx_OR->SetMaxLength( 0 ); 
	bSizer1412->Add( m_dx_OR, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText3312 = new wxStaticText( m_panel10, wxID_ANY, _("NM"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3312->Wrap( -1 );
	bSizer1412->Add( m_staticText3312, 0, wxALL, 5 );
	
	
	bSizer132->Add( bSizer1412, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer14121;
	bSizer14121 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText32121 = new wxStaticText( m_panel10, wxID_ANY, _("Number of Legs   "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32121->Wrap( -1 );
	bSizer14121->Add( m_staticText32121, 0, wxALL, 5 );
	
	m_NLegs_OR = new wxTextCtrl( m_panel10, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_NLegs_OR->SetMaxLength( 0 ); 
	bSizer14121->Add( m_NLegs_OR, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText33121 = new wxStaticText( m_panel10, wxID_ANY, _("Degrees"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33121->Wrap( -1 );
	bSizer14121->Add( m_staticText33121, 0, wxALL, 5 );
	
	
	bSizer132->Add( bSizer14121, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer141111123;
	bSizer141111123 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText321111123 = new wxStaticText( m_panel10, wxID_ANY, _("Search Speed         "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText321111123->Wrap( -1 );
	bSizer141111123->Add( m_staticText321111123, 0, wxALL, 5 );
	
	m_Speed_OR = new wxTextCtrl( m_panel10, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Speed_OR->SetMaxLength( 0 ); 
	bSizer141111123->Add( m_Speed_OR, 0, wxALL, 5 );
	
	m_staticText331111113 = new wxStaticText( m_panel10, wxID_ANY, _("kts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText331111113->Wrap( -1 );
	bSizer141111123->Add( m_staticText331111113, 0, wxALL, 5 );
	
	
	bSizer132->Add( bSizer141111123, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7112;
	bSizer7112 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button31122 = new wxButton( m_panel10, wxID_ANY, _("Calculate"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7112->Add( m_button31122, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_button311112 = new wxButton( m_panel10, wxID_ANY, _("Generate &GPX"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7112->Add( m_button311112, 0, wxALL, 5 );
	
	
	bSizer132->Add( bSizer7112, 1, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	bSizer912->Add( bSizer132, 1, wxEXPAND, 5 );
	
	
	m_panel10->SetSizer( bSizer912 );
	m_panel10->Layout();
	bSizer912->Fit( m_panel10 );
	bSizer6->Add( m_panel10, 0, wxALL, 5 );
	
	m_bitmap_oil_rig = new wxStaticBitmap( m_panek4, wxID_ANY, wxBitmap( wxT("bitmaps/oil_rig.jpg"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	m_bitmap_oil_rig->SetMinSize( wxSize( 300,300 ) );
	
	bSizer6->Add( m_bitmap_oil_rig, 0, wxALL, 5 );
	
	
	m_panek4->SetSizer( bSizer6 );
	m_panek4->Layout();
	bSizer6->Fit( m_panek4 );
	m_notebook1->AddPage( m_panek4, _("SAR: Oil Rig"), false );
	
	bSframe->Add( m_notebook1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxHORIZONTAL );
	
	m_wxNotebook234 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel11 = new wxPanel( m_wxNotebook234, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_panel11, wxID_ANY, _("Datum (Start of Search)") ), wxVERTICAL );
	
	wxBoxSizer* bSizer60;
	bSizer60 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer1321;
	bSizer1321 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer1431;
	bSizer1431 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3231 = new wxStaticText( m_panel11, wxID_ANY, _("Lattitude  "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3231->Wrap( -1 );
	bSizer1431->Add( m_staticText3231, 0, wxALL, 5 );
	
	m_Lat1 = new wxTextCtrl( m_panel11, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Lat1->SetMaxLength( 0 ); 
	bSizer1431->Add( m_Lat1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText3331 = new wxStaticText( m_panel11, wxID_ANY, _("\u00b0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3331->Wrap( -1 );
	bSizer1431->Add( m_staticText3331, 0, wxALL, 5 );
	
	
	bSizer1321->Add( bSizer1431, 0, 0, 5 );
	
	wxBoxSizer* bSizer14311;
	bSizer14311 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText32311 = new wxStaticText( m_panel11, wxID_ANY, _("Longitude"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32311->Wrap( -1 );
	bSizer14311->Add( m_staticText32311, 0, wxALL, 5 );
	
	m_Lon1 = new wxTextCtrl( m_panel11, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Lon1->SetMaxLength( 0 ); 
	bSizer14311->Add( m_Lon1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText33311 = new wxStaticText( m_panel11, wxID_ANY, _("\u00b0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33311->Wrap( -1 );
	bSizer14311->Add( m_staticText33311, 0, wxALL, 5 );
	
	
	bSizer1321->Add( bSizer14311, 0, 0, 5 );
	
	
	bSizer60->Add( bSizer1321, 0, 0, 5 );
	
	m_panel14 = new wxPanel( m_panel11, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxVERTICAL );
	
	m_button11 = new wxButton( m_panel14, wxID_ANY, _("Ship"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button11->SetToolTip( _("Copy ship position (GPS fix required)") );
	
	bSizer61->Add( m_button11, 0, wxALL, 5 );
	
	m_Cursor = new wxButton( m_panel14, wxID_ANY, _("Cursor"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer61->Add( m_Cursor, 0, wxALL, 5 );
	
	
	m_panel14->SetSizer( bSizer61 );
	m_panel14->Layout();
	bSizer61->Fit( m_panel14 );
	bSizer60->Add( m_panel14, 1, wxEXPAND | wxALL, 5 );
	
	
	sbSizer7->Add( bSizer60, 1, wxEXPAND, 5 );
	
	
	bSizer40->Add( sbSizer7, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer712;
	sbSizer712 = new wxStaticBoxSizer( new wxStaticBox( m_panel11, wxID_ANY, _("Search Pattern name for GPX file") ), wxVERTICAL );
	
	wxBoxSizer* bSizer132112;
	bSizer132112 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer143122;
	bSizer143122 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText323122 = new wxStaticText( m_panel11, wxID_ANY, _("Route"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText323122->Wrap( -1 );
	bSizer143122->Add( m_staticText323122, 0, wxALL, 5 );
	
	m_Route = new wxTextCtrl( m_panel11, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Route->SetMaxLength( 0 ); 
	bSizer143122->Add( m_Route, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	
	bSizer132112->Add( bSizer143122, 0, wxEXPAND, 5 );
	
	
	sbSizer712->Add( bSizer132112, 0, wxEXPAND, 5 );
	
	
	bSizer40->Add( sbSizer712, 0, wxEXPAND, 5 );
	
	
	m_panel11->SetSizer( bSizer40 );
	m_panel11->Layout();
	bSizer40->Fit( m_panel11 );
	m_wxNotebook234->AddPage( m_panel11, _("Decimal Degree"), true );
	m_panel15 = new wxPanel( m_wxNotebook234, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer401;
	bSizer401 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer72;
	sbSizer72 = new wxStaticBoxSizer( new wxStaticBox( m_panel15, wxID_ANY, _("Datum (Start of Search)") ), wxVERTICAL );
	
	wxBoxSizer* bSizer13212;
	bSizer13212 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer14313;
	bSizer14313 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText32313 = new wxStaticText( m_panel15, wxID_ANY, _("Lat "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32313->Wrap( -1 );
	bSizer14313->Add( m_staticText32313, 0, wxALL, 5 );
	
	m_Lat1_d = new wxTextCtrl( m_panel15, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lat1_d->SetMaxLength( 0 ); 
	bSizer14313->Add( m_Lat1_d, 0, wxALL, 5 );
	
	m_staticText33313 = new wxStaticText( m_panel15, wxID_ANY, _("\u00b0"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticText33313->Wrap( -1 );
	bSizer14313->Add( m_staticText33313, 0, wxALL, 5 );
	
	m_Lat1_m = new wxTextCtrl( m_panel15, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lat1_m->SetMaxLength( 0 ); 
	bSizer14313->Add( m_Lat1_m, 0, wxALL, 5 );
	
	m_staticText1143 = new wxStaticText( m_panel15, wxID_ANY, _("'"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1143->Wrap( -1 );
	bSizer14313->Add( m_staticText1143, 0, wxALL, 5 );
	
	m_Lat1_s = new wxTextCtrl( m_panel15, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lat1_s->SetMaxLength( 0 ); 
	bSizer14313->Add( m_Lat1_s, 0, wxALL, 5 );
	
	m_staticText1153 = new wxStaticText( m_panel15, wxID_ANY, _("\""), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1153->Wrap( -1 );
	bSizer14313->Add( m_staticText1153, 0, wxALL, 5 );
	
	wxString m_Lat1_NSChoices[] = { _("N"), _("S") };
	int m_Lat1_NSNChoices = sizeof( m_Lat1_NSChoices ) / sizeof( wxString );
	m_Lat1_NS = new wxChoice( m_panel15, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Lat1_NSNChoices, m_Lat1_NSChoices, 0 );
	m_Lat1_NS->SetSelection( 0 );
	bSizer14313->Add( m_Lat1_NS, 0, wxALL, 5 );
	
	
	bSizer13212->Add( bSizer14313, 0, 0, 5 );
	
	wxBoxSizer* bSizer143112;
	bSizer143112 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText323112 = new wxStaticText( m_panel15, wxID_ANY, _("Lon"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText323112->Wrap( -1 );
	bSizer143112->Add( m_staticText323112, 0, wxALL, 5 );
	
	m_Lon1_d = new wxTextCtrl( m_panel15, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lon1_d->SetMaxLength( 0 ); 
	bSizer143112->Add( m_Lon1_d, 0, wxALL, 5 );
	
	m_staticText333112 = new wxStaticText( m_panel15, wxID_ANY, _("\u00b0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText333112->Wrap( -1 );
	bSizer143112->Add( m_staticText333112, 0, wxALL, 5 );
	
	m_Lon1_m = new wxTextCtrl( m_panel15, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lon1_m->SetMaxLength( 0 ); 
	bSizer143112->Add( m_Lon1_m, 0, wxALL, 5 );
	
	m_staticText11412 = new wxStaticText( m_panel15, wxID_ANY, _("'"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11412->Wrap( -1 );
	bSizer143112->Add( m_staticText11412, 0, wxALL, 5 );
	
	m_Lon1_s = new wxTextCtrl( m_panel15, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lon1_s->SetMaxLength( 0 ); 
	bSizer143112->Add( m_Lon1_s, 0, wxALL, 5 );
	
	m_staticText11512 = new wxStaticText( m_panel15, wxID_ANY, _("\""), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11512->Wrap( -1 );
	bSizer143112->Add( m_staticText11512, 0, wxALL, 5 );
	
	wxString m_Lon1_EWChoices[] = { _("E"), _("W") };
	int m_Lon1_EWNChoices = sizeof( m_Lon1_EWChoices ) / sizeof( wxString );
	m_Lon1_EW = new wxChoice( m_panel15, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Lon1_EWNChoices, m_Lon1_EWChoices, 0 );
	m_Lon1_EW->SetSelection( 0 );
	bSizer143112->Add( m_Lon1_EW, 0, wxALL, 5 );
	
	
	bSizer13212->Add( bSizer143112, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	sbSizer72->Add( bSizer13212, 0, 0, 5 );
	
	
	bSizer401->Add( sbSizer72, 0, 0, 5 );
	
	wxBoxSizer* bSizer481;
	bSizer481 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button81 = new wxButton( m_panel15, wxID_ANY, _("Convert to Degree"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer481->Add( m_button81, 0, wxALL, 5 );
	
	
	bSizer401->Add( bSizer481, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	m_panel15->SetSizer( bSizer401 );
	m_panel15->Layout();
	bSizer401->Fit( m_panel15 );
	m_wxNotebook234->AddPage( m_panel15, _("\u00b0 ' \""), false );
	
	bSizer35->Add( m_wxNotebook234, 1, wxEXPAND | wxALL, 5 );
	
	m_panel111 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );
	
	m_panel61 = new wxPanel( m_panel111, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel61->SetMinSize( wxSize( 300,335 ) );
	
	wxBoxSizer* bSizer91111;
	bSizer91111 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText15111 = new wxStaticText( m_panel61, wxID_ANY, _("Results"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15111->Wrap( -1 );
	m_staticText15111->SetFont( wxFont( 20, 70, 90, 92, false, wxEmptyString ) );
	
	bSizer91111->Add( m_staticText15111, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer13111;
	bSizer13111 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer14211;
	bSizer14211 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText32211 = new wxStaticText( m_panel61, wxID_ANY, _("Search Distance"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32211->Wrap( -1 );
	bSizer14211->Add( m_staticText32211, 0, wxALL, 5 );
	
	m_Distance = new wxTextCtrl( m_panel61, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Distance->SetMaxLength( 0 ); 
	bSizer14211->Add( m_Distance, 0, wxALL, 5 );
	
	m_staticText33211 = new wxStaticText( m_panel61, wxID_ANY, _("NM"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33211->Wrap( -1 );
	bSizer14211->Add( m_staticText33211, 0, wxALL, 5 );
	
	
	bSizer13111->Add( bSizer14211, 0, 0, 5 );
	
	wxBoxSizer* bSizer141121;
	bSizer141121 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Statictext34 = new wxStaticText( m_panel61, wxID_ANY, _("Search Time        "), wxDefaultPosition, wxDefaultSize, 0 );
	m_Statictext34->Wrap( -1 );
	bSizer141121->Add( m_Statictext34, 0, wxALL, 5 );
	
	m_Time = new wxTextCtrl( m_panel61, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Time->SetMaxLength( 0 ); 
	bSizer141121->Add( m_Time, 0, wxALL, 5 );
	
	m_staticText331121 = new wxStaticText( m_panel61, wxID_ANY, _("hr"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText331121->Wrap( -1 );
	bSizer141121->Add( m_staticText331121, 0, wxALL, 5 );
	
	
	bSizer13111->Add( bSizer141121, 0, 0, 5 );
	
	
	bSizer91111->Add( bSizer13111, 0, 0, 5 );
	
	
	m_panel61->SetSizer( bSizer91111 );
	m_panel61->Layout();
	bSizer91111->Fit( m_panel61 );
	bSizer31->Add( m_panel61, 0, wxALL, 5 );
	
	
	m_panel111->SetSizer( bSizer31 );
	m_panel111->Layout();
	bSizer31->Fit( m_panel111 );
	bSizer35->Add( m_panel111, 1, wxEXPAND | wxALL, 5 );
	
	
	bSframe->Add( bSizer35, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSframe );
	this->Layout();
	bSframe->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_button311->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnPSCalc ), NULL, this );
	m_button3111->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnPSGPX ), NULL, this );
	m_button3112->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnESCalc ), NULL, this );
	m_button31111->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnESGPX ), NULL, this );
	m_button31121->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnSSCalc ), NULL, this );
	m_button311111->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnSSGPX ), NULL, this );
	m_button31122->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnORCalc ), NULL, this );
	m_button311112->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnORGPX ), NULL, this );
	m_wxNotebook234->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( DlgDef::key_shortcut ), NULL, this );
	m_wxNotebook234->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( DlgDef::OnNoteBookFit ), NULL, this );
	m_panel11->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( DlgDef::key_shortcut ), NULL, this );
	m_Lat1->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( DlgDef::key_shortcut ), NULL, this );
	m_Lon1->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( DlgDef::key_shortcut ), NULL, this );
	m_button11->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnShip ), NULL, this );
	m_Cursor->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnCursorSelect ), NULL, this );
	m_button81->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnConverttoDegree ), NULL, this );
}

DlgDef::~DlgDef()
{
	// Disconnect Events
	m_button311->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnPSCalc ), NULL, this );
	m_button3111->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnPSGPX ), NULL, this );
	m_button3112->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnESCalc ), NULL, this );
	m_button31111->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnESGPX ), NULL, this );
	m_button31121->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnSSCalc ), NULL, this );
	m_button311111->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnSSGPX ), NULL, this );
	m_button31122->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnORCalc ), NULL, this );
	m_button311112->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnORGPX ), NULL, this );
	m_wxNotebook234->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( DlgDef::key_shortcut ), NULL, this );
	m_wxNotebook234->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( DlgDef::OnNoteBookFit ), NULL, this );
	m_panel11->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( DlgDef::key_shortcut ), NULL, this );
	m_Lat1->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( DlgDef::key_shortcut ), NULL, this );
	m_Lon1->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( DlgDef::key_shortcut ), NULL, this );
	m_button11->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnShip ), NULL, this );
	m_Cursor->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnCursorSelect ), NULL, this );
	m_button81->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnConverttoDegree ), NULL, this );
	
}

CfgDlgDef::CfgDlgDef( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("General settings") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, _("Opacity"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	fgSizer3->Add( m_staticText17, 0, wxALL, 5 );
	
	m_sOpacity = new wxSlider( this, wxID_ANY, 50, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	fgSizer3->Add( m_sOpacity, 0, wxALL|wxEXPAND, 5 );
	
	
	sbSizer4->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	
	bSizer2->Add( sbSizer4, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("CPU saving settings") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_CaptureShip = new wxCheckBox( this, wxID_ANY, _("Capture Ship Position"), wxDefaultPosition, wxDefaultSize, 0 );
	m_CaptureShip->SetValue(true); 
	m_CaptureShip->SetToolTip( _("Leave ticked, so you can use ship's position") );
	
	bSizer3->Add( m_CaptureShip, 0, wxALL, 5 );
	
	m_CaptureCursor = new wxCheckBox( this, wxID_ANY, _("Capture Cursor Position"), wxDefaultPosition, wxDefaultSize, 0 );
	m_CaptureCursor->SetValue(true); 
	m_CaptureCursor->SetToolTip( _("Leave ticked, so that you can use cursor position.") );
	
	bSizer3->Add( m_CaptureCursor, 0, wxALL, 5 );
	
	
	fgSizer4->Add( bSizer3, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	
	fgSizer4->Add( bSizer4, 1, wxEXPAND, 5 );
	
	
	sbSizer3->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	
	bSizer2->Add( sbSizer3, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	
	bSizer1->Add( m_sdbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
}

CfgDlgDef::~CfgDlgDef()
{
}

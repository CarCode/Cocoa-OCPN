///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "WeatherFaxUI.h"

///////////////////////////////////////////////////////////////////////////

WeatherFaxBase::WeatherFaxBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 3, 1, 0, 0 );  // war 2,1,0,0
	fgSizer8->AddGrowableCol( 0 );
	fgSizer8->AddGrowableRow( 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	sbFax = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Fax") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer7->AddGrowableCol( 0 );
	fgSizer7->AddGrowableRow( 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxArrayString m_lFaxesChoices;
	m_lFaxes = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_lFaxesChoices, 0 );
	fgSizer7->Add( m_lFaxes, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer16;
	fgSizer16 = new wxFlexGridSizer( 2, 0, 0, 0 );
	fgSizer16->SetFlexibleDirection( wxBOTH );
	fgSizer16->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	fgSizer7->Add( fgSizer16, 1, wxEXPAND, 5 );
	
	sbFax->Add( fgSizer7, 1, wxEXPAND, 5 );
	
	
	fgSizer8->Add( sbFax, 1, wxEXPAND, 0 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Transparency") ), wxVERTICAL );
	
	m_sTransparency = new wxSlider( this, wxID_ANY, 50, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	sbSizer4->Add( m_sTransparency, 0, wxEXPAND, 5 );
	
	m_sWhiteTransparency = new wxSlider( this, wxID_ANY, 150, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	sbSizer4->Add( m_sWhiteTransparency, 0, wxEXPAND, 5 );
	

    fgSizer8->Add( sbSizer4, 1, wxALL|wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer24;
    fgSizer24 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer24->SetFlexibleDirection( wxBOTH );
    fgSizer24->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_cInvert = new wxCheckBox( this, wxID_ANY, _("Invert"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer24->Add( m_cInvert, 0, wxALL, 5 );
 	
    m_cbDisplaySelected = new wxCheckBox( this, wxID_ANY, _("Display Selected"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cbDisplaySelected->SetValue(true);
    fgSizer24->Add( m_cbDisplaySelected, 0, wxALL, 5 );
	
	
	fgSizer8->Add( fgSizer24, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( fgSizer8 );
	this->Layout();
	m_menubar1 = new wxMenuBar( 0 );
    m_menu1 = new wxMenu();
    wxMenuItem* m_menuItem1;
    m_menuItem1 = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Open") ) + wxT('\t') + wxT("Ctrl+o"), wxEmptyString, wxITEM_NORMAL );
    m_menu1->Append( m_menuItem1 );

    m_mEdit = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Edit") ) + wxT('\t') + wxT("Ctrl+e"), wxEmptyString, wxITEM_NORMAL );
    m_menu1->Append( m_mEdit );

    m_mExport = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Export") ) + wxT('\t') + wxT("Ctrl+x"), wxEmptyString, wxITEM_NORMAL );
    m_menu1->Append( m_mExport );

    m_mDelete = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Delete") ) + wxT('\t') + wxT("Ctrl+d"), wxEmptyString, wxITEM_NORMAL );
    m_menu1->Append( m_mDelete );

    m_menu1->AppendSeparator();

    wxMenuItem* m_menuItem9;
    m_menuItem9 = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Preferences") ) + wxT('\t') + wxT("Ctrl+p"), wxEmptyString, wxITEM_NORMAL );
    m_menu1->Append( m_menuItem9 );

    m_menu1->AppendSeparator();

    wxMenuItem* m_menuItem4;
    m_menuItem4 = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Close") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu1->Append( m_menuItem4 );

    m_menubar1->Append( m_menu1, _("File") );

    m_menu2 = new wxMenu();
    wxMenuItem* m_menuItem8;
    m_menuItem8 = new wxMenuItem( m_menu2, wxID_ANY, wxString( _("Audio Capture") ) + wxT('\t') + wxT("Ctrl+a"), wxEmptyString, wxITEM_NORMAL );
    m_menu2->Append( m_menuItem8 );

    wxMenuItem* m_menuItem5;
    m_menuItem5 = new wxMenuItem( m_menu2, wxID_ANY, wxString( _("HF Radio Schedules") ) + wxT('\t') + wxT("ctrl+h"), wxEmptyString, wxITEM_NORMAL );
    m_menu2->Append( m_menuItem5 );

    wxMenuItem* m_menuItem6;
    m_menuItem6 = new wxMenuItem( m_menu2, wxID_ANY, wxString( _("Internet") ) + wxT('\t') + wxT("ctrl+i"), wxEmptyString, wxITEM_NORMAL );
    m_menu2->Append( m_menuItem6 );

    m_menubar1->Append( m_menu2, _("Retrieve") );

    m_menu3 = new wxMenu();
    wxMenuItem* m_menuItem7;
    m_menuItem7 = new wxMenuItem( m_menu3, wxID_ANY, wxString( _("About") ) + wxT('\t') + wxT("F1"), wxEmptyString, wxITEM_NORMAL );
    m_menu3->Append( m_menuItem7 );

    m_menubar1->Append( m_menu3, _("Help") );

    this->SetMenuBar( m_menubar1 );

	
	this->Centre( wxBOTH );
	
	// Connect Events
    this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( WeatherFaxBase::OnClose ) );
	m_lFaxes->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnFaxes ), NULL, this );
    m_lFaxes->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( WeatherFaxBase::OnFaxesToggled ), NULL, this );
    m_sTransparency->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_cInvert->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( WeatherFaxBase::OnInvert ), NULL, this );
    m_cbDisplaySelected->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( WeatherFaxBase::OnFaxesToggled ), NULL, this );
    this->Connect( m_menuItem1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnOpen ) );
    this->Connect( m_mEdit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnEdit ) );
    this->Connect( m_mExport->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnExport ) );
    this->Connect( m_mDelete->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnDelete ) );
    this->Connect( m_menuItem9->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnPreferences ) );
    this->Connect( m_menuItem4->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnClose ) );
    this->Connect( m_menuItem8->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnCapture ) );
    this->Connect( m_menuItem5->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnSchedules ) );
    this->Connect( m_menuItem6->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnInternet ) );
    this->Connect( m_menuItem7->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnAbout ) );
}

WeatherFaxBase::~WeatherFaxBase()
{
	// Disconnect Events
    this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( WeatherFaxBase::OnClose ) );
	m_lFaxes->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnFaxes ), NULL, this );
    m_lFaxes->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( WeatherFaxBase::OnFaxesToggled ), NULL, this );
    m_sTransparency->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sTransparency->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( WeatherFaxBase::TransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_sWhiteTransparency->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( WeatherFaxBase::WhiteTransparencyChanged ), NULL, this );
    m_cInvert->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( WeatherFaxBase::OnInvert ), NULL, this );
    m_cbDisplaySelected->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( WeatherFaxBase::OnFaxesToggled ), NULL, this );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnOpen ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnEdit ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnExport ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnDelete ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnPreferences ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnClose ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnCapture ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnSchedules ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnInternet ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WeatherFaxBase::OnAbout ) );
	
}

SchedulesDialogBase::SchedulesDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxFlexGridSizer* fgSizer25;
    fgSizer25 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer25->AddGrowableCol( 0 );
    fgSizer25->AddGrowableRow( 0 );
    fgSizer25->SetFlexibleDirection( wxBOTH );
    fgSizer25->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_lSchedules = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxLC_REPORT );
    m_lSchedules->SetMinSize( wxSize( -1,160 ) );

    fgSizer25->Add( m_lSchedules, 0, wxALL|wxEXPAND, 5 );

    m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
    m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer* fgSizer26;
    fgSizer26 = new wxFlexGridSizer( 1, 0, 0, 0 );
    fgSizer26->AddGrowableCol( 1 );
    fgSizer26->SetFlexibleDirection( wxBOTH );
    fgSizer26->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxStaticBoxSizer* sbSizer10;
    sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, _("Contains") ), wxVERTICAL );

    wxFlexGridSizer* fgSizer27;
    fgSizer27 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer27->SetFlexibleDirection( wxBOTH );
    fgSizer27->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText24 = new wxStaticText( m_panel1, wxID_ANY, _("Lat"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText24->Wrap( -1 );
    fgSizer27->Add( m_staticText24, 0, wxALL, 5 );

    m_tContainsLat = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer27->Add( m_tContainsLat, 0, wxALL, 5 );

    m_staticText25 = new wxStaticText( m_panel1, wxID_ANY, _("Lon"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText25->Wrap( -1 );
    fgSizer27->Add( m_staticText25, 0, wxALL, 5 );

    m_tContainsLon = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer27->Add( m_tContainsLon, 0, wxALL, 5 );

    m_bBoatPosition = new wxButton( m_panel1, wxID_ANY, _("Boat Position"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer27->Add( m_bBoatPosition, 0, wxALL, 5 );

    m_bReset = new wxButton( m_panel1, wxID_ANY, _("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer27->Add( m_bReset, 0, wxALL, 5 );


    sbSizer10->Add( fgSizer27, 1, wxEXPAND, 5 );


    fgSizer26->Add( sbSizer10, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer12;
    sbSizer12 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, _("Stations") ), wxVERTICAL );

    wxFlexGridSizer* fgSizer28;
    fgSizer28 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer28->AddGrowableCol( 0 );
    fgSizer28->SetFlexibleDirection( wxBOTH );
    fgSizer28->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_lStations = new wxListBox( m_panel1, wxID_ANY, wxDefaultPosition, wxSize( 150,100 ), 0, NULL, wxLB_MULTIPLE );
    fgSizer28->Add( m_lStations, 0, wxALL|wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer29;
    fgSizer29 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer29->SetFlexibleDirection( wxBOTH );
    fgSizer29->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_bAll = new wxButton( m_panel1, wxID_ANY, _("All"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer29->Add( m_bAll, 0, wxALL, 5 );

    m_bNone = new wxButton( m_panel1, wxID_ANY, _("None"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer29->Add( m_bNone, 0, wxALL, 5 );


    fgSizer28->Add( fgSizer29, 1, wxEXPAND, 5 );


    sbSizer12->Add( fgSizer28, 1, wxEXPAND, 5 );


    fgSizer26->Add( sbSizer12, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer14;
    sbSizer14 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, _("Frequency") ), wxVERTICAL );

    wxFlexGridSizer* fgSizer33;
    fgSizer33 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer33->SetFlexibleDirection( wxBOTH );
    fgSizer33->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_skhzmin = new wxSpinCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 30000, 0 );
    fgSizer33->Add( m_skhzmin, 0, wxALL, 5 );

    m_staticText27 = new wxStaticText( m_panel1, wxID_ANY, _("khz min"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText27->Wrap( -1 );
    fgSizer33->Add( m_staticText27, 0, wxALL, 5 );

    m_skhzmax = new wxSpinCtrl( m_panel1, wxID_ANY, wxT("30000"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 30000, 0 );
    fgSizer33->Add( m_skhzmax, 0, wxALL, 5 );

    m_staticText28 = new wxStaticText( m_panel1, wxID_ANY, _("khz max"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText28->Wrap( -1 );
    fgSizer33->Add( m_staticText28, 0, wxALL|wxEXPAND, 5 );

    m_bAllFrequencies = new wxButton( m_panel1, wxID_ANY, _("All"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer33->Add( m_bAllFrequencies, 0, wxALL, 5 );


    sbSizer14->Add( fgSizer33, 1, wxEXPAND, 5 );


    fgSizer26->Add( sbSizer14, 1, wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer46;
    fgSizer46 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer46->SetFlexibleDirection( wxBOTH );
    fgSizer46->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxStaticBoxSizer* sbSizer13;
    sbSizer13 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, _("Constraints") ), wxVERTICAL );

    wxFlexGridSizer* fgSizer35;
    fgSizer35 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer35->SetFlexibleDirection( wxBOTH );
    fgSizer35->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_cbHasArea = new wxCheckBox( m_panel1, wxID_ANY, _("Has Area"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer35->Add( m_cbHasArea, 0, wxALL, 5 );

    m_cbHasValidTime = new wxCheckBox( m_panel1, wxID_ANY, _("Has Valid Time"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer35->Add( m_cbHasValidTime, 0, wxALL, 5 );


    sbSizer13->Add( fgSizer35, 1, wxEXPAND, 5 );


    fgSizer46->Add( sbSizer13, 1, wxEXPAND, 5 );


    fgSizer26->Add( fgSizer46, 1, wxEXPAND, 5 );


    m_panel1->SetSizer( fgSizer26 );
    m_panel1->Layout();
    fgSizer26->Fit( m_panel1 );
    m_notebook1->AddPage( m_panel1, _("Filter"), true );
    m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer* fgSizer31;
    fgSizer31 = new wxFlexGridSizer( 1, 0, 0, 0 );
    fgSizer31->AddGrowableCol( 2 );
    fgSizer31->SetFlexibleDirection( wxBOTH );
    fgSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_cbMessageBox = new wxCheckBox( m_panel2, wxID_ANY, _("Message Box"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer31->Add( m_cbMessageBox, 0, wxALL, 5 );

    m_cbExternalAlarm = new wxCheckBox( m_panel2, wxID_ANY, _("External Alarm"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer31->Add( m_cbExternalAlarm, 0, wxALL, 5 );

    m_tExternalAlarmCommand = new wxTextCtrl( m_panel2, wxID_ANY, _("aplay /usr/local/share/opencpn/sounds/2bells.wav"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer31->Add( m_tExternalAlarmCommand, 0, wxALL|wxEXPAND, 5 );


    m_panel2->SetSizer( fgSizer31 );
    m_panel2->Layout();
    fgSizer31->Fit( m_panel2 );
    m_notebook1->AddPage( m_panel2, _("1 minute alarm"), false );
    m_panel3 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer* fgSizer30;
    fgSizer30 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer30->AddGrowableCol( 0 );
    fgSizer30->SetFlexibleDirection( wxBOTH );
    fgSizer30->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_rbNoAction = new wxRadioButton( m_panel3, wxID_ANY, _("No Action"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer30->Add( m_rbNoAction, 0, wxALL, 5 );

    wxFlexGridSizer* fgSizer32;
    fgSizer32 = new wxFlexGridSizer( 1, 0, 0, 0 );
    fgSizer32->AddGrowableCol( 1 );
    fgSizer32->SetFlexibleDirection( wxBOTH );
    fgSizer32->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_rbExternalCapture = new wxRadioButton( m_panel3, wxID_ANY, _("External Command"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer32->Add( m_rbExternalCapture, 0, wxALL, 5 );

    m_tExternalCapture = new wxTextCtrl( m_panel3, wxID_ANY, _("arecord -f S16_LE"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer32->Add( m_tExternalCapture, 0, wxALL|wxEXPAND, 5 );


    fgSizer30->Add( fgSizer32, 1, wxEXPAND, 5 );

    m_rbManualCapture = new wxRadioButton( m_panel3, wxID_ANY, _("Manual Capture (with external program) automatic Open File"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer30->Add( m_rbManualCapture, 0, wxALL, 5 );

    m_rbAlsaCapture = new wxRadioButton( m_panel3, wxID_ANY, _("Alsa Capture"), wxDefaultPosition, wxDefaultSize, 0 );
    m_rbAlsaCapture->Enable( false );

    fgSizer30->Add( m_rbAlsaCapture, 0, wxALL, 5 );

    m_panel3->SetSizer( fgSizer30 );
    m_panel3->Layout();
    fgSizer30->Fit( m_panel3 );
    m_notebook1->AddPage( m_panel3, _("Capture Options"), false );
    m_panel7 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer* fgSizer49;
    fgSizer49 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer49->SetFlexibleDirection( wxBOTH );
    fgSizer49->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText33 = new wxStaticText( m_panel7, wxID_ANY, _("Select fax images by clicking in the capture (first) column\n\nSchedules can be sorted by clicking the header column\n\nThe schedules need testing as I can only receive from a few of the possible stations.  Corrections can be made by modifying the WeatherFaxSchedules.xml file. Patches can be submitted via github.\n\nAutomatic control of a ssb radio is desireable as well, however the author only has a tecsun pl-600.  If a suitable radio can be donated, support will be implemented."), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText33->Wrap( 640 );
    fgSizer49->Add( m_staticText33, 0, wxALL, 5 );


    m_panel7->SetSizer( fgSizer49 );
    m_panel7->Layout();
    fgSizer49->Fit( m_panel7 );
    m_notebook1->AddPage( m_panel7, _("Information"), false );

    fgSizer25->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );

    wxStaticBoxSizer* sbSizer15;
    sbSizer15 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Capture Status") ), wxVERTICAL );

    wxFlexGridSizer* fgSizer34;
    fgSizer34 = new wxFlexGridSizer( 1, 0, 0, 0 );
    fgSizer34->AddGrowableCol( 1 );
    fgSizer34->SetFlexibleDirection( wxBOTH );
    fgSizer34->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_stCaptureStatus = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 350,-1 ), 0 );
    m_stCaptureStatus->Wrap( -1 );
    fgSizer34->Add( m_stCaptureStatus, 0, wxALL, 5 );

    m_gCaptureStatus = new wxGauge( this, wxID_ANY, 1000, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
    m_gCaptureStatus->SetValue( 0 );
    fgSizer34->Add( m_gCaptureStatus, 0, wxALL|wxEXPAND, 5 );

    m_bClose = new wxButton( this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer34->Add( m_bClose, 0, wxALL, 5 );


    sbSizer15->Add( fgSizer34, 1, wxEXPAND, 5 );


    fgSizer25->Add( sbSizer15, 1, wxEXPAND, 5 );


    this->SetSizer( fgSizer25 );
    this->Layout();
    fgSizer25->Fit( this );

    this->Centre( wxBOTH );

    // Connect Events
    m_lSchedules->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( SchedulesDialogBase::OnSchedulesLeftDown ), NULL, this );
    m_lSchedules->Connect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( SchedulesDialogBase::OnSchedulesSort ), NULL, this );
    m_tContainsLat->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_tContainsLon->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_bBoatPosition->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnBoatPosition ), NULL, this );
    m_bReset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnReset ), NULL, this );
    m_lStations->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_bAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnAllStations ), NULL, this );
    m_bNone->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnNoStations ), NULL, this );
    m_skhzmin->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_skhzmax->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_bAllFrequencies->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnAllFrequencies ), NULL, this );
    m_cbHasArea->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_cbHasValidTime->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_bClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnClose ), NULL, this );
}

SchedulesDialogBase::~SchedulesDialogBase()
{
    // Disconnect Events
    m_lSchedules->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( SchedulesDialogBase::OnSchedulesLeftDown ), NULL, this );
    m_lSchedules->Disconnect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( SchedulesDialogBase::OnSchedulesSort ), NULL, this );
    m_tContainsLat->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_tContainsLon->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_bBoatPosition->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnBoatPosition ), NULL, this );
    m_bReset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnReset ), NULL, this );
    m_lStations->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_bAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnAllStations ), NULL, this );
    m_bNone->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnNoStations ), NULL, this );
    m_skhzmin->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_skhzmax->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_bAllFrequencies->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnAllFrequencies ), NULL, this );
    m_cbHasArea->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_cbHasValidTime->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnFilter ), NULL, this );
    m_bClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SchedulesDialogBase::OnClose ), NULL, this );

}

InternetRetrievalDialogBase::InternetRetrievalDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxFlexGridSizer* fgSizer38;
    fgSizer38 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer38->SetFlexibleDirection( wxBOTH );
    fgSizer38->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
    m_panel4 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer* fgSizer39;
    fgSizer39 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer39->AddGrowableCol( 0 );
    fgSizer39->AddGrowableRow( 0 );
    fgSizer39->SetFlexibleDirection( wxBOTH );
    fgSizer39->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_lUrls = new wxListCtrl( m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
    fgSizer39->Add( m_lUrls, 0, wxALL|wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer42;
    fgSizer42 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer42->SetFlexibleDirection( wxBOTH );
    fgSizer42->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxStaticBoxSizer* sbSizer10;
    sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( m_panel4, wxID_ANY, _("Contains") ), wxVERTICAL );

    wxFlexGridSizer* fgSizer27;
    fgSizer27 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer27->SetFlexibleDirection( wxBOTH );
    fgSizer27->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText24 = new wxStaticText( m_panel4, wxID_ANY, _("Lat"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText24->Wrap( -1 );
    fgSizer27->Add( m_staticText24, 0, wxALL, 5 );

    m_tContainsLat = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer27->Add( m_tContainsLat, 0, wxALL, 5 );

    m_staticText25 = new wxStaticText( m_panel4, wxID_ANY, _("Lon"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText25->Wrap( -1 );
    fgSizer27->Add( m_staticText25, 0, wxALL, 5 );

    m_tContainsLon = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer27->Add( m_tContainsLon, 0, wxALL, 5 );

    m_bBoatPosition = new wxButton( m_panel4, wxID_ANY, _("Boat Position"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer27->Add( m_bBoatPosition, 0, wxALL, 5 );

    m_bReset = new wxButton( m_panel4, wxID_ANY, _("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer27->Add( m_bReset, 0, wxALL, 5 );


    sbSizer10->Add( fgSizer27, 1, wxEXPAND, 5 );


    fgSizer42->Add( sbSizer10, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer12;
    sbSizer12 = new wxStaticBoxSizer( new wxStaticBox( m_panel4, wxID_ANY, _("Servers") ), wxVERTICAL );

    wxFlexGridSizer* fgSizer28;
    fgSizer28 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer28->AddGrowableCol( 0 );
    fgSizer28->SetFlexibleDirection( wxBOTH );
    fgSizer28->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_lServers = new wxListBox( m_panel4, wxID_ANY, wxDefaultPosition, wxSize( 150,60 ), 0, NULL, wxLB_MULTIPLE );
    fgSizer28->Add( m_lServers, 0, wxALL|wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer29;
    fgSizer29 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer29->SetFlexibleDirection( wxBOTH );
    fgSizer29->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_bAll = new wxButton( m_panel4, wxID_ANY, _("All"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer29->Add( m_bAll, 0, wxALL, 5 );

    m_bNone = new wxButton( m_panel4, wxID_ANY, _("None"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer29->Add( m_bNone, 0, wxALL, 5 );


    fgSizer28->Add( fgSizer29, 1, wxEXPAND, 5 );


    sbSizer12->Add( fgSizer28, 1, wxEXPAND, 5 );


    fgSizer42->Add( sbSizer12, 1, wxEXPAND, 5 );


    fgSizer39->Add( fgSizer42, 1, wxEXPAND, 5 );


    m_panel4->SetSizer( fgSizer39 );
    m_panel4->Layout();
    fgSizer39->Fit( m_panel4 );
    m_notebook2->AddPage( m_panel4, _("Url"), true );
    m_panel5 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxFlexGridSizer* fgSizer48;
    fgSizer48 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer48->SetFlexibleDirection( wxBOTH );
    fgSizer48->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText34 = new wxStaticText( m_panel5, wxID_ANY, _("Not Implemented"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText34->Wrap( -1 );
    fgSizer48->Add( m_staticText34, 0, wxALL, 5 );


    m_panel5->SetSizer( fgSizer48 );
    m_panel5->Layout();
    fgSizer48->Fit( m_panel5 );
    m_notebook2->AddPage( m_panel5, _("Email"), false );

    fgSizer38->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );

    wxFlexGridSizer* fgSizer40;
    fgSizer40 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer40->SetFlexibleDirection( wxBOTH );
    fgSizer40->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_bRetrieve = new wxButton( this, wxID_ANY, _("Retrieve"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer40->Add( m_bRetrieve, 0, wxALL, 5 );

    m_bClose = new wxButton( this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer40->Add( m_bClose, 0, wxALL, 5 );


    fgSizer38->Add( fgSizer40, 1, wxEXPAND, 5 );


    this->SetSizer( fgSizer38 );
    this->Layout();
    fgSizer38->Fit( this );

    this->Centre( wxBOTH );

    // Connect Events
    m_lUrls->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( InternetRetrievalDialogBase::OnUrlsLeftDown ), NULL, this );
    m_lUrls->Connect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( InternetRetrievalDialogBase::OnUrlsSor ), NULL, this );
    m_tContainsLat->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( InternetRetrievalDialogBase::OnFilter ), NULL, this );
    m_tContainsLon->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( InternetRetrievalDialogBase::OnFilter ), NULL, this );
    m_bBoatPosition->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnBoatPosition ), NULL, this );
    m_bReset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnReset ), NULL, this );
    m_lServers->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( InternetRetrievalDialogBase::OnFilter ), NULL, this );
    m_bAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnAllServers ), NULL, this );
    m_bNone->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnNoServers ), NULL, this );
    m_bRetrieve->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnRetrieve ), NULL, this );
    m_bClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnClose ), NULL, this );
}

InternetRetrievalDialogBase::~InternetRetrievalDialogBase()
{
    // Disconnect Events
    m_lUrls->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( InternetRetrievalDialogBase::OnUrlsLeftDown ), NULL, this );
    m_lUrls->Disconnect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( InternetRetrievalDialogBase::OnUrlsSor ), NULL, this );
    m_tContainsLat->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( InternetRetrievalDialogBase::OnFilter ), NULL, this );
    m_tContainsLon->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( InternetRetrievalDialogBase::OnFilter ), NULL, this );
    m_bBoatPosition->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnBoatPosition ), NULL, this );
    m_bReset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnReset ), NULL, this );
    m_lServers->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( InternetRetrievalDialogBase::OnFilter ), NULL, this );
    m_bAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnAllServers ), NULL, this );
    m_bNone->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnNoServers ), NULL, this );
	m_bRetrieve->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnRetrieve ), NULL, this );
	m_bClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InternetRetrievalDialogBase::OnClose ), NULL, this );
	
}

WeatherFaxWizardBase::WeatherFaxWizardBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxBitmap& bitmap, const wxPoint& pos, long style )
{
	this->Create( parent, id, title, bitmap, pos, style );
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxWizardPageSimple* m_wizPage1 = new wxWizardPageSimple( this );
	m_pages.Add( m_wizPage1 );
	
	m_wizPage1->SetMinSize( wxSize( 300,-1 ) );
	
	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer11->AddGrowableCol( 0 );
	fgSizer11->AddGrowableRow( 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_swFaxArea1 = new wxScrolledWindow( m_wizPage1, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxHSCROLL|wxVSCROLL );
	m_swFaxArea1->SetScrollRate( 5, 5 );
	fgSizer11->Add( m_swFaxArea1, 1, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer172;
	fgSizer172 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer172->AddGrowableCol( 1 );
	fgSizer172->AddGrowableRow( 0 );
	fgSizer172->SetFlexibleDirection( wxBOTH );
	fgSizer172->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_wizPage1, wxID_ANY, _("Decoder") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer19;
	fgSizer19 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer19->AddGrowableCol( 1 );
	fgSizer19->AddGrowableRow( 0 );
	fgSizer19->SetFlexibleDirection( wxBOTH );
	fgSizer19->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer18;
	fgSizer18 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer18->AddGrowableCol( 0 );
	fgSizer18->AddGrowableRow( 0 );
	fgSizer18->SetFlexibleDirection( wxBOTH );
	fgSizer18->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_bStopDecoding = new wxButton( m_wizPage1, wxID_ANY, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer18->Add( m_bStopDecoding, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer20;
	fgSizer20 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer20->SetFlexibleDirection( wxBOTH );
	fgSizer20->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText212 = new wxStaticText( m_wizPage1, wxID_ANY, _("Sat"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText212->Wrap( -1 );
	fgSizer20->Add( m_staticText212, 0, wxALL, 5 );
	
	m_sMinusSaturationThreshold = new wxSpinCtrl( m_wizPage1, wxID_ANY, wxT("3"), wxDefaultPosition, wxSize( 40,-1 ), wxSP_ARROW_KEYS, 0, 30, 0 );
	fgSizer20->Add( m_sMinusSaturationThreshold, 0, wxALL, 5 );
	
	
	fgSizer18->Add( fgSizer20, 1, wxEXPAND, 5 );
	
	
	fgSizer19->Add( fgSizer18, 1, wxEXPAND, 5 );
	
	m_bPhasingArea = new wxScrolledWindow( m_wizPage1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_bPhasingArea->SetScrollRate( 5, 5 );
	m_bPhasingArea->SetMinSize( wxSize( 100,-1 ) );
	
	fgSizer19->Add( m_bPhasingArea, 1, wxEXPAND | wxALL, 5 );
	
	
	sbSizer7->Add( fgSizer19, 1, wxEXPAND, 5 );
	
	
	fgSizer172->Add( sbSizer7, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( m_wizPage1, wxID_ANY, _("Image Correction") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer10->AddGrowableCol( 3 );
	fgSizer10->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText16 = new wxStaticText( m_wizPage1, wxID_ANY, _("Filter"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer10->Add( m_staticText16, 0, wxALL, 5 );
	
	wxString m_cFilterChoices[] = { _("No Filter"), _("Removal 1"), _("Removal 2"), _("removal 3"), _("Value 1"), _("Value 2"), _("Value 3") };
	int m_cFilterNChoices = sizeof( m_cFilterChoices ) / sizeof( wxString );
	m_cFilter = new wxChoice( m_wizPage1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_cFilterNChoices, m_cFilterChoices, 0 );
	m_cFilter->SetSelection( 0 );
	fgSizer10->Add( m_cFilter, 0, wxALL, 2 );
	
	m_staticText9 = new wxStaticText( m_wizPage1, wxID_ANY, _("Phasing"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer10->Add( m_staticText9, 0, wxALL, 5 );
	
	m_sPhasing = new wxSlider( m_wizPage1, wxID_ANY, 0, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_INVERSE );
	fgSizer10->Add( m_sPhasing, 0, wxALL|wxEXPAND, 2 );
	
	m_staticText17 = new wxStaticText( m_wizPage1, wxID_ANY, _("Rotation"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	fgSizer10->Add( m_staticText17, 0, wxALL, 5 );
	
	wxString m_cRotationChoices[] = { _("None"), _("CCW"), _("CW"), _("180") };
	int m_cRotationNChoices = sizeof( m_cRotationChoices ) / sizeof( wxString );
	m_cRotation = new wxChoice( m_wizPage1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_cRotationNChoices, m_cRotationChoices, 0 );
	m_cRotation->SetSelection( 0 );
	fgSizer10->Add( m_cRotation, 0, wxALL, 2 );
	
	m_staticText101 = new wxStaticText( m_wizPage1, wxID_ANY, _("Skew"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText101->Wrap( -1 );
	fgSizer10->Add( m_staticText101, 0, wxALL, 2 );
	
	m_sSkew = new wxSlider( m_wizPage1, wxID_ANY, 0, -1000, 1000, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	fgSizer10->Add( m_sSkew, 0, wxALL|wxEXPAND, 2 );
	
	
	sbSizer5->Add( fgSizer10, 1, wxEXPAND, 5 );
	
	
	fgSizer172->Add( sbSizer5, 1, wxEXPAND, 5 );
	
	
	fgSizer11->Add( fgSizer172, 1, wxEXPAND, 5 );
	
	
	m_wizPage1->SetSizer( fgSizer11 );
	m_wizPage1->Layout();
	fgSizer11->Fit( m_wizPage1 );
	wxWizardPageSimple* m_wizPage2 = new wxWizardPageSimple( this );
	m_pages.Add( m_wizPage2 );
	
	wxFlexGridSizer* fgSizer111;
	fgSizer111 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer111->AddGrowableCol( 0 );
	fgSizer111->AddGrowableRow( 0 );
	fgSizer111->SetFlexibleDirection( wxBOTH );
	fgSizer111->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer171;
	fgSizer171 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer171->AddGrowableCol( 1 );
	fgSizer171->AddGrowableRow( 0 );
	fgSizer171->SetFlexibleDirection( wxBOTH );
	fgSizer171->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_wizPage2, wxID_ANY, _("Image Coordinates") ), wxVERTICAL );
	
	m_fgSizer434 = new wxFlexGridSizer( 2, 1, 0, 0 );
	m_fgSizer434->SetFlexibleDirection( wxBOTH );
	m_fgSizer434->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_fgSizerUnMappedCoords = new wxFlexGridSizer( 0, 1, 0, 0 );
	m_fgSizerUnMappedCoords->AddGrowableCol( 0 );
	m_fgSizerUnMappedCoords->SetFlexibleDirection( wxBOTH );
	m_fgSizerUnMappedCoords->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_cbCoordSet = new wxComboBox( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0, NULL, 0 ); 
	m_fgSizerUnMappedCoords->Add( m_cbCoordSet, 0, wxALL|wxEXPAND, 5 );
	
	m_bRemoveCoordSet = new wxButton( m_wizPage2, wxID_ANY, _("Remove"), wxDefaultPosition, wxSize( 80,-1 ), 0 );
	m_fgSizerUnMappedCoords->Add( m_bRemoveCoordSet, 0, wxTOP, 5 );
	
	
	m_fgSizer434->Add( m_fgSizerUnMappedCoords, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_rbCoord1UnMapped = new wxRadioButton( m_wizPage2, wxID_ANY, _("Coord Y/X"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_rbCoord1UnMapped->SetForegroundColour( wxColour( 255, 0, 0 ) );
	
	fgSizer7->Add( m_rbCoord1UnMapped, 0, wxALL, 5 );
	
	m_staticText6 = new wxStaticText( m_wizPage2, wxID_ANY, _("Lat/Lon"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
    m_staticText6->SetForegroundColour( wxColour( 255, 0, 0 ) );
    
	fgSizer7->Add( m_staticText6, 0, wxALL, 5 );
	
	m_sCoord1YUnMapped = new wxSpinCtrl( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer7->Add( m_sCoord1YUnMapped, 0, wxALL, 5 );
	
	m_sCoord1LatUnMapped = new wxSpinCtrl( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, -90, 90, 0 );
	fgSizer7->Add( m_sCoord1LatUnMapped, 0, wxALL, 5 );
	
	m_sCoord1XUnMapped = new wxSpinCtrl( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer7->Add( m_sCoord1XUnMapped, 0, wxALL, 5 );
	
	m_sCoord1LonUnMapped = new wxSpinCtrl( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, -180, 180, 0 );
	fgSizer7->Add( m_sCoord1LonUnMapped, 0, wxALL, 5 );
	
	m_rbCoord2UnMapped = new wxRadioButton( m_wizPage2, wxID_ANY, _("Coord Y/X"), wxDefaultPosition, wxDefaultSize, 0 );
	m_rbCoord2UnMapped->SetForegroundColour( wxColour( 32, 192, 32 ) );
	
	fgSizer7->Add( m_rbCoord2UnMapped, 0, wxALL, 5 );
	
	m_staticText8 = new wxStaticText( m_wizPage2, wxID_ANY, _("Lat/Lon"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
    m_staticText8->SetForegroundColour( wxColour( 32, 192, 32 ) );
    
	fgSizer7->Add( m_staticText8, 0, wxALL, 5 );
	
	m_sCoord2YUnMapped = new wxSpinCtrl( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer7->Add( m_sCoord2YUnMapped, 0, wxALL, 5 );
	
	m_sCoord2LatUnMapped = new wxSpinCtrl( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, -90, 90, 0 );
	fgSizer7->Add( m_sCoord2LatUnMapped, 0, wxALL, 5 );
	
	m_sCoord2XUnMapped = new wxSpinCtrl( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer7->Add( m_sCoord2XUnMapped, 0, wxALL, 5 );
	
	m_sCoord2LonUnMapped = new wxSpinCtrl( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, -180, 180, 0 );
	fgSizer7->Add( m_sCoord2LonUnMapped, 0, wxALL, 5 );
	
	
	m_fgSizer434->Add( fgSizer7, 1, wxEXPAND, 5 );
	
	
	sbSizer4->Add( m_fgSizer434, 1, 0, 5 );
	
	
	fgSizer171->Add( sbSizer4, 1, wxEXPAND, 5 );
	
	m_swFaxArea2 = new wxScrolledWindow( m_wizPage2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_swFaxArea2->SetScrollRate( 5, 5 );
	fgSizer171->Add( m_swFaxArea2, 1, wxEXPAND | wxALL, 5 );
	
	
	fgSizer111->Add( fgSizer171, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer51;
	sbSizer51 = new wxStaticBoxSizer( new wxStaticBox( m_wizPage2, wxID_ANY, _("Coordinates Mapping Correction") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer101;
	fgSizer101 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer101->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer101->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer17;
	fgSizer17 = new wxFlexGridSizer( 1, 0, 0, 0 );
	fgSizer17->SetFlexibleDirection( wxBOTH );
	fgSizer17->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText15 = new wxStaticText( m_wizPage2, wxID_ANY, _("Input Type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer17->Add( m_staticText15, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxString m_cMappingChoices[] = { _("mercator"), _("polar"), _("conic"), _("fixed-flat") };
	int m_cMappingNChoices = sizeof( m_cMappingChoices ) / sizeof( wxString );
	m_cMapping = new wxChoice( m_wizPage2, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_cMappingNChoices, m_cMappingChoices, 0 );
	m_cMapping->SetSelection( 0 );
	fgSizer17->Add( m_cMapping, 0, wxALL, 5 );
	
	m_staticText21 = new wxStaticText( m_wizPage2, wxID_ANY, _("Output Type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	fgSizer17->Add( m_staticText21, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText22 = new wxStaticText( m_wizPage2, wxID_ANY, _("Mercator"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	fgSizer17->Add( m_staticText22, 0, wxALL, 5 );
	
	m_staticText24 = new wxStaticText( m_wizPage2, wxID_ANY, _("Size"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	fgSizer17->Add( m_staticText24, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_tMappingMultiplier = new wxTextCtrl( m_wizPage2, wxID_ANY, _("1"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	fgSizer17->Add( m_tMappingMultiplier, 0, wxALL, 5 );
	
	m_staticText20 = new wxStaticText( m_wizPage2, wxID_ANY, _("W/H"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	fgSizer17->Add( m_staticText20, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_tMappingRatio = new wxTextCtrl( m_wizPage2, wxID_ANY, _("1"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	fgSizer17->Add( m_tMappingRatio, 0, wxALL, 5 );
	
	m_staticText211 = new wxStaticText( m_wizPage2, wxID_ANY, _("True Width Ratio"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText211->Wrap( -1 );
	fgSizer17->Add( m_staticText211, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_tTrueRatio = new wxTextCtrl( m_wizPage2, wxID_ANY, _("1"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	fgSizer17->Add( m_tTrueRatio, 0, wxALL, 5 );
	
	
	fgSizer101->Add( fgSizer17, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgChangingLabelSizer;
	fgChangingLabelSizer = new wxFlexGridSizer( 1, 0, 0, 0 );
	fgChangingLabelSizer->SetFlexibleDirection( wxBOTH );
	fgChangingLabelSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_stMappingLabel1 = new wxStaticText( m_wizPage2, wxID_ANY, _("Pole X"), wxDefaultPosition, wxDefaultSize, 0 );
	m_stMappingLabel1->Wrap( -1 );
	fgChangingLabelSizer->Add( m_stMappingLabel1, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_sMappingPoleX = new wxSpinCtrl( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), wxSP_ARROW_KEYS, -100000, 100000, 369 );
	fgChangingLabelSizer->Add( m_sMappingPoleX, 0, wxALL, 5 );
	
	m_stMappingLabel2 = new wxStaticText( m_wizPage2, wxID_ANY, _("Pole Y"), wxDefaultPosition, wxDefaultSize, 0 );
	m_stMappingLabel2->Wrap( -1 );
	fgChangingLabelSizer->Add( m_stMappingLabel2, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_sMappingPoleY = new wxSpinCtrl( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), wxSP_ARROW_KEYS, -100000, 100000, -74 );
	fgChangingLabelSizer->Add( m_sMappingPoleY, 0, wxALL, 5 );
	
	m_stMapping = new wxStaticText( m_wizPage2, wxID_ANY, _("Equator Y"), wxDefaultPosition, wxDefaultSize, 0 );
	m_stMapping->Wrap( -1 );
	fgChangingLabelSizer->Add( m_stMapping, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_sMappingEquatorY = new wxSpinCtrl( m_wizPage2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), wxSP_ARROW_KEYS, -100000, 100000, 560 );
	fgChangingLabelSizer->Add( m_sMappingEquatorY, 0, wxALL, 5 );
	
	m_bGetMapping = new wxButton( m_wizPage2, wxID_ANY, _("Get Mapping "), wxDefaultPosition, wxDefaultSize, 0 );
	fgChangingLabelSizer->Add( m_bGetMapping, 0, wxALL, 5 );
	
	m_bGetEquator = new wxButton( m_wizPage2, wxID_ANY, _("Get Equator"), wxDefaultPosition, wxDefaultSize, 0 );
	fgChangingLabelSizer->Add( m_bGetEquator, 0, wxALL, 5 );
	
	m_bInformation = new wxButton( m_wizPage2, wxID_ANY, _("Information"), wxDefaultPosition, wxDefaultSize, 0 );
	fgChangingLabelSizer->Add( m_bInformation, 0, wxALL, 5 );
	
	
	fgSizer101->Add( fgChangingLabelSizer, 1, wxEXPAND, 5 );
	
	
	sbSizer51->Add( fgSizer101, 1, wxEXPAND, 5 );
	
	
	fgSizer111->Add( sbSizer51, 1, wxEXPAND, 5 );
	
	
	m_wizPage2->SetSizer( fgSizer111 );
	m_wizPage2->Layout();
	fgSizer111->Fit( m_wizPage2 );
	wxWizardPageSimple* m_wizPage3 = new wxWizardPageSimple( this );
	m_pages.Add( m_wizPage3 );
	
	wxFlexGridSizer* fgSizer1111;
	fgSizer1111 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1111->AddGrowableCol( 0 );
	fgSizer1111->AddGrowableRow( 0 );
	fgSizer1111->SetFlexibleDirection( wxBOTH );
	fgSizer1111->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer1711;
	fgSizer1711 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer1711->AddGrowableCol( 1 );
	fgSizer1711->AddGrowableRow( 0 );
	fgSizer1711->SetFlexibleDirection( wxBOTH );
	fgSizer1711->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer41;
	sbSizer41 = new wxStaticBoxSizer( new wxStaticBox( m_wizPage3, wxID_ANY, _("Image Coordinates") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer91;
	fgSizer91 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer91->SetFlexibleDirection( wxBOTH );
	fgSizer91->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer71;
	fgSizer71 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer71->SetFlexibleDirection( wxBOTH );
	fgSizer71->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_rbCoord1 = new wxRadioButton( m_wizPage3, wxID_ANY, _("Coord Y/X"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_rbCoord1->SetForegroundColour( wxColour( 255, 0, 0 ) );
	
	fgSizer71->Add( m_rbCoord1, 0, wxALL, 5 );
	
	m_staticText61 = new wxStaticText( m_wizPage3, wxID_ANY, _("Lat/Lon"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText61->Wrap( -1 );
    m_staticText61->SetForegroundColour( wxColour( 255, 0, 0 ) );

	fgSizer71->Add( m_staticText61, 0, wxALL, 5 );
	
	m_sCoord1Y = new wxSpinCtrl( m_wizPage3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer71->Add( m_sCoord1Y, 0, wxALL, 5 );
	
	m_sCoord1Lat = new wxSpinCtrl( m_wizPage3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, -90, 90, 0 );
	fgSizer71->Add( m_sCoord1Lat, 0, wxALL, 5 );
	
	m_sCoord1X = new wxSpinCtrl( m_wizPage3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer71->Add( m_sCoord1X, 0, wxALL, 5 );
	
	m_sCoord1Lon = new wxSpinCtrl( m_wizPage3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, -180, 180, 0 );
	fgSizer71->Add( m_sCoord1Lon, 0, wxALL, 5 );
	
	m_rbCoord2 = new wxRadioButton( m_wizPage3, wxID_ANY, _("Coord Y/X"), wxDefaultPosition, wxDefaultSize, 0 );
	m_rbCoord2->SetForegroundColour( wxColour( 32, 192, 32 ) );
	
	fgSizer71->Add( m_rbCoord2, 0, wxALL, 5 );
	
	m_staticText81 = new wxStaticText( m_wizPage3, wxID_ANY, _("Lat/Lon"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText81->Wrap( -1 );
    m_staticText81->SetForegroundColour( wxColour( 32, 192, 32 ) );

	fgSizer71->Add( m_staticText81, 0, wxALL, 5 );
	
	m_sCoord2Y = new wxSpinCtrl( m_wizPage3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer71->Add( m_sCoord2Y, 0, wxALL, 5 );
	
	m_sCoord2Lat = new wxSpinCtrl( m_wizPage3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, -90, 90, 0 );
	fgSizer71->Add( m_sCoord2Lat, 0, wxALL, 5 );
	
	m_sCoord2X = new wxSpinCtrl( m_wizPage3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer71->Add( m_sCoord2X, 0, wxALL, 5 );
	
	m_sCoord2Lon = new wxSpinCtrl( m_wizPage3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, -180, 180, 0 );
	fgSizer71->Add( m_sCoord2Lon, 0, wxALL, 5 );
	
	
	fgSizer91->Add( fgSizer71, 1, wxEXPAND, 5 );
	
	
	sbSizer41->Add( fgSizer91, 1, 0, 5 );
	
	
	fgSizer1711->Add( sbSizer41, 1, wxEXPAND, 5 );
	
	m_swFaxArea3 = new wxScrolledWindow( m_wizPage3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_swFaxArea3->SetScrollRate( 5, 5 );
	fgSizer1711->Add( m_swFaxArea3, 1, wxEXPAND | wxALL, 5 );
	
	
	fgSizer1111->Add( fgSizer1711, 1, wxEXPAND, 5 );
	
	
	m_wizPage3->SetSizer( fgSizer1111 );
	m_wizPage3->Layout();
	fgSizer1111->Fit( m_wizPage3 );
	
	this->Centre( wxBOTH );
	
	for ( unsigned int i = 1; i < m_pages.GetCount(); i++ )
	{
		m_pages.Item( i )->SetPrev( m_pages.Item( i - 1 ) );
		m_pages.Item( i - 1 )->SetNext( m_pages.Item( i ) );
	}
	
	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( WeatherFaxWizardBase::OnSetSizes ) );
	this->Connect( wxID_ANY, wxEVT_WIZARD_PAGE_CHANGED, wxWizardEventHandler( WeatherFaxWizardBase::OnWizardPageChanged ) );
	m_swFaxArea1->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( WeatherFaxWizardBase::OnBitmapClickPage1 ), NULL, this );
	m_swFaxArea1->Connect( wxEVT_PAINT, wxPaintEventHandler( WeatherFaxWizardBase::OnPaintImage ), NULL, this );
	m_bStopDecoding->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WeatherFaxWizardBase::OnStopDecoding ), NULL, this );
	m_bPhasingArea->Connect( wxEVT_PAINT, wxPaintEventHandler( WeatherFaxWizardBase::OnPaintPhasing ), NULL, this );
	m_cFilter->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_cRotation->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( WeatherFaxWizardBase::UpdatePage1Rotation ), NULL, this );
	m_sSkew->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_cbCoordSet->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( WeatherFaxWizardBase::OnCoordSet ), NULL, this );
	m_cbCoordSet->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( WeatherFaxWizardBase::OnCoordText ), NULL, this );
	m_bRemoveCoordSet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WeatherFaxWizardBase::OnRemoveCoords ), NULL, this );
	m_sCoord1YUnMapped->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord1XUnMapped->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord2YUnMapped->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord2XUnMapped->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_swFaxArea2->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( WeatherFaxWizardBase::OnBitmapClickPage2 ), NULL, this );
	m_swFaxArea2->Connect( wxEVT_PAINT, wxPaintEventHandler( WeatherFaxWizardBase::OnPaintImage ), NULL, this );
	m_cMapping->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( WeatherFaxWizardBase::OnMappingChoice ), NULL, this );
	m_tTrueRatio->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( WeatherFaxWizardBase::OnUpdateMapping ), NULL, this );
	m_sMappingPoleX->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnUpdateMapping ), NULL, this );
	m_sMappingPoleY->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnUpdateMapping ), NULL, this );
	m_bGetMapping->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WeatherFaxWizardBase::OnGetMapping ), NULL, this );
	m_bGetEquator->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WeatherFaxWizardBase::OnGetEquator ), NULL, this );
	m_bInformation->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WeatherFaxWizardBase::OnInformation ), NULL, this );
	m_sCoord1Y->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord1X->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord2Y->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord2X->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_swFaxArea3->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( WeatherFaxWizardBase::OnBitmapClickPage3 ), NULL, this );
	m_swFaxArea3->Connect( wxEVT_PAINT, wxPaintEventHandler( WeatherFaxWizardBase::OnPaintImage ), NULL, this );
}

WeatherFaxWizardBase::~WeatherFaxWizardBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( WeatherFaxWizardBase::OnSetSizes ) );
	this->Disconnect( wxID_ANY, wxEVT_WIZARD_PAGE_CHANGED, wxWizardEventHandler( WeatherFaxWizardBase::OnWizardPageChanged ) );
	m_swFaxArea1->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( WeatherFaxWizardBase::OnBitmapClickPage1 ), NULL, this );
	m_swFaxArea1->Disconnect( wxEVT_PAINT, wxPaintEventHandler( WeatherFaxWizardBase::OnPaintImage ), NULL, this );
	m_bStopDecoding->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WeatherFaxWizardBase::OnStopDecoding ), NULL, this );
	m_bPhasingArea->Disconnect( wxEVT_PAINT, wxPaintEventHandler( WeatherFaxWizardBase::OnPaintPhasing ), NULL, this );
	m_cFilter->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sPhasing->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_cRotation->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( WeatherFaxWizardBase::UpdatePage1Rotation ), NULL, this );
	m_sSkew->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_sSkew->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( WeatherFaxWizardBase::UpdatePage1 ), NULL, this );
	m_cbCoordSet->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( WeatherFaxWizardBase::OnCoordSet ), NULL, this );
	m_cbCoordSet->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( WeatherFaxWizardBase::OnCoordText ), NULL, this );
	m_bRemoveCoordSet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WeatherFaxWizardBase::OnRemoveCoords ), NULL, this );
	m_sCoord1YUnMapped->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord1XUnMapped->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord2YUnMapped->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord2XUnMapped->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_swFaxArea2->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( WeatherFaxWizardBase::OnBitmapClickPage2 ), NULL, this );
	m_swFaxArea2->Disconnect( wxEVT_PAINT, wxPaintEventHandler( WeatherFaxWizardBase::OnPaintImage ), NULL, this );
	m_cMapping->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( WeatherFaxWizardBase::OnMappingChoice ), NULL, this );
	m_tTrueRatio->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( WeatherFaxWizardBase::OnUpdateMapping ), NULL, this );
	m_sMappingPoleX->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnUpdateMapping ), NULL, this );
	m_sMappingPoleY->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnUpdateMapping ), NULL, this );
	m_bGetMapping->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WeatherFaxWizardBase::OnGetMapping ), NULL, this );
	m_bGetEquator->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WeatherFaxWizardBase::OnGetEquator ), NULL, this );
	m_bInformation->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WeatherFaxWizardBase::OnInformation ), NULL, this );
	m_sCoord1Y->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord1X->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord2Y->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_sCoord2X->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( WeatherFaxWizardBase::OnSpin ), NULL, this );
	m_swFaxArea3->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( WeatherFaxWizardBase::OnBitmapClickPage3 ), NULL, this );
	m_swFaxArea3->Disconnect( wxEVT_PAINT, wxPaintEventHandler( WeatherFaxWizardBase::OnPaintImage ), NULL, this );
	
	m_pages.Clear();
}

WeatherFaxPrefsDialog::WeatherFaxPrefsDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer51;
    fgSizer51 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer51->SetFlexibleDirection( wxBOTH );
    fgSizer51->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer55;
    fgSizer55 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer55->SetFlexibleDirection( wxBOTH );
    fgSizer55->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Audio Decoding Options") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer12->SetFlexibleDirection( wxBOTH );
	fgSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText29 = new wxStaticText( this, wxID_ANY, _("Image Width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	fgSizer10->Add( m_staticText29, 0, wxALL, 5 );
	
	m_sImageWidth = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 16384, 576 );
	fgSizer10->Add( m_sImageWidth, 0, wxALL, 5 );
	
	m_staticText30 = new wxStaticText( this, wxID_ANY, _("Bits per Pixel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText30->Wrap( -1 );
	fgSizer10->Add( m_staticText30, 0, wxALL, 5 );
	
	m_sBitsPerPixel = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 8, 8 );
	fgSizer10->Add( m_sBitsPerPixel, 0, wxALL, 5 );
	
	m_staticText31 = new wxStaticText( this, wxID_ANY, _("Carrier"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	fgSizer10->Add( m_staticText31, 0, wxALL, 5 );
	
	m_sCarrier = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 10000, 1900 );
	fgSizer10->Add( m_sCarrier, 0, wxALL, 5 );
	
	m_staticText32 = new wxStaticText( this, wxID_ANY, _("Deviation"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32->Wrap( -1 );
	fgSizer10->Add( m_staticText32, 0, wxALL, 5 );
	
	m_sDeviation = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 10000, 400 );
	fgSizer10->Add( m_sDeviation, 0, wxALL, 5 );
	
	m_staticText33 = new wxStaticText( this, wxID_ANY, _("Filter"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33->Wrap( -1 );
	fgSizer10->Add( m_staticText33, 0, wxALL, 5 );
	
	wxString m_cFilterChoices[] = { _("narrow"), _("middle"), _("wide") };
	int m_cFilterNChoices = sizeof( m_cFilterChoices ) / sizeof( wxString );
	m_cFilter = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_cFilterNChoices, m_cFilterChoices, 0 );
	m_cFilter->SetSelection( 1 );
	fgSizer10->Add( m_cFilter, 0, wxALL, 5 );
	
	
	fgSizer12->Add( fgSizer10, 1, wxEXPAND, 5 );
	
	m_cbSkip = new wxCheckBox( this, wxID_ANY, _("Skip start, stop and \nphasing detection"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer12->Add( m_cbSkip, 0, wxALL, 5 );
	
	m_cbInclude = new wxCheckBox( this, wxID_ANY, _("Include header data in image"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer12->Add( m_cbInclude, 0, wxALL, 5 );
	
	
	sbSizer6->Add( fgSizer12, 1, wxEXPAND, 5 );
	
	
	fgSizer55->Add( sbSizer6, 1, wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer56;
    fgSizer56 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer56->SetFlexibleDirection( wxBOTH );
    fgSizer56->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxStaticBoxSizer* sbSizer18;
    sbSizer18 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("HF Schedules") ), wxVERTICAL );

    wxFlexGridSizer* fgSizer47;
    fgSizer47 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer47->SetFlexibleDirection( wxBOTH );
    fgSizer47->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_cbLoadSchedulesStart = new wxCheckBox( this, wxID_ANY, _("Load Schedules at Startup"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer47->Add( m_cbLoadSchedulesStart, 0, wxALL, 5 );


    sbSizer18->Add( fgSizer47, 1, wxEXPAND, 5 );


    fgSizer56->Add( sbSizer18, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer17;
    sbSizer17 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Export") ), wxVERTICAL );

    wxFlexGridSizer* fgSizer53;
    fgSizer53 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer53->SetFlexibleDirection( wxBOTH );
    fgSizer53->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxFlexGridSizer* fgSizer511;
    fgSizer511 = new wxFlexGridSizer( 0, 3, 0, 0 );
    fgSizer511->SetFlexibleDirection( wxBOTH );
    fgSizer511->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText36 = new wxStaticText( this, wxID_ANY, _("Reduce to"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText36->Wrap( -1 );
    fgSizer511->Add( m_staticText36, 0, wxALL, 5 );

    m_sExportColors = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 2, 32768, 64 );
    fgSizer511->Add( m_sExportColors, 0, wxALL, 5 );

    m_staticText37 = new wxStaticText( this, wxID_ANY, _("Colors"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText37->Wrap( -1 );
    fgSizer511->Add( m_staticText37, 0, wxALL, 5 );


    fgSizer53->Add( fgSizer511, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer181;
    sbSizer181 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Depth") ), wxVERTICAL );

    wxFlexGridSizer* fgSizer52;
    fgSizer52 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer52->SetFlexibleDirection( wxBOTH );
    fgSizer52->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_rbExportDepthMeters = new wxRadioButton( this, wxID_ANY, _("Meters"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer52->Add( m_rbExportDepthMeters, 0, wxALL, 5 );

    m_rbExportDepthFathoms = new wxRadioButton( this, wxID_ANY, _("Fathoms"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer52->Add( m_rbExportDepthFathoms, 0, wxALL, 5 );


    sbSizer181->Add( fgSizer52, 1, wxEXPAND, 5 );


    fgSizer53->Add( sbSizer181, 1, wxEXPAND, 5 );

    wxFlexGridSizer* fgSizer54;
    fgSizer54 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer54->SetFlexibleDirection( wxBOTH );
    fgSizer54->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText38 = new wxStaticText( this, wxID_ANY, _("fix sounding datum"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText38->Wrap( -1 );
    fgSizer54->Add( m_staticText38, 0, wxALL, 5 );

    m_tExportSoundingDatum = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer54->Add( m_tExportSoundingDatum, 0, wxALL, 5 );


    fgSizer53->Add( fgSizer54, 1, wxEXPAND, 5 );


    sbSizer17->Add( fgSizer53, 1, wxEXPAND, 5 );


    fgSizer56->Add( sbSizer17, 1, wxEXPAND, 5 );


    fgSizer55->Add( fgSizer56, 1, wxEXPAND, 5 );


    fgSizer51->Add( fgSizer55, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	
	fgSizer51->Add( m_sdbSizer1, 0, wxBOTTOM|wxEXPAND|wxTOP, 5 );
	
	
	this->SetSizer( fgSizer51 );
	this->Layout();
	fgSizer51->Fit( this );
	
	this->Centre( wxBOTH );
}

WeatherFaxPrefsDialog::~WeatherFaxPrefsDialog()
{
}

AboutDialogBase::AboutDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxFlexGridSizer* fgSizer90;
    fgSizer90 = new wxFlexGridSizer( 0, 1, 0, 0 );
    fgSizer90->SetFlexibleDirection( wxBOTH );
    fgSizer90->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxFlexGridSizer* fgSizer50;
    fgSizer50 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer50->SetFlexibleDirection( wxBOTH );
    fgSizer50->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText34 = new wxStaticText( this, wxID_ANY, _("Weather Fax Plugin Version"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText34->Wrap( -1 );
    fgSizer50->Add( m_staticText34, 0, wxALL, 5 );

    m_stVersion = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_stVersion->Wrap( -1 );
    fgSizer50->Add( m_stVersion, 0, wxALL, 5 );


    fgSizer90->Add( fgSizer50, 1, wxEXPAND, 5 );

    m_staticText110 = new wxStaticText( this, wxID_ANY, _("The weatherfax plugin for opencpn is intended to reduce the amount of user interaction to receive weather fax images and to overlay them directly onto charts.\n\nTo get started, open either an image or recorded wav audio file of a ssb radio fax transmission.\n\nLicense: GPLv3+\n\nSource Code:\nhttps://github.com/seandepagnier/weatherfax_pi\n\nAuthor:\nSean D'Epagnier\n\nMany thanks to all of the translators and testers."), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText110->Wrap( 400 );
    fgSizer90->Add( m_staticText110, 0, wxALL, 5 );

    wxFlexGridSizer* fgSizer91;
    fgSizer91 = new wxFlexGridSizer( 0, 2, 0, 0 );
    fgSizer91->SetFlexibleDirection( wxBOTH );
    fgSizer91->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_bDonate = new wxButton( this, wxID_ANY, _("Donate"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer91->Add( m_bDonate, 0, wxALL, 5 );

    m_bClose = new wxButton( this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer91->Add( m_bClose, 0, wxALL, 5 );


    fgSizer90->Add( fgSizer91, 1, wxEXPAND, 5 );


    this->SetSizer( fgSizer90 );
    this->Layout();
    fgSizer90->Fit( this );

    this->Centre( wxBOTH );

    // Connect Events
    m_bDonate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AboutDialogBase::OnDonate ), NULL, this );
    m_bClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AboutDialogBase::OnClose ), NULL, this );
}

AboutDialogBase::~AboutDialogBase()
{
    // Disconnect Events
    m_bDonate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AboutDialogBase::OnDonate ), NULL, this );
    m_bClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AboutDialogBase::OnClose ), NULL, this );

}
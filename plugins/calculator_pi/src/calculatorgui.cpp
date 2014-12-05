///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "calculatorgui.h"
#include "calculator_pi.h"
///////////////////////////////////////////////////////////////////////////

DlgDef::DlgDef( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxSize( -1,-1 ) );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_Overview = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );
	
	m_result = new wxTextCtrl( m_Overview, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 420,40 ), wxTE_PROCESS_ENTER );
	m_result->SetMaxLength( 0 ); 
	m_result->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	m_result->SetMinSize( wxSize( 200,40 ) );
	m_result->SetMaxSize( wxSize( -1,40 ) );
	
	bSizer41->Add( m_result, 8, wxALL|wxEXPAND, 5 );
	
	Calculate = new wxButton( m_Overview, wxID_ANY, _("Calculate"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	Calculate->SetMinSize( wxSize( 80,30 ) );
	Calculate->SetMaxSize( wxSize( 80,40 ) );
	
	bSizer41->Add( Calculate, 0, wxALL|wxEXPAND, 5 );
	
	m_Function = new wxButton( m_Overview, wxID_ANY, _("Function"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( m_Function, 0, wxALL|wxEXPAND, 5 );
	
	m_Help = new wxCheckBox( m_Overview, wxID_ANY, _("History"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( m_Help, 0, wxALL|wxEXPAND, 5 );
	
	m_HelpButton = new wxButton( m_Overview, wxID_ANY, _("?"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( m_HelpButton, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer45->Add( bSizer41, 0, wxEXPAND, 5 );
	
	wxBoxSizer* HelpPanel;
	HelpPanel = new wxBoxSizer( wxVERTICAL );
	
	m_listCtrl = new wxListCtrl( m_Overview, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_AUTOARRANGE|wxLC_LIST );
	m_listCtrl->Hide();
	
	HelpPanel->Add( m_listCtrl, 3, wxALL|wxEXPAND, 5 );
	
	
	bSizer45->Add( HelpPanel, 1, wxEXPAND, 5 );
	
	
	m_Overview->SetSizer( bSizer45 );
	m_Overview->Layout();
	bSizer45->Fit( m_Overview );
	bSizer7->Add( m_Overview, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer7 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_result->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( DlgDef::key_shortcut ), NULL, this );
	m_result->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DlgDef::OnCalculate ), NULL, this );
	Calculate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnCalculate ), NULL, this );
	m_Function->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnFunction ), NULL, this );
	m_Help->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DlgDef::OnToggle ), NULL, this );
	m_HelpButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnHelp ), NULL, this );
	m_listCtrl->Connect( wxEVT_CHAR, wxKeyEventHandler( DlgDef::OnKey ), NULL, this );
	m_listCtrl->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( DlgDef::OnItem ), NULL, this );
}

DlgDef::~DlgDef()
{
	// Disconnect Events
	m_result->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( DlgDef::key_shortcut ), NULL, this );
	m_result->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DlgDef::OnCalculate ), NULL, this );
	Calculate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnCalculate ), NULL, this );
	m_Function->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnFunction ), NULL, this );
	m_Help->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DlgDef::OnToggle ), NULL, this );
	m_HelpButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDef::OnHelp ), NULL, this );
	m_listCtrl->Disconnect( wxEVT_CHAR, wxKeyEventHandler( DlgDef::OnKey ), NULL, this );
	m_listCtrl->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( DlgDef::OnItem ), NULL, this );
	
}

CfgDlgDef::CfgDlgDef( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	AddLocaleCatalog( _T("opencpn-calculator_pi") );
    
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Show/Hide Buttons") ), wxVERTICAL );
	
	m_showhelpB = new wxCheckBox( this, wxID_ANY, _("Help Button"), wxDefaultPosition, wxDefaultSize, 0 );
	m_showhelpB->SetValue(true); 
	m_showhelpB->SetToolTip( _("Show/Hide help button (you can always type \"help\" from input window).") );
	
	sbSizer3->Add( m_showhelpB, 0, wxALL, 5 );
	
	m_showhistoryB = new wxCheckBox( this, wxID_ANY, _("History Toggle"), wxDefaultPosition, wxDefaultSize, 0 );
	m_showhistoryB->SetValue(true); 
	m_showhistoryB->SetToolTip( _("Show/Hide history toggle (you can always type \"history\" from input window).") );
	
	sbSizer3->Add( m_showhistoryB, 0, wxALL, 5 );
	
	m_CalculateB = new wxCheckBox( this, wxID_ANY, _("Calculate Button"), wxDefaultPosition, wxDefaultSize, 0 );
	m_CalculateB->SetValue(true); 
	m_CalculateB->SetToolTip( _("Show/Hide Calculate button (you can always press enter at the end of input).") );
	
	sbSizer3->Add( m_CalculateB, 0, wxALL, 5 );
	
	m_showfunction = new wxCheckBox( this, wxID_ANY, _("Function button"), wxDefaultPosition, wxDefaultSize, 0 );
	m_showfunction->SetValue(true); 
	sbSizer3->Add( m_showfunction, 0, wxALL, 5 );
	
	
	bSizer2->Add( sbSizer3, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("History Settings") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_Font_Size_txt = new wxStaticText( this, wxID_ANY, _("Max Results"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Font_Size_txt->Wrap( -1 );
	m_Font_Size_txt->SetToolTip( _("More results use up more memory. Memory is only allocated if required (e.g. if number is 30000, corresponding memory will only be used after this many calculations).") );
	
	fgSizer3->Add( m_Font_Size_txt, 0, wxALL, 5 );
	
	m_MaxResults = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 30000, 30000 );
	m_MaxResults->SetToolTip( _("More results use more memory. Memory will be cleared if Max Results is reached. NB variables are kept") );
	
	fgSizer3->Add( m_MaxResults, 0, wxALL, 5 );
	
	
	sbSizer4->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, _("Opacity"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	m_staticText17->Hide();
	
	sbSizer4->Add( m_staticText17, 0, wxALL, 5 );
	
	m_sOpacity = new wxSlider( this, wxID_ANY, 50, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	m_sOpacity->Hide();
	
	sbSizer4->Add( m_sOpacity, 0, wxALL|wxEXPAND, 5 );
	
	m_showhistory = new wxCheckBox( this, wxID_ANY, _("Show History Window"), wxDefaultPosition, wxDefaultSize, 0 );
	m_showhistory->SetToolTip( _("Show/Hide history") );
	
	sbSizer4->Add( m_showhistory, 0, wxALL, 5 );
	
	m_capturehidden = new wxCheckBox( this, wxID_ANY, _("Capture when Hidden"), wxDefaultPosition, wxDefaultSize, 0 );
	m_capturehidden->SetValue(true); 
	m_capturehidden->SetToolTip( _("Capture results in history, even when the history box is hidden. Uncheck this box to save some memory when history is hidden.") );
	
	sbSizer4->Add( m_capturehidden, 0, wxALL, 5 );
	
	
	bSizer2->Add( sbSizer4, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer41;
	sbSizer41 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Other") ), wxVERTICAL );
	
	m_logresults = new wxCheckBox( this, wxID_ANY, _("Log results to opencpn"), wxDefaultPosition, wxDefaultSize, 0 );
	m_logresults->SetToolTip( _("Log calculator results in OpenCPN log file") );
	
	sbSizer41->Add( m_logresults, 0, wxALL, 5 );
	
	
	bSizer2->Add( sbSizer41, 0, wxEXPAND, 5 );
	
	
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

HlpDlgDef::HlpDlgDef( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	HelpPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrl3 = new wxTextCtrl( HelpPanel, wxID_ANY, _("Calculator Plugin for OpenCPN by SaltyPaws aka Walbert Schulpen\n=======================================\n\nThis is a light weight yet powerful calculator plugin for OpenCPN.  Would you like to know your hull speed?\n\nKey features are:\n* Storing results in variables\n* Shows historic calculations\n* Full scientific functions\n* Screen footprint can be optimised & minimised as required\n\nExamples of expression that work in the calculator are: (comments are in brackets, some results depend on other example calculations):\n=========\nHull speed:\n\tLWL=48\t\t\t(water line lenght in feet)\n\tvhull=1.34*LWL^(1/2)\t(hull speed in knots)\n\nConversions:\n\tftm=0.3048\t\t\t(feet to meters)\n\tkm_to_nm=0.539957\t\t(Kilometers to nautical Mile)\n\tftm*LWL\t\t\t(waterline length in meters)\n\nDistance to horizon\n\tR=6378.1*1000\t\t(Radius of the earth in m)\n\tH=2.5\t\t\t(Height of the eye above sea-level in m)\n\td = R * acos(R/(R + h))\t(Distance to horizon in m)   \n\tans*km_to_nm\t\t(Distance to horizon in nm)\n\nDistance to lighthouse\n\tH1=200\t\t\t(height of lighthouse in m)\n\td1 = R*acos(R/(R + H1))\t(Distance to horizon in m)\n\tdistance=d1+d\t\t(visibility range of lighthouse in m)\n\nOperators:\n\t+ - * / ^ (NB use ^(1/2) for square root)         \n\t% & | << >> = <> < > <= >= ||\n\t! (Factorial)\n \nFunctions:\n\tAbs, Exp, Sign, Sqrt, Log, Log10\n\tSin, Cos, Tan, ASin, ACos, Atan (default entry is in radians use e.g. sin(dtr*90) to calculate in degree)\n\tFactorial\n \nVariables:\n\tPi, e\n\tAns is the result of the previous calulation\n\tdtr is the conversion factor from degrees to radians\t\t\n\tyou can define your own variables (e.g. myvariable=10/8*cos(dtr*90) or yourvariable=Ans)\n\tclear removes results in the history, but leaves your defined variables in tact\n\nUser Interface - type these commands in the command window:\n\thistory - Toggle the history panel\n\tshowhelp - Show/Hide the Help button\n\tshowcalculate - Show/Hide the Calculate button\n\tshowhistory - Show/Hide the history toggle\n\thelp - show the help menu\n\nSettings/Plugins/Preferences:\n\tShow/Hide Calculate, Help and History toggle buttons\n\tHistory Settings: max Results - History will be cleared after this many results have been stored to save memory. Max 30000.\n\tShow history window: Show/Hide default state\n\tCapture when hidden: disable capturing of history when history panel is hidden - this will save some memory\n\tLog to opencpn: Enable/Disable logging of results to opencpn logfile."), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	m_textCtrl3->SetMinSize( wxSize( 600,400 ) );
	
	bSizer10->Add( m_textCtrl3, 100, wxALL|wxEXPAND, 5 );
	
	
	bSizer11->Add( bSizer10, 5, wxEXPAND, 5 );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( HelpPanel, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2->Realize();
	
	bSizer11->Add( m_sdbSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	HelpPanel->SetSizer( bSizer11 );
	HelpPanel->Layout();
	bSizer11->Fit( HelpPanel );
	bSizer7->Add( HelpPanel, 3, wxALL|wxBOTTOM|wxEXPAND|wxTOP, 5 );
	
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_sdbSizer2OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HlpDlgDef::OnClose ), NULL, this );
}

HlpDlgDef::~HlpDlgDef()
{
	// Disconnect Events
	m_sdbSizer2OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HlpDlgDef::OnClose ), NULL, this );
	
}

MyDialog5::MyDialog5( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxVERTICAL );
	
	m_panel14 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );
	
	m_wxNotebook234 = new wxNotebook( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel11 = new wxPanel( m_wxNotebook234, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer402;
	bSizer402 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_panel11, wxID_ANY, _("DD.dddddd\u00b0 (no limit to decimals)") ), wxVERTICAL );
	
	wxBoxSizer* bSizer1321;
	bSizer1321 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer1431;
	bSizer1431 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3231 = new wxStaticText( m_panel11, wxID_ANY, _("Lattitude  "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3231->Wrap( -1 );
	bSizer1431->Add( m_staticText3231, 1, wxALL|wxEXPAND, 5 );
	
	m_Lat1 = new wxTextCtrl( m_panel11, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Lat1->SetMaxLength( 0 ); 
	bSizer1431->Add( m_Lat1, 3, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_staticText3331 = new wxStaticText( m_panel11, wxID_ANY, _("\u00b0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3331->Wrap( -1 );
	bSizer1431->Add( m_staticText3331, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	
	bSizer1321->Add( bSizer1431, 0, 0, 5 );
	
	wxBoxSizer* bSizer14311;
	bSizer14311 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText32311 = new wxStaticText( m_panel11, wxID_ANY, _("Longitude"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32311->Wrap( -1 );
	bSizer14311->Add( m_staticText32311, 1, wxALL|wxEXPAND, 5 );
	
	m_Lon1 = new wxTextCtrl( m_panel11, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_Lon1->SetMaxLength( 0 ); 
	bSizer14311->Add( m_Lon1, 3, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_staticText33311 = new wxStaticText( m_panel11, wxID_ANY, _("\u00b0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33311->Wrap( -1 );
	bSizer14311->Add( m_staticText33311, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	
	bSizer1321->Add( bSizer14311, 0, 0, 5 );
	
	
	sbSizer7->Add( bSizer1321, 0, wxEXPAND, 5 );
	
	
	bSizer402->Add( sbSizer7, 0, wxEXPAND, 5 );
	
	m_button4 = new wxButton( m_panel11, wxID_ANY, _("Convert"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer402->Add( m_button4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	m_panel11->SetSizer( bSizer402 );
	m_panel11->Layout();
	bSizer402->Fit( m_panel11 );
	m_wxNotebook234->AddPage( m_panel11, _("Degrees"), false );
	m_panel151 = new wxPanel( m_wxNotebook234, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4011;
	bSizer4011 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer721;
	sbSizer721 = new wxStaticBoxSizer( new wxStaticBox( m_panel151, wxID_ANY, _("DD.dddddd\u00b0 (no limit to decimals)") ), wxVERTICAL );
	
	wxBoxSizer* bSizer132121;
	bSizer132121 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer143131;
	bSizer143131 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText323131 = new wxStaticText( m_panel151, wxID_ANY, _("Lat "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText323131->Wrap( -1 );
	bSizer143131->Add( m_staticText323131, 0, wxALL, 5 );
	
	m_Lat1_d1 = new wxTextCtrl( m_panel151, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lat1_d1->SetMaxLength( 0 ); 
	bSizer143131->Add( m_Lat1_d1, 0, wxALL, 5 );
	
	m_staticText333131 = new wxStaticText( m_panel151, wxID_ANY, _("\u00b0"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticText333131->Wrap( -1 );
	bSizer143131->Add( m_staticText333131, 0, wxALL, 5 );
	
	m_Lat1_m1 = new wxTextCtrl( m_panel151, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lat1_m1->SetMaxLength( 0 ); 
	bSizer143131->Add( m_Lat1_m1, 0, wxALL, 5 );
	
	m_staticText11431 = new wxStaticText( m_panel151, wxID_ANY, _("'"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11431->Wrap( -1 );
	bSizer143131->Add( m_staticText11431, 0, wxALL, 5 );
	
	wxString m_Lat1_NS1Choices[] = { _("N"), _("S") };
	int m_Lat1_NS1NChoices = sizeof( m_Lat1_NS1Choices ) / sizeof( wxString );
	m_Lat1_NS1 = new wxChoice( m_panel151, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Lat1_NS1NChoices, m_Lat1_NS1Choices, 0 );
	m_Lat1_NS1->SetSelection( 0 );
	bSizer143131->Add( m_Lat1_NS1, 0, wxALL, 5 );
	
	
	bSizer132121->Add( bSizer143131, 0, 0, 5 );
	
	wxBoxSizer* bSizer1431121;
	bSizer1431121 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3231121 = new wxStaticText( m_panel151, wxID_ANY, _("Lon"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3231121->Wrap( -1 );
	bSizer1431121->Add( m_staticText3231121, 0, wxALL, 5 );
	
	m_Lon1_d1 = new wxTextCtrl( m_panel151, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lon1_d1->SetMaxLength( 0 ); 
	bSizer1431121->Add( m_Lon1_d1, 0, wxALL, 5 );
	
	m_staticText3331121 = new wxStaticText( m_panel151, wxID_ANY, _("\u00b0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3331121->Wrap( -1 );
	bSizer1431121->Add( m_staticText3331121, 0, wxALL, 5 );
	
	m_Lon1_m1 = new wxTextCtrl( m_panel151, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lon1_m1->SetMaxLength( 0 ); 
	bSizer1431121->Add( m_Lon1_m1, 0, wxALL, 5 );
	
	m_staticText114121 = new wxStaticText( m_panel151, wxID_ANY, _("'"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText114121->Wrap( -1 );
	bSizer1431121->Add( m_staticText114121, 0, wxALL, 5 );
	
	wxString m_Lon1_EW1Choices[] = { _("E"), _("W") };
	int m_Lon1_EW1NChoices = sizeof( m_Lon1_EW1Choices ) / sizeof( wxString );
	m_Lon1_EW1 = new wxChoice( m_panel151, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Lon1_EW1NChoices, m_Lon1_EW1Choices, 0 );
	m_Lon1_EW1->SetSelection( 0 );
	bSizer1431121->Add( m_Lon1_EW1, 0, wxALL, 5 );
	
	
	bSizer132121->Add( bSizer1431121, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	sbSizer721->Add( bSizer132121, 0, 0, 5 );
	
	
	bSizer4011->Add( sbSizer721, 0, 0, 5 );
	
	wxBoxSizer* bSizer4811;
	bSizer4811 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button811 = new wxButton( m_panel151, wxID_ANY, _("Convert"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4811->Add( m_button811, 0, wxALL, 5 );
	
	
	bSizer4011->Add( bSizer4811, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	m_panel151->SetSizer( bSizer4011 );
	m_panel151->Layout();
	bSizer4011->Fit( m_panel151 );
	m_wxNotebook234->AddPage( m_panel151, _("Degree, Minute"), false );
	m_panel15 = new wxPanel( m_wxNotebook234, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer401;
	bSizer401 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer72;
#ifdef __WXOSX__
	sbSizer72 = new wxStaticBoxSizer( new wxStaticBox( m_panel15, wxID_ANY, _("DDMMSS.s (no limit to digits)") ), wxVERTICAL );
#else
//	sbSizer72 = new wxStaticBoxSizer( new wxStaticBox( m_panel15, wxID_ANY, _("DD\u00b0MM’SS.s (no limit to digits)") ), wxVERTICAL );
#endif
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
	
	m_staticText33313 = new wxStaticText( m_panel15, wxID_ANY, _T("\u00b0"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticText33313->Wrap( -1 );
	bSizer14313->Add( m_staticText33313, 0, wxALL, 5 );
	
	m_Lat1_m = new wxTextCtrl( m_panel15, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lat1_m->SetMaxLength( 0 ); 
	bSizer14313->Add( m_Lat1_m, 0, wxALL, 5 );
	
	m_staticText1143 = new wxStaticText( m_panel15, wxID_ANY, _T("'"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1143->Wrap( -1 );
	bSizer14313->Add( m_staticText1143, 0, wxALL, 5 );
	
	m_Lat1_s = new wxTextCtrl( m_panel15, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lat1_s->SetMaxLength( 0 ); 
	bSizer14313->Add( m_Lat1_s, 0, wxALL, 5 );
	
	m_staticText1153 = new wxStaticText( m_panel15, wxID_ANY, _T("\""), wxDefaultPosition, wxDefaultSize, 0 );
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
	
	m_staticText333112 = new wxStaticText( m_panel15, wxID_ANY, _T("\u00b0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText333112->Wrap( -1 );
	bSizer143112->Add( m_staticText333112, 0, wxALL, 5 );
	
	m_Lon1_m = new wxTextCtrl( m_panel15, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lon1_m->SetMaxLength( 0 ); 
	bSizer143112->Add( m_Lon1_m, 0, wxALL, 5 );
	
	m_staticText11412 = new wxStaticText( m_panel15, wxID_ANY, _T("'"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11412->Wrap( -1 );
	bSizer143112->Add( m_staticText11412, 0, wxALL, 5 );
	
	m_Lon1_s = new wxTextCtrl( m_panel15, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_Lon1_s->SetMaxLength( 0 ); 
	bSizer143112->Add( m_Lon1_s, 0, wxALL, 5 );
	
	m_staticText11512 = new wxStaticText( m_panel15, wxID_ANY, _T("\""), wxDefaultPosition, wxDefaultSize, 0 );
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
	
	m_button81 = new wxButton( m_panel15, wxID_ANY, _("Convert"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer481->Add( m_button81, 0, wxALL, 5 );
	
	
	bSizer401->Add( bSizer481, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	m_panel15->SetSizer( bSizer401 );
	m_panel15->Layout();
	bSizer401->Fit( m_panel15 );
	m_wxNotebook234->AddPage( m_panel15, _("\u00b0 ' \""), true );
	
	bSizer35->Add( m_wxNotebook234, 0, wxALL, 5 );
	
	
	m_panel14->SetSizer( bSizer35 );
	m_panel14->Layout();
	bSizer35->Fit( m_panel14 );
	bSizer40->Add( m_panel14, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer40 );
	this->Layout();
	bSizer40->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_wxNotebook234->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( MyDialog5::OnNoteBookFit ), NULL, this );
	m_button811->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog5::OnConverttoDegree ), NULL, this );
	m_button81->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog5::OnConverttoDegree ), NULL, this );
}

MyDialog5::~MyDialog5()
{
	// Disconnect Events
	m_wxNotebook234->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( MyDialog5::OnNoteBookFit ), NULL, this );
	m_button811->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog5::OnConverttoDegree ), NULL, this );
	m_button81->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyDialog5::OnConverttoDegree ), NULL, this );
	
}

FunDlgDef::FunDlgDef( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxSize( -1,-1 ) );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_panel152 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer11;
	sbSizer11 = new wxStaticBoxSizer( new wxStaticBox( m_panel152, wxID_ANY, _("Select Function Category") ), wxHORIZONTAL );
	
	wxString m_Function_CategoriesChoices[] = { _("All") };
	int m_Function_CategoriesNChoices = sizeof( m_Function_CategoriesChoices ) / sizeof( wxString );
	m_Function_Categories = new wxChoice( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Function_CategoriesNChoices, m_Function_CategoriesChoices, 0 );
	m_Function_Categories->SetSelection( 0 );
	sbSizer11->Add( m_Function_Categories, 1, wxALL, 5 );
	
	
	bSizer26->Add( sbSizer11, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer12;
	sbSizer12 = new wxStaticBoxSizer( new wxStaticBox( m_panel152, wxID_ANY, _("Select Function") ), wxHORIZONTAL );
	
	wxString m_Function_DropdownChoices[] = { _("All") };
	int m_Function_DropdownNChoices = sizeof( m_Function_DropdownChoices ) / sizeof( wxString );
	m_Function_Dropdown = new wxChoice( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Function_DropdownNChoices, m_Function_DropdownChoices, 0 );
	m_Function_Dropdown->SetSelection( 1 );
	sbSizer12->Add( m_Function_Dropdown, 1, wxALL, 5 );
	
	
	bSizer26->Add( sbSizer12, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer14;
	sbSizer14 = new wxStaticBoxSizer( new wxStaticBox( m_panel152, wxID_ANY, _("Function And Description") ), wxVERTICAL );
	
	m_panel101 = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer113;
	bSizer113 = new wxBoxSizer( wxVERTICAL );
	
	m_Function = new wxStaticText( m_panel101, wxID_ANY, _("ere"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Function->Wrap( 400 );
	bSizer113->Add( m_Function, 0, wxALL, 5 );
	
	m_Description = new wxStaticText( m_panel101, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Description->Wrap( 400 );
	bSizer113->Add( m_Description, 0, wxALL, 5 );
	
	
	m_panel101->SetSizer( bSizer113 );
	m_panel101->Layout();
	bSizer113->Fit( m_panel101 );
	sbSizer14->Add( m_panel101, 0, wxALL, 5 );
	
	
	bSizer26->Add( sbSizer14, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer15;
	sbSizer15 = new wxStaticBoxSizer( new wxStaticBox( m_panel152, wxID_ANY, _("Function Result Select Units") ), wxVERTICAL );
	
	m_panel100 = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer64;
	bSizer64 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Function_Result = new wxTextCtrl( m_panel100, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_Function_Result->SetBackgroundColour( wxColour( 240, 240, 240 ) );
	
	bSizer64->Add( m_Function_Result, 1, wxALL|wxEXPAND, 5 );
	
	m_Output_Parameter = new wxStaticText( m_panel100, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Output_Parameter->Wrap( -1 );
	bSizer64->Add( m_Output_Parameter, 0, wxALL, 5 );
	
	wxArrayString m_Output_Parameter_UnitCChoices;
	m_Output_Parameter_UnitC = new wxChoice( m_panel100, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Output_Parameter_UnitCChoices, 0 );
	m_Output_Parameter_UnitC->SetSelection( 0 );
	bSizer64->Add( m_Output_Parameter_UnitC, 0, wxALL, 5 );
	
	
	m_panel100->SetSizer( bSizer64 );
	m_panel100->Layout();
	bSizer64->Fit( m_panel100 );
	sbSizer15->Add( m_panel100, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer26->Add( sbSizer15, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer13;
	sbSizer13 = new wxStaticBoxSizer( new wxStaticBox( m_panel152, wxID_ANY, _("Input Parameters") ), wxVERTICAL );
	
	m_panel = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer = new wxBoxSizer( wxHORIZONTAL );
	
	Value = new wxTextCtrl( m_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer->Add( Value, 0, wxALL, 5 );
	
	Parameter = new wxStaticText( m_panel, wxID_ANY, _("Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
	Parameter->Wrap( -1 );
	bSizer->Add( Parameter, 0, wxALL, 5 );
	
	wxArrayString UnitsChoices;
	Units = new wxChoice( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, UnitsChoices, 0 );
	Units->SetSelection( 0 );
	bSizer->Add( Units, 0, wxALL, 5 );
	
	
	m_panel->SetSizer( bSizer );
	m_panel->Layout();
	bSizer->Fit( m_panel );
	sbSizer13->Add( m_panel, 0, wxALL, 5 );
	
	m_panel1 = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	Value1 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( Value1, 0, wxALL, 5 );
	
	Parameter1 = new wxStaticText( m_panel1, wxID_ANY, _("Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
	Parameter1->Wrap( -1 );
	bSizer1->Add( Parameter1, 0, wxALL, 5 );
	
	wxArrayString Units1Choices;
	Units1 = new wxChoice( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, Units1Choices, 0 );
	Units1->SetSelection( 0 );
	bSizer1->Add( Units1, 0, wxALL, 5 );
	
	
	m_panel1->SetSizer( bSizer1 );
	m_panel1->Layout();
	bSizer1->Fit( m_panel1 );
	sbSizer13->Add( m_panel1, 1, wxEXPAND | wxALL, 5 );
	
	m_panel2 = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	Value2 = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( Value2, 0, wxALL, 5 );
	
	Parameter2 = new wxStaticText( m_panel2, wxID_ANY, _("Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
	Parameter2->Wrap( -1 );
	bSizer2->Add( Parameter2, 0, wxALL, 5 );
	
	wxArrayString Units2Choices;
	Units2 = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, Units2Choices, 0 );
	Units2->SetSelection( 0 );
	bSizer2->Add( Units2, 0, wxALL, 5 );
	
	
	m_panel2->SetSizer( bSizer2 );
	m_panel2->Layout();
	bSizer2->Fit( m_panel2 );
	sbSizer13->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );
	
	m_panel3 = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	Value3 = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( Value3, 0, wxALL, 5 );
	
	Parameter3 = new wxStaticText( m_panel3, wxID_ANY, _("Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
	Parameter3->Wrap( -1 );
	bSizer3->Add( Parameter3, 0, wxALL, 5 );
	
	wxArrayString Units3Choices;
	Units3 = new wxChoice( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, Units3Choices, 0 );
	Units3->SetSelection( 0 );
	bSizer3->Add( Units3, 0, wxALL, 5 );
	
	
	m_panel3->SetSizer( bSizer3 );
	m_panel3->Layout();
	bSizer3->Fit( m_panel3 );
	sbSizer13->Add( m_panel3, 1, wxEXPAND | wxALL, 5 );
	
	m_panel4 = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	Value4 = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( Value4, 0, wxALL, 5 );
	
	Parameter4 = new wxStaticText( m_panel4, wxID_ANY, _("Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
	Parameter4->Wrap( -1 );
	bSizer4->Add( Parameter4, 0, wxALL, 5 );
	
	wxArrayString Units4Choices;
	Units4 = new wxChoice( m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, Units4Choices, 0 );
	Units4->SetSelection( 0 );
	bSizer4->Add( Units4, 0, wxALL, 5 );
	
	
	m_panel4->SetSizer( bSizer4 );
	m_panel4->Layout();
	bSizer4->Fit( m_panel4 );
	sbSizer13->Add( m_panel4, 1, wxEXPAND | wxALL, 5 );
	
	m_panel5 = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	Value5 = new wxTextCtrl( m_panel5, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( Value5, 0, wxALL, 5 );
	
	Parameter5 = new wxStaticText( m_panel5, wxID_ANY, _("Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
	Parameter5->Wrap( -1 );
	bSizer5->Add( Parameter5, 0, wxALL, 5 );
	
	wxArrayString Units5Choices;
	Units5 = new wxChoice( m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, Units5Choices, 0 );
	Units5->SetSelection( 0 );
	bSizer5->Add( Units5, 0, wxALL, 5 );
	
	
	m_panel5->SetSizer( bSizer5 );
	m_panel5->Layout();
	bSizer5->Fit( m_panel5 );
	sbSizer13->Add( m_panel5, 1, wxEXPAND | wxALL, 5 );
	
	m_panel6 = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	Value6 = new wxTextCtrl( m_panel6, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( Value6, 0, wxALL, 5 );
	
	Parameter6 = new wxStaticText( m_panel6, wxID_ANY, _("Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
	Parameter6->Wrap( -1 );
	bSizer6->Add( Parameter6, 0, wxALL, 5 );
	
	wxArrayString Units6Choices;
	Units6 = new wxChoice( m_panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, Units6Choices, 0 );
	Units6->SetSelection( 0 );
	bSizer6->Add( Units6, 0, wxALL, 5 );
	
	
	m_panel6->SetSizer( bSizer6 );
	m_panel6->Layout();
	bSizer6->Fit( m_panel6 );
	sbSizer13->Add( m_panel6, 1, wxEXPAND | wxALL, 5 );
	
	m_panel7 = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	Value7 = new wxTextCtrl( m_panel7, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( Value7, 0, wxALL, 5 );
	
	Parameter7 = new wxStaticText( m_panel7, wxID_ANY, _("Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
	Parameter7->Wrap( -1 );
	bSizer8->Add( Parameter7, 0, wxALL, 5 );
	
	wxArrayString Units7Choices;
	Units7 = new wxChoice( m_panel7, wxID_ANY, wxDefaultPosition, wxDefaultSize, Units7Choices, 0 );
	Units7->SetSelection( 0 );
	bSizer8->Add( Units7, 0, wxALL, 5 );
	
	
	m_panel7->SetSizer( bSizer8 );
	m_panel7->Layout();
	bSizer8->Fit( m_panel7 );
	sbSizer13->Add( m_panel7, 1, wxEXPAND | wxALL, 5 );
	
	m_panel8 = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	Value8 = new wxTextCtrl( m_panel8, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( Value8, 0, wxALL, 5 );
	
	Parameter8 = new wxStaticText( m_panel8, wxID_ANY, _("Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
	Parameter8->Wrap( -1 );
	bSizer9->Add( Parameter8, 0, wxALL, 5 );
	
	wxArrayString Units8Choices;
	Units8 = new wxChoice( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, Units8Choices, 0 );
	Units8->SetSelection( 0 );
	bSizer9->Add( Units8, 0, wxALL, 5 );
	
	
	m_panel8->SetSizer( bSizer9 );
	m_panel8->Layout();
	bSizer9->Fit( m_panel8 );
	sbSizer13->Add( m_panel8, 1, wxEXPAND | wxALL, 5 );
	
	m_panel9 = new wxPanel( m_panel152, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	Value9 = new wxTextCtrl( m_panel9, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( Value9, 0, wxALL, 5 );
	
	Parameter9 = new wxStaticText( m_panel9, wxID_ANY, _("Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
	Parameter9->Wrap( -1 );
	bSizer10->Add( Parameter9, 0, wxALL, 5 );
	
	wxArrayString Units9Choices;
	Units9 = new wxChoice( m_panel9, wxID_ANY, wxDefaultPosition, wxDefaultSize, Units9Choices, 0 );
	Units9->SetSelection( 0 );
	bSizer10->Add( Units9, 0, wxALL, 5 );
	
	
	m_panel9->SetSizer( bSizer10 );
	m_panel9->Layout();
	bSizer10->Fit( m_panel9 );
	sbSizer13->Add( m_panel9, 1, wxEXPAND | wxALL, 5 );
	
	
	bSizer26->Add( sbSizer13, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button7 = new wxButton( m_panel152, wxID_ANY, _("Calculate"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer39->Add( m_button7, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_button10 = new wxButton( m_panel152, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer39->Add( m_button10, 0, wxALL, 5 );
	
	m_checkBox8 = new wxCheckBox( m_panel152, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox8->SetValue(true); 
	bSizer39->Add( m_checkBox8, 0, wxALL, 5 );
	
	
	bSizer26->Add( bSizer39, 0, wxEXPAND, 5 );
	
	
	m_panel152->SetSizer( bSizer26 );
	m_panel152->Layout();
	bSizer26->Fit( m_panel152 );
	bSizer7->Add( m_panel152, 1, wxEXPAND | wxALL, 5 );
	
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_Function_Categories->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( FunDlgDef::OnCategorySelect ), NULL, this );
	m_Function_Dropdown->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( FunDlgDef::OnItemSelect ), NULL, this );
	m_button7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FunDlgDef::OnExtraCalculate ), NULL, this );
	m_button10->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FunDlgDef::OnClose ), NULL, this );
	m_checkBox8->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FunDlgDef::OnToggle ), NULL, this );
}

FunDlgDef::~FunDlgDef()
{
	// Disconnect Events
	m_Function_Categories->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( FunDlgDef::OnCategorySelect ), NULL, this );
	m_Function_Dropdown->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( FunDlgDef::OnItemSelect ), NULL, this );
	m_button7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FunDlgDef::OnExtraCalculate ), NULL, this );
	m_button10->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FunDlgDef::OnClose ), NULL, this );
	m_checkBox8->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FunDlgDef::OnToggle ), NULL, this );
	
}

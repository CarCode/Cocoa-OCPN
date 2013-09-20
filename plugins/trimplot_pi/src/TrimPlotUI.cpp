///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "TrimPlotUI.h"

///////////////////////////////////////////////////////////////////////////

TrimPlotDialogBase::TrimPlotDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxSize( -1,-1 ) );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer8->AddGrowableCol( 0 );
	fgSizer8->AddGrowableRow( 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer14->AddGrowableCol( 0 );
	fgSizer14->AddGrowableRow( 0 );
	fgSizer14->AddGrowableRow( 1 );
	fgSizer14->SetFlexibleDirection( wxBOTH );
	fgSizer14->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_swSpeed = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_swSpeed->SetScrollRate( 5, 5 );
	fgSizer14->Add( m_swSpeed, 1, wxEXPAND | wxALL, 5 );
	
	m_stSpeed = new wxStaticText( this, wxID_ANY, _("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
	m_stSpeed->Wrap( -1 );
	fgSizer14->Add( m_stSpeed, 0, wxALL, 5 );
	
	m_swCourse = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_swCourse->SetScrollRate( 5, 5 );
	fgSizer14->Add( m_swCourse, 1, wxEXPAND | wxALL, 5 );
	
	m_stCourse = new wxStaticText( this, wxID_ANY, _("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
	m_stCourse->Wrap( -1 );
	fgSizer14->Add( m_stCourse, 0, wxALL, 5 );
	
	m_staticText16 = new wxStaticText( this, wxID_ANY, _("Speed via Position"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer14->Add( m_staticText16, 0, wxALL, 5 );
	
	m_stPositionSpeed = new wxStaticText( this, wxID_ANY, _("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
	m_stPositionSpeed->Wrap( -1 );
	fgSizer14->Add( m_stPositionSpeed, 0, wxALL, 5 );
	
	m_staticText18 = new wxStaticText( this, wxID_ANY, _("Speed Percentage vs Straight course"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	fgSizer14->Add( m_staticText18, 0, wxALL, 5 );
	
	m_stSpeedPercentage = new wxStaticText( this, wxID_ANY, _("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
	m_stSpeedPercentage->Wrap( -1 );
	fgSizer14->Add( m_stSpeedPercentage, 0, wxALL, 5 );
	
	
	fgSizer8->Add( fgSizer14, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( fgSizer8 );
	this->Layout();
	fgSizer8->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( TrimPlotDialogBase::OnSize ) );
	m_swSpeed->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( TrimPlotDialogBase::OnDoubleClick ), NULL, this );
	m_swSpeed->Connect( wxEVT_PAINT, wxPaintEventHandler( TrimPlotDialogBase::OnPaint ), NULL, this );
	m_swCourse->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( TrimPlotDialogBase::OnDoubleClick ), NULL, this );
	m_swCourse->Connect( wxEVT_PAINT, wxPaintEventHandler( TrimPlotDialogBase::OnPaint ), NULL, this );
}

TrimPlotDialogBase::~TrimPlotDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( TrimPlotDialogBase::OnSize ) );
	m_swSpeed->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( TrimPlotDialogBase::OnDoubleClick ), NULL, this );
	m_swSpeed->Disconnect( wxEVT_PAINT, wxPaintEventHandler( TrimPlotDialogBase::OnPaint ), NULL, this );
	m_swCourse->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( TrimPlotDialogBase::OnDoubleClick ), NULL, this );
	m_swCourse->Disconnect( wxEVT_PAINT, wxPaintEventHandler( TrimPlotDialogBase::OnPaint ), NULL, this );
	
}

PreferencesDialogBase::PreferencesDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Speed") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_cbSpeed = new wxCheckBox( this, wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbSpeed->SetValue(true); 
	fgSizer10->Add( m_cbSpeed, 0, wxALL, 5 );
	
	
	fgSizer10->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer10->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText14 = new wxStaticText( this, wxID_ANY, _("Scale"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer10->Add( m_staticText14, 0, wxALL, 5 );
	
	m_tSpeedScale = new wxTextCtrl( this, wxID_ANY, _("3"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_tSpeedScale, 0, wxALL, 5 );
	
	m_staticText15 = new wxStaticText( this, wxID_ANY, _("knots"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer10->Add( m_staticText15, 0, wxALL, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, _("Using"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer10->Add( m_staticText7, 0, wxALL, 5 );
	
	m_sSpeedSeconds = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 10 );
	fgSizer10->Add( m_sSpeedSeconds, 0, wxALL, 5 );
	
	m_staticText23 = new wxStaticText( this, wxID_ANY, _("seconds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer10->Add( m_staticText23, 0, wxALL, 5 );
	
	
	sbSizer6->Add( fgSizer10, 1, wxEXPAND, 5 );
	
	
	fgSizer4->Add( sbSizer6, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Course") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer102;
	fgSizer102 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer102->SetFlexibleDirection( wxBOTH );
	fgSizer102->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_cbCourse = new wxCheckBox( this, wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbCourse->SetValue(true); 
	fgSizer102->Add( m_cbCourse, 0, wxALL, 5 );
	
	
	fgSizer102->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer102->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText141 = new wxStaticText( this, wxID_ANY, _("Scale"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText141->Wrap( -1 );
	fgSizer102->Add( m_staticText141, 0, wxALL, 5 );
	
	m_tCourseScale = new wxTextCtrl( this, wxID_ANY, _("20"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer102->Add( m_tCourseScale, 0, wxALL, 5 );
	
	m_staticText151 = new wxStaticText( this, wxID_ANY, _("degrees"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText151->Wrap( -1 );
	fgSizer102->Add( m_staticText151, 0, wxALL, 5 );
	
	m_staticText72 = new wxStaticText( this, wxID_ANY, _("Using"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText72->Wrap( -1 );
	fgSizer102->Add( m_staticText72, 0, wxALL, 5 );
	
	m_sCourseSeconds = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 10 );
	fgSizer102->Add( m_sCourseSeconds, 0, wxALL, 5 );
	
	m_staticText231 = new wxStaticText( this, wxID_ANY, _("seconds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText231->Wrap( -1 );
	fgSizer102->Add( m_staticText231, 0, wxALL, 5 );
	
	
	sbSizer2->Add( fgSizer102, 1, wxEXPAND, 5 );
	
	
	fgSizer4->Add( sbSizer2, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer61;
	sbSizer61 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Course Prediction Display") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1021;
	fgSizer1021 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer1021->SetFlexibleDirection( wxBOTH );
	fgSizer1021->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_cbCoursePrediction = new wxCheckBox( this, wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbCoursePrediction->SetValue(true); 
	fgSizer1021->Add( m_cbCoursePrediction, 0, wxALL, 5 );
	
	
	fgSizer1021->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer1021->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText1411 = new wxStaticText( this, wxID_ANY, _("Length"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1411->Wrap( -1 );
	fgSizer1021->Add( m_staticText1411, 0, wxALL, 5 );
	
	m_sCoursePredictionLength = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 60, 1000000, 0 );
	fgSizer1021->Add( m_sCoursePredictionLength, 0, wxALL, 5 );
	
	m_staticText1511 = new wxStaticText( this, wxID_ANY, _("minutes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1511->Wrap( -1 );
	fgSizer1021->Add( m_staticText1511, 0, wxALL, 5 );
	
	m_staticText721 = new wxStaticText( this, wxID_ANY, _("Using"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText721->Wrap( -1 );
	fgSizer1021->Add( m_staticText721, 0, wxALL, 5 );
	
	m_sCoursePredictionSeconds = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 10 );
	fgSizer1021->Add( m_sCoursePredictionSeconds, 0, wxALL, 5 );
	
	m_staticText2311 = new wxStaticText( this, wxID_ANY, _("seconds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2311->Wrap( -1 );
	fgSizer1021->Add( m_staticText2311, 0, wxALL, 5 );
	
	
	sbSizer61->Add( fgSizer1021, 1, wxEXPAND, 5 );
	
	
	fgSizer4->Add( sbSizer61, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_bAbout = new wxButton( this, wxID_ANY, _("About"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_bAbout, 0, wxALL, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1->Realize();
	
	fgSizer11->Add( m_sdbSizer1, 0, wxBOTTOM|wxEXPAND|wxTOP, 5 );
	
	
	fgSizer4->Add( fgSizer11, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( fgSizer4 );
	this->Layout();
	fgSizer4->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_cbSpeed->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PreferencesDialogBase::OnPlotChange ), NULL, this );
	m_cbCourse->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PreferencesDialogBase::OnPlotChange ), NULL, this );
	m_bAbout->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesDialogBase::OnAbout ), NULL, this );
}

PreferencesDialogBase::~PreferencesDialogBase()
{
	// Disconnect Events
	m_cbSpeed->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PreferencesDialogBase::OnPlotChange ), NULL, this );
	m_cbCourse->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PreferencesDialogBase::OnPlotChange ), NULL, this );
	m_bAbout->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesDialogBase::OnAbout ), NULL, this );
	
}

AboutDialog::AboutDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer90;
	fgSizer90 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer90->SetFlexibleDirection( wxBOTH );
	fgSizer90->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText110 = new wxStaticText( this, wxID_ANY, _("The trimplot plugin for opencpn is\ndesigned to monitor speed and course to make the results of changes to sail trim obvious. \n\nFor example, tightening a vang or adjusting a traveler may produce such a slight result it is difficult to gauge the outcome without feedback.\n\nThe predictor line allows setting the sample time for smoother prediction than the builtin predictor.\n\nLicense: GPLv3+\n\nSource Code:\nhttps://github.com/seandepagnier/trimplot_pi\n\nAuthor:\nSean D'Epagnier\n\nMany thanks to all of the translators and testers."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText110->Wrap( 400 );
	fgSizer90->Add( m_staticText110, 0, wxALL, 5 );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2->Realize();
	
	fgSizer90->Add( m_sdbSizer2, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( fgSizer90 );
	this->Layout();
	fgSizer90->Fit( this );
	
	this->Centre( wxBOTH );
}

AboutDialog::~AboutDialog()
{
}

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ODPropertiesDialogDef.h"

///////////////////////////////////////////////////////////////////////////

MyPanel1::MyPanel1( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
}

MyPanel1::~MyPanel1()
{
}

ODPropertiesDialogDef::ODPropertiesDialogDef( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_SizerProperties = new wxBoxSizer( wxVERTICAL );
	
	m_notebookProperties = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panelGeneral = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerGeneral;
	bSizerGeneral = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxConfirmObjectDelete = new wxCheckBox( m_panelGeneral, wxID_ANY, _("Confirm Object Delete"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_checkBoxConfirmObjectDelete->SetValue(true); 
	fgSizer4->Add( m_checkBoxConfirmObjectDelete, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	
	fgSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_checkBoxShowMagBearings = new wxCheckBox( m_panelGeneral, wxID_ANY, _("Show Magnetic Bearings"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	fgSizer4->Add( m_checkBoxShowMagBearings, 0, wxALL, 5 );
	
	
	fgSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextNavObjBackups = new wxStaticText( m_panelGeneral, wxID_ANY, _("Nav Object Backups"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextNavObjBackups->Wrap( -1 );
	fgSizer4->Add( m_staticTextNavObjBackups, 0, wxALL, 5 );
	
	m_spinCtrlNavObjBackups = new wxSpinCtrl( m_panelGeneral, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 5 );
	fgSizer4->Add( m_spinCtrlNavObjBackups, 0, wxALL, 5 );
	
	m_staticTextEdgePan = new wxStaticText( m_panelGeneral, wxID_ANY, _("Edge Panning Sensitivity\n(percentage screen width) Default 5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextEdgePan->Wrap( -1 );
	fgSizer4->Add( m_staticTextEdgePan, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_sliderEdgePan = new wxSlider( m_panelGeneral, wxID_ANY, 5, 0, 10, wxDefaultPosition, wxSize( -1,-1 ), wxSL_HORIZONTAL|wxSL_LABELS );
	fgSizer4->Add( m_sliderEdgePan, 0, wxALL|wxEXPAND, 5 );
	
	m_staticTextInitialEdgePan = new wxStaticText( m_panelGeneral, wxID_ANY, _("Initial Edge Panning Sensitivity\n(percentage screen width) Default 3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInitialEdgePan->Wrap( -1 );
	fgSizer4->Add( m_staticTextInitialEdgePan, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_sliderInitialEdgePan = new wxSlider( m_panelGeneral, wxID_ANY, 3, 0, 10, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	fgSizer4->Add( m_sliderInitialEdgePan, 0, wxALL|wxEXPAND, 5 );
	
	m_staticTextToolbar = new wxStaticText( m_panelGeneral, wxID_ANY, _("Display Toolbar"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextToolbar->Wrap( -1 );
	fgSizer4->Add( m_staticTextToolbar, 0, wxALL, 5 );
	
	wxString m_choiceToolbarChoices[] = { _("Never"), _("Whilst Drawing"), _("Always") };
	int m_choiceToolbarNChoices = sizeof( m_choiceToolbarChoices ) / sizeof( wxString );
	m_choiceToolbar = new wxChoice( m_panelGeneral, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceToolbarNChoices, m_choiceToolbarChoices, 0 );
	m_choiceToolbar->SetSelection( 1 );
	fgSizer4->Add( m_choiceToolbar, 0, wxALL, 5 );
	
	
	bSizerGeneral->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	
	m_panelGeneral->SetSizer( bSizerGeneral );
	m_panelGeneral->Layout();
	bSizerGeneral->Fit( m_panelGeneral );
	m_notebookProperties->AddPage( m_panelGeneral, _("General"), true );
	m_panelBoundary = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizerBoundarySettings;
	fgSizerBoundarySettings = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizerBoundarySettings->SetFlexibleDirection( wxVERTICAL );
	fgSizerBoundarySettings->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizerBoundarySettingsColours;
	fgSizerBoundarySettingsColours = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizerBoundarySettingsColours->SetFlexibleDirection( wxBOTH );
	fgSizerBoundarySettingsColours->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextActiveBoundayLineColour = new wxStaticText( m_panelBoundary, wxID_ANY, _("Active Line Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextActiveBoundayLineColour->Wrap( -1 );
	fgSizerBoundarySettingsColours->Add( m_staticTextActiveBoundayLineColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_colourPickerActiveBoundaryLineColour = new wxColourPickerCtrl( m_panelBoundary, wxID_ANY, wxColour( 255, 0, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerBoundarySettingsColours->Add( m_colourPickerActiveBoundaryLineColour, 0, wxALL, 5 );
	
	m_staticTextActiveBoundaryFillColour = new wxStaticText( m_panelBoundary, wxID_ANY, _("Active Fill Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextActiveBoundaryFillColour->Wrap( -1 );
	fgSizerBoundarySettingsColours->Add( m_staticTextActiveBoundaryFillColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_colourPickerActiveBoundaryFillColour = new wxColourPickerCtrl( m_panelBoundary, wxID_ANY, wxColour( 255, 0, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerBoundarySettingsColours->Add( m_colourPickerActiveBoundaryFillColour, 0, wxALL, 5 );
	
	m_staticTextInactiveBoundaryLineColour = new wxStaticText( m_panelBoundary, wxID_ANY, _("Inactive Line Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInactiveBoundaryLineColour->Wrap( -1 );
	fgSizerBoundarySettingsColours->Add( m_staticTextInactiveBoundaryLineColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_colourPickerInActiveBoundaryLineColour = new wxColourPickerCtrl( m_panelBoundary, wxID_ANY, wxColour( 214, 214, 214 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerBoundarySettingsColours->Add( m_colourPickerInActiveBoundaryLineColour, 0, wxALL, 5 );
	
	m_staticTextInactiveBoundaryFillColour = new wxStaticText( m_panelBoundary, wxID_ANY, _("Inactive Fill Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInactiveBoundaryFillColour->Wrap( -1 );
	fgSizerBoundarySettingsColours->Add( m_staticTextInactiveBoundaryFillColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_colourPickerInActiveBoundaryFillColour = new wxColourPickerCtrl( m_panelBoundary, wxID_ANY, wxColour( 214, 214, 214 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerBoundarySettingsColours->Add( m_colourPickerInActiveBoundaryFillColour, 0, wxALL, 5 );
	
	m_staticTextBoundaryLineWidth = new wxStaticText( m_panelBoundary, wxID_ANY, _("Line Width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextBoundaryLineWidth->Wrap( -1 );
	fgSizerBoundarySettingsColours->Add( m_staticTextBoundaryLineWidth, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceBoundaryLineWidthChoices[] = { _("1 Pixel"), _("2 Pixels"), _("3 Pixels"), _("4 Pixels"), _("5 Pixels"), _("6 Pixels"), _("7 Pixels"), _("8 Pixels"), _("9 Pixels"), _("10 Pixels") };
	int m_choiceBoundaryLineWidthNChoices = sizeof( m_choiceBoundaryLineWidthChoices ) / sizeof( wxString );
	m_choiceBoundaryLineWidth = new wxChoice( m_panelBoundary, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceBoundaryLineWidthNChoices, m_choiceBoundaryLineWidthChoices, 0 );
	m_choiceBoundaryLineWidth->SetSelection( 0 );
	fgSizerBoundarySettingsColours->Add( m_choiceBoundaryLineWidth, 0, wxALL, 5 );
	
	m_staticTextBoundaryLineStyle = new wxStaticText( m_panelBoundary, wxID_ANY, _("Line Style"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextBoundaryLineStyle->Wrap( -1 );
	fgSizerBoundarySettingsColours->Add( m_staticTextBoundaryLineStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceBoundaryLineStyleChoices[] = { _("Solid"), _("Dot"), _("Long Dash"), _("Short Dash"), _("Dot Dash") };
	int m_choiceBoundaryLineStyleNChoices = sizeof( m_choiceBoundaryLineStyleChoices ) / sizeof( wxString );
	m_choiceBoundaryLineStyle = new wxChoice( m_panelBoundary, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceBoundaryLineStyleNChoices, m_choiceBoundaryLineStyleChoices, 0 );
	m_choiceBoundaryLineStyle->SetSelection( 0 );
	fgSizerBoundarySettingsColours->Add( m_choiceBoundaryLineStyle, 0, wxALL, 5 );
	
	m_staticTextFillTransparency = new wxStaticText( m_panelBoundary, wxID_ANY, _("Fill Density"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFillTransparency->Wrap( -1 );
	fgSizerBoundarySettingsColours->Add( m_staticTextFillTransparency, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_sliderFillTransparency = new wxSlider( m_panelBoundary, wxID_ANY, 175, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	fgSizerBoundarySettingsColours->Add( m_sliderFillTransparency, 0, wxALL|wxEXPAND, 5 );
	
	m_staticTextInclusionBoundarySize = new wxStaticText( m_panelBoundary, wxID_ANY, _("Inclusion Boundary Size"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInclusionBoundarySize->Wrap( -1 );
	fgSizerBoundarySettingsColours->Add( m_staticTextInclusionBoundarySize, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_sliderInclusionBoundarySize = new wxSlider( m_panelBoundary, wxID_ANY, 15, 0, 50, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	fgSizerBoundarySettingsColours->Add( m_sliderInclusionBoundarySize, 1, wxALL|wxEXPAND, 5 );
	
	
	fgSizerBoundarySettings->Add( fgSizerBoundarySettingsColours, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizerBoundaryType;
	fgSizerBoundaryType = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizerBoundaryType->SetFlexibleDirection( wxBOTH );
	fgSizerBoundaryType->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxString m_radioBoxBoundaryTypeChoices[] = { _("Exclusion"), _("Inclusion"), _("Neither") };
	int m_radioBoxBoundaryTypeNChoices = sizeof( m_radioBoxBoundaryTypeChoices ) / sizeof( wxString );
	m_radioBoxBoundaryType = new wxRadioBox( m_panelBoundary, wxID_ANY, _("Boundary Type"), wxDefaultPosition, wxDefaultSize, m_radioBoxBoundaryTypeNChoices, m_radioBoxBoundaryTypeChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxBoundaryType->SetSelection( 0 );
	fgSizerBoundaryType->Add( m_radioBoxBoundaryType, 0, wxALL, 5 );
	
	
	fgSizerBoundarySettings->Add( fgSizerBoundaryType, 1, wxEXPAND, 5 );
	
	
	m_panelBoundary->SetSizer( fgSizerBoundarySettings );
	m_panelBoundary->Layout();
	fgSizerBoundarySettings->Fit( m_panelBoundary );
	m_notebookProperties->AddPage( m_panelBoundary, _("Boundary"), false );
	m_panelBoundaryPoint = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerOCPNPoint;
	bSizerOCPNPoint = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerArrivalRadius;
	bSizerArrivalRadius = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTextArrivalRadius = new wxStaticText( m_panelBoundaryPoint, wxID_ANY, _("Arrival Radius"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextArrivalRadius->Wrap( -1 );
#ifdef __WXOSX__
    bSizerArrivalRadius->Add( m_staticTextArrivalRadius, 1, wxALL|wxEXPAND, 5 );
#else
	bSizerArrivalRadius->Add( m_staticTextArrivalRadius, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
#endif
	m_textCtrlODPointArrivalRadius = new wxTextCtrl( m_panelBoundaryPoint, wxID_ANY, _("0.5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlODPointArrivalRadius->SetMaxLength( 0 );
#ifdef __WXOSX__
    bSizerArrivalRadius->Add( m_textCtrlODPointArrivalRadius, 0, wxALL, 5 );
#else
	bSizerArrivalRadius->Add( m_textCtrlODPointArrivalRadius, 0, wxALIGN_RIGHT|wxALL, 5 );
#endif
	
	bSizerOCPNPoint->Add( bSizerArrivalRadius, 0, wxEXPAND, 5 );
	
	m_SizerNameIcon = new wxBoxSizer( wxHORIZONTAL );
	
	m_checkBoxShowName = new wxCheckBox( m_panelBoundaryPoint, wxID_ANY, _("Show Name"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
#ifdef __WXOSX__
    m_SizerNameIcon->Add( m_checkBoxShowName, 0, wxALL|wxEXPAND, 5 );
#else
	m_SizerNameIcon->Add( m_checkBoxShowName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
#endif
	m_staticTextIcon = new wxStaticText( m_panelBoundaryPoint, wxID_ANY, _("Icon"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextIcon->Wrap( -1 );
#ifdef __WXOSX__
    m_SizerNameIcon->Add( m_staticTextIcon, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
#else
	m_SizerNameIcon->Add( m_staticTextIcon, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
#endif
	m_bitmapPointBitmap = new wxStaticBitmap( m_panelBoundaryPoint, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_SizerNameIcon->Add( m_bitmapPointBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_bcomboBoxODPointIconName = new wxBitmapComboBox( m_panelBoundaryPoint, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
#ifdef __WXOSX__
    m_SizerNameIcon->Add( m_bcomboBoxODPointIconName, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
#else
	m_SizerNameIcon->Add( m_bcomboBoxODPointIconName, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL|wxEXPAND, 5 );
#endif
	
	bSizerOCPNPoint->Add( m_SizerNameIcon, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizerRingsShow;
	fgSizerRingsShow = new wxFlexGridSizer( 5, 2, 0, 0 );
	fgSizerRingsShow->AddGrowableCol( 0 );
	fgSizerRingsShow->AddGrowableCol( 1 );
	fgSizerRingsShow->SetFlexibleDirection( wxBOTH );
	fgSizerRingsShow->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxShowODPointRangeRings = new wxCheckBox( m_panelBoundaryPoint, wxID_ANY, _("Show Range Rings"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	fgSizerRingsShow->Add( m_checkBoxShowODPointRangeRings, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_radioBoxBoundaryPointTypeChoices[] = { _("Exclusion"), _("Inclusion"), _("Neither") };
	int m_radioBoxBoundaryPointTypeNChoices = sizeof( m_radioBoxBoundaryPointTypeChoices ) / sizeof( wxString );
	m_radioBoxBoundaryPointType = new wxRadioBox( m_panelBoundaryPoint, wxID_ANY, _("Boundary Point Type"), wxDefaultPosition, wxDefaultSize, m_radioBoxBoundaryPointTypeNChoices, m_radioBoxBoundaryPointTypeChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxBoundaryPointType->SetSelection( 0 );
	fgSizerRingsShow->Add( m_radioBoxBoundaryPointType, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_fgSizerBoundaryPointFillDensity = new wxFlexGridSizer( 0, 2, 0, 0 );
	m_fgSizerBoundaryPointFillDensity->AddGrowableCol( 0 );
	m_fgSizerBoundaryPointFillDensity->AddGrowableCol( 1 );
	m_fgSizerBoundaryPointFillDensity->SetFlexibleDirection( wxBOTH );
	m_fgSizerBoundaryPointFillDensity->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextBoundaryPointFillDensity = new wxStaticText( m_panelBoundaryPoint, wxID_ANY, _("Fill Density"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextBoundaryPointFillDensity->Wrap( -1 );
	m_fgSizerBoundaryPointFillDensity->Add( m_staticTextBoundaryPointFillDensity, 0, wxALL|wxEXPAND, 5 );
	
	m_sliderBoundaryPointFillTransparency = new wxSlider( m_panelBoundaryPoint, wxID_ANY, 175, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	m_fgSizerBoundaryPointFillDensity->Add( m_sliderBoundaryPointFillTransparency, 1, wxEXPAND, 5 );
	
	
	fgSizerRingsShow->Add( m_fgSizerBoundaryPointFillDensity, 1, wxEXPAND|wxRIGHT, 5 );
	
	m_fgSizerBoundaryPointSize = new wxFlexGridSizer( 0, 2, 0, 0 );
	m_fgSizerBoundaryPointSize->AddGrowableCol( 0 );
	m_fgSizerBoundaryPointSize->AddGrowableCol( 1 );
	m_fgSizerBoundaryPointSize->SetFlexibleDirection( wxBOTH );
	m_fgSizerBoundaryPointSize->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextBoundaryPointSize = new wxStaticText( m_panelBoundaryPoint, wxID_ANY, _("Boundary Point Inclusion\nSize"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextBoundaryPointSize->Wrap( -1 );
	m_fgSizerBoundaryPointSize->Add( m_staticTextBoundaryPointSize, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_sliderInclusionBoundaryPointSize = new wxSlider( m_panelBoundaryPoint, wxID_ANY, 15, 0, 50, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	m_fgSizerBoundaryPointSize->Add( m_sliderInclusionBoundaryPointSize, 1, wxALL|wxEXPAND, 5 );
	
	
	fgSizerRingsShow->Add( m_fgSizerBoundaryPointSize, 1, wxEXPAND, 5 );
	
	
	fgSizerRingsShow->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	bSizerOCPNPoint->Add( fgSizerRingsShow, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizerRingsDetail;
	fgSizerRingsDetail = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizerRingsDetail->AddGrowableCol( 0 );
	fgSizerRingsDetail->SetFlexibleDirection( wxBOTH );
	fgSizerRingsDetail->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextRangeRingNumber = new wxStaticText( m_panelBoundaryPoint, wxID_ANY, _("Number of Range Rings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextRangeRingNumber->Wrap( -1 );
	fgSizerRingsDetail->Add( m_staticTextRangeRingNumber, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceODPointRangeRingNumberChoices[] = { _("None"), _("1"), _("2"), _("3"), _("4"), _("5"), _("6"), _("7"), _("8"), _("9"), _("10") };
	int m_choiceODPointRangeRingNumberNChoices = sizeof( m_choiceODPointRangeRingNumberChoices ) / sizeof( wxString );
	m_choiceODPointRangeRingNumber = new wxChoice( m_panelBoundaryPoint, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceODPointRangeRingNumberNChoices, m_choiceODPointRangeRingNumberChoices, 0 );
	m_choiceODPointRangeRingNumber->SetSelection( 0 );
#ifdef __WXOSX__
    fgSizerRingsDetail->Add( m_choiceODPointRangeRingNumber, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
#else
	fgSizerRingsDetail->Add( m_choiceODPointRangeRingNumber, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
#endif
	m_staticTextRangeRingSteps = new wxStaticText( m_panelBoundaryPoint, wxID_ANY, _("Distance Between Range Rings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextRangeRingSteps->Wrap( -1 );
	fgSizerRingsDetail->Add( m_staticTextRangeRingSteps, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlODPointRangeRingSteps = new wxTextCtrl( m_panelBoundaryPoint, wxID_ANY, _("0.5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlODPointRangeRingSteps->SetMaxLength( 0 );
#ifdef __WXOSX__
    fgSizerRingsDetail->Add( m_textCtrlODPointRangeRingSteps, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
#else
	fgSizerRingsDetail->Add( m_textCtrlODPointRangeRingSteps, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
#endif
	m_staticTextDistanceUnit = new wxStaticText( m_panelBoundaryPoint, wxID_ANY, _("Distance Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDistanceUnit->Wrap( -1 );
	fgSizerRingsDetail->Add( m_staticTextDistanceUnit, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceODPointDistanceUnitChoices[] = { _("Nautical Miles"), _("Kilometres") };
	int m_choiceODPointDistanceUnitNChoices = sizeof( m_choiceODPointDistanceUnitChoices ) / sizeof( wxString );
	m_choiceODPointDistanceUnit = new wxChoice( m_panelBoundaryPoint, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceODPointDistanceUnitNChoices, m_choiceODPointDistanceUnitChoices, 0 );
	m_choiceODPointDistanceUnit->SetSelection( 0 );
#ifdef __WXOSX__
    fgSizerRingsDetail->Add( m_choiceODPointDistanceUnit, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
#else
	fgSizerRingsDetail->Add( m_choiceODPointDistanceUnit, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
#endif
	m_staticTextRangeRingColours = new wxStaticText( m_panelBoundaryPoint, wxID_ANY, _("Range Ring Colours"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextRangeRingColours->Wrap( -1 );
	fgSizerRingsDetail->Add( m_staticTextRangeRingColours, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_colourPickerODPointRangeRingColours = new wxColourPickerCtrl( m_panelBoundaryPoint, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerRingsDetail->Add( m_colourPickerODPointRangeRingColours, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_staticTextRangeRingWidth = new wxStaticText( m_panelBoundaryPoint, wxID_ANY, _("Range Ring Line Width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextRangeRingWidth->Wrap( -1 );
	fgSizerRingsDetail->Add( m_staticTextRangeRingWidth, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceRangeRingWidthChoices[] = { _("1 pixel"), _("2 pixels"), _("3 pixels"), _("4 pixels"), _("5 pixels"), _("6 pixels"), _("7 pixels"), _("8 pixels"), _("9 pixels"), _("10 pixels") };
	int m_choiceRangeRingWidthNChoices = sizeof( m_choiceRangeRingWidthChoices ) / sizeof( wxString );
	m_choiceRangeRingWidth = new wxChoice( m_panelBoundaryPoint, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceRangeRingWidthNChoices, m_choiceRangeRingWidthChoices, 0 );
	m_choiceRangeRingWidth->SetSelection( 1 );
	fgSizerRingsDetail->Add( m_choiceRangeRingWidth, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextRangeRingStyle = new wxStaticText( m_panelBoundaryPoint, wxID_ANY, _("Range Ring Line Style"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextRangeRingStyle->Wrap( -1 );
	fgSizerRingsDetail->Add( m_staticTextRangeRingStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceRangeRingStyleChoices[] = { _("Solid"), _("Dot"), _("Long Dash"), _("Short Dash"), _("Dot Dash") };
	int m_choiceRangeRingStyleNChoices = sizeof( m_choiceRangeRingStyleChoices ) / sizeof( wxString );
	m_choiceRangeRingStyle = new wxChoice( m_panelBoundaryPoint, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceRangeRingStyleNChoices, m_choiceRangeRingStyleChoices, 0 );
	m_choiceRangeRingStyle->SetSelection( 0 );
	fgSizerRingsDetail->Add( m_choiceRangeRingStyle, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	bSizerOCPNPoint->Add( fgSizerRingsDetail, 0, wxEXPAND, 5 );
	
	
	m_panelBoundaryPoint->SetSizer( bSizerOCPNPoint );
	m_panelBoundaryPoint->Layout();
	bSizerOCPNPoint->Fit( m_panelBoundaryPoint );
	m_notebookProperties->AddPage( m_panelBoundaryPoint, _("Boundary Point"), false );
	m_panelTextPoint = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizerTextPoint;
	fgSizerTextPoint = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizerTextPoint->AddGrowableCol( 0 );
	fgSizerTextPoint->SetFlexibleDirection( wxBOTH );
	fgSizerTextPoint->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_SizerTextPointIconName = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTextTextPointIconName = new wxStaticText( m_panelTextPoint, wxID_ANY, _("Text Point Icon"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextTextPointIconName->Wrap( -1 );
#ifdef __WXOSX__
    m_SizerTextPointIconName->Add( m_staticTextTextPointIconName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
#else
	m_SizerTextPointIconName->Add( m_staticTextTextPointIconName, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL|wxEXPAND, 5 );
#endif
	m_bitmapTextPointBitmap = new wxStaticBitmap( m_panelTextPoint, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	m_SizerTextPointIconName->Add( m_bitmapTextPointBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_bcomboBoxTextPointIconName = new wxBitmapComboBox( m_panelTextPoint, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
#ifdef __WXOSX__
    m_SizerTextPointIconName->Add( m_bcomboBoxTextPointIconName, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
#else
	m_SizerTextPointIconName->Add( m_bcomboBoxTextPointIconName, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
#endif
	
	fgSizerTextPoint->Add( m_SizerTextPointIconName, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextPosition = new wxStaticText( m_panelTextPoint, wxID_ANY, _("Text position relative to point"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPosition->Wrap( -1 );
	fgSizer21->Add( m_staticTextPosition, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceTextPositionChoices[] = { _("Top"), _("Top Centre"), _("Bottom"), _("Bottom Centre"), _("Centre"), _("Right"), _("Left") };
	int m_choiceTextPositionNChoices = sizeof( m_choiceTextPositionChoices ) / sizeof( wxString );
	m_choiceTextPosition = new wxChoice( m_panelTextPoint, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceTextPositionNChoices, m_choiceTextPositionChoices, 0 );
	m_choiceTextPosition->SetSelection( 1 );
	fgSizer21->Add( m_choiceTextPosition, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextColour = new wxStaticText( m_panelTextPoint, wxID_ANY, _("Text Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextColour->Wrap( -1 );
	fgSizer21->Add( m_staticTextColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_colourPickerTextColour = new wxColourPickerCtrl( m_panelTextPoint, wxID_ANY, wxColour( 0, 0, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer21->Add( m_colourPickerTextColour, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextBackgroundColour = new wxStaticText( m_panelTextPoint, wxID_ANY, _("Background Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextBackgroundColour->Wrap( -1 );
	fgSizer21->Add( m_staticTextBackgroundColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_colourPickerBackgroundColour = new wxColourPickerCtrl( m_panelTextPoint, wxID_ANY, wxColour( 255, 255, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer21->Add( m_colourPickerBackgroundColour, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextBackgroundTransparency = new wxStaticText( m_panelTextPoint, wxID_ANY, _("Background Density"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextBackgroundTransparency->Wrap( -1 );
	fgSizer21->Add( m_staticTextBackgroundTransparency, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_sliderBackgroundTransparency = new wxSlider( m_panelTextPoint, wxID_ANY, 175, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	fgSizer21->Add( m_sliderBackgroundTransparency, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizerTextPoint->Add( fgSizer21, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerTextFont;
	bSizerTextFont = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTextTextFont = new wxStaticText( m_panelTextPoint, wxID_ANY, _("Text Font"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextTextFont->Wrap( -1 );
	bSizerTextFont->Add( m_staticTextTextFont, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticTextFontFaceExample = new wxStaticText( m_panelTextPoint, wxID_ANY, _("Example"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFontFaceExample->Wrap( -1 );
	bSizerTextFont->Add( m_staticTextFontFaceExample, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_buttonTextFont = new wxButton( m_panelTextPoint, wxID_ANY, _("Fonts"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTextFont->Add( m_buttonTextFont, 0, wxALL, 5 );
	
	
	fgSizerTextPoint->Add( bSizerTextFont, 1, 0, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxString m_radioBoxShowDisplayTextChoices[] = { _("Always"), _("On Rollover Only"), _("Never") };
	int m_radioBoxShowDisplayTextNChoices = sizeof( m_radioBoxShowDisplayTextChoices ) / sizeof( wxString );
	m_radioBoxShowDisplayText = new wxRadioBox( m_panelTextPoint, wxID_ANY, _("Show Display Text"), wxDefaultPosition, wxDefaultSize, m_radioBoxShowDisplayTextNChoices, m_radioBoxShowDisplayTextChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxShowDisplayText->SetSelection( 0 );
	bSizer11->Add( m_radioBoxShowDisplayText, 0, wxALL, 5 );
	
	
	fgSizerTextPoint->Add( bSizer11, 1, wxEXPAND, 5 );
	
	
	m_panelTextPoint->SetSizer( fgSizerTextPoint );
	m_panelTextPoint->Layout();
	fgSizerTextPoint->Fit( m_panelTextPoint );
	m_notebookProperties->AddPage( m_panelTextPoint, _("Text Point"), false );
	m_panelPath = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panelPath->Enable( false );
	m_panelPath->Hide();
	
	wxFlexGridSizer* fgSizerPathSettings;
	fgSizerPathSettings = new wxFlexGridSizer( 6, 4, 0, 0 );
	fgSizerPathSettings->SetFlexibleDirection( wxBOTH );
	fgSizerPathSettings->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextActivePathLineColour = new wxStaticText( m_panelPath, wxID_ANY, _("Active Line Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextActivePathLineColour->Wrap( -1 );
	fgSizerPathSettings->Add( m_staticTextActivePathLineColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_colourPickerActivePathLineColour = new wxColourPickerCtrl( m_panelPath, wxID_ANY, wxColour( 255, 0, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerPathSettings->Add( m_colourPickerActivePathLineColour, 0, wxALL, 5 );
	
	m_staticTextInactivePathLineColour = new wxStaticText( m_panelPath, wxID_ANY, _("Inactive Line Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInactivePathLineColour->Wrap( -1 );
	fgSizerPathSettings->Add( m_staticTextInactivePathLineColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_colourPickerInActivePathLineColour = new wxColourPickerCtrl( m_panelPath, wxID_ANY, wxColour( 214, 214, 214 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerPathSettings->Add( m_colourPickerInActivePathLineColour, 0, wxALL, 5 );
	
	m_staticTextPathLineWidth = new wxStaticText( m_panelPath, wxID_ANY, _("Line Width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPathLineWidth->Wrap( -1 );
	fgSizerPathSettings->Add( m_staticTextPathLineWidth, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choicePathLineWidthChoices[] = { _("1 Pixel"), _("2 Pixels"), _("3 Pixels"), _("4 Pixels"), _("5 Pixels"), _("6 Pixels"), _("7 Pixels"), _("8 Pixels"), _("9 Pixels"), _("10 Pixels") };
	int m_choicePathLineWidthNChoices = sizeof( m_choicePathLineWidthChoices ) / sizeof( wxString );
	m_choicePathLineWidth = new wxChoice( m_panelPath, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choicePathLineWidthNChoices, m_choicePathLineWidthChoices, 0 );
	m_choicePathLineWidth->SetSelection( 0 );
	fgSizerPathSettings->Add( m_choicePathLineWidth, 0, wxALL, 5 );
	
	m_staticTextPathLineStyle = new wxStaticText( m_panelPath, wxID_ANY, _("Line Style"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPathLineStyle->Wrap( -1 );
	fgSizerPathSettings->Add( m_staticTextPathLineStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choicePathLineStyleChoices[] = { _("Solid"), _("Dot"), _("Long Dash"), _("Short Dash"), _("Dot Dash") };
	int m_choicePathLineStyleNChoices = sizeof( m_choicePathLineStyleChoices ) / sizeof( wxString );
	m_choicePathLineStyle = new wxChoice( m_panelPath, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choicePathLineStyleNChoices, m_choicePathLineStyleChoices, 0 );
	m_choicePathLineStyle->SetSelection( 1 );
	fgSizerPathSettings->Add( m_choicePathLineStyle, 0, wxALL, 5 );
	
	
	m_panelPath->SetSizer( fgSizerPathSettings );
	m_panelPath->Layout();
	fgSizerPathSettings->Fit( m_panelPath );
	m_notebookProperties->AddPage( m_panelPath, _("Path"), false );
	m_panelEBL = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizerEBLSettings;
	fgSizerEBLSettings = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizerEBLSettings->AddGrowableCol( 1 );
	fgSizerEBLSettings->SetFlexibleDirection( wxBOTH );
	fgSizerEBLSettings->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextStartPointIcon = new wxStaticText( m_panelEBL, wxID_ANY, _("Start Point Icon"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextStartPointIcon->Wrap( -1 );
	fgSizerEBLSettings->Add( m_staticTextStartPointIcon, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxFlexGridSizer* fgSizerEBLEndPointIcon;
	fgSizerEBLEndPointIcon = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizerEBLEndPointIcon->AddGrowableCol( 1 );
	fgSizerEBLEndPointIcon->SetFlexibleDirection( wxBOTH );
	fgSizerEBLEndPointIcon->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_bitmapEBLStartBitmap = new wxStaticBitmap( m_panelEBL, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerEBLEndPointIcon->Add( m_bitmapEBLStartBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	
	m_bcomboBoxEBLStartIconName = new wxBitmapComboBox( m_panelEBL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizerEBLEndPointIcon->Add( m_bcomboBoxEBLStartIconName, 1, wxALIGN_LEFT|wxALL|wxEXPAND, 5 );
	
	
	fgSizerEBLSettings->Add( fgSizerEBLEndPointIcon, 1, wxEXPAND, 5 );
	
	m_staticTextEndPointIcon = new wxStaticText( m_panelEBL, wxID_ANY, _("End Point Icon"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextEndPointIcon->Wrap( -1 );
	fgSizerEBLSettings->Add( m_staticTextEndPointIcon, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxFlexGridSizer* fgSizerEBLStartPointIcon;
	fgSizerEBLStartPointIcon = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizerEBLStartPointIcon->AddGrowableCol( 1 );
	fgSizerEBLStartPointIcon->SetFlexibleDirection( wxBOTH );
	fgSizerEBLStartPointIcon->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_bitmapEBLEndBitmap = new wxStaticBitmap( m_panelEBL, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerEBLStartPointIcon->Add( m_bitmapEBLEndBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_bcomboBoxEBLEndIconName = new wxBitmapComboBox( m_panelEBL, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizerEBLStartPointIcon->Add( m_bcomboBoxEBLEndIconName, 1, wxALIGN_LEFT|wxALL|wxEXPAND, 5 );
	
	
	fgSizerEBLSettings->Add( fgSizerEBLStartPointIcon, 1, wxALIGN_RIGHT|wxEXPAND, 5 );
	
	m_staticTextActiveEBLLineColour = new wxStaticText( m_panelEBL, wxID_ANY, _("Active EBL Line Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextActiveEBLLineColour->Wrap( -1 );
	fgSizerEBLSettings->Add( m_staticTextActiveEBLLineColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_colourPickerEBLLineColour = new wxColourPickerCtrl( m_panelEBL, wxID_ANY, wxColour( 255, 0, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerEBLSettings->Add( m_colourPickerEBLLineColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticTextInactiveEBLLineColour = new wxStaticText( m_panelEBL, wxID_ANY, _("Inactive EBL Line Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInactiveEBLLineColour->Wrap( -1 );
	fgSizerEBLSettings->Add( m_staticTextInactiveEBLLineColour, 0, wxALL, 5 );
	
	m_colourPickerInActiveEBLLineColour = new wxColourPickerCtrl( m_panelEBL, wxID_ANY, wxColour( 214, 214, 214 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerEBLSettings->Add( m_colourPickerInActiveEBLLineColour, 0, wxALL, 5 );
	
	m_staticTextEBLLineWidth = new wxStaticText( m_panelEBL, wxID_ANY, _("Line Width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextEBLLineWidth->Wrap( -1 );
	fgSizerEBLSettings->Add( m_staticTextEBLLineWidth, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceEBLLineWidthChoices[] = { _("1 Pixel"), _("2 Pixels"), _("3 Pixels"), _("4 Pixels"), _("5 Pixels"), _("6 Pixels"), _("7 Pixels"), _("8 Pixels"), _("9 Pixels"), _("10 Pixels") };
	int m_choiceEBLLineWidthNChoices = sizeof( m_choiceEBLLineWidthChoices ) / sizeof( wxString );
	m_choiceEBLLineWidth = new wxChoice( m_panelEBL, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceEBLLineWidthNChoices, m_choiceEBLLineWidthChoices, 0 );
	m_choiceEBLLineWidth->SetSelection( 0 );
	fgSizerEBLSettings->Add( m_choiceEBLLineWidth, 0, wxALL, 5 );
	
	m_staticTextEBLLineStyle = new wxStaticText( m_panelEBL, wxID_ANY, _("Line Style"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextEBLLineStyle->Wrap( -1 );
	fgSizerEBLSettings->Add( m_staticTextEBLLineStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceEBLLineStyleChoices[] = { _("Solid"), _("Dot"), _("Long Dash"), _("Short Dash"), _("Dot Dash") };
	int m_choiceEBLLineStyleNChoices = sizeof( m_choiceEBLLineStyleChoices ) / sizeof( wxString );
	m_choiceEBLLineStyle = new wxChoice( m_panelEBL, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceEBLLineStyleNChoices, m_choiceEBLLineStyleChoices, 0 );
	m_choiceEBLLineStyle->SetSelection( 0 );
	fgSizerEBLSettings->Add( m_choiceEBLLineStyle, 0, wxALL, 5 );
	
	m_checkBoxRotateWithBoat = new wxCheckBox( m_panelEBL, wxID_ANY, _("Rotate with Boat"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	fgSizerEBLSettings->Add( m_checkBoxRotateWithBoat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_radioBoxMaintainWithChoices[] = { _("Heading"), _("Course over Ground") };
	int m_radioBoxMaintainWithNChoices = sizeof( m_radioBoxMaintainWithChoices ) / sizeof( wxString );
	m_radioBoxMaintainWith = new wxRadioBox( m_panelEBL, wxID_ANY, _("Maintain with"), wxDefaultPosition, wxDefaultSize, m_radioBoxMaintainWithNChoices, m_radioBoxMaintainWithChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxMaintainWith->SetSelection( 0 );
	fgSizerEBLSettings->Add( m_radioBoxMaintainWith, 0, wxALL, 5 );
	
	m_checkBoxEBLFixedEndPosition = new wxCheckBox( m_panelEBL, wxID_ANY, _("EBL Fixed End Position"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	fgSizerEBLSettings->Add( m_checkBoxEBLFixedEndPosition, 0, wxALL, 5 );
	
	m_checkBoxEBLShowArrow = new wxCheckBox( m_panelEBL, wxID_ANY, _("Show EBL Direction Arrow"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	fgSizerEBLSettings->Add( m_checkBoxEBLShowArrow, 0, wxALL, 5 );
	
	wxString m_radioBoxEBLPersistenceChoices[] = { _("Persistent"), _("Persistent over Crash"), _("Never") };
	int m_radioBoxEBLPersistenceNChoices = sizeof( m_radioBoxEBLPersistenceChoices ) / sizeof( wxString );
	m_radioBoxEBLPersistence = new wxRadioBox( m_panelEBL, wxID_ANY, _("EBL Persistence"), wxDefaultPosition, wxDefaultSize, m_radioBoxEBLPersistenceNChoices, m_radioBoxEBLPersistenceChoices, 2, wxRA_SPECIFY_ROWS );
	m_radioBoxEBLPersistence->SetSelection( 0 );
	fgSizerEBLSettings->Add( m_radioBoxEBLPersistence, 0, wxALL, 5 );
	
	m_checkBoxShowVRM = new wxCheckBox( m_panelEBL, wxID_ANY, _("Show VRM"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	fgSizerEBLSettings->Add( m_checkBoxShowVRM, 0, wxALL, 5 );
	
	
	m_panelEBL->SetSizer( fgSizerEBLSettings );
	m_panelEBL->Layout();
	fgSizerEBLSettings->Fit( m_panelEBL );
	m_notebookProperties->AddPage( m_panelEBL, _("EBL"), false );
	m_panelDR = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizerDRSettings;
	fgSizerDRSettings = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizerDRSettings->SetFlexibleDirection( wxBOTH );
	fgSizerDRSettings->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizerDRDefaultSizes;
	fgSizerDRDefaultSizes = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizerDRDefaultSizes->SetFlexibleDirection( wxBOTH );
	fgSizerDRDefaultSizes->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextActiveDRLineColour = new wxStaticText( m_panelDR, wxID_ANY, _("Active DR Line\nColour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextActiveDRLineColour->Wrap( -1 );
	fgSizerDRDefaultSizes->Add( m_staticTextActiveDRLineColour, 0, wxALL, 5 );
	
	m_colourPickerDRLineColour = new wxColourPickerCtrl( m_panelDR, wxID_ANY, wxColour( 255, 0, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerDRDefaultSizes->Add( m_colourPickerDRLineColour, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextInactiveDRLineColour = new wxStaticText( m_panelDR, wxID_ANY, _("Inactive DR Line\nColour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInactiveDRLineColour->Wrap( -1 );
	fgSizerDRDefaultSizes->Add( m_staticTextInactiveDRLineColour, 0, wxALL, 5 );
	
	m_colourPickerInActiveDRLineColour = new wxColourPickerCtrl( m_panelDR, wxID_ANY, wxColour( 214, 214, 214 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerDRDefaultSizes->Add( m_colourPickerInActiveDRLineColour, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextDRLineWidth = new wxStaticText( m_panelDR, wxID_ANY, _("Line Width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDRLineWidth->Wrap( -1 );
	fgSizerDRDefaultSizes->Add( m_staticTextDRLineWidth, 0, wxALL, 5 );
	
	wxString m_choiceDRLineWidthChoices[] = { _("1 Pixel"), _("2 Pixels"), _("3 Pixels"), _("4 Pixels"), _("5 Pixels"), _("6 Pixels"), _("7 Pixels"), _("8 Pixels"), _("9 Pixels"), _("10 Pixels") };
	int m_choiceDRLineWidthNChoices = sizeof( m_choiceDRLineWidthChoices ) / sizeof( wxString );
	m_choiceDRLineWidth = new wxChoice( m_panelDR, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDRLineWidthNChoices, m_choiceDRLineWidthChoices, 0 );
	m_choiceDRLineWidth->SetSelection( 0 );
	fgSizerDRDefaultSizes->Add( m_choiceDRLineWidth, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextDRLineStyle = new wxStaticText( m_panelDR, wxID_ANY, _("Line Style"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDRLineStyle->Wrap( -1 );
	fgSizerDRDefaultSizes->Add( m_staticTextDRLineStyle, 0, wxALL, 5 );
	
	wxString m_choiceDRLineStyleChoices[] = { _("Solid"), _("Dot"), _("Long Dash"), _("Short Dash"), _("Dot Dash") };
	int m_choiceDRLineStyleNChoices = sizeof( m_choiceDRLineStyleChoices ) / sizeof( wxString );
	m_choiceDRLineStyle = new wxChoice( m_panelDR, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDRLineStyleNChoices, m_choiceDRLineStyleChoices, 0 );
	m_choiceDRLineStyle->SetSelection( 0 );
	fgSizerDRDefaultSizes->Add( m_choiceDRLineStyle, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextDefaultSoG = new wxStaticText( m_panelDR, wxID_ANY, _("Speed over Ground"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDefaultSoG->Wrap( -1 );
	fgSizerDRDefaultSizes->Add( m_staticTextDefaultSoG, 0, wxALL, 5 );
	
	m_textCtrlSOG = new wxTextCtrl( m_panelDR, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerDRDefaultSizes->Add( m_textCtrlSOG, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextCOG = new wxStaticText( m_panelDR, wxID_ANY, _("Course over Ground"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextCOG->Wrap( -1 );
	fgSizerDRDefaultSizes->Add( m_staticTextCOG, 0, wxALL, 5 );
	
	m_textCtrlCOG = new wxTextCtrl( m_panelDR, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerDRDefaultSizes->Add( m_textCtrlCOG, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextDRLength = new wxStaticText( m_panelDR, wxID_ANY, _("DR Path Length"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDRLength->Wrap( -1 );
	fgSizerDRDefaultSizes->Add( m_staticTextDRLength, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlDRPathLength = new wxTextCtrl( m_panelDR, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerDRDefaultSizes->Add( m_textCtrlDRPathLength, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextDRPointInterval = new wxStaticText( m_panelDR, wxID_ANY, _("DR Point Interval"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDRPointInterval->Wrap( -1 );
	fgSizerDRDefaultSizes->Add( m_staticTextDRPointInterval, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlDRPointInterval = new wxTextCtrl( m_panelDR, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerDRDefaultSizes->Add( m_textCtrlDRPointInterval, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	fgSizerDRSettings->Add( fgSizerDRDefaultSizes, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer16;
	fgSizer16 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer16->SetFlexibleDirection( wxBOTH );
	fgSizer16->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxString m_radioBoxDRLengthTypeChoices[] = { _("Time"), _("Distance") };
	int m_radioBoxDRLengthTypeNChoices = sizeof( m_radioBoxDRLengthTypeChoices ) / sizeof( wxString );
	m_radioBoxDRLengthType = new wxRadioBox( m_panelDR, wxID_ANY, _("Length Type"), wxDefaultPosition, wxDefaultSize, m_radioBoxDRLengthTypeNChoices, m_radioBoxDRLengthTypeChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxDRLengthType->SetSelection( 1 );
	fgSizer16->Add( m_radioBoxDRLengthType, 0, wxALL, 5 );
	
	wxString m_radioBoxDRIntervalTypeChoices[] = { _("Time"), _("Distance") };
	int m_radioBoxDRIntervalTypeNChoices = sizeof( m_radioBoxDRIntervalTypeChoices ) / sizeof( wxString );
	m_radioBoxDRIntervalType = new wxRadioBox( m_panelDR, wxID_ANY, _("Interval Type"), wxDefaultPosition, wxDefaultSize, m_radioBoxDRIntervalTypeNChoices, m_radioBoxDRIntervalTypeChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxDRIntervalType->SetSelection( 1 );
	fgSizer16->Add( m_radioBoxDRIntervalType, 0, wxALL, 5 );
	
	wxString m_radioBoxDRDistanceUnitsChoices[] = { _("Kilometres"), _("Nautical Miles") };
	int m_radioBoxDRDistanceUnitsNChoices = sizeof( m_radioBoxDRDistanceUnitsChoices ) / sizeof( wxString );
	m_radioBoxDRDistanceUnits = new wxRadioBox( m_panelDR, wxID_ANY, _("Distance Units"), wxDefaultPosition, wxDefaultSize, m_radioBoxDRDistanceUnitsNChoices, m_radioBoxDRDistanceUnitsChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxDRDistanceUnits->SetSelection( 0 );
	fgSizer16->Add( m_radioBoxDRDistanceUnits, 0, wxALL, 5 );
	
	wxString m_radioBoxDRTimeUnitsChoices[] = { _("Minutes"), _("Hours"), _("Days") };
	int m_radioBoxDRTimeUnitsNChoices = sizeof( m_radioBoxDRTimeUnitsChoices ) / sizeof( wxString );
	m_radioBoxDRTimeUnits = new wxRadioBox( m_panelDR, wxID_ANY, _("Time Units"), wxDefaultPosition, wxDefaultSize, m_radioBoxDRTimeUnitsNChoices, m_radioBoxDRTimeUnitsChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxDRTimeUnits->SetSelection( 0 );
	fgSizer16->Add( m_radioBoxDRTimeUnits, 0, wxALL, 5 );
	
	m_checkBoxDRShowArrow = new wxCheckBox( m_panelDR, wxID_ANY, _("Show DR Direction Arrow"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_checkBoxDRShowArrow->SetValue(true); 
	fgSizer16->Add( m_checkBoxDRShowArrow, 0, wxALL, 5 );
	
	wxString m_radioBoxDRPersistenceChoices[] = { _("Persistent"), _("Persistent over Crash"), _("Never") };
	int m_radioBoxDRPersistenceNChoices = sizeof( m_radioBoxDRPersistenceChoices ) / sizeof( wxString );
	m_radioBoxDRPersistence = new wxRadioBox( m_panelDR, wxID_ANY, _("DR Persistence"), wxDefaultPosition, wxDefaultSize, m_radioBoxDRPersistenceNChoices, m_radioBoxDRPersistenceChoices, 2, wxRA_SPECIFY_ROWS );
	m_radioBoxDRPersistence->SetSelection( 2 );
	fgSizer16->Add( m_radioBoxDRPersistence, 0, wxALL, 5 );
	
	
	fgSizerDRSettings->Add( fgSizer16, 1, wxEXPAND, 5 );
	
	
	m_panelDR->SetSizer( fgSizerDRSettings );
	m_panelDR->Layout();
	fgSizerDRSettings->Fit( m_panelDR );
	m_notebookProperties->AddPage( m_panelDR, _("DR"), false );
	m_panelDRPoint = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizerDREndPointIcon;
	fgSizerDREndPointIcon = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizerDREndPointIcon->AddGrowableCol( 0 );
	fgSizerDREndPointIcon->AddGrowableCol( 2 );
	fgSizerDREndPointIcon->SetFlexibleDirection( wxBOTH );
	fgSizerDREndPointIcon->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextDRPointIcon = new wxStaticText( m_panelDRPoint, wxID_ANY, _("DR Point Icon"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDRPointIcon->Wrap( -1 );
	fgSizerDREndPointIcon->Add( m_staticTextDRPointIcon, 1, wxALL|wxEXPAND, 5 );
	
	m_bitmapDRPointBitmap = new wxStaticBitmap( m_panelDRPoint, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerDREndPointIcon->Add( m_bitmapDRPointBitmap, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	
	m_bcomboBoxDRPointIconName = new wxBitmapComboBox( m_panelDRPoint, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizerDREndPointIcon->Add( m_bcomboBoxDRPointIconName, 1, wxALIGN_LEFT|wxALL|wxEXPAND, 5 );
	
	
	bSizer12->Add( fgSizerDREndPointIcon, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizerDRPointRangeRings;
	fgSizerDRPointRangeRings = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizerDRPointRangeRings->SetFlexibleDirection( wxBOTH );
	fgSizerDRPointRangeRings->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxShowDRPointRangeRings = new wxCheckBox( m_panelDRPoint, wxID_ANY, _("Show Range Rings"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	fgSizerDRPointRangeRings->Add( m_checkBoxShowDRPointRangeRings, 0, wxALL, 5 );
	
	
	fgSizerDRPointRangeRings->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextDRPointRangeRingNumber = new wxStaticText( m_panelDRPoint, wxID_ANY, _("Number of Range Rings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDRPointRangeRingNumber->Wrap( -1 );
	fgSizerDRPointRangeRings->Add( m_staticTextDRPointRangeRingNumber, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceDRPointRangeRingNumberChoices[] = { _("None"), _("1"), _("2"), _("3"), _("4"), _("5"), _("6"), _("7"), _("8"), _("9"), _("10") };
	int m_choiceDRPointRangeRingNumberNChoices = sizeof( m_choiceDRPointRangeRingNumberChoices ) / sizeof( wxString );
	m_choiceDRPointRangeRingNumber = new wxChoice( m_panelDRPoint, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDRPointRangeRingNumberNChoices, m_choiceDRPointRangeRingNumberChoices, 0 );
	m_choiceDRPointRangeRingNumber->SetSelection( 0 );
	fgSizerDRPointRangeRings->Add( m_choiceDRPointRangeRingNumber, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextDRPointRangeRingSteps = new wxStaticText( m_panelDRPoint, wxID_ANY, _("Distance Between Range Rings"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDRPointRangeRingSteps->Wrap( -1 );
	fgSizerDRPointRangeRings->Add( m_staticTextDRPointRangeRingSteps, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_textCtrlDRPointRangeRingSteps = new wxTextCtrl( m_panelDRPoint, wxID_ANY, _("0.5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlDRPointRangeRingSteps->SetMaxLength( 0 ); 
	fgSizerDRPointRangeRings->Add( m_textCtrlDRPointRangeRingSteps, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextDRPointDistanceUnit = new wxStaticText( m_panelDRPoint, wxID_ANY, _("Distance Unit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDRPointDistanceUnit->Wrap( -1 );
	fgSizerDRPointRangeRings->Add( m_staticTextDRPointDistanceUnit, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceDRPointDistanceUnitChoices[] = { _("Nautical Miles"), _("Kilometres") };
	int m_choiceDRPointDistanceUnitNChoices = sizeof( m_choiceDRPointDistanceUnitChoices ) / sizeof( wxString );
	m_choiceDRPointDistanceUnit = new wxChoice( m_panelDRPoint, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDRPointDistanceUnitNChoices, m_choiceDRPointDistanceUnitChoices, 0 );
	m_choiceDRPointDistanceUnit->SetSelection( 0 );
	fgSizerDRPointRangeRings->Add( m_choiceDRPointDistanceUnit, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextDRPointRangeRingColours = new wxStaticText( m_panelDRPoint, wxID_ANY, _("Range Ring Colours"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDRPointRangeRingColours->Wrap( -1 );
	fgSizerDRPointRangeRings->Add( m_staticTextDRPointRangeRingColours, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_colourPickerDRPointRangeRingColours = new wxColourPickerCtrl( m_panelDRPoint, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerDRPointRangeRings->Add( m_colourPickerDRPointRangeRingColours, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_staticTextDRPointRangeRingWidth = new wxStaticText( m_panelDRPoint, wxID_ANY, _("Range Ring Line Width"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDRPointRangeRingWidth->Wrap( -1 );
	fgSizerDRPointRangeRings->Add( m_staticTextDRPointRangeRingWidth, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceDRPointRangeRingWidthChoices[] = { _("1 pixel"), _("2 pixels"), _("3 pixels"), _("4 pixels"), _("5 pixels"), _("6 pixels"), _("7 pixels"), _("8 pixels"), _("9 pixels"), _("10 pixels") };
	int m_choiceDRPointRangeRingWidthNChoices = sizeof( m_choiceDRPointRangeRingWidthChoices ) / sizeof( wxString );
	m_choiceDRPointRangeRingWidth = new wxChoice( m_panelDRPoint, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDRPointRangeRingWidthNChoices, m_choiceDRPointRangeRingWidthChoices, 0 );
	m_choiceDRPointRangeRingWidth->SetSelection( 1 );
	fgSizerDRPointRangeRings->Add( m_choiceDRPointRangeRingWidth, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticTextDRPointRangeRingStyle = new wxStaticText( m_panelDRPoint, wxID_ANY, _("Range Ring Line Style"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDRPointRangeRingStyle->Wrap( -1 );
	fgSizerDRPointRangeRings->Add( m_staticTextDRPointRangeRingStyle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxString m_choiceDRPointRangeRingStyleChoices[] = { _("Solid"), _("Dot"), _("Long Dash"), _("Short Dash"), _("Dot Dash") };
	int m_choiceDRPointRangeRingStyleNChoices = sizeof( m_choiceDRPointRangeRingStyleChoices ) / sizeof( wxString );
	m_choiceDRPointRangeRingStyle = new wxChoice( m_panelDRPoint, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDRPointRangeRingStyleNChoices, m_choiceDRPointRangeRingStyleChoices, 0 );
	m_choiceDRPointRangeRingStyle->SetSelection( 0 );
	fgSizerDRPointRangeRings->Add( m_choiceDRPointRangeRingStyle, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	bSizer12->Add( fgSizerDRPointRangeRings, 0, wxEXPAND, 5 );
	
	
	m_panelDRPoint->SetSizer( bSizer12 );
	m_panelDRPoint->Layout();
	bSizer12->Fit( m_panelDRPoint );
	m_notebookProperties->AddPage( m_panelDRPoint, _("DR Point"), false );
	m_panelAbout = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerAbout;
	bSizerAbout = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizerAboutLayout;
	fgSizerAboutLayout = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizerAboutLayout->SetFlexibleDirection( wxBOTH );
	fgSizerAboutLayout->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextName = new wxStaticText( m_panelAbout, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextName->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextName, 0, wxALL, 5 );
	
	m_staticTextNameVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextNameVal->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextNameVal, 0, wxALL, 5 );
	
	m_staticTextVersion = new wxStaticText( m_panelAbout, wxID_ANY, _("Version:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextVersion->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextVersion, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 0, 6, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextMajor = new wxStaticText( m_panelAbout, wxID_ANY, _("Major:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMajor->Wrap( -1 );
	fgSizer6->Add( m_staticTextMajor, 0, wxALL, 5 );
	
	m_staticTextMajorVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMajorVal->Wrap( -1 );
	fgSizer6->Add( m_staticTextMajorVal, 0, wxALL, 5 );
	
	m_staticTextMinor = new wxStaticText( m_panelAbout, wxID_ANY, _("Minor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMinor->Wrap( -1 );
	fgSizer6->Add( m_staticTextMinor, 0, wxALL, 5 );
	
	m_staticTextMinorVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMinorVal->Wrap( -1 );
	fgSizer6->Add( m_staticTextMinorVal, 0, wxALL, 5 );
	
	m_staticTextPatcvh = new wxStaticText( m_panelAbout, wxID_ANY, _("Patch:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPatcvh->Wrap( -1 );
	fgSizer6->Add( m_staticTextPatcvh, 0, wxALL, 5 );
	
	m_staticTextPatchVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPatchVal->Wrap( -1 );
	fgSizer6->Add( m_staticTextPatchVal, 0, wxALL, 5 );
	
	
	fgSizerAboutLayout->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	m_staticTextDate = new wxStaticText( m_panelAbout, wxID_ANY, _("Date:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDate->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextDate, 0, wxALL, 5 );
	
	m_staticTextDateVal = new wxStaticText( m_panelAbout, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDateVal->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextDateVal, 0, wxALL, 5 );
	
	m_staticTextOther = new wxStaticText( m_panelAbout, wxID_ANY, _("Other:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOther->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticTextOther, 0, wxALL, 5 );
	
	m_staticText34 = new wxStaticText( m_panelAbout, wxID_ANY, _("Please report problems using FlySpray at:\nhttp://willkamp.com/opencpn/flyspray/index.php?project=0&do=index&switch=1\n\nor\n\nat the OpenCPN forum:\nhttp://www.cruisersforum.com/forums/f134\n\nor\n\nRaise an issue in GIT at:\nhttps://github.com/jongough/ocpn_draw_pi/issues\n"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText34->Wrap( -1 );
	fgSizerAboutLayout->Add( m_staticText34, 0, wxALL, 5 );
	
	
	bSizerAbout->Add( fgSizerAboutLayout, 1, wxEXPAND, 5 );
	
	
	m_panelAbout->SetSizer( bSizerAbout );
	m_panelAbout->Layout();
	bSizerAbout->Fit( m_panelAbout );
	m_notebookProperties->AddPage( m_panelAbout, _("About"), false );
	
	m_SizerProperties->Add( m_notebookProperties, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizerOKCancelApply;
	bSizerOKCancelApply = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_ANY, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerOKCancelApply->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_ANY, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerOKCancelApply->Add( m_buttonCancel, 0, wxALL, 5 );
	
	m_buttonApply = new wxButton( this, wxID_ANY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerOKCancelApply->Add( m_buttonApply, 0, wxALL, 5 );
	
	
	m_SizerProperties->Add( bSizerOKCancelApply, 0, wxEXPAND, 5 );
	
	
	this->SetSizer( m_SizerProperties );
	this->Layout();
	
	// Connect Events
	m_bcomboBoxODPointIconName->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ODPropertiesDialogDef::OnODPointComboboxSelected ), NULL, this );
	m_bcomboBoxTextPointIconName->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ODPropertiesDialogDef::OnTextPointIconComboboxSelected ), NULL, this );
	m_buttonTextFont->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnButtonClickFonts ), NULL, this );
	m_bcomboBoxEBLStartIconName->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ODPropertiesDialogDef::OnEBLStartIconComboboxSelected ), NULL, this );
	m_bcomboBoxEBLEndIconName->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ODPropertiesDialogDef::OnEBLEndIconComboboxSelected ), NULL, this );
	m_checkBoxRotateWithBoat->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnEBLRotateWithBoat ), NULL, this );
	m_checkBoxEBLFixedEndPosition->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnEBLFixedEndPosition ), NULL, this );
	m_bcomboBoxDRPointIconName->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ODPropertiesDialogDef::OnDRPointIconComboboxSelected ), NULL, this );
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnDrawPropertiesOKClick ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnDrawPropertiesCancelClick ), NULL, this );
	m_buttonApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnDrawPropertiesApplyClick ), NULL, this );
}

ODPropertiesDialogDef::~ODPropertiesDialogDef()
{
	// Disconnect Events
	m_bcomboBoxODPointIconName->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ODPropertiesDialogDef::OnODPointComboboxSelected ), NULL, this );
	m_bcomboBoxTextPointIconName->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ODPropertiesDialogDef::OnTextPointIconComboboxSelected ), NULL, this );
	m_buttonTextFont->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnButtonClickFonts ), NULL, this );
	m_bcomboBoxEBLStartIconName->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ODPropertiesDialogDef::OnEBLStartIconComboboxSelected ), NULL, this );
	m_bcomboBoxEBLEndIconName->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ODPropertiesDialogDef::OnEBLEndIconComboboxSelected ), NULL, this );
	m_checkBoxRotateWithBoat->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnEBLRotateWithBoat ), NULL, this );
	m_checkBoxEBLFixedEndPosition->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnEBLFixedEndPosition ), NULL, this );
	m_bcomboBoxDRPointIconName->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ODPropertiesDialogDef::OnDRPointIconComboboxSelected ), NULL, this );
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnDrawPropertiesOKClick ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnDrawPropertiesCancelClick ), NULL, this );
	m_buttonApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ODPropertiesDialogDef::OnDrawPropertiesApplyClick ), NULL, this );
	
}

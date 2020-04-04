// /////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
// /////////////////////////////////////////////////////////////////////////

#include "squiddioPrefsDialogBase.h"

// /////////////////////////////////////////////////////////////////////////

SquiddioPrefsDialogBase::SquiddioPrefsDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
#ifdef __WXOSX__
    m_fgMainSizer = new wxFlexGridSizer( 1, 0, 0 );
#else
	m_fgMainSizer = new wxFlexGridSizer( 0, 1, 0, 0 );
#endif
	m_fgMainSizer->SetFlexibleDirection( wxBOTH );
	m_fgMainSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
#ifdef __WXOSX__
    m_fgSubSizer = new wxFlexGridSizer( 1, 0, 0 );
#else
	m_fgSubSizer = new wxFlexGridSizer( 0, 1, 0, 0 );
#endif
	m_fgSubSizer->SetFlexibleDirection( wxBOTH );
	m_fgSubSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_fgSubSizer->SetMinSize( wxSize( -1,380 ) );
    m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize( 480,-1 ), 0 );
    m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbDestSizer;
    sbDestSizer = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, _("Destinations") ), wxVERTICAL );

    wxFlexGridSizer* viewDestinations;
#ifdef __WXOSX__
    viewDestinations = new wxFlexGridSizer( 3, 0, 0 );
#else
    viewDestinations = new wxFlexGridSizer( 3, 3, 0, 0 );
#endif
    viewDestinations->SetFlexibleDirection( wxBOTH );
    viewDestinations->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_checkBoxAll = new wxCheckBox( sbDestSizer->GetStaticBox(), ID_CB_ALL, _("All"), wxDefaultPosition, wxDefaultSize, 0 );
    viewDestinations->Add( m_checkBoxAll, 0, wxALL, 5 );

	m_checkBoxMarinas = new wxCheckBox( sbDestSizer->GetStaticBox(), wxID_ANY, _("Marinas"), wxDefaultPosition, wxDefaultSize, 0 );
    viewDestinations->Add( m_checkBoxMarinas, 0, wxALL, 5 );

	m_checkBoxAnchorages = new wxCheckBox( sbDestSizer->GetStaticBox(), wxID_ANY, _("Anchorages"), wxDefaultPosition, wxDefaultSize, 0 );
    viewDestinations->Add( m_checkBoxAnchorages, 0, wxALL, 5 );

    m_checkBoxDocks = new wxCheckBox( sbDestSizer->GetStaticBox(), wxID_ANY, _("Docks/Piers"), wxDefaultPosition, wxDefaultSize, 0 );
    viewDestinations->Add( m_checkBoxDocks, 0, wxALL, 5 );

    m_checkBoxYachtClubs = new wxCheckBox( sbDestSizer->GetStaticBox(), wxID_ANY, _("Yacht Clubs"), wxDefaultPosition, wxDefaultSize, 0 );
    viewDestinations->Add( m_checkBoxYachtClubs, 0, wxALL, 5 );

    m_checkBoxMoorings = new wxCheckBox( sbDestSizer->GetStaticBox(), wxID_ANY, _("Mooring Buoys"), wxDefaultPosition, wxDefaultSize, 0 );
    viewDestinations->Add( m_checkBoxMoorings, 0, wxALL, 5 );

	m_checkBoxFuelStations = new wxCheckBox( sbDestSizer->GetStaticBox(), wxID_ANY, _("Fuel Stations"), wxDefaultPosition, wxDefaultSize, 0 );
    viewDestinations->Add( m_checkBoxFuelStations, 0, wxALL, 5 );

    m_checkBoxBoatYards = new wxCheckBox( sbDestSizer->GetStaticBox(), wxID_ANY, _("Boat Yards"), wxDefaultPosition, wxDefaultSize, 0 );
    viewDestinations->Add( m_checkBoxBoatYards, 0, wxALL, 5 );

    m_checkBoxRamps = new wxCheckBox( sbDestSizer->GetStaticBox(), wxID_ANY, _("Boat Ramps"), wxDefaultPosition, wxDefaultSize, 0 );
    viewDestinations->Add( m_checkBoxRamps, 0, wxALL, 5 );

	m_checkBoxOthers = new wxCheckBox( sbDestSizer->GetStaticBox(), wxID_ANY, _("Other"), wxDefaultPosition, wxDefaultSize, 0 );
    viewDestinations->Add( m_checkBoxOthers, 0, wxALL, 5 );


    sbDestSizer->Add( viewDestinations, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer61;
    sbSizer61 = new wxStaticBoxSizer( new wxStaticBox( sbDestSizer->GetStaticBox(), wxID_ANY, _("Cruising Aids") ), wxVERTICAL );

    m_checkBoxNDBC = new wxCheckBox( sbSizer61->GetStaticBox(), wxID_ANY, _("NDBC Buoy Reports"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxNDBC->SetToolTip( _("Get quasi-real time wind, wave and pressure report from thousands of NDBC buoys right from your chart.") );

    sbSizer61->Add( m_checkBoxNDBC, 0, wxALL, 5 );

    m_checkBoxShipRep = new wxCheckBox( sbSizer61->GetStaticBox(), wxID_ANY, _("NDBC Ship Reports"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxShipRep->SetToolTip( _("Get quasi-real time wind, wave and pressure report from participating NDBC ships right from your chart.") );

    sbSizer61->Add( m_checkBoxShipRep, 0, wxALL, 5 );

    m_checkBoxAIS = new wxCheckBox( sbSizer61->GetStaticBox(), wxID_ANY, _("AIS markers"), wxDefaultPosition, wxDefaultSize, 0 );
    sbSizer61->Add( m_checkBoxAIS, 0, wxALL, 5 );

    sbDestSizer->Add( sbSizer61, 1, wxEXPAND, 5 );

    m_panel1->SetSizer( sbDestSizer );
    m_panel1->Layout();
    sbDestSizer->Fit( m_panel1 );
    m_notebook1->AddPage( m_panel1, _("View"), true );
    rendering = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    rendering->SetToolTip( _("Delete downloaded POI files for sectors you have hidden. This limits the proliferation of old unecessary POI files and reduced the computational burden.") );

    wxStaticBoxSizer* sbODSettings;
    sbODSettings = new wxStaticBoxSizer( new wxStaticBox( rendering, wxID_ANY, wxEmptyString ), wxVERTICAL );

	wxString m_radioBoxOCPNorODChoices[] = { _("OpenCPN Waypoint"), _("ODraw TextPoint") };
	int m_radioBoxOCPNorODNChoices = sizeof( m_radioBoxOCPNorODChoices ) / sizeof( wxString );
    m_radioBoxOCPNorOD = new wxRadioBox( sbODSettings->GetStaticBox(), wxID_ANY, _("Use OCPN or ODraw (recommended)"), wxDefaultPosition, wxDefaultSize, m_radioBoxOCPNorODNChoices, m_radioBoxOCPNorODChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxOCPNorOD->SetSelection( 0 );
    m_radioBoxOCPNorOD->SetToolTip( _("Render sQuiddio POIs as legacy OpenCPN waypoints or  ODraw Textpoints (recommended, provided ODraw plugin is enabled).") );

    sbODSettings->Add( m_radioBoxOCPNorOD, 0, wxALL, 5 );

    m_staticline2 = new wxStaticLine( sbODSettings->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    sbODSettings->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

    wxStaticBoxSizer* sbSizer6;
    sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( sbODSettings->GetStaticBox(), wxID_ANY, _("ODraw Settings") ), wxVERTICAL );
#ifdef __WXOSX__
    m_fgSizerODSettings = new wxFlexGridSizer( 2, 0, 0 );
#else
	m_fgSizerODSettings = new wxFlexGridSizer( 0, 2, 0, 0 );
#endif
	m_fgSizerODSettings->AddGrowableCol( 0 );
	m_fgSizerODSettings->SetFlexibleDirection( wxBOTH );
	m_fgSizerODSettings->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_checkBoxShowName = new wxCheckBox( sbSizer6->GetStaticBox(), wxID_ANY, _("Show Name"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_checkBoxShowName->SetValue(true);
	m_fgSizerODSettings->Add( m_checkBoxShowName, 0, wxALL, 5 );


	m_fgSizerODSettings->Add( 0, 0, 1, wxEXPAND, 5 );

    m_staticText5 = new wxStaticText( sbSizer6->GetStaticBox(), wxID_ANY, _("Text position relative to point"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	m_fgSizerODSettings->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxString m_choicePositionChoices[] = { _("Top"), _("Top Centre"), _("Bottom"), _("Bottom Centre"), _("Centre"), _("Right"), _("Left") };
	int m_choicePositionNChoices = sizeof( m_choicePositionChoices ) / sizeof( wxString );
    m_choicePosition = new wxChoice( sbSizer6->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choicePositionNChoices, m_choicePositionChoices, 0 );
	m_choicePosition->SetSelection( 0 );
	m_fgSizerODSettings->Add( m_choicePosition, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

    m_staticTextTextColour = new wxStaticText( sbSizer6->GetStaticBox(), wxID_ANY, _("Text Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextTextColour->Wrap( -1 );
	m_fgSizerODSettings->Add( m_staticTextTextColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_colourPickerText = new wxColourPickerCtrl( sbSizer6->GetStaticBox(), wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_fgSizerODSettings->Add( m_colourPickerText, 0, wxALIGN_RIGHT|wxALL, 5 );

    m_staticTextBackgroundColour = new wxStaticText( sbSizer6->GetStaticBox(), wxID_ANY, _("Baackground Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextBackgroundColour->Wrap( -1 );
	m_fgSizerODSettings->Add( m_staticTextBackgroundColour, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_colourPickerBackgroundColour = new wxColourPickerCtrl( sbSizer6->GetStaticBox(), wxID_ANY, wxColour( 255, 255, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	m_fgSizerODSettings->Add( m_colourPickerBackgroundColour, 0, wxALIGN_RIGHT|wxALL, 5 );

    m_staticTextBackgroundTransparency = new wxStaticText( sbSizer6->GetStaticBox(), wxID_ANY, _("Background Transparency"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextBackgroundTransparency->Wrap( -1 );
	m_fgSizerODSettings->Add( m_staticTextBackgroundTransparency, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_sliderBackgroundTransparency = new wxSlider( sbSizer6->GetStaticBox(), wxID_ANY, 100, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS );
	m_fgSizerODSettings->Add( m_sliderBackgroundTransparency, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizerFontFace;
	bSizerFontFace = new wxBoxSizer( wxHORIZONTAL );

    m_staticTextFont = new wxStaticText( sbSizer6->GetStaticBox(), wxID_ANY, _("Text Font"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFont->Wrap( -1 );
	bSizerFontFace->Add( m_staticTextFont, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    m_staticTextFontFaceExample = new wxStaticText( sbSizer6->GetStaticBox(), wxID_ANY, _("Example"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFontFaceExample->Wrap( -1 );
	bSizerFontFace->Add( m_staticTextFontFaceExample, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	m_fgSizerODSettings->Add( bSizerFontFace, 1, wxEXPAND, 5 );

    m_buttonTextFont = new wxButton( sbSizer6->GetStaticBox(), wxID_ANY, _("Fonts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_fgSizerODSettings->Add( m_buttonTextFont, 0, wxALIGN_RIGHT|wxALL, 5 );


    sbSizer6->Add( m_fgSizerODSettings, 1, wxEXPAND, 5 );

	wxString m_radioBoxShowDisplayTextChoices[] = { _("Always"), _("On Rollover Only"), _("Never") };
	int m_radioBoxShowDisplayTextNChoices = sizeof( m_radioBoxShowDisplayTextChoices ) / sizeof( wxString );
    m_radioBoxShowDisplayText = new wxRadioBox( sbSizer6->GetStaticBox(), wxID_ANY, _("Show Display Text"), wxDefaultPosition, wxDefaultSize, m_radioBoxShowDisplayTextNChoices, m_radioBoxShowDisplayTextChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioBoxShowDisplayText->SetSelection( 0 );
    sbSizer6->Add( m_radioBoxShowDisplayText, 0, wxALL, 5 );


    sbODSettings->Add( sbSizer6, 1, wxEXPAND, 5 );


    rendering->SetSizer( sbODSettings );
    rendering->Layout();
    sbODSettings->Fit( rendering );
    m_notebook1->AddPage( rendering, _("POI Rendering"), false );
    m_panel3 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxStaticBoxSizer* downloads;
    downloads = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, _("Google Maps") ), wxVERTICAL );

    wxFlexGridSizer* fgSizer6;
#ifdef __WXOSX__
    fgSizer6 = new wxFlexGridSizer( 2, 0, 0 );
#else
    fgSizer6 = new wxFlexGridSizer( 0, 2, 0, 0 );
#endif
    fgSizer6->SetFlexibleDirection( wxBOTH );
    fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText11 = new wxStaticText( downloads->GetStaticBox(), wxID_ANY, _("Zoom Levels"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText11->Wrap( -1 );
    fgSizer6->Add( m_staticText11, 0, wxALL, 5 );

    m_textZoomLevels = new wxTextCtrl( downloads->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_textZoomLevels->SetToolTip( _("Download one satellite map for each destination and for each of these zoom levels. Separate values with a comma. Minimum zoom level is 10, maximum 18. \n\nExample: 15,17. \n\n15 is typically suitable approaching a destination, 17 for detailed map of marina or anchorage. Keep zoom levels to a minimum to reduce download times. ") );

    fgSizer6->Add( m_textZoomLevels, 0, wxALL, 5 );

    m_staticText12 = new wxStaticText( downloads->GetStaticBox(), wxID_ANY, _("Download .kap files to:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText12->Wrap( -1 );
    fgSizer6->Add( m_staticText12, 0, wxALL, 5 );

    m_dirPickerDownload = new wxDirPickerCtrl( downloads->GetStaticBox(), wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE );
    m_dirPickerDownload->SetToolTip( _("Select directory where you want to save downloaded maps. Must be one of your OpenCPN chart directories.") );


    fgSizer6->Add( m_dirPickerDownload, 0, wxALL, 5 );

    m_staticText14 = new wxStaticText( downloads->GetStaticBox(), wxID_ANY, _("Include viewport map"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText14->Wrap( -1 );
    fgSizer6->Add( m_staticText14, 0, wxALL, 5 );

    m_checkBoxVPMap = new wxCheckBox( downloads->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxVPMap->SetValue(true);
    m_checkBoxVPMap->SetToolTip( _("In addition to maps for each destination, download a smaller-scale map to cover the entire viewport area") );

    fgSizer6->Add( m_checkBoxVPMap, 0, wxALL, 5 );


    downloads->Add( fgSizer6, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer8;
    sbSizer8 = new wxStaticBoxSizer( new wxStaticBox( downloads->GetStaticBox(), wxID_ANY, _("House Keeping") ), wxVERTICAL );

    m_checkBoxDelGpxs = new wxCheckBox( sbSizer8->GetStaticBox(), wxID_ANY, _("Delete .gpx files for hidden layers upon exiting OpenCPN"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxDelGpxs->SetToolTip( _("Delete downloaded POI files for sectors you have hidden. This limits the proliferation of unecessary files and reduced OpenCPN's computational burden.") );

    sbSizer8->Add( m_checkBoxDelGpxs, 0, wxALL, 5 );


    downloads->Add( sbSizer8, 1, wxEXPAND, 5 );

    m_panel3->SetSizer( downloads );
    m_panel3->Layout();
    downloads->Fit( m_panel3 );
    m_notebook1->AddPage( m_panel3, _("Downloads"), false );
    logSharing = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxStaticBoxSizer* sbLogSizer;
    sbLogSizer = new wxStaticBoxSizer( new wxStaticBox( logSharing, wxID_ANY, _("Share logs with sQuiddio") ), wxVERTICAL );

    wxFlexGridSizer* fgSizerLogs;
#ifdef __WXOSX__
    fgSizerLogs = new wxFlexGridSizer( 2, 0, 0 );
#else
    fgSizerLogs = new wxFlexGridSizer( 0, 2, 0, 0 );
#endif
    fgSizerLogs->SetFlexibleDirection( wxBOTH );
    fgSizerLogs->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_staticText6 = new wxStaticText( sbLogSizer->GetStaticBox(), wxID_ANY, _("Send"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText6->Wrap( -1 );
    fgSizerLogs->Add( m_staticText6, 0, wxALL, 5 );

    wxString m_choiceHowOftenChoices[] = { _("Never"), _("Once a day"), _("Twice a day"), _("Once an hour"), _("Twice an hour"), _("Once a minute"), _("Twice a minute"), _("Every 10 seconds") };
    int m_choiceHowOftenNChoices = sizeof( m_choiceHowOftenChoices ) / sizeof( wxString );
    m_choiceHowOften = new wxChoice( sbLogSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceHowOftenNChoices, m_choiceHowOftenChoices, 0 );
    m_choiceHowOften->SetSelection( 0 );
    m_choiceHowOften->SetMinSize( wxSize( 250,-1 ) );

    fgSizerLogs->Add( m_choiceHowOften, 0, wxALL, 5 );

    m_staticText4 = new wxStaticText( sbLogSizer->GetStaticBox(), wxID_ANY, _("Receive"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4->Wrap( -1 );
    fgSizerLogs->Add( m_staticText4, 0, wxALL, 5 );

    wxString m_choiceReceiveChoices[] = { _("Never"), _("Once a day"), _("Twice a day"), _("Once an hour"), _("Twice an hour"), _("Once a minute"), _("Twice a minute"), _("Every 10 seconds") };
    int m_choiceReceiveNChoices = sizeof( m_choiceReceiveChoices ) / sizeof( wxString );
    m_choiceReceive = new wxChoice( sbLogSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceReceiveNChoices, m_choiceReceiveChoices, 0 );
    m_choiceReceive->SetSelection( 0 );
    m_choiceReceive->SetMinSize( wxSize( 250,-1 ) );

    fgSizerLogs->Add( m_choiceReceive, 0, wxALL, 5 );

    sbLogSizer->Add( fgSizerLogs, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer11;
    sbSizer11 = new wxStaticBoxSizer( new wxStaticBox( sbLogSizer->GetStaticBox(), wxID_ANY, _("Logs Sharing Options") ), wxVERTICAL );

    m_checkBoxSendXml = new wxCheckBox( sbSizer11->GetStaticBox(), wxID_ANY, _("Include NMEA samples with each send"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxSendXml->SetValue(true);
    m_checkBoxSendXml->SetToolTip( _("Include your Navobj.xml file each time you post an automatic update to sQuidd.io. Your navobj.xml file includes your navigation data (positions, tracks, routes etc) and other session information. You can retrieve previous versions of your navobj.xml file online in case of a crash.") );

    sbSizer11->Add( m_checkBoxSendXml, 0, wxALL, 5 );


    sbLogSizer->Add( sbSizer11, 1, wxEXPAND, 5 );

    logSharing->SetSizer( sbLogSizer );
    logSharing->Layout();
    sbLogSizer->Fit( logSharing );
    m_notebook1->AddPage( logSharing, _("Log Sharing"), false );
    m_panel5 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxStaticBoxSizer* sbSizer7;
    sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( m_panel5, wxID_ANY, _("Your sQuidd.io credentials") ), wxVERTICAL );

    m_staticText7 = new wxStaticText( sbSizer7->GetStaticBox(), wxID_ANY, _("sQuiddio ID"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText7->Wrap( -1 );
    sbSizer7->Add( m_staticText7, 0, wxALL, 5 );

    m_textSquiddioID = new wxTextCtrl( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_textSquiddioID->SetToolTip( _("The email address you use to log into sQuidd.io") );

    sbSizer7->Add( m_textSquiddioID, 0, wxALL|wxEXPAND, 5 );

    m_staticText8 = new wxStaticText( sbSizer7->GetStaticBox(), wxID_ANY, _("API Key"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText8->Wrap( -1 );
    sbSizer7->Add( m_staticText8, 0, wxALL, 5 );

    m_textApiKey = new wxTextCtrl( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_textApiKey->SetToolTip( _("Retrieve your API Key from your sQuidd.io profile. Log into http://squidd.io and click Edit Profile in the Dashboard, then select the Numbers and Keys tab. Click Help for more information. \n\nIMPORTANT! Do not use your sQuiddio Password as your API Key! ") );

    sbSizer7->Add( m_textApiKey, 0, wxALL|wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer10;
    sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString ), wxVERTICAL );

    sbSizer7->Add( sbSizer10, 1, wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer9;
    sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( sbSizer7->GetStaticBox(), wxID_ANY, _("About this plugin") ), wxVERTICAL );

    sbSizer9->SetMinSize( wxSize( 12,1 ) );
    m_version = new wxStaticText( sbSizer9->GetStaticBox(), wxID_ANY, _("version"), wxDefaultPosition, wxDefaultSize, 0 );
    m_version->Wrap( -1 );
    sbSizer9->Add( m_version, 0, wxALL, 5 );


    sbSizer7->Add( sbSizer9, 1, wxEXPAND, 5 );

    m_panel5->SetSizer( sbSizer7 );
    m_panel5->Layout();
    sbSizer7->Fit( m_panel5 );
    m_notebook1->AddPage( m_panel5, _("Authentication"), false );

    m_fgSubSizer->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );


	m_fgMainSizer->Add( m_fgSubSizer, 0, wxEXPAND, 5 );

	m_sdbButtonSizer = new wxStdDialogButtonSizer();
	m_sdbButtonSizerOK = new wxButton( this, wxID_OK );
	m_sdbButtonSizer->AddButton( m_sdbButtonSizerOK );
	m_sdbButtonSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbButtonSizer->AddButton( m_sdbButtonSizerCancel );
	m_sdbButtonSizerHelp = new wxButton( this, wxID_HELP );
	m_sdbButtonSizer->AddButton( m_sdbButtonSizerHelp );
	m_sdbButtonSizer->Realize();

	m_fgMainSizer->Add( m_sdbButtonSizer, 1, wxEXPAND|wxFIXED_MINSIZE, 5 );


	this->SetSizer( m_fgMainSizer );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_checkBoxAll->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SquiddioPrefsDialogBase::OnCheckBoxAll ), NULL, this );
    m_buttonTextFont->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SquiddioPrefsDialogBase::OnButtonClickFonts ), NULL, this );
	m_sdbButtonSizerCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SquiddioPrefsDialogBase::m_sdbButtonSizerOnCancelButtonClick ), NULL, this );
	m_sdbButtonSizerHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SquiddioPrefsDialogBase::LaunchHelpPage ), NULL, this );
	m_sdbButtonSizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SquiddioPrefsDialogBase::m_sdbButtonSizerOnOKButtonClick ), NULL, this );
}

SquiddioPrefsDialogBase::~SquiddioPrefsDialogBase()
{
	// Disconnect Events
	m_checkBoxAll->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SquiddioPrefsDialogBase::OnCheckBoxAll ), NULL, this );
    m_buttonTextFont->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SquiddioPrefsDialogBase::OnButtonClickFonts ), NULL, this );
	m_sdbButtonSizerCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SquiddioPrefsDialogBase::m_sdbButtonSizerOnCancelButtonClick ), NULL, this );
	m_sdbButtonSizerHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SquiddioPrefsDialogBase::LaunchHelpPage ), NULL, this );
	m_sdbButtonSizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SquiddioPrefsDialogBase::m_sdbButtonSizerOnOKButtonClick ), NULL, this );

}

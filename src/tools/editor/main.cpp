#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <filesystem>

#include "viewport.h"

namespace fs = std::filesystem;

enum class Tool {
	Selection,
	Entity,
	Brush,
	Texture,
	Clip
};

static fs::path root_path;
static fs::path res_path;
static Tool current_tool = Tool::Selection;

class BoomEd final: public wxApp {
	wxFrame* m_pFrame;
public:
	bool OnInit() override;
};

class BoomEdNewMapDialogue final: public wxDialog {
	wxStaticText* m_lbMapName;
	wxTextCtrl* m_txMapName;
	wxStaticText* m_lbMapFormat;
	wxChoice* m_chMapFormat;
	wxButton* m_btnOk;
	wxButton* m_btnCancel;
public:
	BoomEdNewMapDialogue(wxWindow* parent): wxDialog(parent, wxID_ANY, "New Map") {
		SetSizeHints( wxDefaultSize, wxDefaultSize );

		wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );

		m_lbMapName = new wxStaticText( this, wxID_ANY, _("Map Name"), wxDefaultPosition, wxDefaultSize, 0 );
		m_lbMapName->Wrap( -1 );
		bSizerMain->Add( m_lbMapName, 0, wxALL, 5 );

		m_txMapName = new wxTextCtrl( this, wxID_ANY, _("Unnamed"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizerMain->Add( m_txMapName, 0, wxALL, 5 );

		m_lbMapFormat = new wxStaticText( this, wxID_ANY, _("Map format"), wxDefaultPosition, wxDefaultSize, 0 );
		m_lbMapFormat->Wrap( -1 );
		bSizerMain->Add( m_lbMapFormat, 0, wxALL, 5 );

		wxString m_chMapFormatChoices[] = { _("Boom Map format (*.bed)"), _("Quake Map format (*.map)"), _("Valve Map format (*.vmf)") };
		int m_chMapFormatNChoices = sizeof( m_chMapFormatChoices ) / sizeof( wxString );
		m_chMapFormat = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_chMapFormatNChoices, m_chMapFormatChoices, 0 );
		m_chMapFormat->SetSelection( 0 );
		bSizerMain->Add( m_chMapFormat, 0, wxALL, 5 );

		wxBoxSizer* bSizerButtons = new wxBoxSizer( wxHORIZONTAL );

		m_btnOk = new wxButton( this, wxID_OK, _("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizerButtons->Add( m_btnOk, 0, wxALL, 5 );

		m_btnCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizerButtons->Add( m_btnCancel, 0, wxALL, 5 );

		bSizerMain->Add( bSizerButtons, 1, wxEXPAND, 5 );

		SetSizer( bSizerMain );
		Layout();

		Centre( wxBOTH );
	}

	[[nodiscard]] inline wxString mapName() const {
		return m_txMapName->GetValue();
	}
	[[nodiscard]] inline int mapFormat() const {
		return m_chMapFormat->GetSelection();
	}
};

enum {
	ID_NewMap = wxID_HIGHEST + 1,
	ID_OpenMap,
	ID_SaveMap,
	ID_SaveMapAs,
};

class BoomEdFrame final: public wxMDIParentFrame {
	wxMenuBar* m_menubar;
	wxMenu* m_menuFile;
	wxStatusBar* m_statusBar;
	wxToolBar* m_toolBar;

	wxNotebook* m_panelProperties;
	wxPanel* m_entitiesPage;
	wxPanel* m_texturesPage;
	wxSizerItem* m_viewportSizer;

	std::vector<wxMDIChildFrame*> m_vecChildren { };
	std::vector<wxBitmapButton*> m_entityButtons { };
	std::vector<wxBitmapButton*> m_textureButtons { };
public:
	BoomEdFrame(): wxMDIParentFrame(nullptr, wxID_ANY, "BoomEd", wxDefaultPosition) {
		this->SetSizeHints( wxDefaultSize, wxDefaultSize );

		m_menuFile = new wxMenu();
		m_menuFile->Append(ID_NewMap, "&New Map\tCtrl-N");
		m_menuFile->Append(ID_OpenMap, "&Open Map\tCtrl-O");
		m_menuFile->Append(ID_SaveMap, "&Save Map\tCtrl-S");
		m_menuFile->Append(ID_SaveMapAs, "S&ave Map As..\tCtrl-Shift-S");
		m_menuFile->AppendSeparator();
		m_menuFile->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");

		m_menubar = new wxMenuBar();
		m_menubar->Append(m_menuFile, "&File");
		SetMenuBar(m_menubar);

		auto select_icon = res_path / "tool_select.png";
		auto entity_icon = res_path / "tool_entity.png";
		auto brush_icon = res_path / "tool_brush.png";
		auto texture_icon = res_path / "tool_texture.png";
		auto clip_icon = res_path / "tool_clip.png";

		m_toolBar = CreateToolBar();
		m_toolBar->AddTool(1001, "Selection Tool",
			wxBitmap(select_icon.c_str(), wxBITMAP_TYPE_PNG), "Selection Tool:\nSelect entities or brushes.", wxITEM_RADIO);
		m_toolBar->AddTool(1002, "Entity Tool",
			wxBitmap(entity_icon.c_str(), wxBITMAP_TYPE_PNG), "Entity Tool:\nPlace new entities.", wxITEM_RADIO);
		m_toolBar->AddTool(1003, "Brush Tool",
			wxBitmap(brush_icon.c_str(), wxBITMAP_TYPE_PNG), "Brush Tool:\nCreate new world geometry.", wxITEM_RADIO);
		m_toolBar->AddTool(1004, "Texture Tool",
			wxBitmap(texture_icon.c_str(), wxBITMAP_TYPE_PNG), "Texture Tool:\nApply texture to world geometry.", wxITEM_RADIO);
		m_toolBar->AddTool(1005, "Clip Tool",
			wxBitmap(clip_icon.c_str(), wxBITMAP_TYPE_PNG), "Clip Tool:\nTool to cut brushes.", wxITEM_RADIO);
		m_toolBar->Realize();

		SetToolBar(m_toolBar);

		m_statusBar = CreateStatusBar(1, wxSTB_SIZEGRIP, wxID_ANY);
		SetStatusBar(m_statusBar);
		SetStatusText("BoomEd v0.1 Alpha");

		wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
		m_viewportSizer = mainSizer->Add(GetClientWindow(), 4, wxEXPAND, 2);

		m_panelProperties = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_RIGHT );
		m_entitiesPage = new wxPanel( m_panelProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
		m_panelProperties->AddPage( m_entitiesPage, _("Entities"), false );
		m_texturesPage = new wxPanel( m_panelProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
		m_panelProperties->AddPage( m_texturesPage, _("Textures"), false );

		wxGridSizer* entitiesGrid = new wxGridSizer(0, 4, 0, 0);
		m_entitiesPage->SetSizer(entitiesGrid);

		wxGridSizer* texturesGrid = new wxGridSizer(0, 4, 0, 0);
		m_texturesPage->SetSizer(texturesGrid);

		mainSizer->Add(m_panelProperties, 1, wxEXPAND | wxALL, 2);

		SetSizer(mainSizer);
		Layout();

		Center(wxBOTH);

		Bind(wxEVT_MENU, &BoomEdFrame::OnNewMap, this, ID_NewMap);
		Bind(wxEVT_MENU, &BoomEdFrame::OnExit, this, wxID_EXIT);
		Bind(wxEVT_TOOL, &BoomEdFrame::OnToolSelected, this, 1001);
		Bind(wxEVT_TOOL, &BoomEdFrame::OnToolSelected, this, 1002);
		Bind(wxEVT_TOOL, &BoomEdFrame::OnToolSelected, this, 1003);
		Bind(wxEVT_TOOL, &BoomEdFrame::OnToolSelected, this, 1004);
		Bind(wxEVT_TOOL, &BoomEdFrame::OnToolSelected, this, 1005);
	}

	void OnNewMap(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event) {
		Close(true);
	}
	void OnToolSelected(wxCommandEvent& event);
	void RemoveChild(const wxMDIChildFrame* child) {
		auto it = std::find(m_vecChildren.begin(), m_vecChildren.end(), child);
		if (it != m_vecChildren.end()) {
			m_vecChildren.erase(it);
		}
	}
};

class BoomEdMapEditorFrame final: public wxMDIChildFrame {
	BoomEdViewport* m_viewport;
public:
	BoomEdMapEditorFrame(wxMDIParentFrame* parent, const wxString& title): wxMDIChildFrame(parent, wxID_ANY, title) {
		m_viewport = new BoomEdViewport(this);

		wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(m_viewport, 1, wxEXPAND);

		SetSizer(sizer);
		Layout();

		Center(wxBOTH);

		Bind(wxEVT_MENU, &BoomEdMapEditorFrame::OnClose, this, wxID_CLOSE);
	}

	void OnClose(wxCommandEvent& event);
};

inline bool BoomEd::OnInit() {
	root_path = fs::path(argv[0].c_str().AsChar()).parent_path().parent_path();
	res_path = root_path / "res" / "editor";

	wxInitAllImageHandlers();
	m_pFrame = new BoomEdFrame();
	m_pFrame->Show(true);
	m_pFrame->Maximize(true);

	auto iconPath = res_path/ "window_icon.png";
	if (fs::exists(iconPath)) {
		auto bitmap = wxBitmap(iconPath.c_str(), wxBITMAP_TYPE_PNG);
		wxIcon icon;
		icon.CopyFromBitmap(bitmap);
		m_pFrame->SetIcon(icon);
	}

	return true;
}

inline void BoomEdFrame::OnNewMap(wxCommandEvent& event) {
	auto diag = new BoomEdNewMapDialogue(this);
	if (diag->ShowModal() == wxID_OK) {
		auto name = diag->mapName();
		auto format = diag->mapFormat();
		auto window = new BoomEdMapEditorFrame(this, name);
		window->Show(true);
		m_vecChildren.push_back(window);
	}
	delete diag;
}

void BoomEdFrame::OnToolSelected(wxCommandEvent& event) {
	int id = event.GetId();
	switch (id) {
		case 1001: {
			current_tool = Tool::Selection;
		} break;
		case 1002: {
			current_tool = Tool::Entity;
		} break;
		case 1003: {
			current_tool = Tool::Brush;
		} break;
		case 1004: {
			current_tool = Tool::Texture;
		} break;
		case 1005: {
			current_tool = Tool::Clip;
		} break;
		default: /* Do nothing */ break;
	}
}

void BoomEdMapEditorFrame::OnClose(wxCommandEvent& event) {
	auto frame = dynamic_cast<BoomEdFrame*>(m_parent);
	if (frame != nullptr) {
		frame->RemoveChild(this);
	}
}

wxIMPLEMENT_APP(BoomEd);

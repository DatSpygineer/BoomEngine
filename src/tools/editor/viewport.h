#pragma once

#include <GL/glew.h>

#include <wx/wx.h>
#include <wx/glcanvas.h>

class BoomEdViewport final: public wxGLCanvas {
	wxGLContext* m_context;
public:
	BoomEdViewport(wxFrame* parent);

protected:
	void InitOpenGL();

	void OnPaintGL(wxPaintEvent& event);
	void OnResize(wxSizeEvent& event);
};
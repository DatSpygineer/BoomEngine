#include "viewport.h"

BoomEdViewport::BoomEdViewport(wxFrame* parent): wxGLCanvas(parent, wxID_ANY, nullptr) {
	m_context = new wxGLContext(this);

	Bind(wxEVT_PAINT, &BoomEdViewport::OnPaintGL, this);
	Bind(wxEVT_SIZE, &BoomEdViewport::OnResize, this);

	// Initialize OpenGL once
	InitOpenGL();

	Show();
}

void BoomEdViewport::InitOpenGL() {
	(void)SetCurrent(*m_context);

	// Initialize GLEW
	if (GLenum err = glewInit(); err != GLEW_OK) {
		wxLogError(wxString(wxT("Failed to initialize OpenGL: ")) + reinterpret_cast<const char*>(glewGetErrorString(err)));
		return;
	}

	// Set up OpenGL settings (basic setup)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void BoomEdViewport::OnPaintGL(wxPaintEvent& event) {
	wxPaintDC dc(this);
	(void)SetCurrent(*m_context);

	// Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw a simple triangle using immediate mode
	glBegin(GL_TRIANGLES);

	// Red vertex
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(-0.5f, -0.5f);

	// Green vertex
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.5f, -0.5f);

	// Blue vertex
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(0.0f, 0.5f);

	glEnd();

	SwapBuffers();
}

void BoomEdViewport::OnResize(wxSizeEvent& event) {
	wxSize size = GetClientSize();

	// Make the context current before modifying viewport
	(void)SetCurrent(*m_context);

	// Adjust OpenGL viewport to the new window size
	glViewport(0, 0, size.x, size.y);

	// Allow wxWidgets to process the event further
	event.Skip();
}

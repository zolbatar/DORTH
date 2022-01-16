#include "UISDL.h"
#include "../Console/TerminalOutput.h"
#include <imgui.h>
#include "../ImGuiBindings/imgui_impl_sdl.h"
#include "../ImGuiBindings/imgui_impl_opengl3.h"

UISDL::UISDL() {
	TerminalInit("SDL/UI");
	if (SDL_Init(SDL_INIT_EVERYTHING)<0) {
		std::cout << "SDL initialization failed. SDL Error: " << SDL_GetError() << std::endl;
	}

	float ddpi, hdpi, vdpi;
	if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi)!=0) {
		std::cout << bright_red << "ERROR: " << bright_white << "Failed to obtain DPI information for display 0: \n"
				  << SDL_GetError() << reset << std::endl;
		exit(1);
	}
#ifdef __APPLE__
	dpi_ratio = ddpi/96;
#else
	dpi_ratio = ddpi/72;
#endif
	dpi_ratio = roundf(dpi_ratio);
	//	if (dpiRatio > 1.0 && dpiRatio < 2.0) dpiRatio = 2.0;
	std::cout << white << "DPI ratio: " << bright_yellow << dpi_ratio << std::endl;

	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm)!=0) {
		std::cout << bright_red << "ERROR: " << bright_white << "SDL_GetDesktopDisplayMode failed: " << SDL_GetError()
				  << reset << std::endl;
		exit(1);
	}
	desktop_screen_width = dm.w;
	desktop_screen_height = dm.h;
	std::cout << white << "Desktop resolution: " << bright_yellow << desktop_screen_width << white << "x"
			  << bright_yellow
			  << desktop_screen_height << std::endl;

	console.Setup(desktop_screen_width, desktop_screen_height, dpi_ratio,
			desktop_screen_width/16, desktop_screen_height/32, false);
}

UISDL::~UISDL() {
	TerminalShutdown("SDL/UI");
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void UISDL::Start() {
	TerminalInit("ImGui");

	// Decide GL+GLSL versions
	TerminalOut("Setting up", "OpenGL version");
#if defined(IMGUI_IMPL_OPENGL_ES2)
	TerminalOut("OpenGL", "GL ES 2.0 + GLSL 150");
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	TerminalOut("OpenGL", "GL 3.2 Core + GLSL 330");
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 330";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	TerminalOut("OpenGL", "GL 3.0 + GLSL 130");
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	// Initialise SDL now and create window
	_CreateWindow();

	// Create context in window
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	TerminalOut("Setting up", "Dear ImGui");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.Fonts->Clear();
	const float font_size = 10.0f;
	io.Fonts->AddFontFromFileTTF("IBMPlexSans-Regular.ttf", font_size*dpi_ratio);
	auto fontMono = io.Fonts->AddFontFromFileTTF("IBMPlexMono-Regular.ttf", font_size*dpi_ratio);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	TerminalOut("Setting up", "Dear ImGui platform");
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool done = false;
	bool window_output;
	while (!done) {

		// Process SDL events like keyboard & mouse
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type==SDL_QUIT)
				done = true;
			if (event.type==SDL_WINDOWEVENT && event.window.event==SDL_WINDOWEVENT_CLOSE
					&& event.window.windowID==SDL_GetWindowID(window))
				done = true;
		}

		glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		// Main menu bar
/*		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New")) {
					//Do something
				}
				if (ImGui::MenuItem("Quit")) {
					done = true;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}*/

		// Full screen stuff
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::SetNextWindowPos(main_viewport->Pos);
		ImGui::SetNextWindowSize(main_viewport->Size);
		ImGui::Begin("Fullscreen", &window_output,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoBackground);
		console.Update(fontMono);
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		// Now render
		ImGui::Render();

		// Show
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}
}

void UISDL::_CreateWindow() {
	// Create window
	TerminalOut("Creating", "SDL window");
	SDL_WindowFlags
			window_flags = (SDL_WindowFlags)(
			SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI);
	window = SDL_CreateWindow("DORTH", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, window_flags);
	if (window==nullptr) {
		std::cout << bright_red << "ERROR: " << bright_white << "Window could not be created. SDL_Error: "
				  << SDL_GetError()
				  << reset << std::endl;
		exit(1);
	}
}


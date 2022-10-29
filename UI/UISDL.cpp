#include <iostream>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include "UISDL.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "Lodepng.h"
#include "../Console/Console.h"
#include "../Compiler/Compiler.h"

size_t frame_count = 0;
extern Console console;
extern Compiler compiler;

UISDL::UISDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL initialization failed. SDL Error: " << SDL_GetError() << std::endl;
	}

	float ddpi, hdpi, vdpi;
	if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi) != 0)
	{
		std::cout << "ERROR: Failed to obtain DPI information for display 0: \n" << SDL_GetError() << std::endl;
		exit(1);
	}
	dpi_ratio = ddpi / 72.0f;
	std::cout << "DPI ratio: " << dpi_ratio << std::endl;

	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		std::cout << "ERROR: SDL_GetDesktopDisplayMode failed: " << SDL_GetError() << std::endl;
		exit(1);
	}
	desktop_screen_width = dm.w;
	desktop_screen_height = dm.h;
	std::cout << "Desktop resolution: " << desktop_screen_width << "x" << desktop_screen_height << std::endl;
}

UISDL::~UISDL()
{
	std::cout << "Shutting down SDL" << std::endl;
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void UISDL::Start()
{
	// Decide GL+GLSL versions
	std::cout << "Setting up OpenGL version" << std::endl;
#if defined(IMGUI_IMPL_OPENGL_ES2)
	TerminalOut("OpenGL", "GL ES 2.0 + GLSL 150");
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	std::cout << "OpenGL:GL 3.2 Core + GLSL 330" << std::endl;
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 330";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	std::cout << "OpenGL:GL GL 3.0 + GLSL 130" << std::endl;
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
	std::cout << "Setting up Dear ImGui" << std::endl;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.Fonts->Clear();
	fontMono = io.Fonts->AddFontFromFileTTF("BerkeleyMonoVariable-Regular.ttf", font_size * dpi_ratio);
	io.FontGlobalScale /= dpi_ratio;
	io.Fonts->Build();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();

	// Output window
	console.Setup(desktop_screen_width, desktop_screen_height, dpi_ratio, desktop_screen_width / console_x_size,
		desktop_screen_height / console_y_size, false);

	// FPS stuff
	float fps_values[64];
	std::string fps_text = "0 FPS";
	uint32_t fps_offset = 0;
	for (auto i = 0; i < 64; i++)
	{
		fps_values[i] = 0.0f;
	}
	std::chrono::steady_clock::time_point fps_clock = std::chrono::steady_clock::now();

	// Setup Platform/Renderer backends
	std::cout << "Setting up Dear ImGui backend" << std::endl;
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Welcome
	console.WriteString("Welcome to Dorth! @thezolbatar\n\n");

	bool done = false;
	bool demo_output = false;
	std::string cmd;
	while (!done)
	{
		// Process SDL events like keyboard & mouse
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
				&& event.window.windowID == SDL_GetWindowID(window))
				done = true;
			if (event.type == SDL_TEXTINPUT)
			{
				cmd += event.text.text;
				console.WriteCharacter(cmd.back());
			}
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_BACKSPACE:
					{
						if (!cmd.empty())
						{
							console.DeleteCharacter();
							cmd.pop_back();
						}
						break;
					}
					case SDLK_RETURN:
					{
						console.WriteString("\n");
						compiler.Compile(cmd);
						console.WriteString("ok\n");
						cmd.clear();
						break;
					}
				}
			}
		}

		// This is so app thread can lock to load fonts etc before start of frame
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		bool window_output = true;
		glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		// Render child windows
		if (demo_output)
		{
			ImGui::ShowDemoWindow(&demo_output);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::SetNextWindowPos(main_viewport->Pos);
		ImGui::SetNextWindowSize(main_viewport->Size);
		ImGui::Begin("Fullscreen", &window_output,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBackground);
		console.Update(fontMono);
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		// FPS histogram
/*		ImGui::SetNextWindowPos(ImVec2(desktop_screen_width - 100, desktop_screen_height - 100));
		ImGui::SetNextWindowSize(ImVec2(100, 50));
		ImGui::Begin("FPS", &window_output,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
		ImGui::PlotHistogram("FPS", fps_values, IM_ARRAYSIZE(fps_values), 0, NULL, 0.0f, 100.0f, ImVec2(100, 50.0f));
		ImGui::TextUnformatted(fps_text.c_str());
		ImGui::End();*/

		// Now render
		ImGui::Render();

		// Show
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}
}

void UISDL::_CreateWindow()
{
	// Create window
	std::cout << "Creating SDL window\n";
	SDL_WindowFlags window_flags;
	if (fullscreen)
	{
		window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI);
		//window_flags = (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
		window = SDL_CreateWindow("Daric",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			desktop_screen_width,
			desktop_screen_height,
			window_flags);
	}
	else
	{
#ifdef __x86_64__
		desktop_screen_width = 1280;
		desktop_screen_height = 720;
#else
		desktop_screen_width = 1280;
		desktop_screen_height = 720;
#endif
		window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
		window = SDL_CreateWindow("Daric",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			desktop_screen_width,
			desktop_screen_height,
			window_flags);
	}
	if (window == nullptr)
	{
		std::cout << "ERROR: Window could not be created. SDL_Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
}

bool UISDL::LoadTextureFromFile(const char* filename, GLuint* outTexture, int* outWidth, int* outHeight)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
	{
		std::cout << "ERROR: Image " << filename << " not found" << std::endl;
		exit(1);
	}

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*outTexture = image_texture;
	*outWidth = image_width;
	*outHeight = image_height;

	return true;
}
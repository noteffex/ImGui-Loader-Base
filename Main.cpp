#include "Main.h"

char user_name[255] = "user";
char pass_word[255] = "pass";

void manage_window_movement() {
    //TODO: Find a better way for moving window than this.
    if (ImGui::IsItemHovered() || ImGui::IsItemActive()) { should_move_window = false; }
}

// Main code
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    //Calculate the window position of our loader
    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect);
    int x = (screen_rect.right - WINDOW_WIDTH) / 2;
    int y = (screen_rect.bottom - WINDOW_HEIGHT) / 2;

    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, LOADER_BRAND, NULL };
    RegisterClassEx(&wc);
    main_hwnd = CreateWindow(wc.lpszClassName, LOADER_BRAND, WS_POPUP, x, y, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ShowWindow(main_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(main_hwnd);

    // Setup Dear ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    style.WindowRounding = 0.f;

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    //Initialize fonts here
    io.Fonts->AddFontDefault();

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (!loader_active) ExitProcess(0);
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Actual rendering starts here
		DWORD window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));
		ImGui::SetNextWindowBgAlpha(1.0f);

        should_move_window = true;
        ImGui::Begin(LOADER_BRAND, &loader_active, window_flags);
		{
            ImGui::InputText("Username", user_name, IM_ARRAYSIZE(user_name));
            manage_window_movement();
            ImGui::InputText("Password", pass_word, IM_ARRAYSIZE(pass_word), ImGuiInputTextFlags_Password);
            manage_window_movement();
            if (ImGui::Button("Login")) {
                //DO YOUR LOGIN HERE
            }
        }
        ImGui::End();

        ImGui::EndFrame();
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

        if (g_pd3dDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        // Handle loss of D3D9 device
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message) {
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
			if (hr == D3DERR_INVALIDCALL) IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		return 0;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

    if (ImGui::GetCurrentContext()) {
        static POINTS m;
        switch (message)
        {
        case WM_LBUTTONDOWN:
            SetCapture(hWnd);
            m = MAKEPOINTS(lParam);
            return true;
        case WM_LBUTTONUP:
            ReleaseCapture();
            return true;
        case WM_MOUSEMOVE:
            if (wParam == MK_LBUTTON)
            {
                if (should_move_window && !ImGui::GetIO().WantTextInput) {
                    RECT rect;
                    GetWindowRect(hWnd, &rect);
                    POINTS p = MAKEPOINTS(lParam);

                    rect.left += p.x - m.x;
                    rect.top += p.y - m.y;

                    SetWindowPos(hWnd, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
                }
                else {
                    m = MAKEPOINTS(lParam);
                }
            }
        }
    }

	return DefWindowProc(hWnd, message, wParam, lParam);
}
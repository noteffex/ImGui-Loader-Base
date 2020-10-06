#pragma once
#include <iostream>
#include <windows.h>
#include <thread>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")

#define LOADER_BRAND "ImGui Loader By EffeX"
#define WINDOW_WIDTH  400
#define WINDOW_HEIGHT 300

static bool loader_active = true;
HWND hWnd = nullptr;

ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp;
extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 伊地田真衣
//
//=============================================================================
#include "Main.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include "AssimpModel.h"
#include "input.h"
#include "debugproc.h"
#include "mesh.h"
#include "Scene.h"
#include "number.h"
#include "Sound.h"
#include "polygon.h"

//-------- ライブラリのリンク
#pragma comment(lib, "winmm")
#pragma comment(lib, "imm32")
#pragma comment(lib, "d3d11")

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		_T("AppClass")			// ウインドウのクラス名
#define WINDOW_NAME		_T("imgui")			// ウインドウのキャプション名

#define MAX_POLYLINE	(20)					// ポリライン数
#define THICKNESS		(10.0f)					// 線の太さ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT Init(HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;
ID3D11RasterizerState*		g_pRs[MAX_CULLMODE];	// ラスタライザ ステート
ID3D11BlendState*			g_pBlendState[MAX_BLENDSTATE];// ブレンド ステート
ID3D11DepthStencilState*	g_pDSS[2];				// Z/ステンシル ステート
ID3D11RenderTargetView*		g_pRenderTargetView;	// フレームバッファ
ID3D11Texture2D*			g_pDepthStencilTexture;	// Zバッファ用メモリ
ID3D11DepthStencilView*		g_pDepthStencilView;	// Zバッファ
UINT						g_uSyncInterval = 1;	// 垂直同期 (0:無, 1:有)
HWND g_hwnd;    // メインウィンドウハンドル
HINSTANCE					g_hInst;				// インスタンス ハンドル

// Forward declarations of helper functions
int	g_nCountFPS;			// FPSカウンタ

//=============================================================================
// メイン関数
//=============================================================================
int main(int, char**)
{
    //----- ウィンドウ作成 -----
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    g_hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, wc.hInstance, NULL);

    //----- 変数初期化 ------
    // FPS制御のための変数
    DWORD dwExecLastTime;
    DWORD dwFPSLastTime;
    DWORD dwCurrentTime;
    DWORD dwFrameCount;
    // メッセージ受け取りのための変数
    MSG msg;

    // COM初期化
    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        MessageBox(NULL, _T("COMの初期化に失敗しました。"), _T("error"), MB_OK);
        return -1;
    }

    // インスタンス ハンドル保存
    g_hInst = wc.hInstance;

    // Direct3D初期化
    if (!CreateDeviceD3D(g_hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // ゲーム関連初期化
    HRESULT hr;
    hr = Init(g_hwnd, true);
    if (FAILED(hr)) {
        return -1;
    }

    // Show the window
    ::ShowWindow(g_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(g_hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // DearImGuiのスタイルの変更
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


    // フレームカウント初期化
    timeBeginPeriod(1);				// 分解能を設定
    dwExecLastTime = dwFPSLastTime = timeGetTime();
    dwCurrentTime = dwFrameCount = 0;

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
#if 0
    //bool done = false;
    //while (!done)
    //{
    //    // Poll and handle messages (inputs, window resize, etc.)
    //    // See the WndProc() function below for our to dispatch events to the Win32 backend.
    //    MSG msg;
    //    while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    //    {
    //        ::TranslateMessage(&msg);
    //        ::DispatchMessage(&msg);
    //        if (msg.message == WM_QUIT)
    //            done = true;
    //    }
    //    if (done)
    //        break;

    //    // Start the Dear ImGui frame
    //    ImGui_ImplDX11_NewFrame();
    //    ImGui_ImplWin32_NewFrame();
    //    ImGui::NewFrame();

    //    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    //    if (show_demo_window)
    //        ImGui::ShowDemoWindow(&show_demo_window);

    //    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    //    {
    //        static float f = 0.0f;
    //        static int counter = 0;

    //        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    //        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    //        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    //        ImGui::Checkbox("Another Window", &show_another_window);

    //        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    //        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    //        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //            counter++;
    //        ImGui::SameLine();
    //        ImGui::Text("counter = %d", counter);

    //        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    //        ImGui::End();
    //    }

    //    // 3. Show another simple window.
    //    if (show_another_window)
    //    {
    //        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    //        ImGui::Text("Hello from another window!");
    //        if (ImGui::Button("Close Me"))
    //            show_another_window = false;
    //        ImGui::End();
    //    }

    //    // Rendering
    //    ImGui::Render();
    //    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    //    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    //    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    //    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    //    g_pSwapChain->Present(1, 0); // Present with vsync
    //    //g_pSwapChain->Present(0, 0); // Present without vsync
    //}
#endif

        // メッセージループ
    while(1){
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                // PostQuitMessage()が呼ばれたらループ終了
                break;
            } else {
                // メッセージの翻訳とディスパッチ
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            dwCurrentTime = timeGetTime();
            if ((dwCurrentTime - dwFPSLastTime) >= 500) {	// 0.5秒ごとに実行
                g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
                dwFPSLastTime = dwCurrentTime;
                dwFrameCount = 0;
            }
            if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60)) {
                dwExecLastTime = dwCurrentTime;
                // 更新処理
                Update();
            }

            RenderBegin();

            // 描画処理
            Draw();

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            ImGui::End();

            // Rendering
            //g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
            //const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
            //g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);

            RenderEnd();


            dwFrameCount++;
        }
    }

    //----- 終了処理 -----
    timeEndPeriod(1);				// 分解能を戻す

    Uninit();   // ゲームの終了処理

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd){
    HRESULT hr = S_OK;

    // デバイス、スワップチェーンの作成
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = SCREEN_WIDTH;
    sd.BufferDesc.Height = SCREEN_HEIGHT;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();

    // バックバッファ生成
    hr = CreateBackBuffer();
    if (FAILED(hr)) {
        return hr;
    }

    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    //g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

//=============================================================================
// バックバッファ生成
//=============================================================================
HRESULT CreateBackBuffer(void) {
    // Zバッファ用テクスチャ生成
    D3D11_TEXTURE2D_DESC td;
    ZeroMemory(&td, sizeof(td));
    td.Width = SCREEN_WIDTH;
    td.Height = SCREEN_HEIGHT;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    HRESULT hr = g_pd3dDevice->CreateTexture2D(&td, nullptr, &g_pDepthStencilTexture);
    if (FAILED(hr)) {
        return hr;
    }

    // Zバッファターゲットビュー生成
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    ZeroMemory(&dsvd, sizeof(dsvd));
    dsvd.Format = td.Format;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencilTexture,
        &dsvd, &g_pDepthStencilView);
    if (FAILED(hr)) {
        return hr;
    }

    // 各ターゲットビューをレンダーターゲットに設定
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    // ビューポート設定
    D3D11_VIEWPORT vp;
    vp.Width = (float)SCREEN_WIDTH;
    vp.Height = (float)SCREEN_HEIGHT;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pd3dDeviceContext->RSSetViewports(1, &vp);

    // ラスタライズ設定
    D3D11_RASTERIZER_DESC rd;
    ZeroMemory(&rd, sizeof(rd));
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;	// カリング無し(両面描画)
    g_pd3dDevice->CreateRasterizerState(&rd, &g_pRs[0]);
    rd.CullMode = D3D11_CULL_FRONT;	// 前面カリング(裏面描画)
    g_pd3dDevice->CreateRasterizerState(&rd, &g_pRs[1]);
    rd.CullMode = D3D11_CULL_BACK;	// 背面カリング(表面描画)
    g_pd3dDevice->CreateRasterizerState(&rd, &g_pRs[2]);
    g_pd3dDeviceContext->RSSetState(g_pRs[2]);

    // ブレンド ステート生成
    D3D11_BLEND_DESC BlendDesc;
    ZeroMemory(&BlendDesc, sizeof(BlendDesc));
    BlendDesc.AlphaToCoverageEnable = FALSE;
    BlendDesc.IndependentBlendEnable = FALSE;
    BlendDesc.RenderTarget[0].BlendEnable = FALSE;
    BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    g_pd3dDevice->CreateBlendState(&BlendDesc, &g_pBlendState[0]);

    // ブレンド ステート生成 (アルファ ブレンド用)
    //BlendDesc.AlphaToCoverageEnable = TRUE;
    BlendDesc.RenderTarget[0].BlendEnable = TRUE;
    g_pd3dDevice->CreateBlendState(&BlendDesc, &g_pBlendState[1]);
    // ブレンド ステート生成 (加算合成用)
    BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    g_pd3dDevice->CreateBlendState(&BlendDesc, &g_pBlendState[2]);
    // ブレンド ステート生成 (減算合成用)
    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
    g_pd3dDevice->CreateBlendState(&BlendDesc, &g_pBlendState[3]);
    SetBlendState(BS_ALPHABLEND);

    // 深度ステンシルステート生成
    CD3D11_DEFAULT def;
    CD3D11_DEPTH_STENCIL_DESC dsd(def);
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    g_pd3dDevice->CreateDepthStencilState(&dsd, &g_pDSS[0]);
    CD3D11_DEPTH_STENCIL_DESC dsd2(def);
    dsd2.DepthEnable = FALSE;
    g_pd3dDevice->CreateDepthStencilState(&dsd2, &g_pDSS[1]);

    return S_OK;
}

//=============================================================================
// バックバッファ解放
//=============================================================================
void ReleaseBackBuffer() {
    if (g_pd3dDeviceContext) {
        g_pd3dDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    }
    SAFE_RELEASE(g_pDepthStencilView);
    SAFE_RELEASE(g_pDepthStencilTexture);
    SAFE_RELEASE(g_pRenderTargetView);
}


void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HWND hWnd, BOOL bWindow) {
    HRESULT hr = S_OK;

    // ポリゴン表示初期化
    hr = InitPolygon(g_pd3dDevice);
    if (FAILED(hr))
        return hr;

    // デバッグ文字列表示初期化
    hr = InitDebugProc();
    if (FAILED(hr))
        return hr;

    // 入力処理初期化
    hr = InitInput();
    if (FAILED(hr))
        return hr;

    // Assimp用シェーダ初期化
    if (!CAssimpModel::InitShader(g_pd3dDevice))
        return E_FAIL;

    // メッシュ初期化
    hr = InitMesh();
    if (FAILED(hr))
        return hr;

    // 数字初期化
    hr = InitNumber();
    if (FAILED(hr))
        return hr;

    // サウンド初期化
    CSound::Init();

    // シーン初期化
    hr = InitScene();
    if (FAILED(hr)) {
        return hr;
    }

    // ボリライン初期化
    //hr = InitPolyline();
    //if (FAILED(hr))
    //	return hr;
    //XMFLOAT4 vColor[8] = {
    //	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
    //	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
    //	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
    //	XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f),
    //	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
    //	XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f),
    //	XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f),
    //	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //};
    //for (int i = 0; i < MAX_POLYLINE; ++i) {
    //	hr = CreatePolyline(&g_polyline[i], THICKNESS, true, vColor[i % 7 + 1], BS_ADDITIVE);
    //	if (FAILED(hr)) {
    //		return hr;
    //	}
    //	XMFLOAT3 pos;
    //	pos.x = rand() % 1240 - 620.0f;
    //	pos.y = rand() % 140 + 10.0f;
    //	pos.z = rand() % 1240 - 620.0f;
    //	AddPolylinePoint(&g_polyline[i], pos);
    //	pos.x = rand() % 1240 - 620.0f;
    //	pos.y = rand() % 140 + 10.0f;
    //	pos.z = rand() % 1240 - 620.0f;
    //	AddPolylinePoint(&g_polyline[i], pos);
    //}

    return hr;
}


//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void) {
    // ポリライン終了処理
    //for (int i = 0; i < MAX_POLYLINE; ++i) {
    //	ReleasePolyline(&g_polyline[i]);
    //}
    //UninitPolyline();

    // サウンド終了処理
    CSound::Fin();

    // シーン終了処理
    UninitScene();

    // 数字終了処理
    UninitNumber();

    // メッシュ終了処理
    UninitMesh();

    // Assimp用シェーダ終了処理
    CAssimpModel::UninitShader();

    // 入力処理終了処理
    UninitInput();

    // デバッグ文字列表示終了処理
    UninitDebugProc();

    // ポリゴン表示終了処理
    UninitPolygon();

    // 深度ステンシルステート解放
    for (int i = 0; i < _countof(g_pDSS); ++i) {
        SAFE_RELEASE(g_pDSS[i]);
    }

    // ブレンド ステート解放
    for (int i = 0; i < MAX_BLENDSTATE; ++i) {
        SAFE_RELEASE(g_pBlendState[i]);
    }

    // ラスタライザ ステート解放
    for (int i = 0; i < MAX_CULLMODE; ++i) {
        SAFE_RELEASE(g_pRs[i]);
    }

    // バックバッファ解放
    ReleaseBackBuffer();

    // スワップチェーン解放
    SAFE_RELEASE(g_pSwapChain);

    // デバイス コンテキストの開放
    SAFE_RELEASE(g_pd3dDeviceContext);

    // デバイスの開放
    SAFE_RELEASE(g_pd3dDevice);

    // ImGuiの終了処理
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    //CleanupDeviceD3D();
    //::DestroyWindow(g_hwnd);
    //::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void) {
    // 入力処理更新
    UpdateInput();	// 必ずUpdate関数の先頭で実行.

    // デバッグ文字列表示更新
    UpdateDebugProc();

    // デバッグ文字列設定
    StartDebugProc();
    PrintDebugProc("FPS:%d\n\n", g_nCountFPS);

    // ポリゴン表示更新
    UpdatePolygon();

    // カメラ更新
    CCamera::Get()->Update();

    // サウンド更新
    CSound::Update();

    // シーン更新
    UpdateScene();

    // ポリライン更新
    //for (int i = 0; i < MAX_POLYLINE; ++i) {
    //	UpdatePolyline(&g_polyline[i]);
    //}

}


//=============================================================================
// 描画処理
//=============================================================================
void Draw(void) {
    // ポリライン描画
    //for (int i = 0; i < MAX_POLYLINE; ++i) {
    //	DrawPolyline(&g_polyline[i]);
    //}

    // シーン描画
    DrawScene();


    // Zバッファ無効(Zチェック無&Z更新無)
    SetZBuffer(false);

    // デバッグ文字列表示
    SetBlendState(BS_ALPHABLEND);
    DrawDebugProc();
    SetBlendState(BS_NONE);
}


//=============================================================================
// メイン ウィンドウ ハンドル取得
//=============================================================================
HWND GetMainWnd() {
    return g_hwnd;
}

//=============================================================================
// インスタンス ハンドル取得
//=============================================================================
HINSTANCE GetInstance() {
    return g_hInst;
}

//=============================================================================
// デバイス取得
//=============================================================================
ID3D11Device* GetDevice() {
    return g_pd3dDevice;
}

//=============================================================================
// デバイス コンテキスト取得
//=============================================================================
ID3D11DeviceContext* GetDeviceContext() {
    return g_pd3dDeviceContext;
}

//=============================================================================
// 深度バッファ有効無効制御
//=============================================================================
void SetZBuffer(bool bEnable) {
    g_pd3dDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS[1], 0);
}

//=============================================================================
// 深度バッファ更新有効無効制御
//=============================================================================
void SetZWrite(bool bEnable) {
    g_pd3dDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS[0], 0);
}

//=============================================================================
// ブレンド ステート設定
//=============================================================================
void SetBlendState(int nBlendState) {
    if (nBlendState >= 0 && nBlendState < MAX_BLENDSTATE) {
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_pd3dDeviceContext->OMSetBlendState(g_pBlendState[nBlendState], blendFactor, 0xffffffff);
    }
}

//=============================================================================
// カリング設定
//=============================================================================
void SetCullMode(int nCullMode) {
    if (nCullMode >= 0 && nCullMode < MAX_CULLMODE) {
        g_pd3dDeviceContext->RSSetState(g_pRs[nCullMode]);
    }
}


void RenderBegin() {
    // バックバッファ＆Ｚバッファのクリア
    float ClearColor[4] = { 0.117647f, 0.254902f, 0.352941f, 1.0f };
    g_pd3dDeviceContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
    g_pd3dDeviceContext->ClearDepthStencilView(g_pDepthStencilView,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    // DearImGui更新
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}


void RenderEnd() {
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, g_pDepthStencilView);
    // バックバッファとフロントバッファの入れ替え
    g_pSwapChain->Present(g_uSyncInterval, 0);
}

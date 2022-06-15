//=============================================================================
//
// ���C������ [main.cpp]
// Author : �ɒn�c�^��
//
//=============================================================================
#include "Main.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

//-------- ���C�u�����̃����N
#pragma comment(lib, "winmm")
#pragma comment(lib, "imm32")
#pragma comment(lib, "d3d11")

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		_T("AppClass")			// �E�C���h�E�̃N���X��
#define WINDOW_NAME		_T("imgui")			// �E�C���h�E�̃L���v�V������

#define MAX_POLYLINE	(20)					// �|�����C����
#define THICKNESS		(10.0f)					// ���̑���

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT Init(HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;
ID3D11RasterizerState*		g_pRs[MAX_CULLMODE];	// ���X�^���C�U �X�e�[�g
ID3D11BlendState*			g_pBlendState[MAX_BLENDSTATE];// �u�����h �X�e�[�g
ID3D11DepthStencilState*	g_pDSS[2];				// Z/�X�e���V�� �X�e�[�g
ID3D11RenderTargetView*		g_pRenderTargetView;	// �t���[���o�b�t�@
ID3D11Texture2D*			g_pDepthStencilTexture;	// Z�o�b�t�@�p������
ID3D11DepthStencilView*		g_pDepthStencilView;	// Z�o�b�t�@
UINT						g_uSyncInterval = 0;	// �������� (0:��, 1:�L)
// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int	g_nCountFPS;			// FPS�J�E���^

//=============================================================================
// ���C���֐�
//=============================================================================
int main(int, char**)
{
    //----- �E�B���h�E�쐬 -----
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    //----- �ϐ������� ------
    // FPS����̂��߂̕ϐ�
    DWORD dwExecLastTime;
    DWORD dwFPSLastTime;
    DWORD dwCurrentTime;
    DWORD dwFrameCount;
    // ���b�Z�[�W�󂯎��̂��߂̕ϐ�
    MSG msg;


    // Direct3D������
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // �t���[���J�E���g������
    timeBeginPeriod(1);				// ����\��ݒ�
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

        // ���b�Z�[�W���[�v
    for (;;) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                // PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
                break;
            } else {
                // ���b�Z�[�W�̖|��ƃf�B�X�p�b�`
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            // Start the Dear ImGui frame
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            dwCurrentTime = timeGetTime();
            if ((dwCurrentTime - dwFPSLastTime) >= 500) {	// 0.5�b���ƂɎ��s
                g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
                dwFPSLastTime = dwCurrentTime;
                dwFrameCount = 0;
            }
            if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60)) {
                dwExecLastTime = dwCurrentTime;
                // �X�V����
                Update();
            }
            // �`�揈��
            Draw();

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            ImGui::End();

            // Rendering
            ImGui::Render();
            const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            dwFrameCount++;
        }
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
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
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
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
// �o�b�N�o�b�t�@����
//=============================================================================
HRESULT CreateBackBuffer(void) {
    // �����_�[�^�[�Q�b�g�r���[����
    ID3D11Texture2D* pBackBuffer = nullptr;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    SAFE_RELEASE(pBackBuffer);

    // Z�o�b�t�@�p�e�N�X�`������
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

    // Z�o�b�t�@�^�[�Q�b�g�r���[����
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
    ZeroMemory(&dsvd, sizeof(dsvd));
    dsvd.Format = td.Format;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencilTexture,
        &dsvd, &g_pDepthStencilView);
    if (FAILED(hr)) {
        return hr;
    }

    // �e�^�[�Q�b�g�r���[�������_�[�^�[�Q�b�g�ɐݒ�
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    // �r���[�|�[�g�ݒ�
    D3D11_VIEWPORT vp;
    vp.Width = (float)SCREEN_WIDTH;
    vp.Height = (float)SCREEN_HEIGHT;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pd3dDeviceContext->RSSetViewports(1, &vp);

    return S_OK;
}


//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HWND hWnd, BOOL bWindow) {
    HRESULT hr = S_OK;

    // �o�b�N�o�b�t�@����
    hr = CreateBackBuffer();
    if (FAILED(hr)) {
        return hr;
    }

    // ���X�^���C�Y�ݒ�
    D3D11_RASTERIZER_DESC rd;
    ZeroMemory(&rd, sizeof(rd));
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;	// �J�����O����(���ʕ`��)
    g_pd3dDevice->CreateRasterizerState(&rd, &g_pRs[0]);
    rd.CullMode = D3D11_CULL_FRONT;	// �O�ʃJ�����O(���ʕ`��)
    g_pd3dDevice->CreateRasterizerState(&rd, &g_pRs[1]);
    rd.CullMode = D3D11_CULL_BACK;	// �w�ʃJ�����O(�\�ʕ`��)
    g_pd3dDevice->CreateRasterizerState(&rd, &g_pRs[2]);
    g_pd3dDeviceContext->RSSetState(g_pRs[2]);

    // �u�����h �X�e�[�g����
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

    // �u�����h �X�e�[�g���� (�A���t�@ �u�����h�p)
    //BlendDesc.AlphaToCoverageEnable = TRUE;
    BlendDesc.RenderTarget[0].BlendEnable = TRUE;
    g_pd3dDevice->CreateBlendState(&BlendDesc, &g_pBlendState[1]);
    // �u�����h �X�e�[�g���� (���Z�����p)
    BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    g_pd3dDevice->CreateBlendState(&BlendDesc, &g_pBlendState[2]);
    // �u�����h �X�e�[�g���� (���Z�����p)
    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
    g_pd3dDevice->CreateBlendState(&BlendDesc, &g_pBlendState[3]);
    SetBlendState(BS_ALPHABLEND);

    // �[�x�X�e���V���X�e�[�g����
    CD3D11_DEFAULT def;
    CD3D11_DEPTH_STENCIL_DESC dsd(def);
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    g_pd3dDevice->CreateDepthStencilState(&dsd, &g_pDSS[0]);
    CD3D11_DEPTH_STENCIL_DESC dsd2(def);
    dsd2.DepthEnable = FALSE;
    g_pd3dDevice->CreateDepthStencilState(&dsd2, &g_pDSS[1]);

    //// �|���S���\��������
    //hr = InitPolygon(g_pd3dDevice);
    //if (FAILED(hr))
    //    return hr;

    //// �f�o�b�O������\��������
    //hr = InitDebugProc();
    //if (FAILED(hr))
    //    return hr;

    //// ���͏���������
    //hr = InitInput();
    //if (FAILED(hr))
    //    return hr;

    //// Assimp�p�V�F�[�_������
    //if (!CAssimpModel::InitShader(g_pd3dDevice))
    //    return E_FAIL;

    //// ���b�V��������
    //hr = InitMesh();
    //if (FAILED(hr))
    //    return hr;

    //// ����������
    //hr = InitNumber();
    //if (FAILED(hr))
    //    return hr;

    //// �T�E���h������
    //CSound::Init();

    //// �V�[��������
    //hr = InitScene();
    //if (FAILED(hr)) {
    //    return hr;
    //}

    // �{�����C��������
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
// �o�b�N�o�b�t�@���
//=============================================================================
void ReleaseBackBuffer() {
    if (g_pd3dDeviceContext) {
        g_pd3dDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    }
    SAFE_RELEASE(g_pDepthStencilView);
    SAFE_RELEASE(g_pDepthStencilTexture);
    SAFE_RELEASE(g_pRenderTargetView);
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void) {
    // �|�����C���I������
    //for (int i = 0; i < MAX_POLYLINE; ++i) {
    //	ReleasePolyline(&g_polyline[i]);
    //}
    //UninitPolyline();

    //// �T�E���h�I������
    //CSound::Fin();

    //// �V�[���I������
    //UninitScene();

    //// �����I������
    //UninitNumber();

    //// ���b�V���I������
    //UninitMesh();

    //// Assimp�p�V�F�[�_�I������
    //CAssimpModel::UninitShader();

    //// ���͏����I������
    //UninitInput();

    //// �f�o�b�O������\���I������
    //UninitDebugProc();

    //// �|���S���\���I������
    //UninitPolygon();

    // �[�x�X�e���V���X�e�[�g���
    for (int i = 0; i < _countof(g_pDSS); ++i) {
        SAFE_RELEASE(g_pDSS[i]);
    }

    // �u�����h �X�e�[�g���
    for (int i = 0; i < MAX_BLENDSTATE; ++i) {
        SAFE_RELEASE(g_pBlendState[i]);
    }

    // ���X�^���C�U �X�e�[�g���
    for (int i = 0; i < MAX_CULLMODE; ++i) {
        SAFE_RELEASE(g_pRs[i]);
    }

    // �o�b�N�o�b�t�@���
    ReleaseBackBuffer();

    // �X���b�v�`�F�[�����
    SAFE_RELEASE(g_pSwapChain);

    // �f�o�C�X �R���e�L�X�g�̊J��
    SAFE_RELEASE(g_pd3dDeviceContext);

    // �f�o�C�X�̊J��
    SAFE_RELEASE(g_pd3dDevice);
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void) {
    //// ���͏����X�V
    //UpdateInput();	// �K��Update�֐��̐擪�Ŏ��s.

    //// �f�o�b�O������\���X�V
    //UpdateDebugProc();

    //// �f�o�b�O������ݒ�
    //StartDebugProc();
    //PrintDebugProc("FPS:%d\n\n", g_nCountFPS);

    //// �|���S���\���X�V
    //UpdatePolygon();

    //// �J�����X�V
    //CCamera::Get()->Update();

    //// �T�E���h�X�V
    //CSound::Update();

    //// �V�[���X�V
    //UpdateScene();

    // �|�����C���X�V
    //for (int i = 0; i < MAX_POLYLINE; ++i) {
    //	UpdatePolyline(&g_polyline[i]);
    //}

}


//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void) {
    // �o�b�N�o�b�t�@���y�o�b�t�@�̃N���A
    float ClearColor[4] = { 0.117647f, 0.254902f, 0.352941f, 1.0f };
    g_pd3dDeviceContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
    g_pd3dDeviceContext->ClearDepthStencilView(g_pDepthStencilView,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


    // �|�����C���`��
    //for (int i = 0; i < MAX_POLYLINE; ++i) {
    //	DrawPolyline(&g_polyline[i]);
    //}

    //// �V�[���`��
    //DrawScene();


    // Z�o�b�t�@����(Z�`�F�b�N��&Z�X�V��)
    SetZBuffer(false);

    //// �f�o�b�O������\��
    //SetBlendState(BS_ALPHABLEND);
    //DrawDebugProc();
    //SetBlendState(BS_NONE);

    // �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
    g_pSwapChain->Present(g_uSyncInterval, 0);
}

//=============================================================================
// �[�x�o�b�t�@�L����������
//=============================================================================
void SetZBuffer(bool bEnable) {
    g_pd3dDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS[1], 0);
}

//=============================================================================
// �[�x�o�b�t�@�X�V�L����������
//=============================================================================
void SetZWrite(bool bEnable) {
    g_pd3dDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS[0], 0);
}

//=============================================================================
// �u�����h �X�e�[�g�ݒ�
//=============================================================================
void SetBlendState(int nBlendState) {
    if (nBlendState >= 0 && nBlendState < MAX_BLENDSTATE) {
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_pd3dDeviceContext->OMSetBlendState(g_pBlendState[nBlendState], blendFactor, 0xffffffff);
    }
}

//=============================================================================
// �J�����O�ݒ�
//=============================================================================
void SetCullMode(int nCullMode) {
    if (nCullMode >= 0 && nCullMode < MAX_CULLMODE) {
        g_pd3dDeviceContext->RSSetState(g_pRs[nCullMode]);
    }
}

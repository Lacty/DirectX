//--------------------------------------------------------------------------------------
// File: EmptyProject11.cpp
//
// Empty starting point for new Direct3D 11 Win32 desktop applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "SDKmisc.h"
#include "DXUTMesh.h"
#include "resource.h"

#pragma warning( disable : 4100 )

using namespace DirectX;


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
DXUTMesh*               g_pMesh[1];
XMVECTORF32             g_vColor[1];
XMMATRIX                g_mWorld[1];
XMMATRIX                g_mView;
XMMATRIX                g_mProj;

ID3D11InputLayout*      g_pVertexLayout = nullptr;
ID3D11VertexShader*     g_pVertexShader = nullptr;
ID3D11PixelShader*      g_pPixelShader = nullptr;

struct CB_VS_PER_OBJECT {
  XMFLOAT4X4 m_WorldViewProj;
  XMFLOAT4X4 m_World;
};
CONST UINT              g_iCBVSPerObjectBind = 0;

struct CB_PS_PER_OBJECT {
  XMFLOAT4 m_vOjectColor;
};
CONST UINT              g_iCBPSPerObjectBind = 0;

struct CB_PS_PER_FRAME {
  XMFLOAT4 m_vLightDirAmbient;
};
CONST UINT              g_iCBPSPERFrameBind = 1;

ID3D11Buffer*           g_pcbVSPerObject = nullptr;
ID3D11Buffer*           g_pcbPSPerObject = nullptr;
ID3D11Buffer*           g_pcbPSPerFrame  = nullptr;

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                      DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext) {
  return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext) {
  pDeviceSettings->d3d11.SyncInterval = 1;
  return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext)
{
  HRESULT hr;

  DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
  dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  ID3DBlob* pVertexShaderBuffer = nullptr;
  V_RETURN(DXUTCompileFromFile(L"sample01_vs.hlsl", nullptr, "VSMain", "vs_4_0_level_9_1", dwShaderFlags, 0, &pVertexShaderBuffer));

  ID3DBlob* pPixelShaderBuffer = nullptr;
  V_RETURN(DXUTCompileFromFile(L"sample01_ps.hlsl", nullptr, "PSMain", "ps_4_0_level_9_1", dwShaderFlags, 0, &pPixelShaderBuffer));

  // Create the Shaders
  V_RETURN(pd3dDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),
                                          pVertexShaderBuffer->GetBufferSize(), nullptr, &g_pVertexShader));
  V_RETURN(pd3dDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
                                         pPixelShaderBuffer->GetBufferSize(), nullptr, &g_pPixelShader));

  // Create our vertex input layout
  const D3D11_INPUT_ELEMENT_DESC layout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };

  V_RETURN(pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
                                         pVertexShaderBuffer->GetBufferPointer(),
                                         pVertexShaderBuffer->GetBufferSize(), &g_pVertexLayout));

  SAFE_RELEASE(pVertexShaderBuffer);
  SAFE_RELEASE(pPixelShaderBuffer);

  // Create models
  g_pMesh[0] = new DXUTMesh();
  g_pMesh[0]->CreateBox(pd3dDevice);
  g_vColor[0] = Colors::White;

  // Setup constant buffer
  D3D11_BUFFER_DESC Desc;
  Desc.Usage = D3D11_USAGE_DYNAMIC;
  Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  Desc.MiscFlags = 0;

  Desc.ByteWidth = sizeof(CB_VS_PER_OBJECT);
  V_RETURN(pd3dDevice->CreateBuffer(&Desc, nullptr, &g_pcbVSPerObject));

  Desc.ByteWidth = sizeof(CB_PS_PER_OBJECT);
  V_RETURN(pd3dDevice->CreateBuffer(&Desc, nullptr, &g_pcbPSPerObject));

  Desc.ByteWidth = sizeof(CB_PS_PER_FRAME);
  V_RETURN(pd3dDevice->CreateBuffer(&Desc, nullptr, &g_pcbPSPerFrame));

  return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
  float fAspectRaito = pBackBufferSurfaceDesc->Width / (FLOAT) pBackBufferSurfaceDesc->Height;

  g_mProj = XMMatrixPerspectiveFovLH(XM_PI / 4, fAspectRaito, 2.0f, 2000.0f);

  static const XMVECTORF32 s_vecEye = { 0.f, 0.f, -10.f, 0.f };
  static const XMVECTORF32 s_vecLookAt = { 0.f, 0.f, 0.f, 0.f };
  static const XMVECTORF32 s_vecUp = { 0.f, 1.f, 0.f, 0.f };
  g_mView = XMMatrixLookAtLH(s_vecEye, s_vecLookAt, s_vecUp);

  return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
  g_mWorld[0] = XMMatrixRotationY(static_cast<float>(fTime) * XM_PI) * XMMatrixTranslation(0, 0, 0);
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
                                 double fTime, float fElapsedTime, void* pUserContext) {
  HRESULT hr;

  // Clear render target and the depth stencil 
  auto pRTV = DXUTGetD3D11RenderTargetView();
  pd3dImmediateContext->ClearRenderTargetView(pRTV, Colors::MidnightBlue);

  auto pDSV = DXUTGetD3D11DepthStencilView();
  pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

  // Get the light direction
  static const XMVECTORF32 s_vLightDir = {-1.f, 1, -1.f, 0.f};
  XMVECTOR vLightDir = XMVector3Normalize(s_vLightDir);

  // Per frame cd update
  D3D11_MAPPED_SUBRESOURCE MappedResource;
  V(pd3dImmediateContext->Map(g_pcbPSPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
  auto pPerFrame = reinterpret_cast<CB_PS_PER_FRAME*>(MappedResource.pData);
  float fAmbient = 0.1f;
  XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&pPerFrame->m_vLightDirAmbient), vLightDir);
  pPerFrame->m_vLightDirAmbient.w = fAmbient;
  pd3dImmediateContext->Unmap(g_pcbPSPerFrame, 0);

  pd3dImmediateContext->PSSetConstantBuffers(g_iCBPSPERFrameBind, 1, &g_pcbPSPerFrame);

  //---------------------------------
  pd3dImmediateContext->IASetInputLayout(g_pVertexLayout);

  // Set the shaders
  pd3dImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
  pd3dImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);

  for (int i = 0; i < 1; ++i) {
    // Set the per object constant data
    XMMATRIX mWorldViewProjection = g_mWorld[i] * g_mView * g_mProj;

    // VS Per Object
    V(pd3dImmediateContext->Map(g_pcbVSPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
    auto pVSPerObject = reinterpret_cast<CB_VS_PER_OBJECT*>(MappedResource.pData);
    XMStoreFloat4x4(&pVSPerObject->m_WorldViewProj, XMMatrixTranspose(mWorldViewProjection));
    XMStoreFloat4x4(&pVSPerObject->m_World, XMMatrixTranspose(g_mWorld[i]));
    pd3dImmediateContext->Unmap(g_pcbVSPerObject, 0);

    pd3dImmediateContext->VSSetConstantBuffers(g_iCBVSPerObjectBind, 1, &g_pcbVSPerObject);

    // PS Per Object
    V(pd3dImmediateContext->Map(g_pcbPSPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
    auto pPSPerObject = reinterpret_cast<CB_PS_PER_OBJECT*>(MappedResource.pData);
    XMStoreFloat4(&pPSPerObject->m_vOjectColor, g_vColor[i]);
    pd3dImmediateContext->Unmap(g_pcbPSPerObject, 0);

    pd3dImmediateContext->PSSetConstantBuffers(g_iCBPSPerObjectBind, 1, &g_pcbPSPerObject);

    // render mesh
    g_pMesh[i]->Draw(pd3dImmediateContext);
  }
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext) {}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice(void* pUserContext) {
  for (int i = 0; i < 1; ++i) {
    delete g_pMesh[i];
    g_pMesh[i] = nullptr;
  }

  SAFE_RELEASE(g_pVertexLayout);
  SAFE_RELEASE(g_pVertexShader);
  SAFE_RELEASE(g_pPixelShader);

  SAFE_RELEASE(g_pcbVSPerObject);
  SAFE_RELEASE(g_pcbPSPerObject);
  SAFE_RELEASE(g_pcbPSPerFrame);
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                         bool* pbNoFurtherProcessing, void* pUserContext) {
  return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext) {}


//--------------------------------------------------------------------------------------
// Handle mouse button presses
//--------------------------------------------------------------------------------------
void CALLBACK OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                      bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                      int xPos, int yPos, void* pUserContext) {}


//--------------------------------------------------------------------------------------
// Call if device was removed.  Return true to find a new device, false to quit
//--------------------------------------------------------------------------------------
bool CALLBACK OnDeviceRemoved(void* pUserContext) {
  return true;
}


//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
  // Enable run-time memory check for debug builds.
#ifdef _DEBUG
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  // DXUT will create and use the best device
  // that is available on the system depending on which D3D callbacks are set below

  // Set general DXUT callbacks
  DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
  DXUTSetCallbackMsgProc(MsgProc);
  DXUTSetCallbackKeyboard(OnKeyboard);
  DXUTSetCallbackFrameMove(OnFrameMove);

  DXUTSetCallbackD3D11DeviceAcceptable(IsD3D11DeviceAcceptable);
  DXUTSetCallbackD3D11DeviceCreated(OnD3D11CreateDevice);
  DXUTSetCallbackD3D11SwapChainResized(OnD3D11ResizedSwapChain);
  DXUTSetCallbackD3D11FrameRender(OnD3D11FrameRender);
  DXUTSetCallbackD3D11SwapChainReleasing(OnD3D11ReleasingSwapChain);
  DXUTSetCallbackD3D11DeviceDestroyed(OnD3D11DestroyDevice);

  DXUTInit(true, true, nullptr);

  DXUTSetCursorSettings(true, true);
  DXUTCreateWindow(L"sample01");
  DXUTCreateDevice(D3D_FEATURE_LEVEL_9_2, true, 800, 600);
  DXUTMainLoop();

  return DXUTGetExitCode();
}



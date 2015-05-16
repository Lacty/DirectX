
#pragma once
#ifndef DXUT_MESH_H
#define DXUT_MESH_H

#include <DirectXMath.h>
//using namespace DirectX;

class DXUTMesh {
  using XMFLOAT3 = DirectX::XMFLOAT3;

  struct SimpleVertex {
    XMFLOAT3 pos;
    XMFLOAT3 Norm;
  };

  ID3D11Buffer* m_pVertexBuffer = nullptr;
  ID3D11Buffer* m_pIndexBuffer = nullptr;

public:
  ~DXUTMesh() {
    Destroy();
  }

  HRESULT CreateBox(ID3D11Device* pd3dDevice) {
    // create vertex buffer
    SimpleVertex vertices[] = {
      { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
      { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
      { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
      { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

      { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
      { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
      { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
      { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

      { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
      { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
      { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
      { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

      { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
      { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
      { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
      { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

      { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
      { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
      { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
      { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

      { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
      { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
      { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
      { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
    };

    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    V_RETURN(pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));

    // Create index buffer
    DWORD indices[] = {
      3, 1, 0,
      2, 1, 3,

      6, 4, 5,
      7, 4, 6,

      11, 9, 8,
      10, 9, 11,

      14, 12, 13,
      15, 12, 14,

      19, 17, 16,
      18, 17, 19,

      22, 20, 21,
      23, 20, 22
    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(DWORD) * 36;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    InitData.pSysMem = indices;
    V_RETURN(pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer));

    return hr;
  };

  void Destroy() {
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pIndexBuffer);
  }

  void Draw(ID3D11DeviceContext* pd3dImmediateContext) {
    // Set vertex buffer
    UINT strides[] = { GetVertexStride() };
    UINT offsets[] = { 0 };
    ID3D11Buffer* pVB[] = { GetVertexBuffer() };
    pd3dImmediateContext->IASetVertexBuffers(0, 1, pVB, strides, offsets);

    // Set index buffer
    pd3dImmediateContext->IASetIndexBuffer(GetIndexBuffer(), GetIBFormat(), 0);

    // Set primitive topology
    pd3dImmediateContext->IASetPrimitiveTopology(GetPrimitiveType());

    // render mesh
    pd3dImmediateContext->DrawIndexed(GetIndexCount(), 0, 0);
  }

  ID3D11Buffer* GetVertexBuffer() const { return m_pVertexBuffer; }
  ID3D11Buffer* GetIndexBuffer() const { return m_pIndexBuffer; }

  UINT GetVertexStride() const { return sizeof(SimpleVertex); }
  UINT GetIndexCount() const {
    return 36;
  };// 36; }

  DXGI_FORMAT GetIBFormat() const { return DXGI_FORMAT_R32_UINT; }
  D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType() const {
    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
  }
};

#endif
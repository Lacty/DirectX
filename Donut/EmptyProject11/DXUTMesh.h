
#pragma once
#ifndef DXUT_MESH_H
#define DXUT_MESH_H

#include <DirectXMath.h>
//using namespace DirectX;

class DXUTMesh {
  /*using XMFLOAT3 = DirectX::XMFLOAT3;

  struct SimpleVertex {
    XMFLOAT3 pos;
    XMFLOAT3 Norm;
  };*/

  ID3D11Buffer* m_pVertexBuffer = nullptr;
  ID3D11Buffer* m_pIndexBuffer  = nullptr;
  unsigned m_uVertexStride = 0;
  unsigned m_uIndexCount   = 0;

public:
  ~DXUTMesh() {
    Destroy();
  }

  HRESULT Create(ID3D11Device* pd3dDevice,
                    UINT VertexStride,
                    CONST void *pVertexData,
                    UINT VErtexCount,
                    CONST void *pIndexData,
                    UINT FaceCount);

  void Destroy() {
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pIndexBuffer);
  }

  void Draw(ID3D11DeviceContext* pd3dImmediateContext) {
    // Set vertex buffer
    UINT offsets = 0;
    pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer,
                                             &m_uVertexStride, &offsets);

    // Set index buffer
    pd3dImmediateContext->IASetIndexBuffer(GetIndexBuffer(), DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    pd3dImmediateContext->IASetPrimitiveTopology(GetPrimitiveType());

    // render mesh
    pd3dImmediateContext->DrawIndexed(m_uIndexCount, 0, 0);
  }

  ID3D11Buffer* GetVertexBuffer() const { return m_pVertexBuffer; }
  ID3D11Buffer* GetIndexBuffer() const  { return m_pIndexBuffer; }

  UINT GetVertexStride() const { return m_uVertexStride; }
  UINT GetIndexCount() const   { return m_uIndexCount; };

  DXGI_FORMAT GetIBFormat() const { return DXGI_FORMAT_R32_UINT; }
  D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType() const {
    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
  }
};

#endif
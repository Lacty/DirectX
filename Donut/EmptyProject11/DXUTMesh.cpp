
#include "DXUT.h"
#include "DXUTMesh.h"

HRESULT DXUTMesh::Create(ID3D11Device *pd3dDevice,
                         UINT VertexStride,
                         CONST void *pVertexData,
                         UINT VertexCount,
                         CONST void *pIndexData,
                         UINT FaceCount)
{
  HRESULT hr = S_OK;

  m_uVertexStride = VertexStride;
  m_uIndexCount = FaceCount * 3;

  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = m_uVertexStride * VertexCount;
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bd.CPUAccessFlags = 0;
  D3D11_SUBRESOURCE_DATA InitData;
  ZeroMemory(&InitData, sizeof(InitData));
  InitData.pSysMem = pVertexData;
  hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
  if (FAILED(hr))
    return hr;

  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(WORD) * m_uIndexCount;
  bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  bd.CPUAccessFlags = 0;
  bd.MiscFlags      = 0;
  InitData.pSysMem = pIndexData;
  hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);

  return hr;
}
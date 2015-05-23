//--------------------------------------------------------------------------------------
// File: DXUTShapes.h
//
// Shape creation functions for DXUT
//
// Copyright (c) Microsoft Corporation. All rights reserved
//--------------------------------------------------------------------------------------
#pragma once
#ifndef DXUT_SHAPES_H
#define DXUT_SHAPES_H

#include "DXUTMesh.h"

HRESULT WINAPI DXUTCreateBox( ID3D11Device* pDevice, float fWidth, float fHeight, float fDepth, DXUTMesh** ppMesh );
HRESULT WINAPI DXUTCreateCylinder( ID3D11Device* pDevice, float fRadius1, float fRadius2, float fLength, UINT uSlices,
                                   UINT uStacks, DXUTMesh** ppMesh );
HRESULT WINAPI DXUTCreatePolygon( ID3D11Device* pDevice, float fLength, UINT uSides, DXUTMesh** ppMesh );
HRESULT WINAPI DXUTCreateSphere( ID3D11Device* pDevice, float fRadius, UINT uSlices, UINT uStacks,
                                 DXUTMesh** ppMesh );
HRESULT WINAPI DXUTCreateTorus( ID3D11Device* pDevice, float fInnerRadius, float fOuterRadius, UINT uSides,
                                UINT uRings, DXUTMesh** ppMesh );
HRESULT WINAPI DXUTCreateTeapot( ID3D11Device* pDevice, DXUTMesh** ppMesh );

#endif

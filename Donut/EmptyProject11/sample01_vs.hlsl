
// File: sample01_vs.hlsl

// Globals
cbuffer cbPerObject : register(b0) {
  matrix g_mWorldViewProjection : packoffset(c0);
  matrix g_mWorld : packoffset(c4);
}

// Input / Output structures
struct VS_INPUT {
  float4 vPosition : POSITION;
  float3 vNormal : NORMAL;
};

struct VS_OUTPUT {
  float3 vNormal : NORMAL;
  float4 vPosition : SV_POSITION;
};

// Vertex Shader
VS_OUTPUT VSMain(VS_INPUT Input) {
  VS_OUTPUT Output;

  Output.vPosition = mul(Input.vPosition, g_mWorldViewProjection);
  Output.vNormal = mul(Input.vNormal, (float3x3)g_mWorld);
  return Output;
}
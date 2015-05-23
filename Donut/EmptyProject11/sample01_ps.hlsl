
// File: sample01_ps.hlsl

// Globals
cbuffer cbPerObject : register(b0) {
  float4 g_vObjectColor : packoffset(c0);
}

cbuffer cbPerFrame : register(b1) {
  float3 g_vLightDir : packoffset(c0);
  float g_fAmbient : packoffset(c0.w);
}

// Input / Output structures
struct PS_INPUT {
  float3 vNormal : NORMAL;
};

// Pixel Shader

float4 PSMain(PS_INPUT Input) : SV_TARGET
{
  float fLighting = saturate(dot(g_vLightDir, Input.vNormal));
  fLighting = max(fLighting, g_fAmbient);
	return g_vObjectColor * fLighting;
}
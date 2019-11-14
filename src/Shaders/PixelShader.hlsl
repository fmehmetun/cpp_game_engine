#include "Lighting.hlsl"

// Provided by entity object.
Texture2D Texture : register(t0);
Texture2D NormalMappingTexture : register(t1);
Texture2D AlphaTexture : register(t2);

// Various samplers.
SamplerState defaultSampler : register(s0);
SamplerState clampSampler : register(s1);
SamplerState whiteBorderSampler : register(s2);

cbuffer EntityPSConstantBuffer : register(b0) {
	float4 entityColor;
	float4 specularHighlightColor;
	float specularIntensity;
	float specularPower;
	bool useTexture;
	bool useNormalMapping;
	bool useAlpha;
	float3 padding;
};

// Shadow map struct and constant buffers.
struct ShadowMap {
	matrix viewMatrix;
	matrix projectionMatrix;
	bool isActive;
};
cbuffer ShadowMapVSConstantBuffer : register(b2) {
	ShadowMap shadowMaps[MAX_SHADOW_CASTER_COUNT];
};

// Input structure of the Pixel shader.
struct PSIn {
	// Vertex values
	float3 positionPS : Position;
	float2 texture_UV : TextureUV;
	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 binormal : Binormal;

	// Camera position
	float3 eyePosition : EyePosition;

	// Shadow map
	// XY, shadow map texture UV coordinates
	// Z, distance from light.
	float4 shadowMapPosition[MAX_SHADOW_CASTER_COUNT] : TEXCOORD0;
};

// Output structure of the Pixel shader.
struct PSOut {
	float4 color : SV_Target;
};

PSOut main(PSIn psIn){
	PSOut psOut;

	// Current pixel color: Use solid entity color, or sample from texture if entity provides it.
	float4 pixelColor = entityColor;
	if (useTexture) {
		pixelColor = Texture.Sample(defaultSampler, psIn.texture_UV);
	}

	psIn.normal = normalize(psIn.normal);
	// Sample from normal map texture if entity uses it.
	if (useNormalMapping) {
		psIn.tangent = normalize(psIn.tangent);
		psIn.binormal = normalize(psIn.binormal);

		// Sample normal vector from texture, these values ranges 0 to 1,
		// we need to expand and reposition them as -1 to 1.
		float4 sampledNormal = NormalMappingTexture.Sample(defaultSampler, psIn.texture_UV);
		sampledNormal = sampledNormal * 2.0f - 1.0f;

		// Tangent space to object space.
		psIn.normal = mul(
			sampledNormal.xyz,
			float3x3(
				psIn.tangent,
				psIn.binormal,
				psIn.normal
				)
		);
	}

	// If uses alpha value from texture, sample it.
	if (useAlpha) {
		float3 alpha = AlphaTexture.Sample(defaultSampler, psIn.texture_UV).rgb;
		pixelColor.a = (alpha.r + alpha.g + alpha.b) / 3;
	}
	else {
		pixelColor.a = 1.0f;
	}

	// Calculate lighting.
	float4 lightingFactor = calculateAllLights(
		psIn.positionPS, psIn.normal, psIn.eyePosition,
		specularHighlightColor, specularIntensity, specularPower
	);

	// Calculate shadowing.
	/*float2 shadowMapCoords;
	float finalDepth;
	float nearestObjectDepth;
	// Process shadow maps.
	for (unsigned int sc = 0; sc < MAX_SHADOW_CASTER_COUNT; sc++) {
		if (shadowMaps[sc].isActive) {
			shadowMapCoords.x = psIn.shadowMapPosition[sc].x / psIn.shadowMapPosition[sc].w * 0.5 + 0.5;
			shadowMapCoords.y = -psIn.shadowMapPosition[sc].y / psIn.shadowMapPosition[sc].w * 0.5 + 0.5;

			if ((saturate(shadowMapCoords.x) == shadowMapCoords.x) && (saturate(shadowMapCoords.y) == shadowMapCoords.y)) {
				finalDepth = (psIn.shadowMapPosition[sc].z / psIn.shadowMapPosition[sc].w) - 0.001f;
				nearestObjectDepth = ShadowMapTexture[sc].Sample(whiteBorderSampler, shadowMapCoords).r;

				if (finalDepth > nearestObjectDepth) {
					lightingFactor *= float4(0.4, 0.4, 0.4, 1);

					// Shadow each pixel only once.
					break;
				}
			}
		}
	}*/

	// Final result: Combine light & pixel color.
	psOut.color = float4(lightingFactor * pixelColor);

	return psOut;
}
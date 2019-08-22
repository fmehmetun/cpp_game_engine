#pragma once
#include "Structs.h"

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
namespace wrl = Microsoft::WRL;

const enum LIGHT_TYPE {
	DIRECTIONAL_LIGHT,
	POINT_LIGHT
};

class Light {
public:
	unsigned int id;
	unsigned int type;

	// Functions for all light types.
	void setIntensity(float intensity);
	void setDirection(Vector3 newDirection);
	void setPosition(Vector3 newPosition);

	void Update(bool initial = false);
	void updateConstantBuffer();
	bool shouldUpdateData;

	// Graphics
	float gIntensity;
	dx::XMFLOAT3 gDirection;
	dx::XMFLOAT3 gPosition;

	LightConstantBuffer gLightConstBuffer;
	wrl::ComPtr<ID3D11Buffer> pLightConstantBuffer;
};
#include "PointLight.h"

PointLight::PointLight(Vector3 position, float intensity, Color color, bool isCastingShadow){
	this->type = LIGHT_TYPE::POINT_LIGHT;

	this->gPosition = position;
	this->gDirection = Vector3(0, 0, 0);
	this->gColor = color;
	this->gIntensity = intensity;

	this->isCastingShadow = isCastingShadow;
	if (this->isCastingShadow) {
		this->gShadowBox = new ShadowBox(
			this->gPosition,
			this->gDirection,
			{ SHADOWMAP_TEXTURE_SIZE_X, SHADOWMAP_TEXTURE_SIZE_Y },
			this->type,
			6
		);
	}

	this->isActive = true;
	this->updateConstantBuffer();
}
#include "SpotLight.h"

SpotLight::SpotLight(Vector3 position, Vector3 direction, float intensity, Color color, float halfSpotAngle, bool shadowCasting) {
	this->type = LIGHT_TYPE::SPOT_LIGHT;

	this->gPosition = position;
	this->gDirection = direction;
	this->gColor = color;
	this->gIntensity = intensity;
	this->shadowCasting = shadowCasting;

	this->gHalfSpotAngle = halfSpotAngle;

	this->isActive = true;
	this->updateConstantBuffer();
}

// Set spot light's angle.
void SpotLight::setSpotAngle(float halfSpotAngle) {
	if (this->type != SPOT_LIGHT) {
		return;
	}

	this->gHalfSpotAngle = halfSpotAngle;
	this->dataChanged = true;
}
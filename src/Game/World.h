#pragma once
#include <vector>

#include "Graphics.h"
#include "Physics.h"
#include "Camera.h"

// Base class for all entities
#include "Entity/BaseEntity.h"
#include "Light/DirectionalLight.h"

class World{
public:
	World(Graphics* pGfx, Physics* pPhy);

	// General
	void Update();

	// Entity functions
	void addEntity(BaseEntity* bEntity);

	// Light functions
	void addLight(DirectionalLight* light, bool activate=true);
	void activateLight(DirectionalLight* light);

	// Camera functions
	void addCamera(Camera* camera, bool setAsMain=false);
	void setCamera(Camera* camera);

	// Main camera pointer.
	Camera* activeCamera;

private:
	Graphics* pGfx;
	Physics* pPhy;

	// List for keeping track of entities.
	std::vector<BaseEntity*> allEntities;

	// List for keeping track of lights.
	std::vector<DirectionalLight*> allLights;

	// List for keeping track of cameras.
	std::vector<Camera*> allCameras;
};
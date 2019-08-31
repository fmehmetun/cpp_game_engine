#include "Game.h"

Game::Game(World* pWorld){
	this->pWorld = pWorld;
}

void Game::Setup(){
	// Add main camera.
	Camera* pCamera = new Camera(
		Vector3(0.0f, 0.0f, 0.0f),
		FOV,
		WIDTH / HEIGHT
	);
	pWorld->addCamera(pCamera, true);

	// Create texture objects.
	Texture* texture = new Texture("texture0", "C:\\VisualStudioProjects\\cpp_game_engine\\assets\\texture.dds");
	TextureSampler* textureSampler = new TextureSampler();
	this->pWorld->createTexture(texture);
	this->pWorld->createTextureSampler(textureSampler);
	
	//// Create box entities.
	// This mesh will be shared between boxes.
	Mesh* boxMesh = new Mesh();
	boxMesh->createBoxShape();

	Entity* boxEntity;

	// Parent box
	this->parentBoxEntity = new Entity(
		{ 1,1,1 },
		{ 0,0,0 },
		{ 0,0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 },
		boxMesh
	);
	this->pWorld->addEntity(this->parentBoxEntity);

	// Child box 1
	boxEntity = new Entity(
		{ 1,1,1 },
		{ 0,0,5 },
		{ 0,0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 },
		boxMesh
	);
	this->pWorld->addEntity(boxEntity);
	this->parentBoxEntity->attachChild(boxEntity);

	// Child box 2
	boxEntity = new Entity(
		{ 1,1,1 },
		{ 0,0,-5 },
		{ 0,0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 },
		boxMesh
	);
	this->pWorld->addEntity(boxEntity);
	this->parentBoxEntity->attachChild(boxEntity);

	// Child box 3
	boxEntity = new Entity(
		{ 1,1,1 },
		{ 0,5,0 },
		{ 0,0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 },
		boxMesh
	);
	this->pWorld->addEntity(boxEntity);
	this->parentBoxEntity->attachChild(boxEntity);

	// Child box 4
	boxEntity = new Entity(
		{ 1,1,1 },
		{ 0,-5,0 },
		{ 0,0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 },
		boxMesh
	);
	this->pWorld->addEntity(boxEntity);
	this->parentBoxEntity->attachChild(boxEntity);

	//// Load animated mesh.
	Mesh* tMesh = new Mesh();
	MeshDeformer* tMeshDeformer = new MeshDeformer();
	tMesh->attachMeshDeformer(tMeshDeformer);

	animatedEntity = new Entity(
		{ 0.025f, 0.025f, 0.025f },
		{ 0,0,30 },
		{ 0,0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 },
		tMesh,
		{0, 3, 0}
	);

	Entity* animatedEntity2 = new Entity(
		{ 0.025f, 0.025f, 0.025f },
		{ 0,0,15 },
		{ 0,0,0,0 },
		{ 0, 0, 0.66f, 1 },
		{ 1,1,1 },
		tMesh,
		{ 0, 0, 0 }
	);

	if (tMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\BaseMesh_Anim_Triangle.fbx")) {
		// Attach mesh and texture to entity.
		animatedEntity->attachTextureAndSampler(texture, textureSampler);

		this->pWorld->addEntity(animatedEntity);
		this->pWorld->addEntity(animatedEntity2);
	}

	PointLight* pointLight = new PointLight(Vector3(-3.0f, 3.0f, -3.0f), 1.0f);
	this->pWorld->addLight(pointLight);

	/*DirectionalLight* directionalLight = new DirectionalLight(Vector3(-1.0f, 1.0f, -1.0f), 1.0f);
	this->pWorld->addLight(directionalLight);*/
}

void Game::Update(){
	if (timer.Peek() > 1.0f) {
		timer.Reset();
	}

	// Circular motion around 0,0,0 for point light.
	float cosx = cos(timer.Peek() * 6.28f) * 5.0f;
	float siny = sin(-timer.Peek() * 6.28f) * 5.0f;

	// Rotate animated entity.
	this->animatedEntity->rotateQuaternion({ 1, 1, 0, timer.Peek() * 6.28f });

	// Transform parent box.
	this->parentBoxEntity->Translate({ 0.01f, 0, 0});
	
	this->pWorld->allLights.at(0)->setPosition({ cosx, siny, 0.0f });
}
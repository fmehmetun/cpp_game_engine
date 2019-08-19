#include "TriangleMesh.h"

// Utils
#include "Utils/FBX_Importer.h"

TriangleMesh::TriangleMesh(Vector3 size, Vector3 position, Vector3 rotation, Vector3 material){
	PxQuat rotationQuaternion;

	// Graphics
	this->gSize = XMFLOAT3(size.x, size.y, size.z);
	this->gPosition = XMFLOAT3(position.x, position.y, position.z);
	//this->gRotationQ = XMFLOAT4(rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z, rotationQuaternion.w);
	this->gRotationQ = XMFLOAT4(0,0,0,0);

	// Set properties of the entity.
	this->type = ENTITY_TYPE::TRIANGLE_MESH;
	this->isDynamic = false;

	this->Update(true);
}

TriangleMesh::~TriangleMesh() {}

void TriangleMesh::Update(bool initial) {
	// Skip static and sleeping dynamic entities.
	this->shouldUpdateData = true;

	if (!initial) {
		// Static check.
		if (!this->isDynamic) {
			this->shouldUpdateData = false;
			return;
		}
	}

	this->updateConstantBuffer();
}

void TriangleMesh::updateConstantBuffer() {
	// Update constant buffer that held on class.
	dx::XMStoreFloat4x4(
		&(this->gConstBuffer.worldMatrix),
		dx::XMMatrixTranspose(
			dx::XMMatrixScaling(this->gSize.x, this->gSize.y, this->gSize.z) *
			dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&this->gRotationQ)) *
			dx::XMMatrixTranslation(this->gPosition.x, this->gPosition.y, this->gPosition.z)
		)
	);
}

bool TriangleMesh::LoadVerticesAndIndicesFBX(const char* fileName){
	std::vector<Vertex>* _vertices = new std::vector<Vertex>();
	std::vector<unsigned int>* _indices = new std::vector<unsigned int>();

	// Load .FBX file to our vectors.
	if (!FBX_Importer::Load(fileName, _vertices, _indices)) {
		return false;
	}

	this->gVertexCount = (UINT) _vertices->size();
	this->gIndexCount = (UINT) _indices->size();

	Vertex* vertices = new Vertex[this->gVertexCount];
	unsigned int* indices = new unsigned int[this->gIndexCount];

	for (int v = 0; v < _vertices->size(); v++) {
		vertices[v] = _vertices->at(v);
	}
	for (int i = 0; i < _indices->size(); i++) {
		indices[i] = _indices->at(i);
	}

	this->gVertices = vertices;
	this->gIndices = indices;

	delete _vertices;
	delete _indices;

	return true;
}
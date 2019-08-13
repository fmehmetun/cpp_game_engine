#include "BaseEntity.h"
#include "Shapes.h"

PxPhysics* BaseEntity::ppxPhysics;

BaseEntity::~BaseEntity() {
	delete[] this->gVertices;
	delete[] this->gIndices;
}

void BaseEntity::Update(){}
void BaseEntity::updateConstantBuffer(){}
void BaseEntity::gCreateVerticesAndIndices(){}
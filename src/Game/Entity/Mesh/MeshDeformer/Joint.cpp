#include "Joint.h"

Joint::Joint(int id, std::string name) {
	this->id = id;
	this->name = name;

	// Initialize translation and rotation vectors.
	this->position = dx::XMFLOAT3(0, 0, 0);
	this->rotationQ = dx::XMFLOAT4(0, 0, 0, 0);

	// Initialize matrices with identity matrix.
	dx::XMStoreFloat4x4(&this->jointModelTransformMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->jointLocalTransformMatrix, dx::XMMatrixTranslation(0,0,0));
	dx::XMStoreFloat4x4(&this->jointBindPoseInverseMatrix, dx::XMMatrixIdentity());
	dx::XMStoreFloat4x4(&this->jointLocalBindTransform, dx::XMMatrixTranslation(0, 0, 0));
}

void Joint::Update() {
	if (
		!this->name.compare("spine") ||
		!this->name.compare("upper_arm.L") ||
		!this->name.compare("upper_arm.R")
	) {
		// Check if quaternion rotation axis is equal to zero.
		if (dx::XMVector4Equal(dx::XMLoadFloat4(&this->rotationQ), dx::XMVectorZero())) {
			this->rotationQ.x = -1.0f;
		}

		this->rotationQ.w += 0.05f;
		this->rotationQ.w = (float)fmod(
			(double)this->rotationQ.w,
			(double)(dx::XM_PI * 2.0f)
		);

		// Build transformation matrix that will be applied to vertex at joint space.
		dx::XMStoreFloat4x4(
			&this->jointLocalTransformMatrix,
			dx::XMMatrixRotationQuaternion(
				dx::XMQuaternionRotationAxis(
					dx::XMLoadFloat3(
						&dx::XMFLOAT3(
							this->rotationQ.x, this->rotationQ.y, this->rotationQ.z
						)
					),
					this->rotationQ.w
				)
			)
		);
	}
}
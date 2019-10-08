#include "MeshDeformer.h"

MeshDeformer::MeshDeformer() {}

void MeshDeformer::Setup(Skeleton* pSkeleton, dx::XMFLOAT3 gEntityScale) {
	this->skeleton = pSkeleton;

	// Create JointTransform object array.
	this->gJointCount = this->skeleton->gJointCount;
	this->gJointTransforms = new JointTransform*[this->gJointCount];

	// Fill JointTransform objects data.
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		// Copy joint values from skeleton.
		this->gJointTransforms[j] = new JointTransform();
		this->gJointTransforms[j]->id = this->skeleton->gJoints[j]->id;
		this->gJointTransforms[j]->name = this->skeleton->gJoints[j]->name;
		this->gJointTransforms[j]->isRoot = this->skeleton->gJoints[j]->isRoot;
	}

	this->rootJointID = this->skeleton->rootJoint->id;

	// Set root joint's initial matrix to identity matrix.
	this->rootInitialMatrix = dx::XMMatrixIdentity();

	// Setup ragdoll joints on physics side.
	// Create physical actor&shape arrays.
	this->pRagdollCollisionActor = new CollisionActor*[this->gJointCount];
	this->pRagdollCollisionShape = new CollisionShape*[this->gJointCount];

	for (unsigned int j = 0; j < this->gJointCount; j++) {
		// Fill physical actor&shape arrays.
		this->pRagdollCollisionActor[j] = new CollisionActor(COLLISION_ACTOR_DYNAMIC);
		this->pRagdollCollisionShape[j] = new CollisionShape();

		Joint* currentJoint = this->skeleton->gJoints[j];

		// Root joint.
		if (currentJoint->isRoot) {
			dx::XMFLOAT3 rootJointAbsolutePos;
			dx::XMStoreFloat3(
				&(rootJointAbsolutePos),
				XMVector3Transform(
					dx::XMVectorZero(),
					(
						dx::XMLoadFloat4x4(&(currentJoint->globalBindPoseMatrix))
					)
				)
			);
			rootJointAbsolutePos.x *= gEntityScale.x;
			rootJointAbsolutePos.y *= gEntityScale.y;
			rootJointAbsolutePos.z *= gEntityScale.z;

			this->pRagdollCollisionShape[j]->createBoxGeometry({
				gEntityScale.x,
				gEntityScale.y,
				gEntityScale.z
			});
			this->pRagdollCollisionActor[j]->initialTransform = PxTransform(
				PxVec3(
					rootJointAbsolutePos.x,
					rootJointAbsolutePos.y,
					rootJointAbsolutePos.z
				)
			);

			this->pRagdollCollisionActor[j]->parentActor = NULL;

			continue;
		}

		this->pRagdollCollisionActor[j]->parentActor = this->pRagdollCollisionActor[currentJoint->parentJointID];

		dx::XMFLOAT3 jointAbsolutePos;
		dx::XMStoreFloat3(
			&jointAbsolutePos,
			XMVector3Transform(
				dx::XMVectorZero(),
				dx::XMLoadFloat4x4(&(currentJoint->globalBindPoseMatrix))
			)
		);
		jointAbsolutePos.x *= gEntityScale.x;
		jointAbsolutePos.y *= gEntityScale.y;
		jointAbsolutePos.z *= gEntityScale.z;

		dx::XMFLOAT3 parentJointAbsolutePos;
		dx::XMStoreFloat3(
			&parentJointAbsolutePos,
			XMVector3Transform(
				dx::XMVectorZero(),
				dx::XMLoadFloat4x4(&(currentJoint->parentJoint->globalBindPoseMatrix))
			)
		);
		parentJointAbsolutePos.x *= gEntityScale.x;
		parentJointAbsolutePos.y *= gEntityScale.y;
		parentJointAbsolutePos.z *= gEntityScale.z;

		// Calculate joint's actor position.
		dx::XMFLOAT3 actorPosition = dx::XMFLOAT3(
			parentJointAbsolutePos.x + ((jointAbsolutePos.x - parentJointAbsolutePos.x) / 2),
			parentJointAbsolutePos.y + ((jointAbsolutePos.y - parentJointAbsolutePos.y) / 2),
			parentJointAbsolutePos.z + ((jointAbsolutePos.z - parentJointAbsolutePos.z) / 2)
		);
		dx::XMVECTOR toChild = dx::XMVectorSet(
			(jointAbsolutePos.x - parentJointAbsolutePos.x),
			(jointAbsolutePos.y - parentJointAbsolutePos.y),
			(jointAbsolutePos.z - parentJointAbsolutePos.z),
			1
		);

		// Calculate joint's actor rotation (Quaternion).
		dx::XMVECTOR xAxisVector = dx::XMVectorSet(1, 0, 0, 0);
		dx::XMFLOAT4 actorRotation;
		dx::XMStoreFloat4(
			&actorRotation,
			dx::XMVector3Cross(
				xAxisVector,
				toChild
			)
		);
		float upLengthSq;
		float toChildLengthSq;
		dx::XMStoreFloat(&upLengthSq, dx::XMVector3LengthSq(xAxisVector));
		dx::XMStoreFloat(&toChildLengthSq, dx::XMVector3LengthSq(toChild));
		float dotProduct;
		dx::XMStoreFloat(&dotProduct, dx::XMVector3Dot(xAxisVector, toChild));
		actorRotation.w = (float) (dotProduct + (float) sqrt((double) (upLengthSq * toChildLengthSq)));

		// Normalize quaternion vector
		dx::XMStoreFloat4(
			&actorRotation,
			dx::XMVector4Normalize(
				dx::XMLoadFloat4(&actorRotation)
			)
		);

		// Measure length
		dx::XMFLOAT3 jointAbsolutePosLen;
		dx::XMStoreFloat3(&jointAbsolutePosLen,XMVector3Transform(dx::XMVectorZero(),dx::XMLoadFloat4x4(&(currentJoint->globalBindPoseMatrix))));

		dx::XMFLOAT3 parentJointAbsolutePosLen;
		dx::XMStoreFloat3(&parentJointAbsolutePosLen,XMVector3Transform(dx::XMVectorZero(),dx::XMLoadFloat4x4(&(currentJoint->parentJoint->globalBindPoseMatrix))));

		dx::XMVECTOR toChildLen = dx::XMVectorSet(
			(jointAbsolutePosLen.x - parentJointAbsolutePosLen.x),
			(jointAbsolutePosLen.y - parentJointAbsolutePosLen.y),
			(jointAbsolutePosLen.z - parentJointAbsolutePosLen.z),
			1
		);

		dx::XMFLOAT3 jointLength;
		dx::XMStoreFloat3(&jointLength, dx::XMVector3Length(toChildLen));

		// Store bone length.
		this->pRagdollCollisionActor[j]->boneLength = gEntityScale.x * jointLength.y;

		this->pRagdollCollisionShape[j]->createBoxGeometry({
			this->pRagdollCollisionActor[j]->boneLength / 2,
			gEntityScale.y * 1,
			gEntityScale.z * 1
		});

		// Set initial transform of the joint actor (bone of ragdoll).
		this->pRagdollCollisionActor[j]->initialTransform = PxTransform(
			PxVec3(actorPosition.x, actorPosition.y, actorPosition.z),
			PxQuat(actorRotation.x, actorRotation.y, actorRotation.z, actorRotation.w)
		);

		this->pRagdollCollisionActor[j]->jointPointSelf = PxTransform(
			PxVec3(
				-(this->pRagdollCollisionActor[j]->boneLength / 2),
				0,
				0
			),
			PxQuat(1, 0, 0, 0)
		);
		this->pRagdollCollisionActor[j]->jointPointParent = PxTransform(
			PxVec3(
				(this->pRagdollCollisionActor[j]->parentActor->boneLength / 2),
				0,
				0
			),
			PxQuat(1, 0, 0, 0)
		);
	}
}

void MeshDeformer::Update() {
	// If animation attached, apply current pose of animation to joints.
	if (this->currentTransformSource == ANIMATION) {
		this->gAnimator->Update(this->gJointTransforms, this->gJointCount);
	}
	// If ragdoll physics is activated, update accordingly.
	else if (this->currentTransformSource == RAGDOLL) {
		PxRigidDynamic* rigidDynamic;
		PxTransform tm;
		for (unsigned int j = 0; j < this->gJointCount; j++) {
			rigidDynamic = this->pRagdollCollisionActor[j]->pActor->is<PxRigidDynamic>();
			if (rigidDynamic == NULL) {
				continue;
			}

			// Get quaternion vector from physics side.
			tm = rigidDynamic->getGlobalPose();

			// Rotation
			PxVec3 axisVector;
			float rotationRadian;
			tm.q.toRadiansAndUnitAxis(rotationRadian, axisVector);

			// Apply to local transform of the joint.
			JointTransform* baseJointTransform = this->gJointTransforms[j];
			dx::XMStoreFloat4x4(
				&baseJointTransform->jointLocalTransformMatrix,
				(
					dx::XMMatrixRotationQuaternion(
						dx::XMQuaternionRotationAxis(
							dx::XMVectorSet(axisVector.x, axisVector.y, axisVector.z, 0),
							rotationRadian
						)
					)
				)
			);
		}
	}

	// Recalculate joint matrices.
	this->recalculateMatrices(
		this->rootJointID,
		&this->rootInitialMatrix
	);

	for (unsigned int j = 0; j < this->gJointCount; j++) {
		if (this->gJointTransforms[j]->dataChanged) {
			this->gMeshDeformerVSConstantBuffer.jointsTransformMatrix[this->skeleton->gJoints[j]->id] = this->gJointTransforms[j]->jointModelTransformMatrix;
			this->shouldUpdateGPUData = true;
			this->gJointTransforms[j]->dataChanged = false;
		}
	}
}

// Recalculates joint matrices recursively.
void MeshDeformer::recalculateMatrices(int baseJointID, dx::XMMATRIX* parentModelTransform) {
	// Calculate current joint's transform in model-space.
	Joint* baseJoint = this->skeleton->gJoints[baseJointID];
	JointTransform* baseJointTransform = this->gJointTransforms[baseJointID];

	dx::XMMATRIX poseModelTransformMatrix = dx::XMLoadFloat4x4(&baseJoint->globalBindPoseMatrix);
	if (this->currentTransformSource == ANIMATION) {
		poseModelTransformMatrix = (
			// Transformation relative to parent.
			dx::XMLoadFloat4x4(&baseJointTransform->jointAnimTransformMatrix) *
			(*parentModelTransform)
		);
	}
	else if (this->currentTransformSource == RAGDOLL) {
		// Transformation relative to parent.
		poseModelTransformMatrix = (
			dx::XMLoadFloat4x4(&baseJointTransform->jointLocalTransformMatrix) *
			dx::XMLoadFloat4x4(&baseJoint->jointLocalBindTransform) *
			(*parentModelTransform)
		);
	}
	else {
		// Transformation relative to parent.
		poseModelTransformMatrix = (
			dx::XMLoadFloat4x4(&baseJointTransform->jointLocalTransformMatrix) *
			dx::XMLoadFloat4x4(&baseJoint->jointLocalBindTransform) *
			(*parentModelTransform)
		);
	}
	
	// Iterate over child joints and recalculate their matrices too.
	for (int cj = 0; cj < baseJoint->childJoints.size(); cj++) {
		this->recalculateMatrices(
			baseJoint->childJoints.at(cj)->id,
			&poseModelTransformMatrix
		);
	}

	// Update final matrix that will be applied to vertices.
	dx::XMStoreFloat4x4(
		&baseJointTransform->jointModelTransformMatrix,
		dx::XMMatrixTranspose(
			dx::XMLoadFloat4x4(&baseJoint->globalBindPoseInverseMatrix) * poseModelTransformMatrix
		)
	);
	baseJointTransform->dataChanged = true;
}

void MeshDeformer::setAnimation(Animation* animation) {
	// If animator isn't created yet, create it.
	if (animation == NULL) {
		return;
	}

	if (this->gAnimator == NULL) {
		this->gAnimator = new Animator();
	}

	this->gAnimator->setAnimation(animation);
	this->currentTransformSource = ANIMATION;
}

// Ragdoll physics
void MeshDeformer::activateRagdoll() {
	// Already active.
	if (this->currentTransformSource == RAGDOLL) {
		return;
	}

	// Apply current animation's current pose transforms to physical joints.
	PxRigidDynamic* rigidDynamic;
	PxTransform tm;
	for (unsigned int j = 0; j < this->gJointCount; j++) {
		rigidDynamic = this->pRagdollCollisionActor[j]->pActor->is<PxRigidDynamic>();
		if (rigidDynamic == NULL) {
			continue;
		}

		// Get animation transformation of the joint.
		/*JointTransform* baseJointTransform = this->gJointTransforms[j];

		dx::XMStoreFloat4x4(
			&baseJointTransform->jointLocalTransformMatrix,
			dx::XMMatrixRotationQuaternion(
				dx::XMQuaternionRotationAxis(
					dx::XMVectorSet(jointQuaternion.x, jointQuaternion.y, jointQuaternion.z, 1.0f),
					jointQuaternion.w
				)
			)
		);

		// Set quaternion vector on physics side.
		tm = rigidDynamic->getGlobalPose();
		Vector4 jointQuaternion;
		PxVec3 axisVector;
		tm.q.toRadiansAndUnitAxis(jointQuaternion.w, axisVector);
		jointQuaternion.x = axisVector.x;
		jointQuaternion.y = axisVector.y;
		jointQuaternion.z = axisVector.z;*/

		// "Unfreeze" dynamic actor.
		rigidDynamic->wakeUp();
	}

	this->currentTransformSource = RAGDOLL;
}
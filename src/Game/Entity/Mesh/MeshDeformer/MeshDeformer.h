#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>
#include <algorithm>

#include "Structs.h"

namespace wrl = Microsoft::WRL;

class MeshDeformer{
public:
	void Setup();
	void Update();
	static void recalculateMatrices(Joint* baseJoint, dx::XMMATRIX* jointLocalTransform);

	// Joints
	Joint** gJoints;
	unsigned int gJointCount;
	Joint* rootJoint;
	Joint* getRootJoint();

	// Constant buffer
	MeshDeformerVSConstantBuffer gMeshDeformerVSConstantBuffer;
	wrl::ComPtr<ID3D11Buffer> pMeshDeformerVSConstantBuffer;
	bool shouldUpdateGPUData = false;
	bool dataChanged = false;
};
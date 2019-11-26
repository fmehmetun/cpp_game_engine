#include "ShadowBox.h"

ShadowBox::ShadowBox(Vector3 position, Vector3 direction, LIGHT_TYPE lightType) {
	this->lightType = lightType;

	Camera* shadowMapCamera;
	RenderTarget* shadowMapRenderTarget;
	Viewport* shadowMapViewport = new Viewport(
		{WIDTH, HEIGHT}
	);

	this->isActive = true;

	switch (this->lightType) {
		case LIGHT_TYPE::SPOT_LIGHT:
			shadowMapCamera = new Camera(position, direction, WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();

			this->gShadowMaps.push_back(
				new ShadowMap(
					shadowMapCamera,
					shadowMapRenderTarget,
					shadowMapViewport
				)
			);
			break;

		case LIGHT_TYPE::POINT_LIGHT:
			// One camera for each axes.
			// +X
			/*shadowBoxCamera = new Camera(position, Vector3::XAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);

			// +Y
			shadowBoxCamera = new Camera(position, Vector3::YAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);

			// +Z
			shadowBoxCamera = new Camera(position, Vector3::ZAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);

			// -X
			shadowBoxCamera = new Camera(position, -Vector3::XAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);

			// -Y
			shadowBoxCamera = new Camera(position, -Vector3::YAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);

			// -Z
			shadowBoxCamera = new Camera(position, -Vector3::ZAxis(), WIDTH, HEIGHT, PROJECTION_TYPE::PERSPECTIVE);
			shadowMapRenderTarget = new RenderTarget();
			this->gShadowMaps.push_back(
				std::make_pair(
					shadowBoxCamera,
					shadowBoxRenderTarget
				)
			);*/
			break;

		case LIGHT_TYPE::DIRECTIONAL_LIGHT:
			shadowMapCamera = new Camera(Vector3(0, 0, 0), direction, WIDTH, HEIGHT, PROJECTION_TYPE::ORTHOGRAPHIC);
			shadowMapCamera->setOrthographicProjection(24, 24);
			shadowMapRenderTarget = new RenderTarget();

			this->gShadowMaps.push_back(
				new ShadowMap(
					shadowMapCamera,
					shadowMapRenderTarget,
					shadowMapViewport
				)
			);
			break;
	}
}

void ShadowBox::Update(Vector3 position, Vector3 direction, Camera* activeCamera) {
	for (unsigned int a = 0; a < this->gShadowMaps.size(); a++) {
		switch (this->lightType) {
			case LIGHT_TYPE::SPOT_LIGHT:
				this->gShadowMaps.at(a)->pCamera->setPosition(position);
				this->gShadowMaps.at(a)->pCamera->setDirection(direction);
				break;

			case LIGHT_TYPE::POINT_LIGHT:
				this->gShadowMaps.at(a)->pCamera->setPosition(position);
				this->gShadowMaps.at(a)->pCamera->setDirection(direction);
				break;

			case LIGHT_TYPE::DIRECTIONAL_LIGHT:
				// Calculate active camera's frustum vertices.
				dx::BoundingFrustum activeCameraFrustum(
					dx::XMMatrixTranspose(
						dx::XMLoadFloat4x4(&activeCamera->gCameraVSConstantBuffer.projectionMatrix)
					)
				);
				dx::XMMATRIX viewInverse = dx::XMMatrixInverse(NULL,
					dx::XMMatrixTranspose(
						dx::XMLoadFloat4x4(&activeCamera->gCameraVSConstantBuffer.viewMatrix)
					)
				);
				activeCameraFrustum.Transform(activeCameraFrustum, viewInverse);

				// Calculate shadow box position and orientation with using active camera's frustum.
				// Get corner positions.
				//dx::XMFLOAT3* shadowMapFrustumCorners = new dx::XMFLOAT3[shadowMapFrustum.CORNER_COUNT];
				dx::XMFLOAT3 activeCameraFrustumCorners[activeCameraFrustum.CORNER_COUNT];
				activeCameraFrustum.GetCorners(activeCameraFrustumCorners);

				this->gShadowMaps.at(a)->pCamera->setPosition(position);
				this->gShadowMaps.at(a)->pCamera->setDirection(direction);
				break;
		}

		this->gShadowMaps.at(a)->pCamera->updateConstantBuffer();
	}
}
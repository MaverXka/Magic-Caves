#include "Camera.h"
#include "Engine.h"

Camera* Camera::MainCamera = nullptr;

Camera::Camera()
{
}

void Camera::RotateCamera(float _Pitch, float _Yaw, float _Roll)
{
	Pitch += _Pitch;
	Yaw += _Yaw;
	Roll += _Roll;
}

XMFLOAT4X4 Camera::GetProjectionViewMatrix()
{
	return ProjectionViewMatrix;
}

void Camera::SetMainCamera(Camera* CamPtr)
{
	MainCamera = CamPtr;
}

void Camera::UpdateProjectionViewMatrix()
{
	int wx, wy;
	Engine::GetEngine()->GetWindowSize(wx, wy);
	if (wx == 0 || wy == 0)
	{
		wx = 1;
		wy = 1;
	}

	XMMATRIX ProjectionMatrix = XMMatrixPerspectiveFovRH(XMConvertToRadians(CameraFieldOfView), (float)wx / (float)wy, 0.01f, 1000);

	XMVECTOR upDir = XMVectorSet(0, 0, 1, 0);

	XMMATRIX ViewMatrix = XMMatrixLookToRH(XMVectorSet(X, Y, Z, 0), XMVectorSet(1, 0, 0, 0), upDir);
	XMMATRIX MatrixMultiplied = XMMatrixMultiply(ViewMatrix, ProjectionMatrix);
	auto fin = XMMatrixTranspose(MatrixMultiplied);
	XMStoreFloat4x4(&ProjectionViewMatrix, fin);
}

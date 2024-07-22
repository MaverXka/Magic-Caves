#include "Camera.h"

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
	XMMATRIX ProjectionMatrix = XMMatrixPerspectiveFovRH(XMConvertToRadians(CameraFieldOfView), 1280.0f / 720.0f, 0.01f, 1000);
	XMMATRIX ViewMatrix = XMMatrixLookToRH(XMVectorSet(X, Y, Z, 0), XMVectorSet(1, 0, 0, 0), XMVectorSet(0, 0, 1, 0));
	XMMATRIX MatrixMultiplied = XMMatrixMultiply(ViewMatrix, ProjectionMatrix);
	auto fin = XMMatrixTranspose(MatrixMultiplied);
	XMStoreFloat4x4(&ProjectionViewMatrix, fin);
}

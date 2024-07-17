#include "Camera.h"

Camera::Camera()
{
	if (MainCamera == nullptr)
	{
		MainCamera = new Camera();
	}
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

void Camera::UpdateProjectionViewMatrix()
{
	XMMATRIX ProjectionMatrix = XMMatrixPerspectiveFovRH(XMConvertToRadians(CameraFieldOfView), 1920.0f / 1080.0f, 0.01f, 1000);
	XMMATRIX ViewMatrix = XMMatrixLookToRH(XMVectorSet(X, Y, Z, 0), XMVectorSet(0, 1, 0, 0), XMVectorSet(0, 0, 1, 0));
	XMMATRIX MatrixMultiplied = XMMatrixMultiply(ViewMatrix, ProjectionMatrix);
	XMStoreFloat4x4(&ProjectionViewMatrix, MatrixMultiplied);
}

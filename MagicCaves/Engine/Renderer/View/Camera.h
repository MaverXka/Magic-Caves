#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:

	Camera();

	/// <summary>
	/// Set's field of view for perspective projection
	/// </summary>
	/// <param name="NewFOV"> - New field of view in degrees</param>
	void SetFieldOfView(float NewFOV) { CameraFieldOfView = NewFOV; UpdateProjectionViewMatrix(); };

	/// <summary>
	/// Set's new camera view position.
	/// Using non structured coords because i have no structured vector standart
	/// </summary>
	/// <param name="NewX"></param>
	/// <param name="NewY"></param>
	/// <param name="NewZ"> - Height</param>
	void SetCameraPosition(float NewX, float NewY, float NewZ) { X = NewX; Y = NewY; Z = NewZ; UpdateProjectionViewMatrix(); };


	/// <summary>
	/// Offset's camera by params
	/// </summary>
	void OffsetCamera(float OffsetX, float OffsetY, float OffsetZ) { X += OffsetX; Y += OffsetY; Z += OffsetZ; UpdateProjectionViewMatrix(); };


	/// <summary>
	/// No implemented for now
	/// </summary>
	void RotateCamera(float _Pitch, float _Yaw, float _Roll);

	/// <summary>
	/// Get's main camera.
	/// </summary>
	static Camera* GetMainCamera() { return MainCamera; };

	/// <summary>
	/// Projection View matrix is calculated only when any actions with camera are created.
	/// </summary>
	/// <returns>Multiplied Projection and View matrix</returns>
	XMFLOAT4X4 GetProjectionViewMatrix();

	static void SetMainCamera(Camera* CamPtr);

protected:


	friend class Enigne;

	void UpdateProjectionViewMatrix();

	static Camera* MainCamera;

	float CameraFieldOfView = 90;
	float X, Y, Z;
	float Pitch, Yaw, Roll;

	XMFLOAT4X4 ProjectionViewMatrix;

};


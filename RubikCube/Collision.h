#pragma once

#include "Camera.h"
#include <DirectXCollision.h>
using namespace DirectX;


struct Ray
{
	Ray();
	Ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction);

	static Ray ScreenToRay(const Camera& camera, float screenX, float screenY);

	bool Hit(const BoundingBox& box, float* pOutDist = nullptr, float maxDist = FLT_MAX);
	bool Hit(const BoundingOrientedBox& box, float* pOutDist = nullptr, float maxDist = FLT_MAX);
	bool Hit(const BoundingSphere& sphere, float* pOutDist = nullptr, float maxDist = FLT_MAX);
	bool XM_CALLCONV Hit(DirectX::FXMVECTOR V0, DirectX::FXMVECTOR V1, DirectX::FXMVECTOR V2, float* pOutDist = nullptr, float maxDist = FLT_MAX);

	DirectX::XMFLOAT3 origin;		// 射线原点
	DirectX::XMFLOAT3 direction;	// 单位方向向量
};
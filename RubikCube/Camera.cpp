#include "Camera.h"
using namespace DirectX;

Camera::Camera()
{
}

Camera::~Camera()
{
}



DirectX::XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&mPosition);
}

DirectX::XMFLOAT3 Camera::GetPosition() const
{
	return mPosition;
}

//向前 向上 向右的坐标轴向量

DirectX::XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&mRight);
}

DirectX::XMFLOAT3 Camera::GetRight() const
{
	return mRight;
}

DirectX::XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&mUp);
}

DirectX::XMFLOAT3 Camera::GetUp() const
{
	return mUp;
}

DirectX::XMVECTOR Camera::GetLookXM() const
{
	return XMLoadFloat3(&mLook);
}

DirectX::XMFLOAT3 Camera::GetLook() const
{
	return mLook;
}

// 获取2D视椎体的长和宽

float Camera::GetNearWindowWidth() const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return mFarWindowHeight;
}


// 获取矩阵

DirectX::XMMATRIX Camera::GetViewXM() const
{
	return XMLoadFloat4x4(&mView);
}

DirectX::XMMATRIX Camera::GetProjXM() const
{
	return XMLoadFloat4x4(&mProj);
}

DirectX::XMMATRIX Camera::GetViewProjXM() const
{
	return XMLoadFloat4x4(&mView) * XMLoadFloat4x4(&mProj);
}

// 获取视口
D3D11_VIEWPORT Camera::GetViewPort() const
{
	return mViewPort;
}

// 设置视椎
void Camera::SetFrustum(float fovY, float aspect, float nearZ, float farZ)
{
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = nearZ;
	mFarZ = farZ;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	XMStoreFloat4x4(&mProj, XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ));
}

// 设置视口

void Camera::SetViewPort(const D3D11_VIEWPORT & viewPort)
{
	mViewPort = viewPort;
}

void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	mViewPort.TopLeftX = topLeftX;
	mViewPort.TopLeftY = topLeftY;
	mViewPort.Width = width;
	mViewPort.Height = height;
	mViewPort.MinDepth = minDepth;
	mViewPort.MaxDepth = maxDepth;
}

ThirdPersonCamera::ThirdPersonCamera()
	: Camera(), mTheta(), mPhi(), mDistance(), mTarget()
{
}

ThirdPersonCamera::~ThirdPersonCamera()
{
}


// 获取当前跟踪物体的位置
DirectX::XMFLOAT3 ThirdPersonCamera::GetTargetPosition() const
{
	return mTarget;
}


// 获取与物体的距离
float ThirdPersonCamera::GetDistance() const
{
	return mDistance;
}


// 获取绕X轴的旋转方向
float ThirdPersonCamera::GetRotationX() const
{
	return mPhi;
}


// 获取绕Y轴的旋转方向
float ThirdPersonCamera::GetRotationY() const
{
	return mTheta;
}

// 绕物体垂直旋转(上下视野角度Phi限制在[pi/6, pi/2])
void ThirdPersonCamera::RotateX(float rad)
{
	mPhi -= rad;
	// 将上下视野角度Phi限制在[pi/6, pi/2]，
	// 即余弦值[0, cos(pi/6)]之间
	if (mPhi < XM_PI / 6)
		mPhi = XM_PI / 6;
	else if (mPhi > XM_PIDIV2)
		mPhi = XM_PIDIV2;
}

// 绕物体水平旋转
void ThirdPersonCamera::RotateY(float rad)
{
	mTheta = XMScalarModAngle(mTheta - rad);
}

// 拉近物体
void ThirdPersonCamera::Approach(float dist)
{
	mDistance += dist;
	// 限制距离在[mMinDist, mMaxDist]之间
	if (mDistance < mMinDist)
		mDistance = mMinDist;
	else if (mDistance > mMaxDist)
		mDistance = mMaxDist;
}

// 设置初始绕X轴的弧度(上下视野角度Phi限制在[pi/6, pi/2])
void ThirdPersonCamera::SetRotationX(float phi)
{
	mPhi = XMScalarModAngle(phi);
	// 将上下视野角度Phi限制在[pi/6, pi/2]，
	// 即余弦值[0, cos(pi/6)]之间
	if (mPhi < XM_PI / 6)
		mPhi = XM_PI / 6;
	else if (mPhi > XM_PIDIV2)
		mPhi = XM_PIDIV2;
}

// 设置初始绕Y轴的弧度
void ThirdPersonCamera::SetRotationY(float theta)
{
	mTheta = XMScalarModAngle(theta);
}

// 设置并绑定待跟踪物体的位置
void ThirdPersonCamera::SetTarget(const DirectX::XMFLOAT3 & target)
{
	mTarget = target;
}

// 设置初始距离
void ThirdPersonCamera::SetDistance(float dist)
{
	mDistance = dist;
}

// 设置最小最大允许距离
void ThirdPersonCamera::SetDistanceMinMax(float minDist, float maxDist)
{
	mMinDist = minDist;
	mMaxDist = maxDist;
}

// 更新观察矩阵
void ThirdPersonCamera::UpdateViewMatrix()
{
	// 球面坐标系
	float x = mTarget.x + mDistance * sinf(mPhi) * cosf(mTheta);
	float z = mTarget.z + mDistance * sinf(mPhi) * sinf(mTheta);
	float y = mTarget.y + mDistance * cosf(mPhi);
	mPosition = { x, y, z };
	XMVECTOR P = XMLoadFloat3(&mPosition);
	XMVECTOR L = XMVector3Normalize(XMLoadFloat3(&mTarget) - P);
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), L));
	XMVECTOR U = XMVector3Cross(L, R);

	// 更新向量
	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);
	XMStoreFloat3(&mLook, L);

	mView = {
		mRight.x, mUp.x, mLook.x, 0.0f,
		mRight.y, mUp.y, mLook.y, 0.0f,
		mRight.z, mUp.z, mLook.z, 0.0f,
		-XMVectorGetX(XMVector3Dot(P, R)), -XMVectorGetX(XMVector3Dot(P, U)), -XMVectorGetX(XMVector3Dot(P, L)), 1.0f
	};
}
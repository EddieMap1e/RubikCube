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

//��ǰ ���� ���ҵ�����������

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

// ��ȡ2D��׵��ĳ��Ϳ�

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


// ��ȡ����

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

// ��ȡ�ӿ�
D3D11_VIEWPORT Camera::GetViewPort() const
{
	return mViewPort;
}

// ������׵
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

// �����ӿ�

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


// ��ȡ��ǰ���������λ��
DirectX::XMFLOAT3 ThirdPersonCamera::GetTargetPosition() const
{
	return mTarget;
}


// ��ȡ������ľ���
float ThirdPersonCamera::GetDistance() const
{
	return mDistance;
}


// ��ȡ��X�����ת����
float ThirdPersonCamera::GetRotationX() const
{
	return mPhi;
}


// ��ȡ��Y�����ת����
float ThirdPersonCamera::GetRotationY() const
{
	return mTheta;
}

// �����崹ֱ��ת(������Ұ�Ƕ�Phi������[pi/6, pi/2])
void ThirdPersonCamera::RotateX(float rad)
{
	mPhi -= rad;
	// ��������Ұ�Ƕ�Phi������[pi/6, pi/2]��
	// ������ֵ[0, cos(pi/6)]֮��
	if (mPhi < XM_PI / 6)
		mPhi = XM_PI / 6;
	else if (mPhi > XM_PIDIV2)
		mPhi = XM_PIDIV2;
}

// ������ˮƽ��ת
void ThirdPersonCamera::RotateY(float rad)
{
	mTheta = XMScalarModAngle(mTheta - rad);
}

// ��������
void ThirdPersonCamera::Approach(float dist)
{
	mDistance += dist;
	// ���ƾ�����[mMinDist, mMaxDist]֮��
	if (mDistance < mMinDist)
		mDistance = mMinDist;
	else if (mDistance > mMaxDist)
		mDistance = mMaxDist;
}

// ���ó�ʼ��X��Ļ���(������Ұ�Ƕ�Phi������[pi/6, pi/2])
void ThirdPersonCamera::SetRotationX(float phi)
{
	mPhi = XMScalarModAngle(phi);
	// ��������Ұ�Ƕ�Phi������[pi/6, pi/2]��
	// ������ֵ[0, cos(pi/6)]֮��
	if (mPhi < XM_PI / 6)
		mPhi = XM_PI / 6;
	else if (mPhi > XM_PIDIV2)
		mPhi = XM_PIDIV2;
}

// ���ó�ʼ��Y��Ļ���
void ThirdPersonCamera::SetRotationY(float theta)
{
	mTheta = XMScalarModAngle(theta);
}

// ���ò��󶨴����������λ��
void ThirdPersonCamera::SetTarget(const DirectX::XMFLOAT3 & target)
{
	mTarget = target;
}

// ���ó�ʼ����
void ThirdPersonCamera::SetDistance(float dist)
{
	mDistance = dist;
}

// ������С����������
void ThirdPersonCamera::SetDistanceMinMax(float minDist, float maxDist)
{
	mMinDist = minDist;
	mMaxDist = maxDist;
}

// ���¹۲����
void ThirdPersonCamera::UpdateViewMatrix()
{
	// ��������ϵ
	float x = mTarget.x + mDistance * sinf(mPhi) * cosf(mTheta);
	float z = mTarget.z + mDistance * sinf(mPhi) * sinf(mTheta);
	float y = mTarget.y + mDistance * cosf(mPhi);
	mPosition = { x, y, z };
	XMVECTOR P = XMLoadFloat3(&mPosition);
	XMVECTOR L = XMVector3Normalize(XMLoadFloat3(&mTarget) - P);
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), L));
	XMVECTOR U = XMVector3Cross(L, R);

	// ��������
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
#include "Collision.h"

Ray::Ray()
	: origin(), direction(0.0f, 0.0f, 1.0f)
{
}

Ray::Ray(const DirectX::XMFLOAT3 & origin, const DirectX::XMFLOAT3 & direction)
	: origin(origin)
{
	// ���ߵ�direction���ȱ���Ϊ1.0f�������1e-5f��
	XMVECTOR dirLength = XMVector3Length(XMLoadFloat3(&direction));
	XMVECTOR error = XMVectorAbs(dirLength - XMVectorSplatOne());
	assert(XMVector3Less(error, XMVectorReplicate(1e-5f)));

	XMStoreFloat3(&this->direction, XMVector3Normalize(XMLoadFloat3(&direction)));
}

Ray Ray::ScreenToRay(const Camera & camera, float screenX, float screenY)
{
	// ******************
	// ��ѡ��DirectX::XMVector3Unproject��������ʡ���˴���������ϵ���ֲ�����ϵ�ı任
	//

	// ����Ļ�������ӿڱ任��NDC����ϵ
	static const XMVECTORF32 D = { { { -1.0f, 1.0f, 0.0f, 0.0f } } };
	XMVECTOR V = XMVectorSet(screenX, screenY, 0.0f, 1.0f);
	D3D11_VIEWPORT viewPort = camera.GetViewPort();

	XMVECTOR Scale = XMVectorSet(viewPort.Width * 0.5f, -viewPort.Height * 0.5f, viewPort.MaxDepth - viewPort.MinDepth, 1.0f);
	Scale = XMVectorReciprocal(Scale);

	XMVECTOR Offset = XMVectorSet(-viewPort.TopLeftX, -viewPort.TopLeftY, -viewPort.MinDepth, 0.0f);
	Offset = XMVectorMultiplyAdd(Scale, Offset, D.v);

	// ��NDC����ϵ�任����������ϵ
	XMMATRIX Transform = XMMatrixMultiply(camera.GetViewXM(), camera.GetProjXM());
	Transform = XMMatrixInverse(nullptr, Transform);

	XMVECTOR Target = XMVectorMultiplyAdd(V, Scale, Offset);
	Target = XMVector3TransformCoord(Target, Transform);

	// �������
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, XMVector3Normalize(Target - camera.GetPositionXM()));
	return Ray(camera.GetPosition(), direction);
}

bool Ray::Hit(const DirectX::BoundingBox & box, float * pOutDist, float maxDist)
{

	float dist;
	bool res = box.Intersects(XMLoadFloat3(&origin), XMLoadFloat3(&direction), dist);
	if (pOutDist)
		*pOutDist = dist;
	return dist > maxDist ? false : res;
}

bool Ray::Hit(const DirectX::BoundingOrientedBox & box, float * pOutDist, float maxDist)
{
	float dist;
	bool res = box.Intersects(XMLoadFloat3(&origin), XMLoadFloat3(&direction), dist);
	if (pOutDist)
		*pOutDist = dist;
	return dist > maxDist ? false : res;
}

bool Ray::Hit(const DirectX::BoundingSphere & sphere, float * pOutDist, float maxDist)
{
	float dist;
	bool res = sphere.Intersects(XMLoadFloat3(&origin), XMLoadFloat3(&direction), dist);
	if (pOutDist)
		*pOutDist = dist;
	return dist > maxDist ? false : res;
}

bool XM_CALLCONV Ray::Hit(FXMVECTOR V0, FXMVECTOR V1, FXMVECTOR V2, float * pOutDist, float maxDist)
{
	float dist;
	bool res = TriangleTests::Intersects(XMLoadFloat3(&origin), XMLoadFloat3(&direction), V0, V1, V2, dist);
	if (pOutDist)
		*pOutDist = dist;
	return dist > maxDist ? false : res;
}
#include "common_pch.h"
#include "XTKPrimitiveBatch.h"
#include "Core/Entity/Camera/JCamera.h"
#include "Core/Graphics/GraphicDevice.h"
#include "Core/Interface/MManagerInterface.h"

XTKPrimitiveBatch::XTKPrimitiveBatch() = default;

XTKPrimitiveBatch::~XTKPrimitiveBatch()
{
	XTKPrimitiveBatch::Release();
}

void XTKPrimitiveBatch::Initialize()
{
	mBatch       = std::make_unique<PrimitiveBatch<VertexPositionColor>>(G_Context.GetImmediateDeviceContext());
	mBatchEffect = std::make_unique<BasicEffect>(G_Context.GetDevice());
	mBatchEffect->SetVertexColorEnabled(true);

	{
		void const* shaderByteCode;
		size_t      byteCodeLength;

		mBatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

		CheckResult(G_Context.GetDevice()->
							  CreateInputLayout(
												VertexPositionColor::InputElements, // Pos, Color
												VertexPositionColor::InputElementCount, // 2
												shaderByteCode,
												byteCodeLength,
												mBatchInputLayout.GetAddressOf()
											   ));
	}
}

void XTKPrimitiveBatch::Update(float_t DeltaTime)
{
	const JCamera* cam = IManager.CameraManager.GetCurrentMainCam();

	mBatchEffect->SetWorld(cam->GetWorldMatrix());
	mBatchEffect->SetView(cam->GetViewMatrix());
	mBatchEffect->SetProjection(cam->GetProjMatrix());
}

void XTKPrimitiveBatch::Release()
{
	mBatchEffect      = nullptr;
	mBatch            = nullptr;
	mBatchInputLayout = nullptr;
}

void XTKPrimitiveBatch::PreRender()
{
	mBatchEffect->Apply(G_Context.GetImmediateDeviceContext());
}

void XTKPrimitiveBatch::Render()
{
	G_Context.GetImmediateDeviceContext()->IASetInputLayout(mBatchInputLayout.Get());

	mBatch->Begin();
}

void XTKPrimitiveBatch::PostRender()
{
	mBatch->End();
}

void XTKPrimitiveBatch::Draw(BoundingSphere& InSphere, FXMVECTOR InColor) const
{
	const XMVECTOR origin = XMLoadFloat3(&InSphere.Center);

	const float radius = InSphere.Radius;

	const XMVECTOR xAxis = XMVectorScale(g_XMIdentityR0, radius);
	const XMVECTOR yAxis = XMVectorScale(g_XMIdentityR1, radius);
	const XMVECTOR zAxis = XMVectorScale(g_XMIdentityR2, radius);

	DrawRing(origin, xAxis, zAxis, InColor);
	DrawRing(origin, xAxis, yAxis, InColor);
	DrawRing(origin, yAxis, zAxis, InColor);
}

void XTKPrimitiveBatch::Draw(const BoundingBox& InBox, FXMVECTOR InColor) const
{
	XMMATRIX       matWorld = XMMatrixScaling(InBox.Extents.x, InBox.Extents.y, InBox.Extents.z);
	const XMVECTOR position = XMLoadFloat3(&InBox.Center);
	matWorld.r[3]           = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

	DrawCube(matWorld, InColor);
}

void XTKPrimitiveBatch::Draw(const BoundingOrientedBox& InObb, FXMVECTOR InColor) const
{
	XMMATRIX       matWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&InObb.Orientation));
	const XMMATRIX matScale = XMMatrixScaling(InObb.Extents.x, InObb.Extents.y, InObb.Extents.z);
	matWorld                = XMMatrixMultiply(matScale, matWorld);
	const XMVECTOR position = XMLoadFloat3(&InObb.Center);
	matWorld.r[3]           = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

	DrawCube(matWorld, InColor);
}

void XTKPrimitiveBatch::Draw(const BoundingFrustum& InFrustum, FXMVECTOR InColor) const
{
	XMFLOAT3 corners[BoundingFrustum::CORNER_COUNT];
	InFrustum.GetCorners(corners);

	VertexPositionColor verts[24] = {};
	verts[0].position             = corners[0];
	verts[1].position             = corners[1];
	verts[2].position             = corners[1];
	verts[3].position             = corners[2];
	verts[4].position             = corners[2];
	verts[5].position             = corners[3];
	verts[6].position             = corners[3];
	verts[7].position             = corners[0];

	verts[8].position  = corners[0];
	verts[9].position  = corners[4];
	verts[10].position = corners[1];
	verts[11].position = corners[5];
	verts[12].position = corners[2];
	verts[13].position = corners[6];
	verts[14].position = corners[3];
	verts[15].position = corners[7];

	verts[16].position = corners[4];
	verts[17].position = corners[5];
	verts[18].position = corners[5];
	verts[19].position = corners[6];
	verts[20].position = corners[6];
	verts[21].position = corners[7];
	verts[22].position = corners[7];
	verts[23].position = corners[4];

	for (size_t j = 0; j < std::size(verts); ++j)
	{
		XMStoreFloat4(&verts[j].color, InColor);
	}

	mBatch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, verts, static_cast<UINT>(std::size(verts)));
}

void XTKPrimitiveBatch::DrawCube(CXMMATRIX matWorld, FXMVECTOR color) const
{
	static const XMVECTORF32 s_verts[8] =
	{
		{{{-1.f, -1.f, -1.f, 0.f}}},
		{{{1.f, -1.f, -1.f, 0.f}}},
		{{{1.f, -1.f, 1.f, 0.f}}},
		{{{-1.f, -1.f, 1.f, 0.f}}},
		{{{-1.f, 1.f, -1.f, 0.f}}},
		{{{1.f, 1.f, -1.f, 0.f}}},
		{{{1.f, 1.f, 1.f, 0.f}}},
		{{{-1.f, 1.f, 1.f, 0.f}}}
	};

	static const WORD s_indices[] =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	VertexPositionColor verts[8];
	for (size_t i = 0; i < 8; ++i)
	{
		const XMVECTOR v = XMVector3Transform(s_verts[i], matWorld);
		XMStoreFloat3(&verts[i].position, v);
		XMStoreFloat4(&verts[i].color, color);
	}

	mBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINELIST, s_indices, static_cast<UINT>(std::size(s_indices)), verts, 8);
}


void XTKPrimitiveBatch::DrawGrid(FXMVECTOR InXAxis, FXMVECTOR InYAxis, FXMVECTOR InOrigin,
								 size_t    InXdivs, size_t    InYdivs, GXMVECTOR InColor) const
{
	InXdivs = std::max<size_t>(1, InXdivs);
	InYdivs = std::max<size_t>(1, InYdivs);

	for (size_t i = 0; i <= InXdivs; ++i)
	{
		float percent  = float(i) / float(InXdivs);
		percent        = (percent * 2.f) - 1.f;
		XMVECTOR scale = XMVectorScale(InXAxis, percent);
		scale          = XMVectorAdd(scale, InOrigin);

		const VertexPositionColor v1(XMVectorSubtract(scale, InYAxis), InColor);
		const VertexPositionColor v2(XMVectorAdd(scale, InYAxis), InColor);
		mBatch->DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= InYdivs; i++)
	{
		FLOAT percent  = float(i) / float(InYdivs);
		percent        = (percent * 2.f) - 1.f;
		XMVECTOR scale = XMVectorScale(InYAxis, percent);
		scale          = XMVectorAdd(scale, InOrigin);

		const VertexPositionColor v1(XMVectorSubtract(scale, InXAxis), InColor);
		const VertexPositionColor v2(XMVectorAdd(scale, InXAxis), InColor);
		mBatch->DrawLine(v1, v2);
	}
}

void XTKPrimitiveBatch::DrawRing(FXMVECTOR InOrigin, FXMVECTOR InMajorAxis, FXMVECTOR InMinorAxis, GXMVECTOR InColor) const
{
	constexpr size_t c_ringSegments = 32;

	VertexPositionColor verts[c_ringSegments + 1];

	constexpr float fAngleDelta = XM_2PI / float(c_ringSegments);
	// Instead of calling cos/sin for each segment we calculate
	// the sign of the angle delta and then incrementally calculate sin
	// and cosine from then on.
	const XMVECTOR           cosDelta       = XMVectorReplicate(cosf(fAngleDelta));
	const XMVECTOR           sinDelta       = XMVectorReplicate(sinf(fAngleDelta));
	XMVECTOR                 incrementalSin = XMVectorZero();
	static const XMVECTORF32 s_initialCos   =
	{
		{{1.f, 1.f, 1.f, 1.f}}
	};
	XMVECTOR incrementalCos = s_initialCos.v;
	for (size_t i = 0; i < c_ringSegments; i++)
	{
		XMVECTOR pos = XMVectorMultiplyAdd(InMajorAxis, incrementalCos, InOrigin);
		pos          = XMVectorMultiplyAdd(InMajorAxis, incrementalSin, pos);
		XMStoreFloat3(&verts[i].position, pos);
		XMStoreFloat4(&verts[i].color, InColor);
		// Standard formula to rotate a vector.
		const XMVECTOR newCos = XMVectorSubtract(XMVectorMultiply(incrementalCos, cosDelta),
												 XMVectorMultiply(incrementalSin, sinDelta));
		const XMVECTOR newSin = XMVectorAdd(XMVectorMultiply(incrementalCos, sinDelta),
											XMVectorMultiply(incrementalSin, cosDelta));
		incrementalCos = newCos;
		incrementalSin = newSin;
	}
	verts[c_ringSegments] = verts[0];

	mBatch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, c_ringSegments + 1);
}

void XTKPrimitiveBatch::DrawRay(FXMVECTOR InOrigin, FXMVECTOR InDirection, bool bNormalize, FXMVECTOR InColor) const
{
	VertexPositionColor verts[3];
	XMStoreFloat3(&verts[0].position, InOrigin);

	XMVECTOR normDirection = XMVector3Normalize(InDirection);
	XMVECTOR rayDirection  = (bNormalize) ? normDirection : InDirection;

	XMVECTOR perpVector = XMVector3Cross(normDirection, g_XMIdentityR1);

	if (XMVector3Equal(XMVector3LengthSq(perpVector), g_XMZero))
	{
		perpVector = XMVector3Cross(normDirection, g_XMIdentityR2);
	}
	perpVector = XMVector3Normalize(perpVector);

	XMStoreFloat3(&verts[1].position, XMVectorAdd(rayDirection, InOrigin));
	perpVector    = XMVectorScale(perpVector, 0.0625f);
	normDirection = XMVectorScale(normDirection, -0.25f);
	rayDirection  = XMVectorAdd(perpVector, rayDirection);
	rayDirection  = XMVectorAdd(normDirection, rayDirection);
	XMStoreFloat3(&verts[2].position, XMVectorAdd(rayDirection, InOrigin));

	XMStoreFloat4(&verts[0].color, InColor);
	XMStoreFloat4(&verts[1].color, InColor);
	XMStoreFloat4(&verts[2].color, InColor);

	mBatch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 2);
}

void XTKPrimitiveBatch::DrawTriangle(FXMVECTOR InPointA, FXMVECTOR InPointB, FXMVECTOR InPointC, GXMVECTOR InColor) const
{
	VertexPositionColor verts[4];
	XMStoreFloat3(&verts[0].position, InPointA);
	XMStoreFloat3(&verts[1].position, InPointB);
	XMStoreFloat3(&verts[2].position, InPointC);
	XMStoreFloat3(&verts[3].position, InPointA);

	XMStoreFloat4(&verts[0].color, InColor);
	XMStoreFloat4(&verts[1].color, InColor);
	XMStoreFloat4(&verts[2].color, InColor);
	XMStoreFloat4(&verts[3].color, InColor);

	mBatch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 4);
}

void XTKPrimitiveBatch::DrawQuad(FXMVECTOR InPointA, FXMVECTOR InPointB, FXMVECTOR InPointC, GXMVECTOR InPointD,
								 HXMVECTOR InColor) const
{
	VertexPositionColor verts[5];
	XMStoreFloat3(&verts[0].position, InPointA);
	XMStoreFloat3(&verts[1].position, InPointB);
	XMStoreFloat3(&verts[2].position, InPointC);
	XMStoreFloat3(&verts[3].position, InPointD);
	XMStoreFloat3(&verts[4].position, InPointA);

	XMStoreFloat4(&verts[0].color, InColor);
	XMStoreFloat4(&verts[1].color, InColor);
	XMStoreFloat4(&verts[2].color, InColor);
	XMStoreFloat4(&verts[3].color, InColor);
	XMStoreFloat4(&verts[4].color, InColor);

	mBatch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 5);
}

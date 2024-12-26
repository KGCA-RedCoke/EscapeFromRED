#include "XTKPrimitiveBatch.h"
#include "Core/Entity/Camera/JCameraComponent.h"
#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Interface/JWorld.h"

XTKPrimitiveBatch::XTKPrimitiveBatch() = default;

XTKPrimitiveBatch::~XTKPrimitiveBatch()
{
	XTKPrimitiveBatch::Release();
}

void XTKPrimitiveBatch::Initialize()
{
	mBatch       = std::make_unique<PrimitiveBatch<VertexPositionColor>>(GetWorld.D3D11API->GetImmediateDeviceContext());
	mBatchEffect = std::make_unique<BasicEffect>(GetWorld.D3D11API->GetDevice());
	mBatchEffect->SetVertexColorEnabled(true);

	{
		void const* shaderByteCode;
		size_t      byteCodeLength;

		mBatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

		CheckResult(GetWorld.D3D11API->GetDevice()->
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
	// JCameraComponent* cam = MShaderManager::Get().mCachedCamera;
	// if (!cam)
	// 	return;
	//
	// mBatchEffect->SetWorld(FMatrix::Identity);
	// mBatchEffect->SetView(cam->GetViewMatrix());
	// mBatchEffect->SetProjection(cam->GetProjMatrix());
}

void XTKPrimitiveBatch::Release()
{
	mBatchEffect      = nullptr;
	mBatch            = nullptr;
	mBatchInputLayout = nullptr;
}

void XTKPrimitiveBatch::PreRender(const FMatrix& InView, const FMatrix& InProj)
{
	mBatchEffect->SetWorld(FMatrix::Identity);
	mBatchEffect->SetView(InView);
	mBatchEffect->SetProjection(InProj);

	MShaderManager::Get().UpdateShader(nullptr, nullptr);
	mBatchEffect->Apply(GetWorld.D3D11API->GetImmediateDeviceContext());

	GetWorld.D3D11API->GetImmediateDeviceContext()->IASetInputLayout(mBatchInputLayout.Get());

	mBatch->Begin();
}

void XTKPrimitiveBatch::PostRender()
{
	mBatch->End();
}

void XTKPrimitiveBatch::Draw()
{
	constexpr FVector4 origin = {0, 0, 0, 0};

	// Draw X Axis (Infinity Line)
	DrawRay_Implement(
					  origin,
					  {10000.f, 0.f, 0.f, 0.f},
					  1,
					  false,
					  {1.f, 0.f, 0.f, 1.f}
					 );

	// Draw Z Axis
	DrawRay_Implement(
					  origin,
					  {0.f, 0.f, 10000.f, 0.f},
					  1,
					  false,
					  {0.f, 0.f, 1.f, 1.f}
					 );

	// Draw Y Axis
	DrawRay_Implement(
					  origin,
					  {0.f, 10000.f, 0.f, 0.f},
					  1,
					  false,
					  {0.f, 1.f, 0.f, 1.f}
					 );

	DrawCapsule_Implement(FMatrix::Identity, 40.f, 200.f);

}

void XTKPrimitiveBatch::Draw(BoundingSphere& InSphere, FXMVECTOR InColor) const
{
	const XMVECTOR origin = XMLoadFloat3(&InSphere.Center);

	const float radius = InSphere.Radius;

	const XMVECTOR xAxis = XMVectorScale(g_XMIdentityR0, radius);
	const XMVECTOR yAxis = XMVectorScale(g_XMIdentityR1, radius);
	const XMVECTOR zAxis = XMVectorScale(g_XMIdentityR2, radius);

	DrawRing_Implement(origin, xAxis, zAxis, InColor);
	DrawRing_Implement(origin, xAxis, yAxis, InColor);
	DrawRing_Implement(origin, yAxis, zAxis, InColor);
}

void XTKPrimitiveBatch::Draw(const BoundingBox& InBox, FXMVECTOR InColor) const
{
	XMMATRIX       matWorld = XMMatrixScaling(InBox.Extents.x, InBox.Extents.y, InBox.Extents.z);
	const XMVECTOR position = XMLoadFloat3(&InBox.Center);
	matWorld.r[3]           = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

	DrawCube_Implement(matWorld, InColor);
}

void XTKPrimitiveBatch::Draw(const BoundingOrientedBox& InObb, FXMVECTOR InColor) const
{
	XMMATRIX       matWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&InObb.Orientation));
	const XMMATRIX matScale = XMMatrixScaling(InObb.Extents.x, InObb.Extents.y, InObb.Extents.z);
	matWorld                = XMMatrixMultiply(matScale, matWorld);
	const XMVECTOR position = XMLoadFloat3(&InObb.Center);
	matWorld.r[3]           = XMVectorSelect(matWorld.r[3], position, g_XMSelect1110);

	DrawCube_Implement(matWorld, InColor);
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

void XTKPrimitiveBatch::DrawCube_Implement(CXMMATRIX matWorld, FXMVECTOR color) const
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


void XTKPrimitiveBatch::DrawGrid_Implement(FXMVECTOR InXAxis, FXMVECTOR InYAxis, FXMVECTOR InOrigin,
										   size_t    InXdivs, size_t    InYdivs, GXMVECTOR InColor) const
{
	InXdivs = FMath::Max<size_t>(1, InXdivs);
	InYdivs = FMath::Max<size_t>(1, InYdivs);

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

void XTKPrimitiveBatch::DrawRing_Implement(FXMVECTOR InOrigin, FXMVECTOR InMajorAxis, FXMVECTOR InMinorAxis,
										   GXMVECTOR InColor) const
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

void XTKPrimitiveBatch::DrawRay_Implement(FXMVECTOR InOrigin, FXMVECTOR   InDirection, float InLength,
										  bool      bNormalize, FXMVECTOR InColor) const
{
	VertexPositionColor verts[3];
	XMStoreFloat3(&verts[0].position, InOrigin);

	XMVECTOR normDirection = XMVector3Normalize(InDirection);
	XMVECTOR rayDirection  = (bNormalize) ? normDirection : InDirection;
	rayDirection           = XMVectorScale(rayDirection, InLength);

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

void XTKPrimitiveBatch::DrawTriangle_Implement(FXMVECTOR InPointA, FXMVECTOR InPointB, FXMVECTOR InPointC,
											   GXMVECTOR InColor) const
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

void XTKPrimitiveBatch::DrawQuad_Implement(FXMVECTOR InPointA, FXMVECTOR InPointB, FXMVECTOR InPointC, GXMVECTOR InPointD,
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

void XTKPrimitiveBatch::DrawCylinder_Implement(CXMMATRIX MatWorld, float Radius, float Height, FXMVECTOR Color) const
{
	const int   slices    = 16;
	const float angleStep = XM_2PI / slices;

	VertexPositionColor verts[2 * slices];
	WORD                indices[4 * slices];

	for (int i = 0; i < slices; ++i)
	{
		float angle = i * angleStep;
		float x     = Radius * cosf(angle);
		float z     = Radius * sinf(angle);

		// 상단과 하단의 정점
		XMVECTOR top    = XMVector3Transform(XMVectorSet(x, Height * 0.5f, z, 0.f), MatWorld);
		XMVECTOR bottom = XMVector3Transform(XMVectorSet(x, -Height * 0.5f, z, 0.f), MatWorld);

		XMStoreFloat3(&verts[i].position, top);
		XMStoreFloat3(&verts[i + slices].position, bottom);

		XMStoreFloat4(&verts[i].color, Color);
		XMStoreFloat4(&verts[i + slices].color, Color);

		// 인덱스 설정
		indices[i * 4]     = i;
		indices[i * 4 + 1] = (i + 1) % slices;
		indices[i * 4 + 2] = i + slices;
		indices[i * 4 + 3] = (i + 1) % slices + slices;
	}

	mBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINELIST, indices, 4 * slices, verts, 2 * slices);
}

void XTKPrimitiveBatch::DrawHemisphere_Implement(CXMMATRIX MatWorld, float Radius, int LongitudeLines, int LatitudeLines,
												 FXMVECTOR Color) const
{
	// // 수평선(위도)과 수직선(경도) 개수를 제한적으로 설정
	// for (int lat = 1; lat <= LatitudeLines; ++lat)
	// {
	// 	float phi = XM_PIDIV2 * (float(lat) / LatitudeLines); // 0 ~ π/2
	// 	float r = Radius * sinf(phi);
	// 	float y = Radius * cosf(phi);
	//
	// 	// 수평선 (원)
	// 	XMVECTOR prev = XMVectorSet(r, y, 0.f, 1.f);
	// 	for (int lon = 1; lon <= LongitudeLines; ++lon)
	// 	{
	// 		float theta = XM_2PI * (float(lon) / LongitudeLines);
	// 		XMVECTOR curr = XMVectorSet(r * cosf(theta), y, r * sinf(theta), 1.f);
	//
	// 		prev = XMVector3Transform(prev, MatWorld);
	// 		curr = XMVector3Transform(curr, MatWorld);
	//
	// 		mBatch->DrawLine(prev, curr, Color);
	// 		prev = curr;
	// 	}
	// }
	//
	// // 수직선 (경도)
	// for (int lon = 0; lon < LongitudeLines; ++lon)
	// {
	// 	float theta = XM_2PI * (float(lon) / LongitudeLines);
	// 	float x = Radius * cosf(theta);
	// 	float z = Radius * sinf(theta);
	//
	// 	XMVECTOR prev = XMVectorSet(0.f, Radius, 0.f, 1.f); // 반구의 꼭대기
	// 	for (int lat = 1; lat <= LatitudeLines; ++lat)
	// 	{
	// 		float phi = XM_PIDIV2 * (float(lat) / LatitudeLines); // 0 ~ π/2
	// 		float r = Radius * sinf(phi);
	// 		float y = Radius * cosf(phi);
	//
	// 		XMVECTOR curr = XMVectorSet(r * x / Radius, y, r * z / Radius, 1.f);
	//
	// 		prev = XMVector3Transform(prev, MatWorld);
	// 		curr = XMVector3Transform(curr, MatWorld);
	//
	// 		mBatch->DrawLine(prev, curr, Color);
	// 		prev = curr;
	// 	}
	// }
}


void XTKPrimitiveBatch::DrawCapsule_Implement(CXMMATRIX MatWorld, float          Radius, float Height, int LongitudeLines,
											  int       LatitudeLines, FXMVECTOR Color) const
{
	const float HalfHeight = Height * 0.5f;

	// 원통을 그리기 위한 세로 라인
	for (int lon = 0; lon < LongitudeLines; ++lon)
	{
		float theta = XM_2PI * (float(lon) / LongitudeLines);
		float x     = Radius * cosf(theta);
		float z     = Radius * sinf(theta);

		VertexPositionColor top;
		top.position = {x, HalfHeight, z};
		XMStoreFloat4(&top.color, Color);

		VertexPositionColor bottom;
		bottom.position = {x, -HalfHeight, z};
		XMStoreFloat4(&bottom.color, Color);

		mBatch->DrawLine(top, bottom);
	}


	VertexPositionColor curr;
	VertexPositionColor next;
	XMStoreFloat4(&curr.color, Color);
	XMStoreFloat4(&next.color, Color);

	// 반구 위쪽 (위도)
	for (int lat = 0; lat < LatitudeLines; ++lat)
	{
		float phi = XM_PIDIV2 * (float(lat) / LatitudeLines); // 0 ~ π/2
		float r   = Radius * sinf(phi);
		float y   = Radius * cosf(phi);


		for (int lon = 0; lon < LongitudeLines; ++lon)
		{
			float theta = XM_2PI * (float(lon) / LongitudeLines);
			float x     = r * cosf(theta);
			float z     = r * sinf(theta);

			curr.position = {x, y + HalfHeight, z};

			next.position = {
				r * cosf(theta + XM_2PI / LongitudeLines),
				y + HalfHeight,
				r * sinf(theta + XM_2PI / LongitudeLines)
			};

			XMStoreFloat3(&curr.position, XMVector3Transform(XMLoadFloat3(&curr.position), MatWorld));

			XMStoreFloat3(&next.position, XMVector3Transform(XMLoadFloat3(&next.position), MatWorld));


			mBatch->DrawLine(curr, next);
		}
	}

	// 반구 아래쪽 (위도)
	for (int lat = 0; lat < LatitudeLines; ++lat)
	{
		float phi = XM_PIDIV2 * (float(lat) / LatitudeLines); // 0 ~ π/2
		float r   = Radius * sinf(phi);
		float y   = Radius * cosf(phi);


		for (int lon = 0; lon < LongitudeLines; ++lon)
		{
			float theta = XM_2PI * (float(lon) / LongitudeLines);
			float x     = r * cosf(theta);
			float z     = r * sinf(theta);

			curr.position = {x, -y - HalfHeight, z};

			next.position = {
				r * cosf(theta + XM_2PI / LongitudeLines),
				-y - HalfHeight,
				r * sinf(theta + XM_2PI / LongitudeLines)
			};

			XMStoreFloat3(&curr.position, XMVector3Transform(XMLoadFloat3(&curr.position), MatWorld));
			XMStoreFloat3(&next.position, XMVector3Transform(XMLoadFloat3(&next.position), MatWorld));

			mBatch->DrawLine(curr, next);
		}
	}
}

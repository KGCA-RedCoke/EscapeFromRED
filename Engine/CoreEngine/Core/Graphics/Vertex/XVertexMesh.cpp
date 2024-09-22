// #include "XVertexMesh.h"
// #include "Core/Graphics/GraphicDevice.h"
// #include "Core/Graphics/ConstantBuffer/CWorldBuffer.h"
// #include "Core/Graphics/Shader/MShaderManager.h"
//
// XVertexMesh::XVertexMesh()
// 	: mPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
// 	  mDrawColor(FLinearColor::Green),
// 	  mTransform(XMMatrixIdentity())
// {
// 	mWorldBuffer = std::make_unique<CWorldBuffer>();
//
// 	mVertexArray.reserve(3);
// }
//
// XVertexMesh::XVertexMesh(JWTextView InFbxFile)
// 	: XVertexMesh()
// {
// 	SetMesh(InFbxFile);
// }
//
// XVertexMesh::~XVertexMesh()
// {
// 	XVertexMesh::Release();
// }
//
// void XVertexMesh::Initialize()
// {
// 	for (int32_t i = 0; i < mMeshDatas.size(); ++i)
// 	{
// 		mMeshDatas[i].Initialize();
// 	}
//
// 	// mWorldBuffer->Initialize();
//
// 	// FBX모델로 부터 Vertex 생성
// 	CreateVertexArray();
//
// 	// 정점, 인덱스 버퍼 생성
// 	CreateVertexBuffer();
// 	CreateIndexBuffer();
//
// 	LoadShader(mShaderFileName);
// }
//
// void XVertexMesh::Update(float DeltaTime)
// {
// 	mWorldBuffer->SetWorldMatrix(mTransform);
// 	mWorldBuffer->Update(DeltaTime);
// }
//
// void XVertexMesh::PreRender() {}
//
// void XVertexMesh::Render()
// {
// 	UINT stride = mVertexArray[0].GetStride();
// 	UINT offset = 0;
//
// 	mWorldBuffer->Render();
// 	mShader->Render();
//
// 	G_Context.GetImmediateDeviceContext()->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
// 	G_Context.GetImmediateDeviceContext()->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
// 	G_Context.GetImmediateDeviceContext()->IASetPrimitiveTopology(mPrimitiveType);
// 	G_Context.GetImmediateDeviceContext()->DrawIndexed(mIndiceArray.size(), 0, 0);
// }
//
// void XVertexMesh::PostRender() {}
//
// ELayerType XVertexMesh::GetLayerType()
// {
// 	// (Base) returns 3D FBX Model 
// 	return ELayerType::GameObject;
// }
//
// void XVertexMesh::SetMesh(JWTextView InFbxFile)
// {
// 	// TODO: Load Fbx Model (InFbxFile)
//
// }
//
// void XVertexMesh::Release()
// {
// 	mIndexBuffer  = nullptr;
// 	mVertexBuffer = nullptr;
// 	mWorldBuffer  = nullptr;
// }
//
// void XVertexMesh::CreateVertexArray()
// {
// 	mVertexArray.resize(mVertexCount);
//
// 	for (int32_t i = 0; i < mVertexCount; ++i)
// 	{
// 		mVertexArray[i].Position = mFbxModelInfo[i].Position;
// 		mVertexArray[i].Normal   = mFbxModelInfo[i].Normal;
// 		mVertexArray[i].Color    = mFbxModelInfo[i].Color;
// 		mVertexArray[i].UV       = mFbxModelInfo[i].UV;
//
// 		mIndiceArray.push_back(i);
// 	}
// }
//
// void XVertexMesh::CreateVertexBuffer()
// {
// 	D3D11_BUFFER_DESC bufferDesc;
// 	{
// 		bufferDesc.ByteWidth      = std::size(mVertexArray) * sizeof(FVertexInfo_Simple); // 버퍼크기
// 		bufferDesc.Usage          = D3D11_USAGE_DEFAULT;	// 버퍼의 읽기/쓰기 방법 지정
// 		bufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER; // 파이프라인에 바인딩될 방법
// 		bufferDesc.CPUAccessFlags = 0; // 생성될 버퍼에 CPU가 접근하는 유형 (DX 성능에 매우 중요)
// 		bufferDesc.MiscFlags      = 0; // 추가적인 옵션 플래그
// 	}
//
// 	D3D11_SUBRESOURCE_DATA vertexData;
// 	{
// 		vertexData.pSysMem = mVertexArray.data(); // 초기화 데이터 포인터 (정점 배열의 주소를 넘겨준다)
// 		// InitData.SysMemPitch (텍스처 리소스의 한줄의 크기)
// 		// InitData.SysMemSlicePitch (3차원 텍스처의 깊이 간격)
// 	}
//
// 	CheckResult(
// 				G_Context.GetDevice()->CreateBuffer(
// 													&bufferDesc,
// 													&vertexData,
// 													mVertexBuffer.GetAddressOf()
// 												   ));
// }
//
// void XVertexMesh::CreateIndexBuffer()
// {
// 	const UINT numIndex = mIndiceArray.size();
//
// 	D3D11_BUFFER_DESC bufferDesc;
// 	{
// 		bufferDesc.ByteWidth      = numIndex * sizeof(WORD);
// 		bufferDesc.Usage          = D3D11_USAGE_DEFAULT;
// 		bufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
// 		bufferDesc.CPUAccessFlags = 0;
// 		bufferDesc.MiscFlags      = 0;
// 	}
//
// 	D3D11_SUBRESOURCE_DATA indexData;
// 	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
// 	indexData.pSysMem = mIndiceArray.data();
//
// 	CheckResult(
// 				G_Context.GetDevice()->CreateBuffer(
// 													&bufferDesc,
// 													&indexData,
// 													mIndexBuffer.GetAddressOf()
// 												   ));
// }
//
// void XVertexMesh::LoadShader(JWTextView InShaderFileName)
// {
// 	//TODO: Load Shader
// 	mShader = Manager_Shader.CreateOrLoad<XShader>(InShaderFileName.data());
// }

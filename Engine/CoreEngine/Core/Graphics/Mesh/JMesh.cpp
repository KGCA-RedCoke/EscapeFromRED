#include "common_pch.h"
#include "JMesh.h"

#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Interface/MManagerInterface.h"

void JMesh::Serialize(std::ofstream& FileStream)
{
	// Mesh Name
	size_t nameSize = mName.size();
	FileStream.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
	FileStream.write(reinterpret_cast<char*>(mName.data()), nameSize);

	// Mesh Index
	FileStream.write(reinterpret_cast<char*>(&mIndex), sizeof(mIndex));

	// Mesh Type
	FileStream.write(reinterpret_cast<char*>(&mClassType), sizeof(mClassType));

	// Material Reference ID
	FileStream.write(reinterpret_cast<char*>(&mMaterialRefNum), sizeof(mMaterialRefNum));

	// Face Count
	FileStream.write(reinterpret_cast<char*>(&mFaceNum), sizeof(mFaceNum));

	// Parent Mesh
	bool bParentMeshValid = mParentMesh != nullptr;
	FileStream.write(reinterpret_cast<char*>(&bParentMeshValid), sizeof(bParentMeshValid));
	if (bParentMeshValid)
	{
		mParentMesh->Serialize(FileStream);
	}

	// SubMesh
	size_t subMeshSize = mSubMesh.size();
	FileStream.write(reinterpret_cast<char*>(&subMeshSize), sizeof(subMeshSize));
	for (int32_t i = 0; i < mSubMesh.size(); ++i)
	{
		mSubMesh[i]->Serialize(FileStream);
	}

	// ChildMesh
	size_t childMeshSize = mChildMesh.size();
	FileStream.write(reinterpret_cast<char*>(&childMeshSize), sizeof(childMeshSize));
	for (int32_t i = 0; i < mChildMesh.size(); ++i)
	{
		mChildMesh[i]->Serialize(FileStream);
	}

	bool bVertexDataValid = mVertexData != nullptr;
	FileStream.write(reinterpret_cast<char*>(&bVertexDataValid), sizeof(bVertexDataValid));
	if (bVertexDataValid)
	{
		mVertexData->Serialize(FileStream);
	}

	bool bMaterialValid = mMaterial != nullptr;
	FileStream.write(reinterpret_cast<char*>(&bMaterialValid), sizeof(bMaterialValid));
	if (bMaterialValid)
	{
		mMaterial->Serialize(FileStream);
	}
}

void JMesh::DeSerialize(std::ifstream& InFileStream)
{
	// Mesh Name
	size_t nameSize;
	InFileStream.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
	mName.resize(nameSize);
	InFileStream.read(reinterpret_cast<char*>(mName.data()), nameSize);

	// Mesh Index
	InFileStream.read(reinterpret_cast<char*>(&mIndex), sizeof(mIndex));

	// Mesh Type
	InFileStream.read(reinterpret_cast<char*>(&mClassType), sizeof(mClassType));

	// Material Reference ID
	InFileStream.read(reinterpret_cast<char*>(&mMaterialRefNum), sizeof(mMaterialRefNum));

	// Face Count
	InFileStream.read(reinterpret_cast<char*>(&mFaceNum), sizeof(mFaceNum));

	// Parent Mesh
	bool bParentMeshValid;
	InFileStream.read(reinterpret_cast<char*>(&bParentMeshValid), sizeof(bParentMeshValid));
	mParentMesh = bParentMeshValid ? std::make_shared<JMesh>() : nullptr;
	if (bParentMeshValid)
	{
		mParentMesh->DeSerialize(InFileStream);
	}

	// SubMesh
	size_t subMeshSize;
	InFileStream.read(reinterpret_cast<char*>(&subMeshSize), sizeof(subMeshSize));
	mSubMesh.resize(subMeshSize);
	for (int32_t i = 0; i < subMeshSize; ++i)
	{
		mSubMesh[i] = std::make_shared<JMesh>();
		mSubMesh[i]->DeSerialize(InFileStream);
	}

	// ChildMesh
	size_t childMeshSize;
	InFileStream.read(reinterpret_cast<char*>(&childMeshSize), sizeof(childMeshSize));
	mChildMesh.resize(childMeshSize);
	for (int32_t i = 0; i < childMeshSize; ++i)
	{
		mChildMesh[i] = std::make_shared<JMesh>();
		mChildMesh[i]->DeSerialize(InFileStream);
	}

	// VertexData
	bool bVertexDataValid;
	InFileStream.read(reinterpret_cast<char*>(&bVertexDataValid), sizeof(bVertexDataValid));
	mVertexData = bVertexDataValid ? std::make_shared<JData<Vertex::FVertexInfo_Base>>() : nullptr;
	mVertexData->DeSerialize(InFileStream);

	// Material
	bool bMaterialValid;
	InFileStream.read(reinterpret_cast<char*>(&bMaterialValid), sizeof(bMaterialValid));
	mMaterial = bMaterialValid ? std::make_shared<JMaterial>() : nullptr;
	if (bMaterialValid)
	{
		mMaterial->DeSerialize(InFileStream);
	}
}

bool JMesh::ApplyMaterial() const
{
	auto material = GetMaterial();
	if (!material)
	{
		return false;
	}

	if (auto diffuse = material->GetMaterialParam("DiffuseColor"))
	{
		diffuse->TextureValue = IManager.TextureManager.CreateOrLoad(diffuse->StringValue);
	}
	if (auto normal = material->GetMaterialParam("NormalMap"))
	{
		normal->TextureValue = IManager.TextureManager.CreateOrLoad(normal->StringValue);
	}
	if (auto specular = material->GetMaterialParam("SpecularMap"))
	{
		specular->TextureValue = IManager.TextureManager.CreateOrLoad(specular->StringValue);
	}
	if (auto roughness = material->GetMaterialParam("RoughnessMap"))
	{
		roughness->TextureValue = IManager.TextureManager.CreateOrLoad(roughness->StringValue);
	}
	if (auto metallic = material->GetMaterialParam("MetallicMap"))
	{
		metallic->TextureValue = IManager.TextureManager.CreateOrLoad(metallic->StringValue);
	}
	if (auto ao = material->GetMaterialParam("AOMap"))
	{
		ao->TextureValue = IManager.TextureManager.CreateOrLoad(ao->StringValue);
	}
	if (auto emissive = mMaterial->GetMaterialParam("EmissiveMap"))
	{
		emissive->TextureValue = IManager.TextureManager.CreateOrLoad(emissive->StringValue);
	}
	if (auto opacity = mMaterial->GetMaterialParam("OpacityMap"))
	{
		opacity->TextureValue = IManager.TextureManager.CreateOrLoad(opacity->StringValue);
	}

	return true;
}

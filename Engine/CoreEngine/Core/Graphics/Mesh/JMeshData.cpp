#include "JMeshData.h"
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Interface/MManagerInterface.h"

void JMeshData::Serialize(std::ofstream& FileStream)
{
	// Mesh Name
	Utils::Serialization::Serialize_Text(mName, FileStream);

	// Mesh Index
	Utils::Serialization::Serialize_Primitive(&mIndex, sizeof(mIndex), FileStream);

	// Mesh Type
	Utils::Serialization::Serialize_Primitive(&mClassType, sizeof(mClassType), FileStream);

	// Material Reference ID
	Utils::Serialization::Serialize_Primitive(&mMaterialRefNum, sizeof(mMaterialRefNum), FileStream);

	// Face Count
	Utils::Serialization::Serialize_Primitive(&mFaceCount, sizeof(mFaceCount), FileStream);

	// SubMesh
	size_t subMeshSize = mSubMesh.size();
	Utils::Serialization::Serialize_Primitive(&subMeshSize, sizeof(subMeshSize), FileStream);
	for (int32_t i = 0; i < mSubMesh.size(); ++i)
	{
		mSubMesh[i]->Serialize(FileStream);
	}

	// ChildMesh
	size_t childMeshSize = mChildMesh.size();
	Utils::Serialization::Serialize_Primitive(&childMeshSize, sizeof(childMeshSize), FileStream);
	for (int32_t i = 0; i < mChildMesh.size(); ++i)
	{
		mChildMesh[i]->Serialize(FileStream);
	}

	bool bVertexDataValid = mVertexData != nullptr;
	Utils::Serialization::Serialize_Primitive(&bVertexDataValid, sizeof(bVertexDataValid), FileStream);
	if (bVertexDataValid)
	{
		mVertexData->Serialize(FileStream);
	}

	bool bMaterialValid = mMaterial != nullptr;
	Utils::Serialization::Serialize_Primitive(&bMaterialValid, sizeof(bMaterialValid), FileStream);
	if (bMaterialValid)
	{
		JText matName = WString2String(mMaterial->GetMaterialName());
		Utils::Serialization::Serialize_Text(matName, FileStream);
		// mMaterial->Serialize(FileStream);
	}

	// Initial Transform
	Utils::Serialization::Serialize_Primitive(&mInitialModelTransform, sizeof(mInitialModelTransform), FileStream);
}

void JMeshData::DeSerialize(std::ifstream& InFileStream)
{
	// Mesh Name
	Utils::Serialization::DeSerialize_Text(mName, InFileStream);

	// Mesh Index
	InFileStream.read(reinterpret_cast<char*>(&mIndex), sizeof(mIndex));

	// Mesh Type
	InFileStream.read(reinterpret_cast<char*>(&mClassType), sizeof(mClassType));

	// Material Reference ID
	InFileStream.read(reinterpret_cast<char*>(&mMaterialRefNum), sizeof(mMaterialRefNum));

	// Face Count
	InFileStream.read(reinterpret_cast<char*>(&mFaceCount), sizeof(mFaceCount));

	// SubMesh
	size_t subMeshSize;
	InFileStream.read(reinterpret_cast<char*>(&subMeshSize), sizeof(subMeshSize));
	mSubMesh.reserve(subMeshSize);
	for (int32_t i = 0; i < subMeshSize; ++i)
	{
		mSubMesh.push_back(std::make_shared<JMeshData>());
		mSubMesh[i]->DeSerialize(InFileStream);
	}

	// ChildMesh
	size_t childMeshSize;
	InFileStream.read(reinterpret_cast<char*>(&childMeshSize), sizeof(childMeshSize));
	mChildMesh.reserve(childMeshSize);
	for (int32_t i = 0; i < childMeshSize; ++i)
	{
		mChildMesh.push_back(MakePtr<JMeshData>());
		mChildMesh[i]->DeSerialize(InFileStream);
		mChildMesh[i]->mParentMesh = shared_from_this();
	}

	// VertexData
	bool bVertexDataValid;
	InFileStream.read(reinterpret_cast<char*>(&bVertexDataValid), sizeof(bVertexDataValid));
	mVertexData = bVertexDataValid ? std::make_shared<JVertexData<Vertex::FVertexInfo_Base>>() : nullptr;
	mVertexData->DeSerialize(InFileStream);

	// Material
	bool bMaterialValid;
	InFileStream.read(reinterpret_cast<char*>(&bMaterialValid), sizeof(bMaterialValid));
	if (bMaterialValid)
	{
		JText matName;
		Utils::Serialization::DeSerialize_Text(matName, InFileStream);
		mMaterial = IManager.MaterialManager.CreateOrLoad(matName);
		// mMaterial->DeSerialize(InFileStream);
	}

	// Initial Transform
	InFileStream.read(reinterpret_cast<char*>(&mInitialModelTransform), sizeof(mInitialModelTransform));
}


bool JMeshData::ApplyMaterial() const
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

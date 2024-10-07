#include "JMeshData.h"
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Interface/MManagerInterface.h"

uint32_t JMeshData::GetType() const
{
	return StringHash("JMeshData");
}

bool JMeshData::Serialize_Implement(std::ofstream& FileStream)
{
	if (!Utils::Serialization::SerializeMetaData(FileStream, this))
	{
		return false;
	}

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
		mSubMesh[i]->Serialize_Implement(FileStream);
	}

	// ChildMesh
	size_t childMeshSize = mChildMesh.size();
	Utils::Serialization::Serialize_Primitive(&childMeshSize, sizeof(childMeshSize), FileStream);
	for (int32_t i = 0; i < mChildMesh.size(); ++i)
	{
		mChildMesh[i]->Serialize_Implement(FileStream);
	}

	bool bVertexDataValid = mVertexData != nullptr;
	Utils::Serialization::Serialize_Primitive(&bVertexDataValid, sizeof(bVertexDataValid), FileStream);
	if (bVertexDataValid)
	{
		mVertexData->Serialize_Implement(FileStream);
	}

	bool bMaterialValid = mMaterial != nullptr;
	Utils::Serialization::Serialize_Primitive(&bMaterialValid, sizeof(bMaterialValid), FileStream);
	if (bMaterialValid)
	{
		auto matName = mMaterial->GetMaterialName();
		Utils::Serialization::Serialize_Text(matName, FileStream);
		// mMaterial->Serialize_Implement(FileStream);
	}

	// Initial Transform
	Utils::Serialization::Serialize_Primitive(&mInitialModelTransform, sizeof(mInitialModelTransform), FileStream);

	return true;
}

bool JMeshData::DeSerialize_Implement(std::ifstream& InFileStream)
{
	JAssetMetaData metaData;
	if (!Utils::Serialization::DeserializeMetaData(InFileStream, metaData, GetType()))
	{
		return false;
	}

	// Mesh Name
	Utils::Serialization::DeSerialize_Text(mName, InFileStream);

	// Mesh Index
	Utils::Serialization::DeSerialize_Primitive(&mIndex, sizeof(mIndex), InFileStream);

	// Mesh Type
	Utils::Serialization::DeSerialize_Primitive(&mClassType, sizeof(mClassType), InFileStream);

	// Material Reference ID
	Utils::Serialization::DeSerialize_Primitive(&mMaterialRefNum, sizeof(mMaterialRefNum), InFileStream);

	// Face Count
	Utils::Serialization::DeSerialize_Primitive(&mFaceCount, sizeof(mFaceCount), InFileStream);

	// SubMesh
	size_t subMeshSize;
	Utils::Serialization::DeSerialize_Primitive(&subMeshSize, sizeof(subMeshSize), InFileStream);
	mSubMesh.reserve(subMeshSize);
	for (int32_t i = 0; i < subMeshSize; ++i)
	{
		mSubMesh.push_back(std::make_shared<JMeshData>());
		mSubMesh[i]->DeSerialize_Implement(InFileStream);
	}

	// ChildMesh
	size_t childMeshSize;
	Utils::Serialization::DeSerialize_Primitive(&childMeshSize, sizeof(childMeshSize), InFileStream);
	mChildMesh.reserve(childMeshSize);
	for (int32_t i = 0; i < childMeshSize; ++i)
	{
		mChildMesh.push_back(MakePtr<JMeshData>());
		mChildMesh[i]->DeSerialize_Implement(InFileStream);
		mChildMesh[i]->mParentMesh = shared_from_this();
	}

	// VertexData
	bool bVertexDataValid;
	Utils::Serialization::DeSerialize_Primitive(&bVertexDataValid, sizeof(bVertexDataValid), InFileStream);
	mVertexData = bVertexDataValid ? std::make_shared<JVertexData<Vertex::FVertexInfo_Base>>() : nullptr;
	mVertexData->DeSerialize_Implement(InFileStream);

	// Material
	bool bMaterialValid;
	Utils::Serialization::DeSerialize_Primitive(&bMaterialValid, sizeof(bMaterialValid), InFileStream);
	if (bMaterialValid)
	{
		JText matName;
		Utils::Serialization::DeSerialize_Text(matName, InFileStream);
		mMaterial = IManager.MaterialManager->CreateOrLoad(matName);
		// mMaterial->DeSerialize_Implement(InFileStream);
	}

	// Initial Transform
	Utils::Serialization::DeSerialize_Primitive(&mInitialModelTransform, sizeof(mInitialModelTransform), InFileStream);

	return true;
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
		diffuse->TextureValue = IManager.TextureManager->CreateOrLoad(diffuse->StringValue);
	}
	if (auto normal = material->GetMaterialParam("NormalMap"))
	{
		normal->TextureValue = IManager.TextureManager->CreateOrLoad(normal->StringValue);
	}
	if (auto specular = material->GetMaterialParam("SpecularMap"))
	{
		specular->TextureValue = IManager.TextureManager->CreateOrLoad(specular->StringValue);
	}
	if (auto roughness = material->GetMaterialParam("RoughnessMap"))
	{
		roughness->TextureValue = IManager.TextureManager->CreateOrLoad(roughness->StringValue);
	}
	if (auto metallic = material->GetMaterialParam("MetallicMap"))
	{
		metallic->TextureValue = IManager.TextureManager->CreateOrLoad(metallic->StringValue);
	}
	if (auto ao = material->GetMaterialParam("AOMap"))
	{
		ao->TextureValue = IManager.TextureManager->CreateOrLoad(ao->StringValue);
	}
	if (auto emissive = mMaterial->GetMaterialParam("EmissiveMap"))
	{
		emissive->TextureValue = IManager.TextureManager->CreateOrLoad(emissive->StringValue);
	}
	if (auto opacity = mMaterial->GetMaterialParam("OpacityMap"))
	{
		opacity->TextureValue = IManager.TextureManager->CreateOrLoad(opacity->StringValue);
	}

	return true;
}

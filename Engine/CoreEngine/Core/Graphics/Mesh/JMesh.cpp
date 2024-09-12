#include "common_pch.h"
#include "JMesh.h"

#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Interface/MManagerInterface.h"

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

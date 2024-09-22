#include "MMaterialManager.h"

#include "Core/Interface/MManagerInterface.h"

void MMaterialManager::Initialize()
{
	DefaultMaterial = CreateOrLoad("DefaultMaterial");
	// DefaultMaterial->SetShader(IManager.ShaderManager.GetBasicShader());
	//
	// FMaterialParams albedoTex;
	// albedoTex.ParamType   = EMaterialParamType::Texture2D;
	// albedoTex.Name        = "AlbedoTex";
	// albedoTex.StringValue = "rsc/Engine/Material/Default/TemplateGrid_albedo.png";
	// albedoTex.Flags       = EMaterialFlag::Diffuse;
	// DefaultMaterial->AddMaterialParam(albedoTex);
	//
	// FMaterialParams normalTex;
	// normalTex.ParamType   = EMaterialParamType::Texture2D;
	// normalTex.Name        = "NormalTex";
	// normalTex.StringValue = "rsc/Engine/Material/Default/TemplateGrid_normal.png";
	// normalTex.Flags       = EMaterialFlag::Normal;
	// DefaultMaterial->AddMaterialParam(normalTex);
	//
	// FMaterialParams ambientOcclusionTex;
	// ambientOcclusionTex.ParamType   = EMaterialParamType::Texture2D;
	// ambientOcclusionTex.Name        = "AmbientOcclusionTex";
	// ambientOcclusionTex.StringValue = "rsc/Engine/Material/Default/TemplateGrid_orm.png";
	// ambientOcclusionTex.Flags       = EMaterialFlag::Ambient;
	// DefaultMaterial->AddMaterialParam(ambientOcclusionTex);
	//
	// FMaterialParams roughnessTex;
	// roughnessTex.ParamType   = EMaterialParamType::Texture2D;
	// roughnessTex.Name        = "RoughnessTex";
	// roughnessTex.StringValue = "rsc/Engine/Material/Default/TemplateGrid_orm.png";
	// roughnessTex.Flags       = EMaterialFlag::Alpha;
	// DefaultMaterial->AddMaterialParam(roughnessTex);
	//
	// FMaterialParams metallicTex;
	// metallicTex.ParamType   = EMaterialParamType::Texture2D;
	// metallicTex.Name        = "MetallicTex";
	// metallicTex.StringValue = "rsc/Engine/Material/Default/TemplateGrid_orm.png";
	// metallicTex.Flags       = EMaterialFlag::None;
	// DefaultMaterial->AddMaterialParam(metallicTex);


	Utils::Serialization::DeSerialize("rsc/Engine/Material/GridMaterial.jasset", DefaultMaterial);
}

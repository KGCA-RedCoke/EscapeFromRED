#pragma once
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Shader/JShader.h"

class JMaterialInstance : public IManagedInterface, public ISerializable
{
public:
	JMaterialInstance(JTextView InName);
	JMaterialInstance(JWTextView InName);
	~JMaterialInstance() override = default;

public:
	Ptr<IManagedInterface> Clone() const override;

public:
	uint32_t GetHash() const override;
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void BindMaterial(ID3D11DeviceContext* InDeviceContext);
	void UpdateWorldMatrix(ID3D11DeviceContext* InDeviceContext, const FMatrix& InWorldMatrix) const;
	void UpdateCamera(ID3D11DeviceContext* InDeviceContext, const Ptr<class JCamera>& InCameraObj) const;
	void UpdateLightColor(ID3D11DeviceContext* InDeviceContext, const FVector4& InLightColor) const;
	void UpdateLightLoc(ID3D11DeviceContext* InDeviceContext, const FVector4& InLightLoc) const;

public:
	FMaterialParam* GetInstanceParam(const JText& InParamName);
	void            AddInstanceParam(const FMaterialParam& InParamValue);
	void            EditInstanceParam(const JText& InParamName, const FMaterialParam& InParamValue);

public:
	FORCEINLINE JText   GetMaterialName() const { return ParseFile(mFileName); }
	FORCEINLINE JText   GetMaterialPath() const { return mFileName; }
	FORCEINLINE int32_t GetParamCount() const { return mInstanceParams.size(); }

public:
	void SetParentMaterial(const Ptr<JMaterial>& InParentMaterial);

private:
	void GetInstanceParams();

protected:
	JText                  mFileName;		// 파일 이름
	Ptr<JMaterial>         mParentMaterial;	// 부모 머티리얼 레퍼런스
	WPtr<JShader>          mShader;			// 셰이더 레퍼런스
	JArray<FMaterialParam> mInstanceParams;	// 인스턴스 파라미터 (편집 가능)

	friend class GUI_Editor_Material;
};

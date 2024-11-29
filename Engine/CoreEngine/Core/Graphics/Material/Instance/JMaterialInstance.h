#pragma once
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Shader/JShader.h"
#include "Core/Manager/IManagedInterface.h"

DECLARE_DYNAMIC_DELEGATE(FOnMaterialInstanceParamChanged);

class JMaterialInstance : public IManagedInterface, public ISerializable
{
public:
	FOnMaterialInstanceParamChanged OnMaterialInstanceParamChanged;

public:
	JMaterialInstance(JTextView InName);
	JMaterialInstance(JWTextView InName);
	JMaterialInstance(const JMaterialInstance& Other);
	~JMaterialInstance() override = default;

public:
	UPtr<IManagedInterface> Clone() const override;
	void                    SetAsDefaultMaterial();

public:
	uint32_t GetHash() const override;
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void BindMaterial(ID3D11DeviceContext* InDeviceContext) const;
	void UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName, const void* InData,
							const uint32_t       InOffset = 0) const;
	void UpdateConstantData(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName, const JText& InDataName,
							const void*          InData) const;

public:
	FMaterialParam* GetInstanceParam(const JText& InParamName);
	void            AddInstanceParam(const FMaterialParam& InParamValue);
	void            EditInstanceParam(const JText& InParamName, const FMaterialParam& InParamValue);

public:
	FORCEINLINE JText   GetMaterialName() const { return ParseFile(mFileName); }
	FORCEINLINE JText   GetMaterialPath() const { return mFileName; }
	FORCEINLINE int32_t GetParamCount() const { return mInstanceParams.size(); }

public:
	void SetParentMaterial(JMaterial* InParentMaterial);

private:
	void GetInstanceParams();

protected:
	JText                  mFileName;		// 파일 이름
	JMaterial*             mParentMaterial;	// 부모 머티리얼 레퍼런스
	JShader*               mShader;			// 셰이더 레퍼런스
	JArray<FMaterialParam> mInstanceParams;	// 인스턴스 파라미터 (편집 가능)

	friend class GUI_Editor_Material;
};

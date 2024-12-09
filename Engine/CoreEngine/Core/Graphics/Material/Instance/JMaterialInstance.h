#pragma once
#include "Core/Graphics/Material/JMaterial.h"
#include "Core/Graphics/Shader/JShader.h"
#include "Core/Manager/IManagedInterface.h"

DECLARE_DYNAMIC_DELEGATE(FOnMaterialInstanceParamChanged);

class JMaterialInstance : public IManagedInterface, public JAsset
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
	void        BindMaterial(ID3D11DeviceContext* InDeviceContext) const;
	const void* GetMaterialData() const { return mMaterialRawData.data(); }
	const float GetMaterialSize() const { return mMaterialRawData.size() * sizeof(float); }

public:
	void* GetInstanceParam(const JText& InParamName, bool bTextureParam = false);
	void* GetInstanceParam(uint32_t InParamKey, bool bTextureParam = false);
	void  EditInstanceParam(const JText& InParamName, const FMaterialParam& InParamValue);

public:
	FORCEINLINE JText   GetMaterialName() const { return ParseFile(mFileName); }
	FORCEINLINE JText   GetMaterialPath() const { return mFileName; }
	FORCEINLINE int32_t GetParamCount() const { return mMaterialRawData.size(); }

public:
	void SetParentMaterial(JMaterial* InParentMaterial);

private:
	void GetInstanceParams();

protected:
	JText      mFileName;		// 파일 이름
	JMaterial* mParentMaterial;	// 부모 머티리얼 레퍼런스
	JShader*   mShader;			// 셰이더 레퍼런스

	JArray<float>              mMaterialRawData;
	JHash<uint32_t, uint32_t>  mMaterialParamToRawDataIndex;
	JHash<uint32_t, JTexture*> mTextureMap;

	friend class GUI_Editor_Material;
};

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
	void BindMaterialBuffer(ID3D11DeviceContext* InDeviceContext, const uint32_t InSlot = CBuffer::SLOT_MATERIAL);
	void UpdateWorldMatrix(ID3D11DeviceContext* InDeviceContext, const FMatrix& InWorldMatrix) const;
	void UpdateCamera(ID3D11DeviceContext* InDeviceContext, const Ptr<class JCamera>& InCameraObj) const;
	void UpdateLightColor(ID3D11DeviceContext* InDeviceContext, const FVector4& InLightColor) const;
	void UpdateLightLoc(ID3D11DeviceContext* InDeviceContext, const FVector4& InLightLoc) const;
	// void UpdateCamera(ID3D11DeviceContext* InDeviceContext, const Ptr<JCamera>& InCameraObj);
	// template <typename Data>
	// void UpdateMaterialBuffer(ID3D11DeviceContext* InDeviceContext, const JText& InBufferName, const Data& InData)
	// {
	// 	mParentMaterial->mShader->UpdateConstantData(InDeviceContext, InBufferName, InData);
	//
	// 	// Utils::DX::UpdateDynamicBuffer(InDeviceContext,
	// 	// 							   mMaterialBuffer.Get(),
	// 	// 							   &InData,	// BUG: 아 슈발 이걸 어떻게 구분하지
	// 	// 							   mParentMaterial->mMaterialBufferSize);
	// }

public:
	JText GetMaterialName() const { return ParseFile(mFileName); }
	JText GetMaterialPath() const { return mFileName; }

private:
	void GenerateMaterialBuffer();
	void GetInstanceParams();

public:
	FORCEINLINE void SetParentMaterial(const Ptr<JMaterial>& InParentMaterial);

protected:
	JText                  mFileName;		// 파일 이름
	Ptr<JMaterial>         mParentMaterial;	// 부모 머티리얼 레퍼런스
	JArray<FMaterialParam> mInstanceParams;	// 인스턴스 파라미터 (편집 가능)
	ComPtr<ID3D11Buffer>   mMaterialBuffer;	// 머티리얼 상수 버퍼


	friend class GUI_Editor_Material;
};

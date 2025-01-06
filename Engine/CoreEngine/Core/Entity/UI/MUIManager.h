#pragma once
#include "Core/Entity/JObject.h"
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Interface/IGUIEditable.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Manager/Manager_Base.h"
#include "Core/Utils/Math/Vector2.h"

class JWidgetComponent;
class JFont;
class JShader_UI;
class JShader;

struct FVertexData_UI
{
	FVector2 Position;
	FVector2 TexCoord;
};

struct FInstanceData_UI
{
	FVector2 Position;
	FVector2 Size         = FVector2::UnitVector;
	FVector4 Color        = FVector4(0.2, 0.2, 0.2, 1.f);
	int32_t  TextureIndex = 0;
	int32_t  OpacityIndex = 0;
};

struct FInstanceData_UI_Picking
{
	FVector2 Position;
	FVector2 Size;
	FVector4 Color;
};

DECLARE_DYNAMIC_DELEGATE(FOnUIHovered);

DECLARE_DYNAMIC_DELEGATE(FOnUIUnhovered);

DECLARE_DYNAMIC_DELEGATE(FOnUIPressed);

DECLARE_DYNAMIC_DELEGATE(FOnUIReleased);

DECLARE_DYNAMIC_DELEGATE(FAnimationEvent, float DeltaTime);

class JUIComponent : public JObject, public IRenderable, public IGUIEditable
{
public:
	FOnUIHovered    OnHovered;
	FOnUIUnhovered  OnUnhovered;
	FOnUIPressed    OnPressed;
	FOnUIReleased   OnReleased;
	FAnimationEvent OnAnimationEvent;

public:
	JUIComponent();
	JUIComponent(const JText& InName);
	JUIComponent(const JText& InName, const FInstanceData_UI& InInstanceData);
	~JUIComponent() override = default;

public:
	bool Serialize_Implement(std::ofstream& FileStream) override;
	bool DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Initialize() override;
	void Tick(float DeltaTime) override;

public:
	void PreRender() override;
	void AddInstance(float InCameraDistance) override;
	void Draw() override;
	void DrawID(uint32_t ID) override;
	void PostRender() override;

public:
	void ShowEditor() override;

public:
	void SetPosition(const FVector2& InPosition) { mInstanceData.Position = InPosition; }
	void SetSize(const FVector2& InSize) { mInstanceData.Size = InSize; }
	void SetColor(const FVector4& InColor) { mInstanceData.Color = InColor; }
	void SetTexture(class JTexture* InTexture) { mTexture = InTexture; }
	void AddText(JWTextView          InText, const FVector2& InPosition, const FVector2& InRectSize, const float InSize,
				 const FLinearColor& InColor);

	void SetWidgetComponent(JWidgetComponent* InWidgetComponent) { mWidgetComponent = InWidgetComponent; }

public:
	FInstanceData_UI& GetInstanceData() { return mInstanceData; }

protected:
	JWidgetComponent* mWidgetComponent;
	UPtr<JFont>       mText;
	JText             mRawString;
	FInstanceData_UI  mInstanceData;
	int32_t           mLayer = 0;
	class JTexture*   mTexture;
	class JTexture*   mOpacityTexture;
};

REGISTER_CLASS_TYPE(JUIComponent)

class JWidgetComponent : public JObject
{
public:
	JWidgetComponent();
	JWidgetComponent(const JText& InName);
	~JWidgetComponent() override = default;

public:
	uint32_t GetType() const override;
	bool     Serialize_Implement(std::ofstream& FileStream) override;
	bool     DeSerialize_Implement(std::ifstream& InFileStream) override;

public:
	void Tick(float DeltaTime) override;

	void AddInstance();
	void Draw();

public:
	void               SetMousePos(const FVector2& InMousePos) { mMousePos = InMousePos; }
	void               SetRenderTarget(ID2D1RenderTarget* InRenderTarget) { mRenderTarget = InRenderTarget; }
	ID2D1RenderTarget* GetRenderTarget() const { return mRenderTarget; }

public:
	JUIComponent* GetClickedComponent(const FVector2& InMousePos, const FVector2& InScreenPos) const;
	uint32_t      GetComponentSize() const;

public:
	void AddUIComponent(JTextView InName);

public:
	ID2D1RenderTarget*         mRenderTarget;
	JArray<UPtr<JUIComponent>> mUIComponents;
	FVector2                   mMousePos;
};

class MUIManager : public Manager_Base<JWidgetComponent, MUIManager>
{
public:
	void PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
						void*        Entity) override;

public:
	void PushCommand(FInstanceData_UI&         InInstanceData,
					 ID3D11ShaderResourceView* InTex,
					 ID3D11ShaderResourceView* InOpacityTex,
					 _Out_ int32_t&            OutTexIndex,
					 _Out_ int32_t&            OutOpacityTexIndex,
					 uint32_t                  ID);
	void FlushCommandList(ID3D11DeviceContext* InContext);

	void SetHUD(JWidgetComponent* InHUD);

public:
	JWidgetComponent* LoadingScreen;

private:
	JShader_UI* mShader;
	JShader*    mPickingShader;

	JArray<FVertexData_UI> mVertexData;
	JArray<uint32_t>       mIndexData;

	JArray<FInstanceData_UI>          mInstanceData;
	JArray<ID3D11ShaderResourceView*> mTextureSRVs;
	JArray<ID3D11ShaderResourceView*> mOpacityTextureSRVs;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MUIManager>;

	MUIManager();
	~MUIManager();

public:
	MUIManager(const MUIManager&)            = delete;
	MUIManager& operator=(const MUIManager&) = delete;

	#pragma endregion
};

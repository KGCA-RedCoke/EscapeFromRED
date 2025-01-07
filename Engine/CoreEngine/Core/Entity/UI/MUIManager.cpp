#include "MUIManager.h"

#include "Core/Entity/Camera/MCameraManager.h"
#include "Core/Graphics/XD3DDevice.h"
#include "Core/Graphics/Font/JFont.h"
#include "Core/Graphics/Shader/JShader_UI.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Texture/JTexture.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Interface/JWorld.h"
#include "Core/Utils/Graphics/DXUtils.h"
#include "imgui/imgui_stdlib.h"


static constexpr uint32_t   g_VertexStride   = sizeof(FVertexData_UI);
static constexpr uint32_t   g_InstanceStride = sizeof(FInstanceData_UI);
static ComPtr<ID3D11Buffer> g_VertexBuffer;
static ComPtr<ID3D11Buffer> g_IndexBuffer;
static ComPtr<ID3D11Buffer> g_InstanceBuffer;


JUIComponent::JUIComponent()
	: mInstanceData(),
	  mTexture(nullptr),
	  mOpacityTexture(nullptr) {}

JUIComponent::JUIComponent(const JText& InName)
	: JObject(InName),
	  mInstanceData(),
	  mTexture(nullptr),
	  mOpacityTexture(nullptr)
{}

JUIComponent::JUIComponent(const JText& InName, const FInstanceData_UI& InInstanceData)
	: JObject(InName),
	  mInstanceData(InInstanceData),
	  mTexture(nullptr),
	  mOpacityTexture(nullptr)
{}

bool JUIComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JObject::Serialize_Implement(FileStream))
	{
		return false;
	}

	// Serialize Instance Data
	Utils::Serialization::Serialize_Primitive(&mInstanceData, sizeof(mInstanceData), FileStream);

	// Layer
	Utils::Serialization::Serialize_Primitive(&mLayer, sizeof(mLayer), FileStream);

	// Texture
	if (mTexture)
	{
		JText path = WString2String(mTexture->GetTextureName());
		Utils::Serialization::Serialize_Text(path, FileStream);
	}
	else
	{
		JText empty;
		Utils::Serialization::Serialize_Text(empty, FileStream);
	}

	// Opacity Texture
	if (mOpacityTexture)
	{
		JText path = WString2String(mOpacityTexture->GetTextureName());
		Utils::Serialization::Serialize_Text(path, FileStream);
	}
	else
	{
		JText empty;
		Utils::Serialization::Serialize_Text(empty, FileStream);
	}

	return true;
}

bool JUIComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JObject::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	// DeSerialize Instance Data
	Utils::Serialization::DeSerialize_Primitive(&mInstanceData, sizeof(mInstanceData), InFileStream);

	// Layer
	Utils::Serialization::DeSerialize_Primitive(&mLayer, sizeof(mLayer), InFileStream);

	// Texture
	JText path;
	Utils::Serialization::DeSerialize_Text(path, InFileStream);

	// Opacity Texture
	JText opacityPath;
	Utils::Serialization::DeSerialize_Text(opacityPath, InFileStream);

	if (path.empty())
	{
		mTexture = nullptr;
	}
	else
	{
		mTexture = MTextureManager::Get().Load(String2WString(path));
	}

	if (opacityPath.empty())
	{
		mOpacityTexture = nullptr;
	}
	else
	{
		mOpacityTexture = MTextureManager::Get().Load(String2WString(opacityPath));
	}

	return true;
}

void JUIComponent::PreRender()
{}

void JUIComponent::AddInstance(float InCameraDistance)
{
	auto* texSrv        = mTexture ? mTexture->GetSRV() : MTextureManager::Get().WhiteTexture->GetSRV();
	auto* opacityTexSrv = mOpacityTexture ? mOpacityTexture->GetSRV() : MTextureManager::Get().WhiteTexture->GetSRV();
	MUIManager::Get().PushCommand(mInstanceData,
								  texSrv,
								  opacityTexSrv,
								  mInstanceData.TextureIndex,
								  mInstanceData.OpacityIndex,
								  GetHash());

	if (mText)
		mText->Draw();
}

void JUIComponent::Draw()
{
	auto* context = G_Device.GetImmediateDeviceContext();
	assert(context);

	ID3D11Buffer* buffers[2] = {g_VertexBuffer.Get(), g_InstanceBuffer.Get()};
	uint32_t      strides[2] = {g_VertexStride, g_InstanceStride};
	uint32_t      offsets[2] = {0, 0};

	context->IASetVertexBuffers(0,
								2,
								buffers,
								strides,
								offsets);

	context->IASetIndexBuffer(g_IndexBuffer.Get(),
							  DXGI_FORMAT_R32_UINT,
							  0);

	context->DrawIndexedInstanced(6,
								  1,
								  0,
								  0,
								  0);

	mText->Draw();
}

void JUIComponent::DrawID(uint32_t ID)
{}

void JUIComponent::PostRender()
{}

void JUIComponent::ShowEditor()
{
	ImGui::InputText(u8("이름"),
					 &mName,
					 ImGuiInputTextFlags_CharsNoBlank);

	ImGui::Separator();

	if (ImGui::CollapsingHeader(u8("변환 정보")))
	{
		ImGui::DragFloat2(u8("위치 (NDC 좌표)"), &mInstanceData.Position.x, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat2(u8("크기"), &mInstanceData.Size.x, 0.01f, 0.1f, 100.0f);
		if (mText)
			mText->SetNDCPosition(mInstanceData.Position);
	}

	if (ImGui::CollapsingHeader(u8("텍스트")))
	{
		static JText rawText;
		// rawText = WString2String(text);
		ImGui::InputText(u8("텍스트 슬롯"), &rawText);

		if (mText)
		{
			mText->SetText(String2WString(rawText));

			FLinearColor color = mText->GetColor();
			ImGui::ColorEdit4(u8("텍스트 색상"), &color.R);

			int32_t fontSize = mText->GetFontSize();
			ImGui::SliderInt(u8("폰트 크기"), &fontSize, 10, 100);

			mText->SetColor(color);
			mText->SetFontSize(fontSize);
		}
		if (!mText)
		{
			AddText(String2WString(rawText),
					mInstanceData.Position,
					mInstanceData.Size * 0.025,
					24.0f,
					FLinearColor::Black);
		}


	}

	if (ImGui::CollapsingHeader(u8("디자인")))
	{
		ImGui::InputInt(u8("레이어"), &mLayer);

		ImGui::ColorEdit4(u8("색상"), &mInstanceData.Color.x);

		ImGui::Text(u8("텍스처"));
		ImGui::SameLine();
		auto* tex = mTexture ? mTexture->GetSRV() : MTextureManager::Get().WhiteTexture->GetSRV();
		ImGui::Image(tex, ImVec2(100, 100));

		if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();

			const char* assetPath = static_cast<const char*>(payload->Data);
			mTexture              = MTextureManager::Get().Load(assetPath);
		}

		ImGui::Text(u8("투명도 텍스처"));
		ImGui::SameLine();
		tex = mOpacityTexture ? mOpacityTexture->GetSRV() : MTextureManager::Get().WhiteTexture->GetSRV();
		ImGui::Image(tex, ImVec2(100, 100));

		if (ImGui::IsMouseReleased(0) && ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();

			const char* assetPath = static_cast<const char*>(payload->Data);
			mOpacityTexture       = MTextureManager::Get().Load(assetPath);
		}
	}
}

void JUIComponent::AddText(JWTextView InText, const FVector2& InPosition, const FVector2& InRectSize, const float InSize,
						   const FLinearColor& InColor)
{
	assert(mWidgetComponent);
	auto newText = MakeUPtr<JFont>(mWidgetComponent->GetRenderTarget());
	newText->SetText(InText.data());
	newText->SetNDCTextRect(InPosition, InRectSize);
	newText->SetFontSize(InSize);
	newText->SetColor(InColor);

	mText = std::move(newText);
}

void JUIComponent::Initialize()
{
	mObjectFlags |= EObjectFlags::IsVisible;
}

void JUIComponent::Tick(float DeltaTime)
{
	OnAnimationEvent.Execute(DeltaTime);
}

JWidgetComponent::JWidgetComponent(const JText& InName)
	: JObject(InName) {}

uint32_t JWidgetComponent::GetType() const
{
	return HASH_ASSET_TYPE_WIDGET;
}

bool JWidgetComponent::Serialize_Implement(std::ofstream& FileStream)
{
	if (!JObject::Serialize_Implement(FileStream))
	{
		return false;
	}

	// Serialize Num
	int32_t num = mUIComponents.size();
	Utils::Serialization::Serialize_Primitive(&num, sizeof(num), FileStream);

	for (auto& ui : mUIComponents)
	{
		ui->Serialize_Implement(FileStream);
	}

	return true;
}

bool JWidgetComponent::DeSerialize_Implement(std::ifstream& InFileStream)
{
	if (!JObject::DeSerialize_Implement(InFileStream))
	{
		return false;
	}

	int32_t num;
	Utils::Serialization::DeSerialize_Primitive(&num, sizeof(num), InFileStream);

	mUIComponents.reserve(num);
	for (int32_t i = 0; i < num; ++i)
	{
		auto ui = MakeUPtr<JUIComponent>();
		ui->DeSerialize_Implement(InFileStream);
		ui->SetWidgetComponent(this);
		ui->SetVisible(true);
		mUIComponents.push_back(std::move(ui));
	}

	return true;
}

void JWidgetComponent::Tick(float DeltaTime)
{
	for (auto& ui : mUIComponents)
	{
		if (ui->IsVisible())
			ui->Tick(DeltaTime);
	}

	/*assert(mRenderTarget);

	float x = mRenderTarget->GetSize().width;
	float y = mRenderTarget->GetSize().height;
	for (int32_t i = mUIComponents.size() - 1; i >= 0; --i)
	{
		auto*    ui           = mUIComponents[i].get();
		FVector2 instancePos  = ui->GetInstanceData().Position;
		FVector2 instanceSize = ui->GetInstanceData().Size;
		instancePos.x         = (instancePos.x * 0.5f + 0.5f) * x;
		instancePos.y         = (instancePos.y * -0.5f + 0.5f) * y;

		int32_t minX = instancePos.x - instanceSize.x * x * 0.025f;
		int32_t maxX = instancePos.x + instanceSize.x * x * 0.025f;
		int32_t minY = instancePos.y - instanceSize.y * y * 0.025f;
		int32_t maxY = instancePos.y + instanceSize.y * y * 0.025f;

		if (mMousePos.x >= minX && mMousePos.x <= maxX && mMousePos.y >= minY && mMousePos.y <= maxY)
		{
			ui->OnHovered.Execute();
		}
		else
		{
			ui->OnUnhovered.Execute();
		}
	}*/
}

void JWidgetComponent::AddInstance()
{
	for (int32_t i = 0; i < mUIComponents.size(); ++i)
	{
		if (mUIComponents[i]->IsVisible())
			mUIComponents[i]->AddInstance(0);
	}
}

void JWidgetComponent::Draw()
{
	for (auto& ui : mUIComponents)
	{
		ui->Draw();
	}
}

JUIComponent* JWidgetComponent::GetClickedComponent(const FVector2& InMousePos, const FVector2& InScreenPos) const
{
	for (int32_t i = mUIComponents.size() - 1; i >= 0; --i)
	{
		auto*    ui           = mUIComponents[i].get();
		FVector2 instancePos  = ui->GetInstanceData().Position;
		FVector2 instanceSize = ui->GetInstanceData().Size;
		instancePos.x         = (instancePos.x * 0.5f + 0.5f) * InScreenPos.x;
		instancePos.y         = (instancePos.y * -0.5f + 0.5f) * InScreenPos.y;

		int32_t minX = instancePos.x - instanceSize.x * InScreenPos.x * 0.025f;
		int32_t maxX = instancePos.x + instanceSize.x * InScreenPos.x * 0.025f;
		int32_t minY = instancePos.y - instanceSize.y * InScreenPos.y * 0.025f;
		int32_t maxY = instancePos.y + instanceSize.y * InScreenPos.y * 0.025f;

		if (InMousePos.x >= minX && InMousePos.x <= maxX && InMousePos.y >= minY && InMousePos.y <= maxY)
		{
			return ui;
		}
	}

	return nullptr;
}

uint32_t JWidgetComponent::GetComponentSize() const
{
	return mUIComponents.size();
}

void JWidgetComponent::AddUIComponent(JTextView InName)
{
	auto newUIComp = MakeUPtr<JUIComponent>(InName.data());
	newUIComp->Initialize();
	newUIComp->SetWidgetComponent(this);
	mUIComponents.emplace_back(std::move(newUIComp));
}

void MUIManager::PostInitialize(const JText& OriginalNameOrPath, const JText& ParsedName, const uint32_t NameHash,
								void*        Entity)
{
	if (std::filesystem::is_regular_file(OriginalNameOrPath))
	{
		Utils::Serialization::DeSerialize(OriginalNameOrPath.data(), static_cast<ISerializable*>(Entity));
	}
}

void MUIManager::PushCommand(FInstanceData_UI&         InInstanceData, ID3D11ShaderResourceView* InTex,
							 ID3D11ShaderResourceView* InOpacityTex, _Out_ int32_t&              OutTexIndex,
							 _Out_ int32_t&            OutOpacityTexIndex, uint32_t              ID)
{
	mInstanceData.emplace_back(InInstanceData);
	mTextureSRVs.emplace_back(InTex);
	mOpacityTextureSRVs.emplace_back(InOpacityTex);

	OutTexIndex        = mTextureSRVs.size() - 1;
	OutOpacityTexIndex = mOpacityTextureSRVs.size() - 1;
}


void MUIManager::FlushCommandList(ID3D11DeviceContext* InContext)
{
	if (mInstanceData.empty())
	{
		return;
	}

	uint32_t maxNum = mTextureSRVs.size() > mOpacityTextureSRVs.size() ? mTextureSRVs.size() : mOpacityTextureSRVs.size();

	for (int32_t i = 0; i < maxNum; ++i)
	{
		mShader->SetTextureParams(mTextureSRVs[i], mOpacityTextureSRVs[i]);
	}

	mShader->BindShaderPipeline(InContext);

	Utils::DX::UpdateDynamicBuffer(InContext,
								   g_InstanceBuffer.Get(),
								   mInstanceData.data(),
								   sizeof(FInstanceData_UI) * mInstanceData.size());

	ID3D11Buffer* buffers[2] = {g_VertexBuffer.Get(), g_InstanceBuffer.Get()};
	uint32_t      strides[2] = {g_VertexStride, g_InstanceStride};
	uint32_t      offsets[2] = {0, 0};
	InContext->IASetVertexBuffers(0,
								  2,
								  buffers,
								  strides,
								  offsets);

	InContext->IASetIndexBuffer(g_IndexBuffer.Get(),
								DXGI_FORMAT_R32_UINT,
								0);

	InContext->DrawIndexedInstanced(6,
									mInstanceData.size(),
									0,
									0,
									0);


	mInstanceData.clear();
	mTextureSRVs.clear();
	mOpacityTextureSRVs.clear();
}

void MUIManager::SetHUD(JWidgetComponent* InHUD)
{}

MUIManager::MUIManager()
{
	mVertexData.reserve(4);
	mVertexData.emplace_back(FVector2(-0.05, -0.05), FVector2(0, 1)); // Bottom-Left
	mVertexData.emplace_back(FVector2(+0.05, -0.05), FVector2(1, 1)); // Bottom-Right
	mVertexData.emplace_back(FVector2(+0.05, +0.05), FVector2(1, 0)); // Top-Right
	mVertexData.emplace_back(FVector2(-0.05, +0.05), FVector2(0, 0)); // Top-Left

	mIndexData.reserve(6);
	mIndexData.push_back(0);	// Bottom-Left
	mIndexData.push_back(1);	// Bottom-Right
	mIndexData.push_back(2);	// Top-Right
	mIndexData.push_back(2);	// Top-Right
	mIndexData.push_back(3); // Top-Left
	mIndexData.push_back(0);	// Bottom-Left

	auto* device = G_Device.GetDevice();
	Utils::DX::CreateBuffer(
							device,
							D3D11_BIND_VERTEX_BUFFER,
							reinterpret_cast<void**>(mVertexData.data()),
							sizeof(FVertexData_UI),
							mVertexData.size(),
							g_VertexBuffer.GetAddressOf()
						   );

	Utils::DX::CreateBuffer(
							device,
							D3D11_BIND_VERTEX_BUFFER,
							nullptr,
							sizeof(FInstanceData_UI),
							64,
							g_InstanceBuffer.GetAddressOf(),
							D3D11_USAGE_DYNAMIC,
							D3D11_CPU_ACCESS_WRITE
						   );

	Utils::DX::CreateBuffer(
							device,
							D3D11_BIND_INDEX_BUFFER,
							reinterpret_cast<void**>(mIndexData.data()),
							sizeof(uint32_t),
							mIndexData.size(),
							g_IndexBuffer.GetAddressOf()
						   );

	mShader        = MShaderManager::Get().UIShader;
	mPickingShader = MShaderManager::Get().UIElementShader;

	LoadingScreen = Load("Game/UI/loadingding.jasset");


	LoadingScreen->mUIComponents[2]->OnAnimationEvent.Bind([&](float DeltaTime)
	{
		auto& data = LoadingScreen->mUIComponents[2]->GetInstanceData();
		data.Color.w =  (sin(GetWorld.GetGameTime() * 7.14159f / 2) + 1) / 2;
	});
	
	LoadingScreen->mUIComponents[3]->OnAnimationEvent.Bind([&](float DeltaTime)
	{
		auto& data = LoadingScreen->mUIComponents[3]->GetInstanceData();
		data.Color.w =  (sin((GetWorld.GetGameTime()-0.32f) * 7.14159f / 2) + 1) / 2;
	});
	
	LoadingScreen->mUIComponents[4]->OnAnimationEvent.Bind([&](float DeltaTime)
	{
		auto& data = LoadingScreen->mUIComponents[4]->GetInstanceData();
		data.Color.w =  (sin((GetWorld.GetGameTime()-0.64f) * 7.14159f / 2) + 1) / 2;
	});
}

MUIManager::~MUIManager() {}
